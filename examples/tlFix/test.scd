p = SerialPort.new("/dev/ttyUSB0", 230400, crtscts: true)
a = [[0x3C,0x71,0xBF,0x10,0x80,0x5D]]; // Green perf board
j = a.collect({|e, i| JTLFix.new(i, e, p)});

t = JFixtureCollection.new(p);
t.readConfigFile("/home/jildert/JFixLib/examples/tlFix/tlFix.config", 1);
t.gui
t.children

(
{
t.children.at([16]).do{|e|
    e.setOTAServer("Klank",  ~pw, "https://192.168.10.164/.pio/build/esp32dev/firmware.bin");
    35.wait;
  };
  "Done".postln;
}.fork;
)
j.deepSleep(1);
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
t.children.do{|e| e.setBackground([1.0, 0.7, 0.4, 0.0] * 0.1);};
t.children.do{|e| e.setBackground([1.0, 0.7, 0.4, 0.0] * 0.0);};

t.children[0].deleteEvents
c = JTlFixComp.new(t)
~osc = c.hzTicks;
~osc.setVal('b',1.0);
~osc.setVal('f', 6.0);
~osc.setValN('o', (0..4) / 4);
~osc.setVal('r', 0.1);
~osc.setVal('q', 10.0);
~osc.setVal('c', [1.0, 0.8, 0.5, 1.0]);


