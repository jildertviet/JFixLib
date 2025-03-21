JMotorController : JFixture { 
  *new{
    |id, addr, serial|
  "Motor controller".postln;
    ^super.new(id, addr, serial);
  }
  // init{}

  move{|relative=10|
    this.send(0xFF!6 ++ 0x35 ++ this.getAddress() ++ [0x01] ++ relative.asFloat.asBytes32 ++ "end");
  }
  moveN{|relative=#[10,0]|
    this.send(0xFF!6 ++ 0x35 ++ this.getAddress() ++ [0x05] ++ relative.asFloat.asBytes32F ++ "end");
  }
  moveTo{|absolute=10|
    this.send(0xFF!6 ++ 0x35 ++ this.getAddress() ++ [0x02] ++ absolute.asFloat.asBytes32 ++ "end");
  }
  moveToN{|relative=#[10,0]|
    this.send(0xFF!6 ++ 0x35 ++ this.getAddress() ++ [0x06] ++ relative.asFloat.asBytes32F ++ "end");
  }
  setAcceleration{|accell=10|
    this.send(0xFF!6 ++ 0x35 ++ this.getAddress() ++ [0x03] ++ accell.asFloat.asBytes32 ++ "end");
  }
  setMaxSpeed{|speed=1.0|
    this.send(0xFF!6 ++ 0x35 ++ this.getAddress() ++ [0x04] ++ speed.asFloat.asBytes32 ++ "end");
  }
}
