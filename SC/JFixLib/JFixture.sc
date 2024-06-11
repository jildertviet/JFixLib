JFixtureSynthController : JModes{
  var <> color; // use Color instead of array (as in ESP32)
  var <> brightness = 1.0; // Amp of synth
  var <> brightnessAdd = 0.0; // DC added to output
  var <> lagTime = 0; // Lag time in synth, not to be confused with lagTime in ESP32
  var <> bus = nil;
  var <> asr;
  var <> mode = "static"; // [static, st_rgbw, st_brightness]
  var <> synth = nil;
  var <> guiDict;
  var <> bSyncServer = true;
  var <> espnowBridge;
  var <> broadcaster;
  setAttack{|v| asr[0] = v; synth.set(\a, v);}
  setSustain{|v| asr[1] = v; synth.set(\s, v);}
  setRelease{|v| asr[2] = v; synth.set(\r, v);}
  trigger{ synth.set(\gate, 1.0.rand);}
}

JFixture : JFixtureSynthController{
	var <>address = #[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF];
  var <>id;
  var <>serial;
  var <>bBroadcast = false;
  var <>bCollectMsgs = false;
  var <> msgList;
  *new{
    |id, addr, serial|
    ^super.new.init(id, addr, serial);
  }
  init{
    |id, addr, serial|
    this.id = id;
    this.address = addr;
    if(serial.class == SerialPort,{
      "Use SerialPort".postln;
      this.serial = serial;
    },{
      if(serial.class == NetAddr, {
        if(serial.hostname.split($.)[3] == "255", {
          this.broadcaster = serial;
          NetAddr.broadcastFlag = true;
          "Use broadcast UDP".postln;
        }, {
          "Use ESPNOW-bridge".postln;
          this.espnowBridge = serial;
        });
      });
    });
    msgList = List.new;
    color = Color.fromArray(0!4);
    asr = [0.1, 1.0, 1.0];
  }
  getAddress{
    if(bBroadcast == true, {
      ^0xFF!6;
    },{
      ^this.address;
    });
  }
  getAddressHexString{
    ^(this.address.collect({|e| e.asHexString.at([6,7])}).collect({|e| "0x" ++ e[0] ++ e[1]}).asString.replace("[", "").replace("]", ""));
  }
  send{
		|msg, bMute = false|
    if(bMute == false, {msg.postln});
    broadcaster.postln;
		if((serial != nil).or(espnowBridge != nil).or(broadcaster != nil), {
      if(bCollectMsgs, {
        // "Save msg".postln;
        msgList.add(msg);
      }, {
        if((mode == "static").and(bSyncServer == true), {
          {this.sendRaw(msg);}.defer(Server.default.latency);
        },{
         this.sendRaw(msg); 
        });
      });
		});
	}
  sendRaw{
    |msg|
    if(serial != nil, {
      serial.putAll(msg);
    },{
      6.do{msg.removeAt(0);}; // Remove 0xFF (Used in espnowSender (dongle))
      3.do{msg.removeAt(msg.size-1);}; // Remove "end"-bytes
      msg.postln;
      if(espnowBridge != nil, {
        msg = msg.collect({|e| if(e.isInteger, {e}, {e.ascii})});
        espnowBridge.sendMsg("/espnow", Int8Array.newFrom(msg));
      },{
        if(broadcaster != nil, {
          msg = msg.collect({|e| if(e.isInteger, {e}, {e.ascii})});
          broadcaster.sendRaw(Int8Array.newFrom(msg));
        })
      });
    });
  }
  start{
    bCollectMsgs = true;
    msgList.clear();
  }
  end{
    var msg = 0xFF!6 ++ [0x30] ++ this.getAddress();
    bCollectMsgs = false;
    msg = msg ++ msgList.size;
    msgList.do{|m| msg = msg ++ (m.size - 6 - 3)};
    msgList.do{|m|
      6.do{m.removeAt(0)}; // Remove 0xFF!6 from msg, too much info
      3.do{m.removeAt(m.size-1)}; // Remove 'end' from msg, so espnowSender doesn't split them
      msg = msg ++ m;
    };
    msg = msg ++ "end"; // 'end' for espnowSender
    this.send(msg);
    msgList.clear();
  }
  writeID{ // Use this to map the ID's to a .conf file, like jonisk.conf
    |id_|
    this.send(0xFF!6 ++ [0x11] ++ this.getAddress() ++ [id_] ++ "end"); // Dummy 0x00's, why!?
  }
  deepSleep{
		|minutes=1|
		var msg;
		msg = 0xFF!6 ++ [0x10] ++ this.getAddress() ++ minutes.asFloat.asBytes32 ++ "end";
		this.send(msg);
	}
  setRGBW{
    |rgbw|
    if(rgbw.isArray, {
      if((mode == "static").or(mode == "st_brightness"), {
        var msg = 0xFF!6 ++ [0x20] ++ this.getAddress() ++ rgbw.asBytes32F() ++ "end";
        this.send(msg);
      });
      color = Color.fromArray(rgbw);
      synth.set(\rgbw, rgbw); 
   });
  }
  setBrightness{
    |b|
    // if(mode == "static", {
      var msg = 0xFF!6 ++ [0x21] ++ this.getAddress() ++ b.asFloat.asBytes32 ++ "end";
      this.send(msg);
    // });
    brightness = b;
    // synth.set(\amp, b);
  }
  setLag{
    |dst="b", val=0|
    var lagID = 0; // lagID 0 is brightness
    var msg = 0xFF!6 ++ [0x22] ++ this.getAddress() ++ [lagID] ++ val.asFloat.asBytes32 ++ "end";
    this.send(msg);
  }
}
