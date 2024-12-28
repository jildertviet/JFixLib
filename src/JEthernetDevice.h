#ifdef ENABLE_ETHERNET
// #define UDP_TX_PACKET_MAX_SIZE 256
#include <Ethernet.h>
#include <EthernetUdp.h>
// #define UDP_TX_PACKET_MAX_SIZE 256
// Modify file in .pio/build/libdeps/Ethernet/Ethernet.h

//     Ethernet/examples/UDPSendReceiveString/UDPSendReceiveString.ino
class JEthernetDevice {
public:
  JEthernetDevice(){};
  IPAddress ip;
  unsigned int localPort = 1111;
  char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; // buffer to hold incoming packet,
  EthernetUDP Udp;
  bool bConnected = false;

  bool initEthernet(char id, uint8_t *addr) {
    // RESET
    pinMode(25, OUTPUT);
    digitalWrite(25, LOW);
    delay(1);
    digitalWrite(25, HIGH);
    delay(2000);

    if (id == 255 || id == 0) {
      ip = IPAddress(192, 168, 1, 1);
    } else {
      ip = IPAddress(192, 168, 1, id);
    }

    Ethernet.init(21);
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
    if (Udp.begin(localPort))
      bConnected = true;

    return true;
  }

  void receiveUDP(void (*ptr)(const uint8_t *, const uint8_t *, int)) {
    if (!bConnected)
      return;
    int packetSize = Udp.parsePacket();
    if (packetSize && packetSize < UDP_TX_PACKET_MAX_SIZE) {
      Serial.println(packetSize);
      // read the packet into packetBuffer
      Udp.read(packetBuffer,
               packetSize); // Write to char[]
      Serial.println("Msg received");
      ptr(nullptr, (const uint8_t *)packetBuffer, packetSize);
    }
  }
};
#endif
