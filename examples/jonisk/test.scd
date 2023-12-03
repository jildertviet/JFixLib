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
t.children[0].gui
(
{
j.at([0]).do{|e|
    e.setOTAServer("Klank",  ~pw, "https://192.168.10.164/.pio/build/esp32dev/firmware.bin");
    35.wait;
  };
  "Done".postln;
}.fork;
)
