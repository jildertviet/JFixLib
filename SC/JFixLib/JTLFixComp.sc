JTlFixComp { 
  var <> mainColor;
  var <> f; // Fixtures
  var <> synths;
  var <> patterns;
  var <> stopFunc;
  *new{
    |collection|
    ^super.new.init(collection);
  } 
  init{
    |collection|
    f = collection.children;
    mainColor = Color.white;
    patterns = List.new();
    // mainColor = Color.fromArray([1.0, 0.8, 0.5, 0.0]);
  }
  hzTicks{
    // Start synths 
    var e = f[0];
    var osc;
    var pattern;
    // f.do{|e, i| e.setViewportOffset([0, 0]);};
    JTLFix.setViewportOffset(f, false);
    e.bBroadcast = true;
    osc = JFixEvent_JOsc.new(e)
    .phaseOffset_(1.0.rand)
    .bWaitForEnv_(0)
    .freq_(6.0)
    .powVal_(10)
    .rgba_(mainColor.asArray)
    .loc_([0.0, 0.0])
    .size_([1.0, 1.0])
    .range_(0.1);
    // "tlFixSinePulse.sc".error;
    pattern = "/home/jildert/Music/supercollider/tlFixSinePulse.scd".load;
    patterns.add(pattern);
    TempoClock.default.schedAbs(TempoClock.default.nextBar, {e.addEvent(osc);});
    stopFunc = {e.deleteEvents};
    // Register pattern and synth
    ^osc;
  }
  stop{
    "Stop".postln;
    patterns.do{|e| e.stop};
    stopFunc.();
    patterns.clear();
  }
  minimal1{
    var pattern;
    JTLFix.setViewportOffset(f);
    f[0].bBroadcast = true;
    ~f = f; // Make global?
    pattern = "/home/jildert/Music/supercollider/tlFixKickStart_v1.scd".load;
    patterns.add(pattern);
    stopFunc = {
      ~compressor.free; 
      ~kickG.free; 
      ~bassG.free; 
      f[0].bBroadcast = false; 
      // f.do{|e, i| e.setViewportOffset([0, 0]);};
      JTLFix.setViewportOffset(f, false);
    };
  }
  minimal2{
    var p;
    ~f = f; 
    JTLFix.setViewportOffset(f);
    f[0].bBroadcast = true;
    p = "/home/jildert/Music/supercollider/aokiTest_v1.sc".load;
    patterns.add(p);
  }
  soundscape{
    var p, osc;
    var update;
    ~f = f; 
    JTLFix.setViewportOffset(f, false);
    // f.do{|e|
      // e.bSyncServer = false;
    // };
    f[0].bBroadcast = true;
    p = "/home/jildert/Music/supercollider/tlFixAmDrone_v1.scd".load;
    patterns.add(p);

    osc = JFixEvent_JOsc.new(f[0])
    .phaseOffset_(0)
    .bWaitForEnv_(1)
    .freq_(0.0)
    .powVal_(30)
    .rgba_([1.0, 1.0, 1.0, 0.0])
    .loc_([0.0, 0.00])
    .size_([1.0, 1.0])
    .range_(0.5);
    
    {
      f[0].start();
      f[0].addEvent(osc);
      osc.doEnv('b', 1000, 900000, 4000, 1.0, bKill: 1); // 900 sec max!!!
      f[0].end();
    }.defer(0.2); // After setViewportOffset

    update = {
      0.3.wait;
      ((32+24)/0.01).do{
        |i|
        var v = sin(i*0.001 + ((0..4)*0.7)) * 0.125 + 1.0;
        osc.setValN('o', v); // setValN N N N
        0.01.wait;
      }
    }.fork;
    patterns.add(update);

    stopFunc = {
      // Release groups 
      ~echo.free;
      ~preEcho.free;
      ~echoBus.free;
      ~scapeEcho.free;
      // f.do{|e|
        // e.bSyncServer = true;
      // };
      f[0].deleteEvents();
    }
  }
  perlin{
    // Sends Event to all fixtures
    f.do{
      |e, i|
      var p = JFixEvent_Perlin.new(e)
      .noiseTimeScale_(0.01)
      .rgba_(1.0!4)
      .bWaitForEnv_(0)
      .horizontalPixelOffset_(30 * i)
      .noiseScale_(0.01)
      .noiseTimeScale_(0.0003);
      e.addEvent(p);
      // e.setViewportOffset([0,0]);
      {p.sync}.defer(1);
    };
    stopFunc = {
      f.do{|e|
        e.deleteEvents();
      }
    }
  }
  klankTickers{
    var p;
    f.do{|e|
      e.bSyncServer = false;
    };
    JTLFix.setViewportOffset(f, true);
    f[0].bBroadcast = true;
    ~f = f;
    p = "/home/jildert/Music/supercollider/tlFixKlankTicker.scd".load;
    patterns.add(p);
    stopFunc = {
      ~x.release;
      ~o.free; ~r.free; // OSCdefs
       f.do{|e|
      e.bSyncServer = true;
    } 

    }
  }
}
