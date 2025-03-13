( // 1/4
  ServerOptions
p = NetAddr.new("192.168.1.255", 1111);
t = JFixtureCollection.new(p);
t.readConfigFile("/home/jildert/JFixLib/examples/llllllllllll/llllllllllll.config", 2);
t.children.do{|e| e.bSyncServer = false};
// t.children.do{|e, i| e.bBroadcast = false; e.setViewportOffset([1.0 * i, 0.0]);};

~num = 12;
~r = 0!(t.children.size * 0 + ~num);
// ~perlins = 0!(t.children.size);

~numBussesPerEvent = 6;
~busses = Bus.alloc(\control, s, ~num * ~numBussesPerEvent).set(0.0);
// ~brightnessPerlin = Bus.alloc(\control, s, 3).set(0.5);

~sineMax = Bus.alloc(\control, s, 1).set(0.5);
~sineFreq = Bus.alloc(\control, s, 1).set(0.9);
~sineAdd = Bus.alloc(\control, s, 1).set(0.0);

~env = [1e-2, 1.0, 1.0]; // a, r, level
~lineIndex = 0;
)

j.inspect
( // TEST
j = t.children[3]; // 4 and 2 and 3
j.bBroadcast = true;
j.setBackground([0.7, 0.4, 0.0, 1.0] * 0.5); // Test
)
t.children.at([2,3,4]).do{|e| e.setBackground([0.7, 0.4, 0.0, 0.0] * 0.0)};

t.children[2].bBroadcast = false; t.children[2].deleteEvents
t.children[4].bBroadcast = false; t.children[4].deleteEvents
t.children[3].bBroadcast = false; t.children[3].deleteEvents

( // 2/4
~init = {
  var j = t.children;
  j.do{
    |e, i|
    var r, p;
    var o;
      e.bBroadcast = false;
      e.deleteEvents;
      e.bBroadcast = true;
      // e.start;
      1.do{
        |h|
        var index = (i*1)+h; // 0 til 5
        var color = [Color.red, Color.blue].at(0).toJV / 255.0;
        r = JFixEvent_JRect.new(e)
        .size_([0.4, 1.0])
        .rgba_([0.7, 0.4, 1.0, 1.0])
        // .rgba_([1.0, 0.571, 0.0, 1.0]) // Normalized?
        // .rgba_(color * 1.0)
        // .rgba_([Color.red, Color.blue, Color.green, Color.white, Color.red, Color.yellow].at(index).toJV / 255.0)
        .bWaitForEnv_(0).loc_([0.0, 0.0]);
                // e.bBroadcast = true;
        e.addEvent(r);
        r.linkBus('b', index * 6); // 0, 1, 2 >> 0, 4, 8, [1,2,3] for R, G, B
        r.linkBus('r', 1 + (index * 6)); 
        r.linkBus('g', 2 + (index * 6)); 
        r.linkBus('B', 3 + (index * 6)); 
        r.linkBus('r', 1 + (index * 6)); 
        r.linkBus('g', 2 + (index * 6)); 
        // r.linkBus('y', 0 + (index * 6)); 
        // r.linkBus('h', 0 + (index * 6)); 
        ~r[index] = [r, [
          Bus.newFrom(~busses, (index * 6) + 0, 1),
          Bus.newFrom(~busses, (index * 6) + 1, 1),
          Bus.newFrom(~busses, (index * 6) + 2, 1),
          Bus.newFrom(~busses, (index * 6) + 3, 1),
          Bus.newFrom(~busses, (index * 6) + 4, 1),
          Bus.newFrom(~busses, (index * 6) + 5, 1)
        ]];
      };
    //   e.bBroadcast = false;
    //   p = JFixEvent_Perlin.new(e)
    //     .rgba_(Color.blue.toJV / 255.0)
    //     .bWaitForEnv_(0)
    //     .horizontalPixelOffset_(120 * i)
    //     .noiseScale_(0.003)
    //     .noiseTimeScale_(0.0002);
    //     ~perlins[i] = [p, Bus.newFrom(~busses, (4*6)+i, 1)];
    //     e.addEvent(p);
    //     p.linkBus('b', (6*4)+i); // bRect, R, G, B, bRect, R, etc, bPerlin, bPerlin
    //     p.linkBus('r', (i * 4 * 2) + 1);
    //     p.linkBus('g', (i * 4 * 2) + 2);
    //     p.linkBus('B', (i * 4 * 2) + 3);
    // e.bBroadcast = true;
    };
  };
~init.();
~rBrightnessBusses = ~r.collect({|e| e[1][0]});
// ~pBrightnessBusses = ~perlins.collect({|e| e[1]});

~colorGlobal = {
  |color=#[1.0, 0.0, 0.0]|
  ~r.do{|e| 
    e[1][1].set(color[0]);
    e[1][2].set(color[1]);
    e[1][3].set(color[2]);
  };
};

~doEnv = {
  |a=0.01, r=1, level=0.7, id= -1, single=0| // Single: for single beam: id in that case 0<>6, otherwise 0<>2
  var curve = -4;
  if(id < 0, { // All
    {Out.kr(~rBrightnessBusses, Env.perc(a, r, curve: curve, level: level).kr(gate: 1, doneAction: 2))}.play;
  }, 
  {
    if(single == 0, {
      {Out.kr(~rBrightnessBusses.at(id*2 + [0, 1]), Env.perc(a, r, curve: curve, level: level).kr(gate: 1, doneAction: 2))}.play;
    }, {
      {Out.kr(~rBrightnessBusses.at(id), Env.perc(a, r, curve: curve, level: level).kr(gate: 1, doneAction: 2))}.play;
    });
  });
};
~doEnvExt = {~doEnv.(a: ~env[0], r: ~env[1], level: ~env[2])};
)

( // 3/4 Sine Osc @ brightness
// var phases = 3.collect{rrand(0, 2pi)}.stutter;
// var mul = 3.collect{[1,0.5].choose}.stutter;
"Started oscillators".postln;
~brightnessSynths.free;
~brightnessSynths = {
  ~r.collect{
    |r,i|
    // var out = SinOsc.kr(In.kr(~sineFreq).lag(0.2) * mul[i], phases[i]).pow(2).range(0.1, In.kr(~sineMax).lag(0.5));
    // out = Mix.kr([out, In.kr(~sineAdd).lag(0.1)]);
    var out = SinOsc.ar(0.5).range(0, 1.0);
    Out.kr(r[1][0], out);
  };
}.play;
)

( // 4/4 ROUTINE
var sender = t.children[3];
sender.bBroadcast = true;
g.stop;
g = {
  inf.do{
    |i|
    if(i%30==0, {"Running".postln});
    ~busses.get({|v|
      sender.setParameterBusN(0, v.min(1.0)); 
      // sender.setParameterBusN(0, 0.0!(12*5)); 
    };);
    (1/60).wait;
  }
}.fork;
)

~busses.setn((0.0!(12*6)))

{In.kr(~busses, 27)}.scope
{In.kr(~busses, 5)}.scope
~r.do{|e|e[1][4].set(0.0)}

~colorGlobal.([1.0, 0.0, 0.0])
