JMotorController : JFixture { 
  *new{
    |id, addr, serial|
  "Motor controller".postln;
    ^super.new().init();
  }
  init{}

  move{|relative=10|
    this.send(0xFF!6 ++ 0x35 ++ this.getAddress() ++ [0x01] ++ relative.asBytes32 ++ "end");
  }
  moveTo{|absolute=10|
    this.send(0xFF!6 ++ 0x35 ++ this.getAddress() ++ [0x01] ++ absolute.asBytes32 ++ "end");
  }
  setAccelleration{|accell=10|
    this.send(0xFF!6 ++ 0x35 ++ this.getAddress() ++ [0x01] ++ accell.asBytes32 ++ "end");
  }
  setMaxSpeed{|speed=1.0|
    this.send(0xFF!6 ++ 0x35 ++ this.getAddress() ++ [0x01] ++ speed.asBytes32 ++ "end");
  }
}
