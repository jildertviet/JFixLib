// Tag 20: SetOtaUrlCmd { url (string, max 128 bytes) }.
// Writes the OTA URL to NVS; device must reboot to apply.
// Note: WiFi credentials are set separately via JFixture.setWifi().
JOtaServer : JFixture{
  *new{
    |id, addr, serial|
    ^super.new(id, addr, serial);
  }

  setOTAServer{ |url = ""|
    // Always unicast — target specific device, never broadcast OTA URL.
    var savedBroadcast = bBroadcast;
    bBroadcast = false;
    this.send(JPb.command(this.getCommandId(), 20,
      JPb.string(1, url)
    ));
    bBroadcast = savedBroadcast;
  }
}
