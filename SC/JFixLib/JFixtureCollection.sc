JFixtureCollection {
  var <> serial;
  var <> children;
  var <> window;
	var <> lastSeenData;
  var frameRate;
	var frameRateIndex = 2;
  var msgBuffer = #[0,0,0];
	var <> message = "";
	var <>bWriteMsg = false;
  var <> serialReadRoutine;
  var <> updateRoutine;
  var <> frameDur = 0.01;
  var <> mode = "static";
  var modes = #["static", "st_rgbw", "st_brightness"];
  var <> modeIndex = 0; // ["static", "st_rgbw", "st_brightness"];
  var <> guiDict; // For JFixtureMain window
  var <> guiItems;
  var <> globalGuiDict;
  var globalSettingsWindow;
  var <> espnowBridge;
  var <> broadcaster;
  *new{
    |serial=nil|
    ^super.new.init(serial);
  }
  init{
    |serial|
    if(serial == nil, {
      this.serial = JFixtureCollection.openDefaultSerial();
    }, {
      if(serial.class == SerialPort, {
        this.serial = serial;
          "Use Serial".postln;
      },{
        if(serial.class == NetAddr, {
          if(serial.hostname.split($.)[3] == "255", {
            "Use broadcast UDP".postln;
            this.broadcaster = serial;
          }, {
            this.espnowBridge = serial;
            "Use ESPNOW-Bridge".postln;
          });
        });
      });
    });
    children = List.new;
    guiItems = List.new;
    this.initGuiDict();
    this.startReadingSerial();
  }
  *openDefaultSerial{
    var p = SerialPort.new("/dev/ttyUSB0", 230400, crtscts: true);
    ^p;
  }
  readConfigFile{
		|path, type="0: JJonisk, 1: JTlFixture, 2: llllllllllll"|
		var file = JSONFileReader.read(path);
    children.clear();
		file[0]["active"].do{
			|j, i|
      var serialOrNetAddr;
			var addr;//  = j[0];
      if(j["address"] != nil, {addr = j["address"]}, {addr = j[0]});
      if(serial != nil, {
        serialOrNetAddr = serial;
      },{
        if(espnowBridge != nil, {
          serialOrNetAddr = espnowBridge;
        });
        if(broadcaster != nil, {
          serialOrNetAddr = broadcaster;
        })
      });
			addr = addr.collect({|e| e.split($x)[1].asHexIfPossible});
      switch(type, 
        0, {children.add(JJonisk.new(i, addr, serialOrNetAddr));},
        1, {children.add(JTLFix.new(i, addr, serialOrNetAddr));},
        2, {children.add(Jllllllllllll.new(i, addr, serialOrNetAddr));},
      );
		};
    lastSeenData = [0, 0]!children.size; // [button, time]
	}
  storeByte{
		|byte|
		msgBuffer = msgBuffer.rotate(-1);
		msgBuffer[2] = byte;
	}
	checkForMsgStart{
		|byte|
		if(byte == 'g'.ascii[0], {
			if(msgBuffer[1] == 's'.ascii[0], {
				if(msgBuffer[0] == 'm'.ascii[0], {
					bWriteMsg = true;
				});
			});
		});
	}
	checkForMsgEnd{
		|byte|
		if(byte == 'd'.ascii[0], {
			if(msgBuffer[1] == 'n'.ascii[0], {
				if(msgBuffer[0] == 'e'.ascii[0], {
					bWriteMsg = false;
					this.parseMsg();
					^ true;
				});
			});
		});
		^ false;
	}
	parseMsg{
		var msg = message;
		msg = msg.replace("en", ""); // Remove last
		("\nMSG: " ++ msg).postln;
		if(msg.find("b") == 0, { // Battery update
			var addr = (0!6);
			var batteryVoltage;
			msg = msg.replace("b", ""); // ?
			for(0, 5, {|e, i| addr[i] = msg.split($:)[i].asInteger});
			addr = addr + [0, 0, 0, 0, 0, 1];
			addr.postln;
			batteryVoltage = msg.split($:).last.asInteger;
			batteryVoltage = batteryVoltage / 2.pow(12); // 0V is 0, 16.8V is 4096. Value is now a percentage / 100.
			batteryVoltage = batteryVoltage - (2/3); // 0.66 is shut-down voltage
			batteryVoltage = (batteryVoltage * 300).asInteger; // 1/3 is scaled back to 100%
			batteryVoltage.postln;
			// children.do{|e| if(e.address == addr, {
				// e.setBatteryPct(batteryVoltage);
			// })};
			/*			{
			window.close;
			this.gui();
			}.defer;*/
		});
		// 0x91
		// 0xD9
		if(msg.find("a") == 0, { // Alive ping?
			var addr = (0!6);
			var batteryVoltage;
			var fwVersion = "...";
			msg = msg.replace("a", "");
			for(0, 5, {|e, i| addr[i] = msg.split($:)[i].asInteger});
			addr = addr + [0, 0, 0, 0, 0, 1];
			addr.postln;

			batteryVoltage = msg.split($:)[6].asInteger;
			batteryVoltage = batteryVoltage / 2.pow(12); // 0V is 0, 16.8V is 4096. Value is now a percentage / 100.
			batteryVoltage = batteryVoltage - (2/3); // 0.66 is shut-down voltage
			batteryVoltage = (batteryVoltage * 300).asInteger; // 1/3 is scaled back to 100%

			if(msg.split($:).size >= 8, {
				fwVersion = msg.split($:).at([7, 8]).ascii.reshape(2);
			});

			children.do{
				|e, i|
				if(e.address == addr, {
					{lastSeenData[i][0].background_(Color.green)}.defer;
					// e.setBatteryPct(batteryVoltage);
          this.guiItems.do{|c| if(c[0].sum == addr.sum, {
            {
              c[1].string_(batteryVoltage.asString ++ "%");
              c[2].string_(fwVersion.asString);
            }.defer;
          })};
					lastSeenData[i][1] = Date.getDate.rawSeconds;
				}
			)};
		});
		message = "";
	}

  startReadingSerial {
		if(serial != nil, {
			if(serial.isOpen, {
				"Start reading serial".postln;
				serialReadRoutine = Routine({
					var byte;
					inf.do{|i|
						byte = serial.next;
						while({byte != nil}, {
							this.storeByte(byte);
							if(this.checkForMsgEnd(byte), { // This calles MsgParse()

							}, {
								if(byte < 127, {
									if(byte == 10, { // Newline?
										// "".postln;
									}, {
										// byte.asAscii.post; // Monitor incoming bytes
									});
								});
							});
							if(bWriteMsg == true, {
								message = message ++ byte.asAscii;
							});

							this.checkForMsgStart(byte);
							byte = serial.next;
						});
						0.05.wait; // Was 0.1
					};
				}).play;
			}, {
				"Serial not open".error;
			});
		});
	}

  checkLastSeen{
		{
			inf.do{
				lastSeenData.do{
					|t, i|
					if(t[1] > 0, { // Only check actual set timestamps
						if(Date.getDate.rawSeconds - t[1] > 120, {
							{lastSeenData[i][0].background_(Color.gray)}.defer;
						});
					});
				};
				2.wait;
			}
		}.fork;
	}

  updateRGBW{
    // Read synths, send to all
    this.sendRGBWn();
    // "URGBW".postln;
  }
  updateBrightness{
    // Read synths, send to all
    this.sendBrightnessN();
    // "UBR".postln;
  }
  start{
		if(serial != nil, {
			if(serial.isOpen, {
				updateRoutine.stop;
        "Init values".error;
        switch(mode,
        "static", {
          // Send brightness and colors
          children[0].bBroadcast = true;
          children[0].start();
          children[0].setBrightness(children[0].brightness);
          children[0].setRGBW(children[0].color.asArray);
          children[0].end();
          children[0].bBroadcast = false;
        },
        "st_rgbw", {
          // Send brightness
          children[0].bBroadcast = true;
          children[0].setBrightness(children[0].brightness);
          children[0].bBroadcast = false;
        },
        "st_brightness", {
          // Send colors
          children[0].bBroadcast = true;
          children[0].setRGBW(children[0].color.asArray);
          children[0].bBroadcast = false;
        });
        if(mode != "static", {
          updateRoutine = {
            inf.do{
              switch(mode, 
              "st_rgbw", {this.updateRGBW},
              "st_brightness", {this.updateBrightness}
              );
              frameDur.wait;
            }
          }.fork;
        });
				children.do{|e| e.mode = mode};
				^true;
			}, {
				"Serial port not open".error;
				^false;
			});
		}, {
			"No serial port initiated".error;
			^false;
		});
	}
	stop{
		children.do{|e| e.mode = "static"};
		updateRoutine.stop;
	}

  sendRGBWn{
    children[0].bus.getn(children.size * 4, {
			|v|
			var msg = (0xFF!6) ++ [0x33] ++ v.collect({|e| Int16(e * 65536).asBytes}).reshape(v.size * 2) ++ "end";
      children[0].send(msg, true);
		});
  }

  sendBrightnessN{ // Send brightness as float
    children[0].bus.getn(children.size * 4, {
			|v|
			var msg;
      var arrayToWrite = 0!(v.size/4);
      (v.size/4).do{|i| arrayToWrite[i] = v[i*4]}; // Channel 0. Set mode to 1 in synths 
      msg = (0xFF!6) ++ [0x34] ++ arrayToWrite.asBytes32F ++ "end";
      children[0].send(msg, true);
		});
  }

  gui{
    |windowName="JFixtures"|
		var canvasLocal = View(); // .background_(Color.black);
		var globalButton;
		var canvas,layout;
		var bounds = Rect(0, 0, 500, 700);
    var liveButton;
		window = Window(windowName, bounds, scroll: true).front;
		window.view.hasBorder_(false);
		window = ScrollView(window, bounds:  bounds.insetBy(0,0)).hasBorder_(false); // Why the inset?
		window.palette_(QPalette.dark);
		canvasLocal.palette_(QPalette.dark);
    lastSeenData = Array.fill(children.size, {
			[View().background_(Color.gray), 0]
		});
		window.layout = VLayout(
			HLayout(
        [PopUpMenu().items_(["Static", "RGBW","Brightness"]).stringColor_(Color.white).action_({
					|menu|
          this.stop;
          modeIndex = menu.value;
          mode = modes[modeIndex];
          if(mode == "st_rgbw", {
            children.do{|e| e.synth.set(\mode, 0);};
            if(globalGuiDict[\scope] != nil, {
              globalGuiDict[\scope].scopeView.visible_(true);
            });
            globalGuiDict[\rgbaColors].visible_(false);
          });
          if(mode == "st_brightness", {
            if(globalGuiDict[\scope] != nil, {
              globalGuiDict[\scope].scopeView.visible_(true);
            });
            globalGuiDict[\rgbaColors].visible_(false);
            children.do{|e| e.synth.set(\mode, 1);};
          });
          if(mode == "static", {
            if(globalGuiDict[\scope] != nil, {
              globalGuiDict[\scope].scopeView.visible_(false);
            });
            globalGuiDict[\rgbaColors].visible_(true);
          });
          this.start;
				}).value_(modeIndex)],
				[PopUpMenu().items_([10, 25, 30, 60]).stringColor_(Color.white).action_({
					|menu|
					frameDur = (menu.item.reciprocal);
					// lagTime = frameDur;
          children[0].bBroadcast = true;
          children[0].setLag(frameDur);
          children[0].bBroadcast = false;
					frameRate = menu.item;
          frameDur.postln;
				}).value_(frameRateIndex)],
        globalButton = Button().string_("Global").action_({this.openGlobalGui()}),
				Button().string_("Config").action_({/*this.configLights*/ "To do: testpatter".error}),
				Button().string_("Test pattern").action_({
					// this.toggleTestPatttern();
					// Document.open(testPatternPath).front;
          "Not implemented".error;
				});
				/*				window.view.bounds.width * 0.25,*/
			),
			*(children.collect({|e, i|
				var guiButtonView = View();
				var guiButton = Button(guiButtonView, Rect(0, 0, 40, 20)).states_([
					["GUI", Color.white, Color.black.alpha_(0.1)],
				]).action_({
					var guiWindow = e.gui;
					var bounds = guiWindow[0].bounds;
					guiWindow[0].bounds_(bounds + Rect(window.bounds.width, 0, 0, 0))});
				var testButtonView = View();
				var testButton = Button(testButtonView, Rect(0, 0, 40, 20)).states_([
					["Test", Color.white, Color.black.alpha_(0.1)],
				]).action_({e.testLed()});
        var batteryField = StaticText.new().background_(Color.black.alpha_(0.1)).align_(\center).string_("" ++ "%");
        var fwVersion = StaticText.new().background_(Color.black.alpha_(0.1)).align_(\center).string_("..."); 
        guiItems.add([e.address, batteryField, fwVersion]);
				HLayout(
					[StaticText.new().string_(e.id).background_(Color.black.alpha_(0.1)), s: 5],
					[StaticText.new().string_(e.getAddressHexString.asCompileString.replace("\"", "").replace("]", "").replace("[", "")).background_(Color.black.alpha_(0.1)), s: 60],
          [batteryField, s: 10],          
          [fwVersion, s: 10],
					[guiButtonView, s: 10],
					[testButtonView, s: 10],
					[lastSeenData[i][0], s: 1]
			)}));
		);
		canvasLocal.layout = layout;
		window.canvas = canvasLocal;
		globalButton.valueAction_(1);
	}
  openGlobalGui{ // JFixture GUI, but controlling all children of JFixtureCollection
		var bounds;
		var windowAndSliders = JFixture.getGuiWindow(nil, guiDict);
    children[0].synth.get(\amp, {|v| {windowAndSliders[1][\synthBrightness].value_(v)}.defer});
    children[0].synth.get(\brightnessAdd, {|v| {windowAndSliders[1][\brightnessAdd].value_(v);}.defer});
    globalGuiDict = windowAndSliders[1];

    if(globalSettingsWindow != nil, {
      globalSettingsWindow.close();
    });
		globalSettingsWindow = windowAndSliders[0];
  	bounds = globalSettingsWindow.bounds;
		globalSettingsWindow.bounds_(bounds + Rect(window.bounds.width, 0, 0, 0));
	}

  initGuiDict{
		guiDict = Dictionary.newFrom([
			\test, {"Inactive".postln},
			\ota, {},
			\battery, {},
			\testEnv, {children.do{|j| j.trigger()}},
			\deepsleep, {
				var w = Window("Deep sleep duration: all", Rect(window.bounds.left, 500, window.bounds.width, 100));
				var b = NumberBox(w, Rect(150, 10, 100, 20));
				b.value = 1;
				b.action = {
					arg numb;
					var min = numb.value;
					w.close;
					{
						("Put all Jonisks to sleep for " ++ min.asString ++ " minutes").postln;
						children.do{|e|
							e.deepSleep(min);
							0.25.wait;
						};
					}.fork;
				};
				w.front;
			},
			\setColor, {
				|e, i|
        var newColor = children[0].color.asArray.put(i, e.value);
        children[0].bBroadcast = true;
        children[0].setRGBW(newColor);
        children[0].bBroadcast = false;
        children.do{|object|
          object.synth.set(\rgbw, newColor);
          object.color = Color.fromArray(newColor);
        };
			},
			\getColor, {
				|i|
        children[0].color.asArray[i];
			},
			\getEnv, {
				|i|
				children[0].asr.at(i);
			},
			\setEnv, {
				|e, i|
				children.do{
					|object|
					switch(i,
						0, {object.setAttack(e.value)},
						1, {object.setSustain(e.value)},
						2, {object.setRelease(e.value)}
					);
				}
			},
			\getBrightness, {
				children[0].brightness;
			},
			\setBrightness, {
				|e|
        children[0].bBroadcast = true;
        children[0].setBrightness(e.value);
        children[0].bBroadcast = false;
				children.do{ |object|
					object.brightness = e.value;
				}
			},
			\getBrightnessAdd, {
				children[0].brightnessAdd;
			},
      \setSynthBrightness, {
        |e|
        children.do{|object|
          object.synth.set(\amp, e.value);
        }
      },
			\setBrightnessAdd, {
				|e|
				children.do{ |object|
          object.brightnessAdd = e.value;
          object.synth.set(\brightnessAdd, e.value);
				}
			},
			\getAddress, {
				"All";
			},
			\getBus, {
        if(children[0].bus != nil, {
				children[0].bus;
      }, {0});
			}
		]);
	}
}
