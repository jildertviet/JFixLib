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
~ids = [ 4, 6, 7, 11, 13, 14, 15, 16, 17, 19, 22, 23 ];

t.globalGuiDict['rgbaColors'].children[0].

t.children[0].guiDict[\scope].scopeView.visible_(true)
t.globalGuiDict[\scope].scopeView.visible_()
t.globalGuiDict[\rgbaColors]
t.guiItems
t.children[0].guiDict[\synthBrightness].value_(0.88)
t.children[0].synth.set(\rgbw, [0.0, 0.5, 1.0, 1.0]);
.children[8].setLag(60.reciprocal)
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

( // Shoot a line
q.stop; q = {
// var a = t.lastSeenData.collect{|e,i|if(e[1] > 0, {i}, {-1})};
var a = ~ids;
  8.do{
  var times = [16, 32, 8, 64].reciprocal.choose * 0.8;
    ~ids.do{
      |e, i|
      // var id = a.wrapAt(i);
      var id = e;
      id.postln;
      if(id >= 0, {
        t.children[id].trigger();
        times.wait;
      })
    };
  };
  "Done".postln;
}.fork;
)

q.stop;
t.children.do{|e|e.synth.set(\noiseMul, 1)}
t.children.do{|e|e.synth.set(\noiseMul, 0)}
t.children[0].
