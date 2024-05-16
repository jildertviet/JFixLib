Jllllllllllll : JFixtureAddr{
  var <>motor;
  *new{
    |id, addr, serial|
    ^super.new(id, addr, serial).creatMotorController(id, addr, serial);
  }
  creatMotorController{|id, addr, serial| motor = JMotorController.new(id, addr, serial)} // Overrides...
  * setViewportOffset{
    |tlFixtures = #[], active=true|
    tlFixtures.do{
      |e, i|
      if(active == true, {
        e.setViewportOffset([1.0 * i,0]);
      }, {
        e.setViewportOffset([0,0]);
      });
    }
  }
  respondsTo { |aSymbol|
        ^(super.respondsTo(aSymbol) || motor.respondsTo(aSymbol));
    }

    doesNotUnderstand { |selector ... args|
        if(motor.respondsTo(selector)) {
	    ^motor.performList(selector, args);
        };
	^this.superPerformList(\doesNotUnderstand, selector, args);
    }
}

