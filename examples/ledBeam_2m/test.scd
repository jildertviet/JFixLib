p = NetAddr.new("localhost", 7997);
t = JFixtureCollection.new(p);
t.readConfigFile("/home/jildert/JFixLib/examples/ledBeam_2m/ledBeam_2m.config", 1);
c = JTlFixComp.new(t)
// Define red function
t.children[0].send(0xFF!6 ++ [0x09, 0x03, 0x00] ++ "end");
JTLFix

t.children[0].address
Int8Array.newFrom(([0,1]++"n".ascii))
[0,1,'n'.ascii].reshape(3)

p = SerialPort.new("/dev/ttyUSB0", 230400, crtscts: true)
a = [[0x3C,0x71,0xBF,0x10,0x80,0x5D]]; // Green perf board
j = a.collect({|e, i| JTLFix.new(i, e, p)});
0xA5
t.gui
~f = t.children[0].address.asInteger
t.children[0].address = [0x24,0x6F,0x28,0xDD,0x46,0x5D]
t.children[0].address =  [0x24,0x6F,0x28,0xDD,0x48,0x29]
t.children[0].address =  [0x24,0x6F,0x28,0xDD,0x49,0x45]
t.children[0].address =  [0x24,0x6F,0x28,0xDB,0x14,0x79]
t.children[0].address =  [0x24,0x6F,0x28,0xF2,0xB2,0xE5]
t.children[0].address =  [0x24,0x6F,0x28,0xF2,0xB2,0xD1]

t.children.do{|e| e.setBackground([1.0, 1.0, 1,0, 0.0] * 0.0);};

p.sendMsg("/espnow", Int8Array.newFrom([1,2,3]));
[ 36, 111, 40, 242, 178, 210 ] .do{|e|e.asHexString.postln}
93.asHexString
[0x24,0x6F,0x28,0xDD,0x46,0x5D].asArray
(
{
t.children.at([0]).do{|e|
    e.setOTAServer("Klank", ~pw, "https://192.168.10.104/.pio/build/esp32dev/firmware.bin");
    // 35.wait;
  };
  "Done".postln;
}.fork;
)

{inf.do{|i|j.setBrightness(v.wrapAt(i)); 1.0.wait;}}.fork;
j.setRGBW([1, 0, 0, 0]);
j.setBrightness(1.0.rand);
j.setLag("b", 100.0);
(
x.stop;
x = {
  inf.do{
    |i|
    t.children.at([0]).do{|e| e.setBackground([0.1, 0.5, 0.25].wrapAt(i)!4);};
    0.2.wait;
  }
}.fork;
)
[36,111,40,221,72,41].do{|e|e.asHexString.postln}

// t.children.at([0]).do{|e| e.setBackground([1.0, 1.0, 1.0, 0.0] * 0.1);};
t.children.do{|e| e.setBackground([1.0, 1.0, 1,0, 0.0] * 0.9);};

t.children[0].deleteEvents
t.children[0].bBroadcast = true
JTLFix.setViewportOffset(t.children, true);

(// RED
~red = {
    var j = t.children;
var b;
j[0].start;
b = JFixEvent_JRect.new(j[0]).size_([0.5, 1.0]).rgba_([1.0, 0.0, 0.0, 0.0]).bWaitForEnv_(0).loc_([0.0, 0.0]);
j[0].addEvent(b);
b.doEnv('b', 1, 900, 50, bKill: 1);
// b.doEnv('w', 500, 900, 400, 10); // seq on from L to R. 
b.doEnv('w', 1, 1000, 400, 10);
j[0].end;
};
~red.();
)
t.children[0].deleteEvents
s.scope
0xC8
172.asHexString
(
{
8.do{ // RECORD AUDIO
  c.minimal1;
  14.wait;
  c.stop; 
  0.3.wait;
  c.minimal2;
  36.wait; // Compensate for tempo change...
  "Stop".postln;
  c.stop; 0.3.wait;
  TempoClock.default.tempo = 1;
  c.hzTicks;
  10.wait;
  c.stop; 
  1.0.wait;
  c.soundscape;
  30.wait;
  c.stop; 
  0.3.wait;
  c.klankTickers;
  30.wait;
  c.stop; 0.3.wait;
  c.perlin;
  20.wait;
  c.stop; 0.3.wait;
  ~red.();
  5.wait;
  }
}.fork;
)

t.children.do{|e| e.deepSleep(1.0)};
