JTlFixComp { 
  var <> mainColor;
  var <> f;
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
    f.postln;
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
    e.addEvent(osc);
    stopFunc = {e.deleteEvents};
    // Register pattern and synth
    ^osc;
  }
  stop{
    patterns.do{|e| e.stop};
    stopFunc.();
    patterns.clear();
  }
  minimal1{

  }
}
