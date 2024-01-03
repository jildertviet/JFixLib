p = SerialPort.new("/dev/ttyUSB0", 230400, crtscts: true)
a = [[0x3C,0x71,0xBF,0x10,0x80,0x5D]]; // Green perf board
(
a = [[0x34,0x86,0x5d,0x79,0x6f,0xf9]]; // Jonisk 17
j = a.collect({|e, i| JJonisk.new(i, e, p)});
)
p = NetAddr("127.0.0.1", 7997);
t = JFixtureCollection.new(p);
t.readConfigFile("/home/jildert/of_v0.11.2_linux64gcc6_release/apps/TIYCS/jonisk.config", 0);
t.gui
t.globalGuiDict['rgbaColors'].children[0].

View
t.children[0].guiDict[\scope].scopeView.visible_(true)
t.globalGuiDict[\scope].scopeView.visible_()
t.globalGuiDict[\rgbaColors]
Stethoscope
visible
[0,1,2,3,4,].hash
t.guiItems
t.guiItems[17][1].hash
t.guiItems[17][0].sum
t.children[0].guiDict[\synthBrightness].value_(0.88)
EZSlider
t.children[0].synth.set(\rgbw, [0.0, 0.5, 1.0, 1.0]);
a = (0..15).collect({|e, i| if(i.mod(4) == 0, {e}, {nil})})
[0,1,2].put(1, 3)
t.children[17].gui
t.children[17].mode = "static";
t.children[17].brightness
t.children[17].setRGBW([1.0, 0.0, 0.0, 0]);
t.children[17].setBrightness(1.0)
t.children[8].setLag(60.reciprocal)
t.children[8].synth.set(\noiseMul, 0)
1/30
if((1==1).or(2==1), {2})
(
{
(t.children.at([15])).do{|e|
    e.setOTAServer("Klank",  ~pw, "https://192.168.10.47/.pio/build/esp32dev/firmware.bin");
    // e.setOTAServer("Klank",  ~pw, "https://192.168.10.164/firmware.bin");
    35.wait;                     
  };
  "Done".postln;
}.fork;
)
(
q.stop; q = {
var a = t.lastSeenData.collect{|e,i|if(e[1] > 0, {i}, {-1})};
  inf.do{
    |i|
    var id = a.wrapAt(i);
    if(id >= 0, {
      t.children[a.wrapAt(i)].trigger();
      0.2.wait;
    })
  }
}.fork;
)
q.stop;
t.children.do{|e|e.synth.set(\noiseMul, 1)}
t.children[0].
