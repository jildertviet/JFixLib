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

// v2 protocol: commands are ProtoBuf-encoded Command messages (simple.proto).
// Targeting uses Command.id (int): 0 = broadcast, N = specific device.
// MAC address is kept for documentation / transport-level routing only.
JFixture : JFixtureSynthController{
  var <> address = #[0xFF,0xFF,0xFF,0xFF,0xFF,0xFF]; // kept for reference / ESP-NOW bridge
  var <> id;
  var <> serial;
  var <> bBroadcast = false;
  var <> bCollectMsgs = false;
  var <> msgList;

  *new{
    |id, addr, serial|
    ^super.new.init(id, addr, serial);
  }

  init{
    |id, addr, serial|
    this.id = id;
    this.address = addr;
    if(serial.class == SerialPort, {
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

  // Returns 0 (broadcast) or this.id (unicast) depending on bBroadcast flag.
  getCommandId{
    if(bBroadcast == true, { ^0 }, { ^this.id });
  }

  getAddressHexString{
    ^(this.address.collect({|e| e.asHexString.at([6,7])}).collect({|e| "0x" ++ e[0] ++ e[1]}).asString.replace("[", "").replace("]", ""));
  }

  send{
    |msg, bMute = false|
    if((serial != nil).or(espnowBridge != nil).or(broadcaster != nil), {
      if(bCollectMsgs, {
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
    var bytes = Int8Array.newFrom(msg);
    if(serial != nil, {
      // Note: v2 UART uses a framed protocol (length + CRC).
      // For now raw bytes are sent; update if firmware framing is required.
      serial.putAll(msg);
    },{
      if(espnowBridge != nil, {
        espnowBridge.sendMsg("/espnow", bytes);
      });
      if(broadcaster != nil, {
        broadcaster.sendRaw(bytes);
      });
    });
  }

  // Collect messages until end() is called, then send them individually.
  start{
    bCollectMsgs = true;
    msgList.clear();
  }

  end{
    bCollectMsgs = false;
    msgList.do { |msg| this.sendRaw(msg) };
    msgList.clear();
  }

  // ── Simple commands ────────────────────────────────────────────────────────

  // Tag 5: IdCmd { id }  — store device ID in NVS.
  writeID{ |id_|
    this.send(JPb.command(this.getCommandId(), 5,
      JPb.int32(1, id_)
    ));
  }

  // Tag 8: SleepCmd { duration_ms }  — 0 = indefinite deep sleep.
  deepSleep{ |minutes = 1|
    this.send(JPb.command(this.getCommandId(), 8,
      JPb.int32(1, (minutes * 60 * 1000).asInteger)
    ));
  }

  // Tag 2: LedCmd { brightness }.
  setBrightness{ |b|
    this.send(JPb.command(this.getCommandId(), 2,
      JPb.float32(1, b)
    ));
    brightness = b;
  }

  // Tag 3: ChannelCmd { channel, value }  — send one per RGBW channel (0–3).
  setRGBW{ |rgbw|
    if(rgbw.isArray, {
      if((mode == "static").or(mode == "st_brightness"), {
        var cmdId = this.getCommandId();
        rgbw.doWithIndex { |v, ch|
          this.send(JPb.command(cmdId, 3,
            JPb.int32(1, ch) ++ JPb.float32(2, v)
          ));
        };
      });
      color = Color.fromArray(rgbw);
      synth.set(\rgbw, rgbw);
    });
  }

  // Tag 9: LagCmd { lagger_id, lag_time_ms }.
  // val is in seconds; firmware expects ms.
  setLag{ |dst = "b", val = 0|
    var lagID = 0; // 0 = brightness lagger
    this.send(JPb.command(this.getCommandId(), 9,
      JPb.int32(1, lagID) ++ JPb.float32(2, val * 1000)
    ));
  }

  // Tag 7: BlinkCmd { on_ms, off_ms }.
  blink{ |on_ms = 500, off_ms = 500|
    this.send(JPb.command(this.getCommandId(), 7,
      JPb.int32(1, on_ms) ++ JPb.int32(2, off_ms)
    ));
  }

  // Tag 12: RebootCmd {}.
  reboot{
    this.send(JPb.command(this.getCommandId(), 12, []));
  }

  // Tag 4: WifiCmd { ssid, password }.
  setWifi{ |ssid = "", password = ""|
    this.send(JPb.command(this.getCommandId(), 4,
      JPb.string(1, ssid) ++ JPb.string(2, password)
    ));
  }

  // Tag 19: SetParamBusCmd { bus_index, value }.
  setParameterBus{ |busIndex = 0, value = 1.0|
    this.send(JPb.command(this.getCommandId(), 19,
      JPb.int32(1, busIndex) ++ JPb.float32(2, value)
    ));
  }

  // Tag 19 (unicast per device): no broadcast-with-per-device-values in v2 proto.
  // Sends individual set_param_bus commands to devices 1..values.size.
  setParameterBusN{ |busIndex = 0, values = #[0, 0]|
    values.doWithIndex { |v, i|
      this.sendRaw(JPb.command(i + 1, 19,
        JPb.int32(1, busIndex) ++ JPb.float32(2, v)
      ));
    };
  }

}
