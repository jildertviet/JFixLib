JFixtureGraphics : JEspnowDevice{
  var <> lastAdded = nil;
  *new{
    |id, addr, serial|
    ^super.new(id, addr, serial);
  }
  addEvent{
    |event|
    lastAdded = event;
    this.send(event.getConstMsg());
    // Send a msg via Serial
  }
  deleteEvents{
    this.send(0xFF!6 ++ [0x25] ++ this.getAddress() ++ "end");
  }
  setViewportOffset{
    |val=#[0,0]|
    this.send(0xFF!6 ++ [0x27] ++ this.getAddress() ++ val.asBytes32F ++ "end");
  }
}
