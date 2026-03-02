JFixtureGraphics : JEspnowDevice{
  var <> lastAdded = nil;

  *new{
    |id, addr, serial|
    ^super.new(id, addr, serial);
  }

  // Tag 13: send AddEventCmd (built by event.getConstMsg).
  // Note: for JFixEvent_Perlin and JFixEvent_JOsc, getConstMsg also sends
  // follow-up SetCustomArgCmd / SetValCmd messages for type-specific params.
  addEvent{ |event|
    lastAdded = event;
    event.getConstMsg();
  }

  // Tag 10: DeleteEventsCmd {}.
  deleteEvents{
    this.send(JPb.command(this.getCommandId(), 10, []));
  }

  // Tag 22: SetViewportOffsetCmd { x, y }.
  // Shifts the coordinate origin used by all events on this fixture.
  setViewportOffset{ |val = #[0.0, 0.0]|
    this.send(JPb.command(this.getCommandId(), 22,
      JPb.float32(1, val[0]) ++
      JPb.float32(2, val[1])
    ));
  }
}
