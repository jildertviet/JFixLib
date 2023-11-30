JTLFix : JFixtureAddr{
  *new{
    |id, addr, serial|
    ^super.new(id, addr, serial);
  }
  *initDefault{
    var j, a;
    var p = SerialPort.new("/dev/ttyUSB0", 230400, crtscts: true);
    a = [
      [0xC8,0xF0,0x9E,0xCF,0x94,0xAD], // WS2816B tlFix
      [0xC8,0xF0,0x9E,0xD1,0x1D,0xC9], // Second tlFix
      [0xC8,0xF0,0x9E,0xD1,0x1D,0x5D], // 3rd
      [0xC8,0xF0,0x9E,0xD1,0x1D,0xAD], 
      [0xC8,0xF0,0x9E,0xD1,0x1E,0x55]
    ];
    j = a.collect({|e, i| JTLFix.new(i, e, p)});
    ^j;
  }
  * setViewportOffset{
    |tlFixtures = #[]|
    tlFixtures.do{
      |e, i|
      e.setViewportOffset([1.0 * i,0]);
    }
  }
}

