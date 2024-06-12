p = NetAddr.new("localhost", 7997);
p = NetAddr.new("192.168.1.255", 1111); // Ethernet
t = JFixtureCollection.new(p);
t.readConfigFile("/home/jildert/JFixLib/examples/llllllllllll/llllllllllll.config", 2);
j = t.children[1]
j.bBroadcast = true;
j.bBroadcast = false;
j.motor.bBroadcast = true; // Confusing...
j.motor.bBroadcast = false; // Confusing...
j.move([1,-1].choose*9);
x = {inf.do{j.move([1,-1].choose*34);5.wait;}}.fork;
x.stop
x = {inf.do{t.children.do{|j| j.setMaxSpeed([10].choose);j.move([-1, 1].choose * rrand(10, 80))}; 9.wait; }}.fork;
x = {inf.do{|i| t.children.do{|j, h| j.move([-1, 1].wrapAt(i + h) * 30)}; 7.wait; }}.fork;
x.stop
t.children.do{|j| j.moveTo(0)}

j.setMaxSpeed(15.0)
j.setAcceleration(2.0);

j.setBackground([0.1, 0.0, 0.1, 0.0] * 0);
t.children.do({|j| j.setBackground([[0.1, 0.0, 0.0, 0.0], [0.0, 0.0, 0.1, 0.0]].choose * 9)});
t.children[0].setBackground([0.1, 0.0, 0.0, 0.0]);
t.children[1].move(-10);
t.children.do{|j,i| j.writeID(i+1)};
j.writeID(0x02);
j.address
t.children.do{|j, i|j.bBroadcast = false; j.setViewportOffset([i * 1.0, 0])};
(
t.children.do{|j, i|
  var p;
  j.bBroadcast = false;
  p = JFixEvent_Perlin.new(j)
  // .noiseTimeScale_(0.01)
  .size_([1,1])
  // .rgba_(0.4!4)
  .rgba_([0.1, 0.005, 0.1, 0.0] * 8)
  .bWaitForEnv_(0)
  .horizontalPixelOffset_(30 * i)
  .noiseScale_(0.005)
  .noiseTimeScale_(0.0003);
  j.addEvent(p);
  {p.sync}.defer(1);
};
)
(
var p = JFixEvent_JOsc.new(t.children[1]).freq_(0.15).rgba_([0.1, 0.0, 0.0, 0.0] * 4);
// var p = JFixEvent_JOsc.new(t.children[1]).freq_(0.1).rgba_([0.0, 0.0, 0.1, 0.0] * 4);
t.children[1].addEvent(p);
)
j.bBroadcast = true;j.deleteEvents
c = JTlFixComp.new(t)
c.perlin
c.stop
"1223.123.123.4444".split($.)[3]
NetAddr
// Define red function
d = Dictionary.newFrom(["a", 1]);
d["a"]
SerialPort.new("/dev/ttyUSB0", 230400, crtscts: true)
a = [[0x3C,0x71,0xBF,0x10,0x80,0x5D]]; // Green perf board
j = a.collect({|e, i| JTLFix.new(i, e, p)});

t.gui
("{}".asArray ++ [0x00]).asArray[2]
[0x00] ++ "End"
"End" ++ [0x00]
~f = t.children
e = t.children[1];
e.setOTAServer("Klank", ~pw, "https://192.168.10.249/.pio/build/esp32dev/firmware.bin");
e.address ++ Int8Array.newFrom([0x00]);
(
{
t.children.at([0]).do{|e|
    e.setOTAServer("Klank", ~pw, "https://192.168.10.55/.pio/build/esp32dev/firmware.bin");
    35.wait;
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
    [j].do{|e| e.setBackground([0.1, 0.5, 0.25].wrapAt(i)!4);};
    0.2.wait;
  }
}.fork;
)

// t.children.at([0]).do{|e| e.setBackground([1.0, 1.0, 1.0, 0.0] * 0.1);};
t.children.do{|e| e.setBackground([1.0, 1.0, 1,0, 0.0] * 0.1);};

t.children[1].deleteEvents
t.children[0].bBroadcast = true
JTLFix.setViewportOffset(t.children, true);

(// RED
~red = {
    var j = t.children;
var b;
// j[1].start;
b = JFixEvent_JRect.new(j[1]).size_([0.5, 1.0]).rgba_([1.0, 0.0, 0.0, 0.0]).bWaitForEnv_(0).loc_([0.0, 0.0]);
j[1].addEvent(b);
b.doEnv('b', 1, 900, 50, bKill: 1);
// b.doEnv('w', 500, 900, 400, 10); // seq on from L to R. 
// b.doEnv('w', 1, 1000, 400, 10);
// j[1].end;
};
~red.();
)
t.children[0].deleteEvents
s.scope

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



