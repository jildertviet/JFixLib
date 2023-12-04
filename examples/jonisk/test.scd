p = SerialPort.new("/dev/ttyUSB0", 230400, crtscts: true)
a = [[0x3C,0x71,0xBF,0x10,0x80,0x5D]]; // Green perf board
(
a = [[0x34,0x86,0x5d,0x79,0x6f,0xf9]]; // Jonisk 17
j = a.collect({|e, i| JJonisk.new(i, e, p)});
)
t = JFixtureCollection.new(p);
t.readConfigFile("/home/jildert/of_v0.11.2_linux64gcc6_release/apps/TIYCS/jonisk.config", 0);
t.gui
t.children[0].guiDict[\synthBrightness].value_(0.3)
EZSlider
t.children[0].synth.set(\rgbw, [0.0, 0.5, 1.0, 1.0]);
a = (0..15).collect({|e, i| if(i.mod(4) == 0, {e}, {nil})})
[0,1,2].put(1, 3)
t.children[17].gui
t.children[17].mode = "static";
t.children[17].setRGBW([1.0, 0.0, 0.0, 0]);
t.children[17].setBrightness(0.2)
t.children[17].setLag(60.reciprocal)
1/30
if((1==1).or(2==1), {2})
(
{
(t.children[17]).do{|e|
    // e.setOTAServer("Klank",  ~pw, "https://192.168.10.164/.pio/build/esp32dev/firmware.bin");
    e.setOTAServer("Klank",  ~pw, "https://192.168.10.164/firmware.bin");
    35.wait;
  };
  "Done".postln;
}.fork;
)

{[Env.perc(curve: -4).ar(), Env.perc(curve: 0).ar()]}.plot(2)
(x = {var a = \test.kr(0); 0; }.play;)
x.get(\test, {|e|e.postln})
x.set(\test, 1.0.rand)
Synth
(
w = Window.new.front;
a = EZSlider.new(w);
a.postln;
)
