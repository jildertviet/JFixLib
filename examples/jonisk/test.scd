p = SerialPort.new("/dev/ttyUSB0", 230400, crtscts: true)
a = [[0x3C,0x71,0xBF,0x10,0x80,0x5D]]; // Green perf board
(
a = [[0x34,0x86,0x5d,0x79,0x6f,0xf9]]; // Jonisk 17
j = a.collect({|e, i| JJonisk.new(i, e, p)});
)
t = JFixtureCollection.new(p);
t.readConfigFile("/home/jildert/of_v0.11.2_linux64gcc6_release/apps/TIYCS/jonisk.config", 0);
t.gui;
t.startReadingSerial;
(
{
j.at([0]).do{|e|
    e.setOTAServer("Klank",  ~pw, "https://192.168.10.164/.pio/build/esp32dev/firmware.bin");
    35.wait;
  };
  "Done".postln;
}.fork;
)
0xf9
t.children[17].address
t.children
t.children[17].setRGBW([1.0, 0, 0, 1.0]);
j[0].setRGBW([1.0, 0, 0, 1.0]);
j[0].deepSleep(1);
j[0].setBrightness(0.0);
t.children[17].setBrightness(0.0);
j[0].setLag("b", 100.0);

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
j.do{|e| e.setBackground([1.0, 0.7, 0.4, 0.0] * 0.001);};
j.bBroadcast = false
j.bBroadcast = true
