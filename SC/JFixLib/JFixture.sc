JFixture : JModes{
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
    this.serial = serial;
    msgList = List.new;
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
		|msg|
    msg.postln;
		if(serial != nil, {
      if(bCollectMsgs, {
        "Save msg".postln;
        msgList.add(msg);
      }, {
        {serial.putAll(msg);}.defer(Server.default.latency);
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
      var msg = 0xFF!6 ++ [0x20] ++ this.getAddress() ++ rgbw.asBytes32F() ++ "end";
      this.send(msg);
    });
  }
  setBrightness{
    |b|
    var msg = 0xFF!6 ++ [0x21] ++ this.getAddress() ++ b.asFloat.asBytes32 ++ "end";
    this.send(msg);
  }
  setLag{
    |dst="b", val=0|
    var lagID = 0; // lagID 0 is brightness
    var msg = 0xFF!6 ++ [0x22] ++ this.getAddress() ++ [lagID] ++ val.asFloat.asBytes32 ++ "end";
    this.send(msg);
  }
}
