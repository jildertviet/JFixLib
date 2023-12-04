+ JFixture {
	*getGuiWindow{
		|object, functions|
		// "getGuiTest".postln;
		var window;
		var id = if(object != nil, {object.id}, {"X"});
		var colorSliders;
		var windowName = if(id != nil, {"JFixture " ++ id.asString}, {"JFixture x"});
		var dict = Dictionary(); // Holds GUI items
    var scope;
    var margin = 5;
    var newLine = {
      window.view.decorator.nextLine;
      window.view.decorator.left = margin;
    };
    if(object != nil, {object.guiDict = dict});
		window = Window(windowName).front.setInnerExtent(360, 410);
		window.bounds_(window.bounds.moveTo(0,0));
		window.view.palette_(QPalette.dark);
		window.view.decorator_(FlowLayout(window.view.bounds));
		window.view.decorator.left = margin;
		if(functions==nil, {
			functions = Dictionary.newFrom([
				\test, {object.testLed()},
				\otaServer, {object.setOTAServer()},
				// \battery, {object.requestBattery()},
				\testEnv, {object.trigger()},
				\deepsleep, {
					var w = Window("Deep sleep duration", Rect(window.bounds.left, 500, window.bounds.width, 100));
					var b = NumberBox(w, Rect(150, 10, 100, 20));
					b.value = 1;
					b.action = {arg numb; object.deepSleep(numb.value); w.close;};
					w.front;
				},
				\setColor, {
					|e, i|
					var newColor = object.color.asArray.put(i, e.value);
					object.setRGBW(newColor.postln);
					object.synth.set(\rgbw, newColor);
				},
				\getColor, {
					|i|
					object.color.asArray[i];
				},
				\getEnv, {
					|i|
          if(object.synth != nil, {
            object.asr.at(i)
        }, {0});
				},
				\setEnv, {
					|e, i|
          if(object.synth != nil, {
            switch(i,
              0, {object.setAttack(e.value)},
              1, {object.setSustain(e.value)},
              2, {object.setRelease(e.value)}
            );
          });
				},
		  	\getBrightness, {
					object.brightness;
				},
				\setSynthBrightness, {
					|e|
          "SXDADSAsd".postln;
					object.synth.set(\amp, e.value);
				},
  			\setBrightness, {
					|e|
					object.setBrightness(e.value);
				},
				\getBrightnessAdd, {
					object.brightnessAdd;
				},
				\setBrightnessAdd, {
					|e|
					object.brightnessAdd = e.value;
          object.synth.set(\brightnessAdd, e.value);
				},
				\getAddress, {
					object.getAddressHexString
				},
				\getBus, {
					object.bus;
				}
			]);
		});
		[
			["Test",functions[\test]],
			["OTA", functions[\ota]],
			["Battery",functions[\battery]],
			["TestEnv",functions[\testEnv]],
			["Sleep",functions[\deepsleep]],
			// ["OtaS",functions[\otaServer]],
		].do{
			|e|
			Button.new(window, 60@40).string_(e[0]).action_(e[1]);
		};
    newLine.();
    dict[\rgbw] = colorSliders = Array.fill(4, {
			|i|
			var slider = EZSlider.new(window, label:["Red","Green","Blue","White"].at(i), controlSpec: ControlSpec(0.0, 1.0, 'lin', step: 1e-3)).value_(functions[\getColor].value(i)).action_({
				|e|
				functions[\setColor].value(e, i);
			};
			);
      newLine.();
      slider;
		});
		dict[\asr] = Array.fill(3, {
			|i|
			var slider = EZSlider.new(window, label:["Attack", "Sustain", "Release"].at(i), controlSpec: ControlSpec(0, [4,10,10].at(i), 'lin')).value_(functions[\getEnv].value(i)).action_({
				|e|
				functions[\setEnv].value(e, i);
			});
      newLine.();
      slider;
		});
		dict[\synthBrightness] = EZSlider.new(window,
      label:"synthBrightness",
			controlSpec: ControlSpec(0, 1, 'lin', 1e-3),
			labelWidth: 80).action_(functions[\setSynthBrightness]);
    newLine.();
		dict[\brightnessAdd] = EZSlider.new(window,
			label:"synth bAdd",
			controlSpec: ControlSpec(0, 1, 'lin', 1e-3),
			labelWidth: 80).value_(functions[\getBrightnessAdd].value()).action_(functions[\setBrightnessAdd]);
    newLine.();
    dict[\brightness] = EZSlider.new(window,
			label:"Brightness",
			controlSpec: ControlSpec(0, 1, 'lin', 1e-3),
			labelWidth: 80).value_(functions[\getBrightness].value()).action_(functions[\setBrightness]);
    // Set properties for all EZSliders
    dict.do{|e| if(e.isArray == true, {
      e.do{
        |c|
        c.setColors(numNormalColor: Color.white).labelView.align_(\left)
      }
    }, {
      e.setColors(numNormalColor: Color.white).labelView.align_(\left)
    });
   };

		// window.view.decorator.top = window.view.decorator.top + margin;
		TextView.new(window, Rect(0,0, window.bounds.width - (2*margin), 28)).string_(functions[\getAddress].value()).editable_(false).hasVerticalScroller_(false);
    window.view.decorator.nextLine;
		window.view.decorator.left = margin;
		// window.view.decorator.top = window.view.decorator.top + 30;
    window.view.decorator.nextLine;
		window.view.decorator.left = margin;
		scope = JStethoscope.new(Server.default, functions[\getBus].value().numChannels, functions[\getBus].value().index, rate: 'control', view: window.view, bAddSliders: false);
    scope.style_(1);
    scope.view.bounds_(Rect(scope.view.bounds.left, scope.view.bounds.top, window.bounds.width - (2*margin), 100));
    // scope.view.bounds.height_(100);
    scope.scopeView.waveColors_([Color.red, Color.green, Color.blue, Color.white]).alpha_(0.1);
    if(object != nil, {
      object.synth.get(\amp, {|v| {dict[\synthBrightness].value_(v);}.defer});
      object.synth.get(\brightnessAdd, {|v| {dict[\brightnessAdd].value_(v);}.defer});
    });
		^[window, dict];
	}
	gui{
		^JFixture.getGuiWindow(this);
	}
}

