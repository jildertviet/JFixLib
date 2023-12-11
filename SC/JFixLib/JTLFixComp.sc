JTlFixComp { 
  var <> mainColor;
  var <> f;
  *new{
    |collection|
    ^super.new.init(collection);
  } 
  init{
    |collection|
    f = collection.children;
    mainColor = Color.white;
    // mainColor = Color.fromArray([1.0, 0.8, 0.5, 0.0]);
  }
  hzTicks{
    // Start synths 
    var e = f[0];
    var osc;
    f.postln;
    e.bBroadcast = true;
    osc = JFixEvent_JOsc.new(e)
    .phaseOffset_(1.0.rand)
    .bWaitForEnv_(0)
    .freq_(6.0)
    // .powVal_(rrand(10, 40))
    .rgba_(mainColor.asArray)
    .loc_([0.0, 0.0])
    .size_([1.0, 1.0])
    .range_(0.1);
    // Q = 10
    "tlFixSinePulse.sc".error;
    e.addEvent(osc);
    ^osc;
  }
}
