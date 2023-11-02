p = SerialPort.new("/dev/ttyUSB0", 230400, crtscts: true)
a = [0x3C,0x71,0xBF,0x10,0x80,0x5D]; // Green perf board
a = [0x24, 0x6F, 0x28, 0xDD, 0x01, 0xB1];
a = [0xC8,0xF0,0x9E,0xCF,0x94,0xAD]; // WS2816B tlFix
// j = JJonisk.new(0, a, p);

j = JTLFix.new(0, a, p);
j.id
j.address
j.setOTAServer("Klank",  ~pw, "https://192.168.10.173/.pio/build/esp32dev/firmware.bin");
j.writeID(2)
j.deepSleep(1);
v = [1, 0, 0.1]
v = [0.1, 0.5, 1.0, 0.5].stutter(2).scramble
{inf.do{|i|j.setBrightness(v.wrapAt(i)); 1.0.wait;}}.fork;
j.setRGBW([1, 0, 0, 0]);
j.setBrightness(1.0.rand);
j.setLag("b", 100.0);
