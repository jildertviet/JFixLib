JFixtureAddr : JFixtureGraphics{
  *new{
    |id, addr, serial|
    ^super.new(id, addr, serial);
  }
  setBackground{
    |rgba = #[1.0, 1.0, 1.0, 1.0]|
    this.send(0xFF!6 ++ 0x26 ++ this.getAddress() ++ rgba.asBytes32F ++ "end");
  }
}

