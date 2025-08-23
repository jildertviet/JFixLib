#include "JFixture.h"

JFixture::JFixture() {
  laggers[0].link(&brightness);
  brightnessLag = &laggers[0];
  // channels = new float[numChannels]; // Do in specific classes
}

void JFixture::setup(String networkName) {
  Serial.begin(115200);
  Serial.println("Start");
  Serial.print("Version: ");
  Serial.print((int)VERSION[0]);
  Serial.print(".");
  Serial.println((int)VERSION[1]);
  readEEPROM();
  writeStatusLedPtr = &digitalWriteBuiltinLed;
}

void JFixture::update() {
  updateLaggers();
  handleOtaServer();
  checkOneShots();
  aliveBlink();
  sendPing();
}

void JFixture::setLedBuiltin(char pin) {
  ledBuiltin = pin;
  pinMode(pin, OUTPUT);
}

void JFixture::aliveBlink(bool bOverride) { // GPIO_5
  if (bAliveBlink == false)
    return;
  if (millis() > lastBlinked + blinkInterval[blinkIndex] || bOverride) {
    lastBlinked = millis();
    // digitalWriteBuiltinLed(ledBuiltin, blinkIndex);
    writeStatusLedPtr(ledBuiltin, blinkIndex);
    blinkIndex = (blinkIndex + 1) % 2; // 0, 1, 0, etc
  }
}

void JFixture::digitalWriteBuiltinLed(char pin, char status) {
  // digitalWriteBuiltinLedAddr(pin, status);
  digitalWrite(pin, status);
}

void JFixture::checkOneShots() {
  // JOtaServer::checkOneShots();
  std::map<OneShot, JMsgArguments>::iterator it;
  for (it = oneShots.begin(); it != oneShots.end(); it++) {
    if (!it->second.bActive == true)
      continue;
    Serial.println("OneShot triggered");
    switch (it->first) {
    case OneShot::START_DEEPSLEEP: {
      // How to get the duration? w/ Busses?
      Serial.println((float)it->second.arguments[0]);
      float *minutesToSleep = &(it->second.arguments[0]); // For readability
      if (*minutesToSleep > 0) {                          // busses[0] = minutes
        blink(Channel::RED, 1);                           // Do one blink
        // esp_sleep_enable_ext0_wakeup(GPIO_NUM_14,1);
        Serial.println("Go to sleep");
        esp_sleep_enable_timer_wakeup(*minutesToSleep * 60 * 1000000ULL);
        esp_deep_sleep_start();
      }
    } break;
    case OneShot::SET_CHANNELS: {
      for (int i = 0; i < numChannels; i++) {
        setChannel(i, it->second.arguments[i]);
      }
    } break;
    case OneShot::SET_BRIGHTNESS: {
      this->setBrightness(it->second.arguments[0]);
    } break;
    }
    it->second.bActive = false;
  }
}

void JFixture::readEEPROM() {
  if (!EEPROM.begin(64)) {
    Serial.println("failed to initialise EEPROM, id not read. Default to 0");
    return;
  }

  id = EEPROM.read(0);
  Serial.print("My ID: ");
  Serial.println((int)id);
  if (EEPROM.read(1) == 1) {
    Serial.println("Start as static light");
    bStatic = true;
    float rgba[4];
    for (int i = 0; i < 4; i++) {
      EEPROM.get(2 + (i * sizeof(float)), rgba[i]);
      Serial.println(rgba[i]);
    }
    memcpy(rgbaBackground, rgba, sizeof(float) * 3);
    brightness = rgba[3];
    Serial.println();
  }
}

void JFixture::setBrightness(float b) { brightnessLag->set(b); }

void JFixture::updateLaggers() {
  for (int i = 0; i < NUM_LAGGERS; i++) {
    laggers[i].update();
  }
}

float JFixture::getBrightness() { return brightness; }

void JFixture::setParameterBus(const uint8_t *data, int data_len) {
  uint8_t busIndex = 0;
  float value = 0.0;
  memcpy(&busIndex, data, sizeof(char));
  memcpy(&value, data + sizeof(char), sizeof(float));
  if (busIndex < NUM_PARAMETER_BUSSES) {
    parameterBusses[busIndex] = value;
  }
}

void JFixture::setParameterBusN(const uint8_t *data, int data_len) {
  short busIndex = 0; // StartIndex
  memcpy(&busIndex, data, sizeof(char));
  short numValues = (data_len - sizeof(char)) / sizeof(float);

  // Serial.print("Set bus N, numValues: ");
  // Serial.println((int)numValues);
  if (busIndex < NUM_PARAMETER_BUSSES &&
      (busIndex + numValues - 1) < NUM_PARAMETER_BUSSES) {
    for (short i = 0; i < numValues; i++) {
      memcpy(parameterBusses +
                 (busIndex + i), // (int)(busIndex), removed (int))
             data + sizeof(char) + (sizeof(int) * i),
             sizeof(float)); // 4, start of values array
    }
  }
}
