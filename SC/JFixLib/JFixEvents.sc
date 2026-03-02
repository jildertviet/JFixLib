// Event types match proto EventType enum:
//   EVENT_PERLIN = 1, EVENT_RECT = 2, EVENT_OSC = 3
JFixEvent{
  var <> loc = #[0, 0];
  var <> size = #[1,1];
  var <> rgba = #[1.0, 1.0, 1.0, 1.0];
  var <>parent = nil;
  var <>bWaitForEnv = 0;
  var <>id;

  *new{
    ^super.new.setID();
  }
  setID{
    id = this.getEventID;
  }
  getConstMsg{
    "getConstMsg not implemented".warning;
  }
  checkParent{
    if(parent != nil, {^true}, {
      "Event has no parent JFixture".error;
      ^false
    });
  }
  getEventID{
    if(~jFixLibEventCounter == nil, {~jFixLibEventCounter = 0}, {~jFixLibEventCounter = ~jFixLibEventCounter + 1});
    ^~jFixLibEventCounter;
  }

  // Tag 14: AddEnvCmd { event_id, var, attack_ms, sustain_ms, release_ms, target, kill }.
  // type: Symbol like \b, \x, \y — converted to ASCII int.
  doEnv{ |type='b', aTime=1000, sTime=1000, dTime=1000, brightness = 1.0, bKill=1|
    parent.send(JPb.command(parent.getCommandId(), 14,
      JPb.int32(1, id) ++
      JPb.int32(2, type.ascii[0]) ++
      JPb.int32(3, aTime.asInteger) ++
      JPb.int32(4, sTime.asInteger) ++
      JPb.int32(5, dTime.asInteger) ++
      JPb.float32(6, brightness) ++
      JPb.bool(7, bKill != 0)
    ));
  }

  // Tag 15: SetValCmd { event_id, var, value }.
  // For var='c' (full color): pass value as [r, g, b, a] array.
  setVal{ |type='b', value=1.0|
    var payload = JPb.int32(1, id) ++ JPb.int32(2, type.ascii[0]);
    if(value.isArray, {
      // var='c': set full rgba
      payload = payload ++
        JPb.float32(4, value[0]) ++
        JPb.float32(5, value[1]) ++
        JPb.float32(6, value[2]) ++
        JPb.float32(7, value[3]);
    }, {
      payload = payload ++ JPb.float32(3, value);
    });
    parent.send(JPb.command(parent.getCommandId(), 15, payload));
  }

  // Tag 16: SetValNCmd { event_id, var, values[] }.
  setValN{ |type='b', values=#[0]|
    parent.send(JPb.command(parent.getCommandId(), 16,
      JPb.int32(1, id) ++
      JPb.int32(2, type.ascii[0]) ++
      JPb.repeatedFloat(3, values)
    ));
  }

  // Tag 18: LinkBusCmd { event_id, var, bus_index }.
  linkBus{ |type='b', busIndex=0|
    parent.send(JPb.command(parent.getCommandId(), 18,
      JPb.int32(1, id) ++
      JPb.int32(2, type.ascii[0]) ++
      JPb.int32(3, busIndex)
    ));
  }

  // Tag 17: SetCustomArgCmd { event_id, arg_id, value }.
  setCustomArg{ |argID_, val_|
    parent.send(JPb.command(parent.getCommandId(), 17,
      JPb.int32(1, id) ++
      JPb.int32(2, argID_) ++
      JPb.float32(3, val_.asFloat)
    ));
  }

  // Tag 11: SyncCmd { event_id }.
  sync{
    parent.send(JPb.command(parent.getCommandId(), 11,
      JPb.int32(1, id)
    ));
  }
}

// ── Perlin noise event ────────────────────────────────────────────────────────
// Type-specific params set via SetCustomArgCmd after add_event:
//   arg_id 0 = noiseScale, 1 = noiseTimeScale, 2 = horizPixelOffset
JFixEvent_Perlin : JFixEvent{
  var <>noiseScale = 0.01;
  var <>noiseTimeScale = 0.0005;
  var <>horizontalPixelDistance = 10;
  var <>horizontalPixelOffset = 0;

  *new{
    |parent_ = nil, noiseScale_ = 0.01, noiseTimeScale_ = 0.0005, horizontalPixelDistance_ = 10, horizontalPixelOffset_ = 0, loc_ = #[0,0], size_ = #[1,1], rgba_ = #[1.0, 1.0, 1.0, 1.0]|
    ^super.new.init(parent_, noiseScale_, noiseTimeScale_, horizontalPixelDistance_, horizontalPixelOffset_, loc_, size_, rgba_);
  }
  init{
    |parent_, noiseScale_, noiseTimeScale_, horizontalPixelDistance_, horizontalPixelOffset_, loc_, size_, rgba_|
    parent = parent_;
    noiseScale = noiseScale_;
    noiseTimeScale = noiseTimeScale_;
    horizontalPixelDistance = horizontalPixelDistance_;
    horizontalPixelOffset = horizontalPixelOffset_;
    loc = loc_;
    size = size_;
    rgba = rgba_;
  }

  // Tag 13: AddEventCmd. Type-specific args follow via SetCustomArgCmd.
  getConstMsg{
    if(this.checkParent() == true, {
      var msg = JPb.command(parent.getCommandId(), 13,
        JPb.int32(1, id) ++
        JPb.int32(2, 1) ++   // EVENT_PERLIN = 1
        JPb.float32(3, loc[0]) ++
        JPb.float32(4, loc[1]) ++
        JPb.float32(5, size[0]) ++
        JPb.float32(6, size[1]) ++
        JPb.float32(7, rgba[0]) ++
        JPb.float32(8, rgba[1]) ++
        JPb.float32(9, rgba[2]) ++
        JPb.float32(10, rgba[3]) ++
        JPb.bool(11, bWaitForEnv != 0)
      );
      // Follow up with type-specific custom args
      parent.send(msg);
      this.setCustomArg(0, noiseScale.asFloat);
      this.setCustomArg(1, noiseTimeScale.asFloat);
      this.setCustomArg(2, horizontalPixelOffset.asFloat);
      ^msg;
    }, {
      ^[];
    });
  }

  setNoiseScale{ |val| this.setCustomArg(0, val.asFloat); noiseScale = val }
  setNoiseTimeScale{ |val| this.setCustomArg(1, val.asFloat); noiseTimeScale = val }
  setHorizontalPixelOffset{ |val| this.setCustomArg(2, val.asFloat); horizontalPixelOffset = val }
}

// ── Rectangle event ───────────────────────────────────────────────────────────
// bInvertHeight controlled via SetCustomArgCmd arg_id=0.
JFixEvent_JRect : JFixEvent{
  var <> bInvertHeight = 0;

  *new{
    |parent_ = nil|
    ^super.new.init(parent_);
  }
  init{ |parent_| parent = parent_; }

  // Tag 13: AddEventCmd { type=EVENT_RECT }.
  getConstMsg{
    if(this.checkParent() == true, {
      var msg = JPb.command(parent.getCommandId(), 13,
        JPb.int32(1, id) ++
        JPb.int32(2, 2) ++   // EVENT_RECT = 2
        JPb.float32(3, loc[0]) ++
        JPb.float32(4, loc[1]) ++
        JPb.float32(5, size[0]) ++
        JPb.float32(6, size[1]) ++
        JPb.float32(7, rgba[0]) ++
        JPb.float32(8, rgba[1]) ++
        JPb.float32(9, rgba[2]) ++
        JPb.float32(10, rgba[3]) ++
        JPb.bool(11, bWaitForEnv != 0)
      );
      parent.send(msg);
      ^msg;
    }, {
      ^[];
    });
  }

  invertHeight{
    bInvertHeight = 1 - bInvertHeight;
    this.setCustomArg(0, bInvertHeight.asFloat);
  }
}

// ── Oscillator event ──────────────────────────────────────────────────────────
// Type-specific params set via SetValCmd after add_event:
//   var='f' = freq, var='R' = range, var='o' = phaseOffset, var='q' = powVal
JFixEvent_JOsc : JFixEvent{
  var <> freq = 1;
  var <> phaseOffset = 0;
  var <> range = 1;
  var <> powVal = 10;

  *new{
    |parent_ = nil|
    ^super.new.init(parent_);
  }
  init{ |parent_| parent = parent_; }

  // Tag 13: AddEventCmd { type=EVENT_OSC }, followed by SetValCmd for osc params.
  getConstMsg{
    if(this.checkParent() == true, {
      var msg = JPb.command(parent.getCommandId(), 13,
        JPb.int32(1, id) ++
        JPb.int32(2, 3) ++   // EVENT_OSC = 3
        JPb.float32(3, loc[0]) ++
        JPb.float32(4, loc[1]) ++
        JPb.float32(5, size[0]) ++
        JPb.float32(6, size[1]) ++
        JPb.float32(7, rgba[0]) ++
        JPb.float32(8, rgba[1]) ++
        JPb.float32(9, rgba[2]) ++
        JPb.float32(10, rgba[3]) ++
        JPb.bool(11, bWaitForEnv != 0)
      );
      // Follow up with osc-specific params
      parent.send(msg);
      this.setVal('f', freq.asFloat);
      this.setVal('R', range.asFloat);
      this.setVal('o', phaseOffset.asFloat);
      this.setVal('q', powVal.asFloat);
      ^msg;
    }, {
      ^[];
    });
  }
}
