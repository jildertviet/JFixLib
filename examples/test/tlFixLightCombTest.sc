j = JTLFix.initDefault;
JTLFix.setViewportOffset(j);

j[0].bBroadcast = true
(
~doComb = {
  5.do{
    |index|
    var e = j[0];
    var toFlip = [0,1].choose;
    2.do{
      |i|
      b = JFixEvent_JRect.new(e)
      .size_([0.25, 0.0])
      .loc_([index + (i * 0.5),0.5])
      .rgba_([1.0, 1.0, 1.0, 1.0])
      .bWaitForEnv_(1);
      e.start;
      e.addEvent(b);
      // b.doEnv('b', 20, 1, 1000, 1.0, bKill: 1);
      b.doEnv('h', 0, 0, 400, 0.5, bKill: 1);
      if(i == toFlip, {
        b.invertHeight();
        // b.doEnv('y', 1000, 0, 0, 0.47);
      });
      e.end;
  };
}
};
~doComb.();
~preComb = {
  var e = j[0];
  b = JFixEvent_JRect.new(e)
  .size_([5.0, 0.1])
  .loc_([0.0,0.5 - 0.05])
  .rgba_([1.0, 1.0, 1.0, 1.0])
  .bWaitForEnv_(1);
  e.start;
  e.addEvent(b);
  b.doEnv('b', 20, 1, 100, 1.0, bKill: 1);
  e.end;
};
)
~preComb.();
j[0].deleteEvents
t.stop; t = {inf.do{~doComb.(); 3.wait;}}.fork;
t.stop;
(
~linesH = {
  |yStart = 0.3, brightness = 0.8|
  var num = 1;
  var spacing = rrand(0.04, 0.06) + 0.1;
  num.do{
  |i|
  var e = j[0];
  var h = 0.1;
    var b = JFixEvent_JRect.new(e)
    .size_([5.0, h])
    .loc_([0.0, yStart + (spacing * i) - (h*0.5) - ((num-1) * spacing * 0.5)])
    .bWaitForEnv_(1)
    .rgba_(1.0!4);
    e.start;
    e.addEvent(b);
    b.doEnv('b', 1, 200, 2, brightness, bKill: 1);
    e.end();
  };
}
// ~linesH = {};
)

(
~kickLight = {
  |a=0.7|
  var e = j[0];
  var h = 1.0;
  var b;
  if(a.asFloat >= 0.5, {
    b = JFixEvent_JRect.new(e)
    .size_([0.75, h])
    .loc_([5.rand.asFloat, 0.0])
    .bWaitForEnv_(1)
    .rgba_(1.0!4);
    e.start;
    e.addEvent(b);
    b.doEnv('b', 10, 1, 400, 0.2, bKill: 1);
    e.end();
  });
};
)
~kickLight.();
