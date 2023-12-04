JJonisk : JFixtureDimmer{
  *new{
    |index, addr, serial|
    JJonisk.loadSynthDef();
    ^super.new(index, addr, serial).customInit;
  }
  customInit{
    bus = Bus.alloc(\control, Server.default, 4);
		synth = Synth(\jonisk, [out: bus, curve: 0]);
  }
  *loadSynthDef{
    SynthDef(\jonisk, {
			|
			brightnessAdd=1, rgbw=#[0,0,0,0], out=0, curve = -4,
			trigRand=1, a=0.1, s=1.0, r=1.0, gate=0, level=1, noiseMul=0, lagTime=0.01,
			colorMap=#[0,1,2,3]
			|
			var output;
			var env = EnvGen.kr(Env.linen(a, s, r, level, curve: curve), Changed.kr(gate));
			var brightness = Mix.kr([env, Lag.kr(brightnessAdd, 1)]).min(1);
			var noise = LFDNoise1.ar(1/4, noiseMul).abs.min(1);
			brightness = (Lag2.kr(brightness , lagTime) + noise);
      output = brightness * rgbw;
			output = output.min(1.0); // Clip to 1.0
			output = Select.ar(colorMap, output); // Option to set different order of colors for one fixture
      output = Select.ar(\mode.kr(0), [output, brightness]);
			Out.kr(out, output * \amp.kr(1, 1));
		}).add();
  }
}
