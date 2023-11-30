JFixEvent{
  var <> loc = #[0, 0];
  var <> size = #[1,1];
  var <> rgba = #[1.0, 1.0, 1.0, 1.0];
  var <> type = #[
   \perlin, 0x01,
    \jrect, 0x02,
    \josc, 0x03
  ];
  var <>parent = nil;
  var <>bWaitForEnv = 0;
  var <>id;
  *new{
    // this.type = Dictionary.newFrom(this.type);
    // 'a'.loctln;
    ^super.new.setDictionary();
  }
  setDictionary{
    id = this.getEventID;
    type = Dictionary.newFrom(aCollection:(type));
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
  doEnv{
    |type='b', aTime=1000, sTime=1000, dTime=1000, brightness = 1.0, bKill=1|
    parent.send(0xFF!6 ++ [0x24] ++ parent.getAddress() ++ [aTime, sTime, dTime, brightness].asBytes32F ++ [bKill, type.ascii[0]] ++ id.asBytes32 ++ "end");
  }
  setVal{
    |type='b', value=1.0|
    var valueArray;
    if(value.isArray, {
      valueArray = value.asBytes32F;
    }, {
      valueArray = value.asBytes32;
    });
    parent.send(0xFF!6 ++ [0x28] ++ parent.getAddress() ++ id.asBytes32 ++ type.ascii[0] ++ valueArray ++ "end");
  }
  setValN{
    |type='b', values=#[0]|
    parent.send(0xFF!6 ++ [0x29] ++ parent.getAddress() ++ id.asBytes32 ++ type.ascii[0] ++ values.asBytes32F ++ "end");
  }
  setCustomArg{
    |argID_, val_|
    parent.send(0xFF!6 ++ [0x31] ++ parent.getAddress() ++ id.asBytes32 ++ [argID_, val_].asBytes32F ++ "end"); // Dummy 0x00's, why!?
  }
  sync{
    parent.send(0xFF!6 ++ [0x32] ++ parent.getAddress() ++ id.asBytes32 ++ "end"); 
  }
}

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
  getConstMsg{
    if(this.checkParent() == true, {
      ^(0xFF!6 ++ [0x23] ++ parent.getAddress() ++ [type[\perlin]] ++ id.asBytes32 ++ loc.asBytes32F ++ size.asBytes32F ++ rgba.asBytes32F ++ [bWaitForEnv] ++ [noiseScale, noiseTimeScale, horizontalPixelOffset].asBytes32F ++ "end");
    }, {
      ^[];
    });
  }
  setNoiseScale{ |val| this.setCustomArg(0, val.asFloat); noiseScale = val}
  setNoiseTimeScale{ |val| this.setCustomArg(1, val.asFloat); noiseTimeScale = val}
  setHorizontalPixelOffset{ |val| this.setCustomArg(2, val.asFloat); horizontalPixelOffset = val}
}

JFixEvent_JRect : JFixEvent{
  var <> bInvertHeight = 0;
  *new{
    |parent_ = nil|
    ^super.new.init(parent_);
  }
  init{
    |parent_|
    parent = parent_;
  } 
  getConstMsg{
    if(this.checkParent() == true, {
      ^(0xFF!6 ++ [0x23] ++ parent.getAddress() ++ [type[\jrect]] ++ id.asBytes32 ++ loc.asBytes32F ++ size.asBytes32F ++ rgba.asBytes32F ++ [bWaitForEnv] ++"end");
    }, {
      ^[];
    });
  }
  invertHeight{
    if(bInvertHeight == 0, {
      bInvertHeight = 1;
    }, {
      bInvertHeight = 0;
    });
    this.setCustomArg(0, bInvertHeight.asFloat);
  }

}

JFixEvent_JOsc : JFixEvent{
  var <> freq = 1;
  var <> phaseOffset = 0;
  var <> range = 1;
  var <> powVal = 10;
  *new{
    |parent_ = nil|
    ^super.new.init(parent_);
  }
  init{
    |parent_|
    parent = parent_;
  } 
  getConstMsg{
    if(this.checkParent() == true, {
      ^(0xFF!6 ++ [0x23] ++ parent.getAddress() ++ [type[\josc]] ++ id.asBytes32 ++ loc.asBytes32F ++ size.asBytes32F ++ rgba.asBytes32F ++ [freq, phaseOffset, range, powVal].asBytes32F ++ [bWaitForEnv] ++"end");
    }, {
      ^[];
    });
  }
}
