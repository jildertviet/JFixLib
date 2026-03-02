JFixtureAddr : JFixtureGraphics{
  *new{
    |id, addr, serial|
    ^super.new(id, addr, serial);
  }

  // Tag 21: SetBackgroundCmd { r, g, b, a }.
  // Sets the per-pixel colour floor: pixels will never be rendered darker than this.
  setBackground{ |rgba = #[0.0, 0.0, 0.0, 0.0]|
    this.send(JPb.command(this.getCommandId(), 21,
      JPb.float32(1, rgba[0]) ++
      JPb.float32(2, rgba[1]) ++
      JPb.float32(3, rgba[2]) ++
      JPb.float32(4, rgba[3])
    ));
  }
}
