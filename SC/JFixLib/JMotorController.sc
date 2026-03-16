// Tag 6: MotorCmd { steps (int32), speed (float), relative (bool) }
// speed = 0.0 means "use firmware default".
JMotorController : JFixture {
  *new{
    |id, addr, serial|
    "Motor controller".postln;
    ^super.new(id, addr, serial);
  }

  // Relative move by N steps.
  move{ |steps = 10, speed = 0.0|
    this.send(JPb.command(this.getCommandId(), 6,
      JPb.int32(1, steps.asInteger) ++
      JPb.float32(2, speed) ++
      JPb.bool(3, true)
    ));
  }

  // Absolute move to position.
  moveTo{ |position = 0, speed = 0.0|
    this.send(JPb.command(this.getCommandId(), 6,
      JPb.int32(1, position.asInteger) ++
      JPb.float32(2, speed) ++
      JPb.bool(3, false)
    ));
  }

  // Broadcast relative move — all devices receive the same step count.
  // For per-device values, send individual move commands instead.
  moveN{ |steps = 10, speed = 0.0|
    var saved = bBroadcast;
    bBroadcast = true;
    this.move(steps, speed);
    bBroadcast = saved;
  }

  // Per-device absolute moves: send unicast MotorCmd to each device ID 0..positions.size-1.
  moveToN{ |positions = #[0, 0], speed = 0.0|
    positions.doWithIndex { |pos, i|
      this.sendRaw(JPb.command(i, 6,
        JPb.int32(1, pos.asInteger) ++
        JPb.float32(2, speed) ++
        JPb.bool(3, false)
      ));
    };
  }

  // setAcceleration and setMaxSpeed have no equivalent in v2 MotorCmd.
  // Pass speed directly in move/moveTo instead.
  setAcceleration{ |accel = 10|
    "setAcceleration: not in v2 ProtoBuf schema — pass speed in move/moveTo".warn;
  }
  setMaxSpeed{ |speed = 1.0|
    "setMaxSpeed: not in v2 ProtoBuf schema — pass speed in move/moveTo".warn;
  }
}
