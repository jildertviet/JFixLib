p = SerialPort.new("/dev/ttyUSB0", 230400, crtscts: true)
a = [0x3C,0x71,0xBF,0x10,0x80,0x5D]; // Green perf board
a = [0x24, 0x6F, 0x28, 0xDD, 0x01, 0xB1];
(
a = [
  [0xC8,0xF0,0x9E,0xCF,0x94,0xAD], // WS2816B tlFix
  [0xC8,0xF0,0x9E,0xD1,0x1D,0xC9], // Second tlFix
  [0xC8,0xF0,0x9E,0xD1,0x1D,0x5D], // 3rd
  [0xC8,0xF0,0x9E,0xD1,0x1D,0xAD], 
  [0xC8,0xF0,0x9E,0xD1,0x1E,0x55]
];
j = a.collect({|e, i| JTLFix.new(i, e, p)});
// j = JTLFix.new(0, a, p);
// h = JTLFix.new(0, t, p);
// k = JTLFix.new(0, q, p); // 3rd
)

j.id
j.address
(
{
  j.do{|e|
    e.setOTAServer("Klank",  ~pw, "https://192.168.10.173/.pio/build/esp32dev/firmware.bin");
    50.wait;
  };
  "Done".postln;
}.fork;
)

j.deepSleep(1);
v = [1, 0, 0.1]
v = [0.1, 0.5, 1.0, 0.5].stutter(2).scramble
{inf.do{|i|j.setBrightness(v.wrapAt(i)); 1.0.wait;}}.fork;
j.setRGBW([1, 0, 0, 0]);
j.setBrightness(1.0.rand);
j.setLag("b", 100.0);
(
var val = 0.01;
)
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
[j].do{|e| e.setBackground([1.0, 0.7, 0.4, 0.0] * 0.001);};
j.bBroadcast = false
j.bBroadcast = true
j.getAddress()
if(~test == nil, {~test = 0
  j.getEventID
1.asInteger.asBytes32
1.0.asBytes32
256.asBytes32
Integer
