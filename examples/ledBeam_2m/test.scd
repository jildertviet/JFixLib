p = NetAddr.new("localhost", 7997);
t = JFixtureCollection.new(p);
t.readConfigFile("/home/jildert/JFixLib/examples/ledBeam_2m/ledBeam_2m.config", 1);
c = JTlFixComp.new(t)
// Define red function

t.children[1].writeID()
var brightness = 0.5; t.children[0].setBackground([1.0, 0.0, 0.0, 1.0] * brightness);
var brightness = 0.0; t.children[18].setBackground([1.0, 0.0, 0.0, 1.0] * brightness);

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

p.sendMsg("/espnow", Int8Array.newFrom([1,2,3]));
[ 36, 111, 40, 242, 178, 210 ] .do{|e|e.asHexString.postln}
93.asHexString
[0x24,0x6F,0x28,0xDD,0x46,0x5D].asArray

t.children[11].reboot;
(
{
t.children.at([11]).do{|e|
    e.setOTAServer("http://192.168.10.21/ledBeam_2m.bin");
    // e.setWifi("Klank", ~pw); 
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
    var j = t.children.choose;
var b;
j.start;
b = JFixEvent_JRect.new(j).size_([0.5, 0.1]).rgba_([1.0, 0.0, 0.0, 0.0]).bWaitForEnv_(0).loc_([0.0, 0.0]);
j.addEvent(b);
b.doEnv('b', 1, 90, 1000, 1.0, bKill: 1);
// b.doEnv('w', 500, 900, 400, 10); // seq on from L to R. 
b.doEnv('h', 300, 10, 400, 1.0, bKill: 1);
j.end;
};
~red.();
)
t.children[0].deleteEvents
s.scope


t.children.do{|e| e.deepSleep(1.0)};
