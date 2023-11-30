"/home/jildert/Music/supercollider/synths/kick.scd".load;
"/home/jildert/Music/supercollider/synths/hihat.scd".load;
~bassBus = Bus.alloc(\audio, s, 2);
~kickBus = Bus.alloc(\audio, s, 2);
~klankBus = Bus.alloc(\audio, s, 2);
~kickG = Group.new();
~bassG = Group.new(~kickG, \addAfter);
TempoClock.default.tempo = 100/120;
Pdefn(\rhythm, Pseq([16, 16/7, 4, 4].reciprocal, inf) * 2).quant_(4);
(
Pdef(\kickStart2,
  Pbind(
    \instrument, \kickSine,
    \freq, (32).midicps,
    \numCycles, 1.5,
    \a, 1e-8,
    \r, 0.05 * Pseq([0.5, 1, 1, 1], inf),
    \delta, Pdefn(\rhythm), 
    \resTickAmp, 0.0,
    \amp, 0.75,
    \group, ~kickG,
   \f, Pfunc({
    var h = 0.1;
    var b;
    b = JFixEvent_JRect.new(j[0]).size_([5.0, h])
    .loc_([0.0,0.25 - (h*0.5)])
    .rgba_([1.0, 0.7, 0.4, 1.0])
    // .rgba_([1.0, 1.0, 1.0, 1.0])
    .bWaitForEnv_(1);
    j[0].start;
    j[0].addEvent(b);
    b.doEnv('b', 1, 1, 200, rrand(0.8,1.0), bKill: 1);
    j[0].end;
    1;
  }),
 \out, [0, ~kickBus]
  )
).quant_([4, -1.25]).play; // Was -3/4
)
s.plotTree
(
SynthDef(\sine,{
  var f = \freq.kr(100);
  var snd = SinOsc.ar(f * Line.ar(\lineStart.kr(2), 1, 0.02), 0).pow(1.01) ! 2;
  var env = Env.adsr(\a.kr(0.01), \d.kr(1), 0.4, \rel.kr(0.1), curve: \curve.kr(0)).ar(gate: \gate.kr(1), doneAction: 2);
  snd = snd + (snd * PinkNoise.ar(0.01));
  snd = snd * env + HPF.ar(Impulse.ar(0), 10000, 0.5);
  Out.ar(\out.kr(0), snd * \amp.kr(0.8));
}).add
)
(
Pbindef(\kickStartBass, 
  \instrument, \sine,
  \octave, 3,
  \ctranspose, -4,
  \scale, Scale.minor.tuning_(\just),
  \degree, Pseq([0,0,0,-1], inf),
  \r, 0.3,
  // \delta, Pdefn(\rhythm) + Place([0, 0, [0, -1/8], [0, 1/8]].rotate(1), inf),
  \delta, Pdefn(\rhythm) + Place([0, 0, [-1/8], [1/8]].rotate(1), inf),
  \dur, Pdefn(\rhythm) * Pseq([0.25, 0.5, 0.75, 0.5], inf),
  \lineStart, 2,
  \a, 0.01,
  \amp, 0.75,
  \d, 0.2,
  \curve, -2,
  \group, ~bassG,
  \out, [~bassBus],
  \f, Pfunc({
    var h = 0.15;
    var envAmp = rrand(0.05, 0.15);
    var b = JFixEvent_JRect.new(j[0]).size_([5.0, h])
    .loc_([0.0,0.5 - (h*0.5)])
    .rgba_([1.0, 0.7, 0.4, 1.0])
    // .rgba_([1.0, 1.0, 1.0, 1.0])
    .bWaitForEnv_(1);
    j[0].start;
    j[0].addEvent(b);
    b.doEnv('b', 10, 1, 500, 1.0, bKill: 1);
    b.doEnv('h', 10, 1, 200, envAmp * 2, bKill: 0);
    b.doEnv('y', 10, 1, 200, envAmp * -1, bKill: 0);
    j[0].end;
    1;
  })
).quant_(4).play;
)

(
c.free; 
  c = {
var a = Compander.ar(in: In.ar(~bassBus, 2), control: In.ar(0), thresh:1/5, slopeBelow:1.0, slopeAbove:0.2, clampTime:0.002, relaxTime:0.05, mul:1.0, add:0.0);
// var b = Compander.ar(in: In.ar(~klankBus, 2), control: In.ar(0), thresh:1/8, slopeBelow:1.0, slopeAbove:0.5, clampTime:0.002, relaxTime:0.08, mul:1.0, add:0.0);
var b = In.ar(~klankBus, 2); // No compression;
Out.ar(\out.kr(0), a + b);
}.play(s, target: ~bassG, addAction: \addAfter);
)
(
Pdef(\hat, Pbind(
	\instrument, \hihat,
	\dur, 1/32, // Nodes exist
	\delta, 1/8,
	\freq, 14000 + Pwhite(0, 100),
	\release, Pseq(([0.5] ++ (0.1!3)).rotate(2), inf) * 0.5,
  \curve, -20,
	\amp, Pseq([1] ++ (0.35!3), inf) * -2.dbamp,
  \pan, Pseq(8.collect{exprand(0.01, 1.0) * 1.negPos}, inf),
  \f, Pfunc({|e|
    var h = 0.1;
    var b;
    if(j == nil, {^1});
    b = JFixEvent_JRect.new(j[0]).size_([0.5, h])
    .loc_([((0..9)/2).wrapAt((e['pan'] + 1) * 5),0.75 - (h*0.5)])
    .rgba_([1.0, 0.7, 0.4, 1.0])
    // .rgba_([1.0, 1.0, 1.0, 1.0])
    .bWaitForEnv_(1);
    j[0].start;
    j[0].addEvent(b);
    b.doEnv('b', 10, 1, 160, e['amp'], bKill: 1);
    j[0].end;
    1;
  })
)).quant_([4,0]).play;
)

s.record;
s.stopRecording
// Compressor?
s.scope
b = Buffer.alloc(s, s.sampleRate * 2);
{RecordBuf.ar(In.ar(0), b, doneAction: 2); 0; }.play(s, addAction: \addToTail);
s.plotTree;
b.plot

s.scope;
s.mute
s.unmute
