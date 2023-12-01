JOtaServer : JFixture{
  *new{
    |id, addr, serial|
    ^super.new(id, addr, serial);
  }
  setOTAServer{
    |ssid="", password="", url="https://192.168.1.100/.pio/build/esp32dev/firmware.bin"|
    var json = "{\"ssid\":\""++ ssid ++"\", \"password\":\""++password++"\", \"url\":\""++url++"\"}";
    var msg = 0xFF!6 ++ [0x15] ++ this.address ++ json ++ "end"; // Always single, no broadcast
    this.send(msg);
  }
}
