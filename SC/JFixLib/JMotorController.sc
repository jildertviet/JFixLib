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
  moveTo{|absolute=10|
    this.send(0xFF!6 ++ 0x35 ++ this.getAddress() ++ [0x02] ++ absolute.asFloat.asBytes32 ++ "end");
  }
  setAcceleration{|accell=10|
    this.send(0xFF!6 ++ 0x35 ++ this.getAddress() ++ [0x03] ++ accell.asFloat.asBytes32 ++ "end");
  }
  setMaxSpeed{|speed=1.0|
    this.send(0xFF!6 ++ 0x35 ++ this.getAddress() ++ [0x04] ++ speed.asFloat.asBytes32 ++ "end");
  }
}
