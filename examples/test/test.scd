p = SerialPort.new("/dev/ttyUSB1", 230400, crtscts: true)
a = [0x3C,0x71,0xBF,0x10,0x80,0x5D];
j = JJonisk.new(0, a, p);
j.id
j.address
// j.id = 1
// j.inspect
j.setOTAServer("X", "Y", "");

x = Jonisk.new()
x.inspect
