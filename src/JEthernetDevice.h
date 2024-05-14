#include <Ethernet.h>
#include <EthernetUdp.h>

//     Ethernet/examples/UDPSendReceiveString/UDPSendReceiveString.ino
class JEthernetDevice {
public:
  JEthernetDevice(){};
  IPAddress ip;
  unsigned int localPort = 1111;
  char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; // buffer to hold incoming packet,
  EthernetUDP Udp;

  bool initEthernet(char id, uint8_t *addr) {
    if (id == 255 || id == 0) {
      ip = IPAddress(192, 168, 1, 1);
    } else {
      ip = IPAddress(192, 168, 1, id);
    }

    Ethernet.init(5);
    Ethernet.begin(addr, ip);
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without "
                     "hardware. :(");
      return false;
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
      return false;
    }

    // start UDP
    Udp.begin(localPort);
    return true;
  }

  void receiveUDP(void (*ptr)(const uint8_t *, const uint8_t *, int)) {
    int packetSize = Udp.parsePacket();
    if (packetSize) {
      // read the packet into packetBuffer
      Udp.read(packetBuffer,
               UDP_TX_PACKET_MAX_SIZE); // Write to char[]
      Serial.println("Msg received");
      ptr(nullptr, (const uint8_t *)packetBuffer, packetSize);
    }
  }
};
