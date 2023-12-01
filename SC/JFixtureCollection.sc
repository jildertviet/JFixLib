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
  *new{
    |serial=nil|
    ^super.new.init(serial);
  }
  init{
    |serial|
    if(serial != nil, {
      this.serial = JFixtureCollection.openDefaultSerial();
    }, {
      this.serial = serial;
    });
    children = List.new;
    this.startReadingSerial();
  }
  *openDefaultSerial{
    var p = SerialPort.new("/dev/ttyUSB0", 230400, crtscts: true);
    ^p;
  }
  readConfigFile{
		|path, type="JJonisk[0], JTlFixture[1]"|
		var file = JSONFileReader.read(path);
		file[0]["activeJonisks"].do{
			|j, i|
			var addr = j[0];
			// addr.postln;
			addr = addr.collect({|e| e.split($x)[1].asHexIfPossible});
      switch(type, 
        0, {children.add(JJonisk.new(i, addr, serial));},
        1, {children.add(JTlFix.new(i, addr, serial));},
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
					// {e.fwVersionField.string_(fwVersion.asString)}.defer;
          "To do: battery and fwVersion".error;
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
										"".postln;
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

  gui{
    |windowName="JFixtures"|
		var canvasLocal = View(); // .background_(Color.black);
		var globalButton;
		var canvas,layout;
		var bounds = Rect(0, 0, 500, 700);
    var liveButton;
		window = Window(windowName, bounds, scroll: true).front;
		window.view.hasBorder_(false);
		window = ScrollView(window, bounds:  bounds.insetBy(2,0)).hasBorder_(false);
		window.palette_(QPalette.dark);
		liveButton = Button().string_("Idle").action_({
			|e|
			e.states_([["GO LIVE", Color.white, Color.new255(49,222,75)], ["STOP", Color.white, Color.new255(255,65,54)]]).action_({
				|e|
			// 	if(e.value == 1, {
			// 		"Start live mode".postln;
			// 		if(this.start() == true, {
			//
			// 		}, {
			// 			e.value_(0);
			// 		});
			// 	}, {
			// 		"Stop live mode".postln;
			// 		this.stop();
			// 	})
			});
			e.valueAction_(1);
		});

		canvasLocal.palette_(QPalette.dark);
    lastSeenData = Array.fill(children.size, {
			[View().background_(Color.gray), 0]
		});
		globalButton = Button().string_("Global").action_({/*this.openGlobalGui*/});
		window.layout = VLayout(
			HLayout(
				[liveButton],
				// [NumberBox().value_(frameDur).action_({|e| frameDur = e.value}).normalColor_(Color.white)],
				[PopUpMenu().items_([10, 25, 30, 60]).stringColor_(Color.white).action_({
					|menu|
					// menu.item.postln;
					// frameDur = (1/menu.item);
					// lagTime = frameDur;
					// frameRate = menu.item;
          "Not implemented".error;
				}).value_(frameRateIndex)],
				globalButton,
				Button().string_("Config").action_({/*this.configLights*/}),
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
				HLayout(
					[StaticText.new().string_(e.id).background_(Color.black.alpha_(0.1)), s: 5],
					[StaticText.new().string_(e.getAddressHexString.asCompileString.replace("\"", "").replace("]", "").replace("[", "")).background_(Color.black.alpha_(0.1)), s: 60],
					// [e.createBatteryField(), s: 10],
					// [e.createFwVersionField(), s: 10],
					[guiButtonView, s: 10],
					[testButtonView, s: 10],
					[lastSeenData[i][0], s: 1]
			)}));
		);
		canvasLocal.layout = layout;
		window.canvas = canvasLocal;
		globalButton.valueAction_(1);
	}
}