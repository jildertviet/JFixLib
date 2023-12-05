p = SerialPort.new("/dev/ttyUSB0", 230400, crtscts: true)
a = [[0x3C,0x71,0xBF,0x10,0x80,0x5D]]; // Green perf board
(
a = [[0x34,0x86,0x5d,0x79,0x6f,0xf9]]; // Jonisk 17
j = a.collect({|e, i| JJonisk.new(i, e, p)});
)
t = JFixtureCollection.new(p);
t.readConfigFile("/home/jildert/of_v0.11.2_linux64gcc6_release/apps/TIYCS/jonisk.config", 0);
t.gui
[0,1,2,3,4,].hash
t.guiItems
t.guiItems[17][1].hash
t.children[0].guiDict[\asr].isArray\sk
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
    e.setOTAServer("Klank",  ~pw, "https://192.168.10.47/.pio/build/esp32dev/firmware.bin");
    // e.setOTAServer("Klank",  ~pw, "https://192.168.10.164/firmware.bin");
    35.wait;
  };
  "Done".postln;
}.fork;
)
Pseq( {|i| [1, 0, 0].wrapAt(i.mod(7)) }.dup(7), inf).asStream.nextN(7)
[1,0,0].wchoose([1,0,0])

(
var a = 0.45;
var b = a.linlin(0, 1, 1/3, 1-(1/3));
var c = a.linlin(0, 1, 2/3, 1-(2/3));
b.postln;
c.postln;
)
ceil
