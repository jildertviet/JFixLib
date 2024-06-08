p = NetAddr.new("localhost", 7997);
p = NetAddr.new("192.168.1.255", 1111); // Ethernet
t = JFixtureCollection.new(p);
t.readConfigFile("/home/jildert/JFixLib/examples/llllllllllll/llllllllllll.config", 2);
j = t.children[1]
j.bBroadcast = true;
j.motor.bBroadcast = true; // Confusing...
j.move([1,-1].choose*9);
x = {inf.do{j.move([1,-1].choose*9);3.wait;}}.fork;

j.setMaxSpeed(9.0)

j.setBackground([0.1, 0.0, 0.1, 0.0] * 0);
t.children.do{|j,i| j.writeID(i+1)};
j.writeID(0x02);
j.address

(
  var p = JFixEvent_Perlin.new(t.children[1])
  .noiseTimeScale_(0.01)
  .size_([2,4])
  .rgba_(0.1!4)
  .bWaitForEnv_(0)
  .horizontalPixelOffset_(30 * 1)
  .noiseScale_(0.01)
  .noiseTimeScale_(0.0003);
  t.children[1].addEvent(p);
)
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
~f = t.children
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
t.children.do{|e| e.setBackground([1.0, 1.0, 1,0, 0.0] * 0.0);};

t.children[0].deleteEvents
t.children[0].bBroadcast = true
JTLFix.setViewportOffset(t.children, true);

(// RED
~red = {
    var j = t.children;
var b;
j[1].start;
b = JFixEvent_JRect.new(j[1]).size_([0.5, 1.0]).rgba_([1.0, 0.0, 0.0, 0.0]).bWaitForEnv_(0).loc_([0.0, 0.0]);
j[1].addEvent(b);
b.doEnv('b', 1, 900, 50, bKill: 1);
// b.doEnv('w', 500, 900, 400, 10); // seq on from L to R. 
b.doEnv('w', 1, 1000, 400, 10);
j[1].end;
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



