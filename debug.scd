p = NetAddr.new("localhost", 7997);
t = JFixtureCollection.new(p);
t.readConfigFile("/home/jildert/JFixLib/examples/ledBeam_2m/ledBeam_2m.config", 1);

// Print the raw bytes for OTA command
(
var otaBytes = JPb.command(255, 20,
	JPb.string(1, "https://192.168.10.21/ledBeam_2m.bin")
);
(">> OTA packet (" ++ otaBytes.size ++ " bytes): " ++ otaBytes).postln;
)

// Test 1: baseline - setBackground (known working, using sendRaw directly)
t.children[0].sendRaw(JPb.command(255, 21,
	JPb.float32(1, 0.1) ++ JPb.float32(2, 0.0) ++ JPb.float32(3, 0.0) ++ JPb.float32(4, 1.0)
));
">> Sent setBackground".postln;

// Test 2: OTA URL via manual JPb (broadcast, bypass setOTAServer method)
t.children[0].sendRaw(JPb.command(255, 20,
	JPb.string(1, "https://192.168.10.21/ledBeam_2m.bin")
));
">> Sent setOtaUrl".postln;

// Wait for messages to be sent, then exit
{1.wait; 0.exit}.fork;
