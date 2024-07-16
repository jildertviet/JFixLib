// Don't use broadcast?
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
x = {inf.do{t.children.do{|j| j.setMaxSpeed([10].choose);j.move([-1, 1].choose * rrand(10, 80))}; 9.wait; }}.fork;
x = {inf.do{|i| t.children.do{|j, h| j.move([-1, 1].wrapAt(i + h) * 30)}; 7.wait; }}.fork;
x.stop
t.children.do{|j| j.moveTo(0)}

j.setMaxSpeed(5.0)
j.setAcceleration(2.0);

j.setBackground([0.1, 0.005, 0.1, 0.0] * 4); // Pink
j.setBackground([0.0, 0.0, 0.1, 0.0] * 4); // Blue
t.children.do({|j| j.setBackground([[0.1, 0.0, 0.0, 0.0], [0.0, 0.0, 0.1, 0.0]].choose * 9)}); // Blue or red

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
var colorIndex = 0;
var color = [[0.1, 0.0, 0.0, 0.0], [0.0, 0.0, 0.1, 0.0]].at(colorIndex) * 0.4;
var p = JFixEvent_JOsc.new(t.children[1]).freq_(0.25).rgba_(color).powVal_(5); // Red
t.children[1].addEvent(p);
)
t.children.do{|e| e.deleteEvents}; // Safer?
j.bBroadcast = true;j.deleteEvents

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



(
////////////////////////////// LED MODES
MIDIdef.noteOn(key:\zero, func:{
  "Led mode 0".postln;
  t.children.do{|e| e.deleteEvents()};
  t.children.do{|e, i|
    var p;
    e.bBroadcast = false;
    p = JFixEvent_Perlin.new(e)
    .size_([1,1])
    // .rgba_(0.4!4)
    .rgba_([0.1, 0.005, 0.1, 0.0] * 8) // Pink
    .bWaitForEnv_(0)
    .horizontalPixelOffset_(10 * i)
    .noiseScale_(0.005)
    .noiseTimeScale_(0.0003);
    e.addEvent(p);
    {p.sync}.defer(1);
  };
}, noteNum:0);
MIDIdef.noteOn(key:\one, func:{
  var colorIndex = 0; // 0: red, 1: blue
  var color = [[0.1, 0.0, 0.0, 0.0], [0.0, 0.0, 0.1, 0.0]].at(colorIndex) * 4;
  var p = JFixEvent_JOsc.new(t.children[1]).freq_(0.25).rgba_(color).powVal_(5); 
  "Led mode 1".postln;
  // Oscillators
  j.bBroadcast = true;
  j.deleteEvents();
  j.addEvent(p);
  j.bBroadcast = false;
}, noteNum:1);
///////////////////////////// GLOBAL FUNCTIONS
MIDIdef.noteOn(key:\two, func:
  {
    "RESET".postln;
    j.bBroadcast = false;
    x.stop;
    t.children.do{|e| e.deepSleep(0.1)};
  }, noteNum:2);
MIDIdef.noteOn(key:\three, func:
  {
    "IDLE".postln;
    x.stop;
    t.children.do{|e| 
      e.deleteEvents();
      e.setBackground([0.0, 0.0, 0.01, 0.0]); // Dim standby color
    };
    x.stop;
  }, noteNum:3);
//////////////////////////// MOTOR MODES
MIDIdef.noteOn(\four, {
  "Motor mode 0".postln;
  x.stop;
  t.children.do{|e| e.setMaxSpeed(5.0); e.setAcceleration(2.0)};
  {
    x = {inf.do{|i| t.children.do{|j, h| j.move([-1, 1].wrapAt(i + h) * 30)}; 7.wait; }}.fork;
  }.defer(1);
}, 4);
MIDIdef.noteOn(\five, {
  "Motor mode 1".postln;
  x.stop;
  t.children.do{|e| e.setMaxSpeed(5.0); e.setAcceleration(4.0)};
  x = {
    inf.do{
      |i|
      t.children.do{
        |j,h| 
        // j.setMaxSpeed([10].choose);
        j.move([-1, 1, 1, -1].wrapAt((i+h)) * rrand(10, 80))
      }; 
      9.wait; 
    }
  }.fork;
}, 5);
///////////////////////// SLIDERS
MIDIdef.cc(\brightness, {
  |val| 
  "Brightness: ".post;
  val = val.linlin(0, 127, 0.0, 1.0);
  t.children.do{|e| e.setBrightness(val)};
}, 0);
)
