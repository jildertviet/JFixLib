#include "JFixture.h"
// #include "JEspnowDevice.h"
// #include "JFixtureDimmer.h"
#include "JFixtureAddr.h"
#include <Ethernet.h>
#include <EthernetUdp.h>

class JllllSettings {
public:
  JllllSettings(){};
  String networkName = "JV_";
  char numChannels = 3;
  uint8_t numLedsPerString = 144;
  char numStrings = 1;
  char ledBuiltin = 5;
  uint8_t *pins = nullptr;
};

class Jllll : public JFixtureAddr {
public:
  // void setup(String networkName, char numChannels = 4, uint8_t* pins =
  // nullptr){
  //   JEspnowDevice::setup(networkName);
  //   JFixtureDimmer::setup(numChannels, pins);
  // }
  //     Ethernet/examples/UDPSendReceiveString/UDPSendReceiveString.ino
  IPAddress ip;
  unsigned int localPort = 1111;
  char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; // buffer to hold incoming packet,
  EthernetUDP Udp;

  void setup(JllllSettings settings) {
    bEspnowEnabled = false;
    JEspnowDevice::setup(settings.networkName);
    Serial.println("After espnow setup");

    // if (id == 0) {
    ip = IPAddress(192, 168, 1, 1);
    // } else {
    // ip = IPAddress(192, 168, 1, id);

    Ethernet.init(5);
    Ethernet.begin(myAddr, ip);
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without "
                     "hardware. :(");
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }

    // start UDP
    Udp.begin(localPort);

    if (settings.pins) {
      JFixtureAddr::setup(settings.numChannels, settings.pins,
                          settings.numLedsPerString,
                          JAddressableMode::J_WS2816B, settings.numStrings);
    }
    setLedBuiltin(settings.ledBuiltin);
    // horizontalPixelDistance = settings.horizontalPixelDistance;
    sendPing(true);
  }

  void update() override {
    JFixtureAddr::update();
    int packetSize = Udp.parsePacket();
    if (packetSize) {
      Serial.print("Received packet of size ");
      Serial.println(packetSize);
      Serial.print("From ");
      IPAddress remote = Udp.remoteIP();
      for (int i = 0; i < 4; i++) {
        Serial.print(remote[i], DEC);
        if (i < 3) {
          Serial.print(".");
        }
      }
      Serial.print(", port ");
      Serial.println(Udp.remotePort());

      // read the packet into packetBuffer
      Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
      Serial.println("Contents:");
      Serial.println(packetBuffer);
    }
    // JFixture::update();
  }
  void blink(char num = 1, short dur = 100, short delayTime = 100,
             char channel = 0) override {}
};
