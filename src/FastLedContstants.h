void addLeds(int pin, CRGB* leds, int numLedsPerString){
  switch (pin) {
    case 0: FastLED.addLeds<NEOPIXEL, 0>(leds, numLedsPerString);
        break;
    case 1: FastLED.addLeds<NEOPIXEL, 1>(leds, numLedsPerString);
        break;
    case 2: FastLED.addLeds<NEOPIXEL, 2>(leds, numLedsPerString);
        break;
    case 3: FastLED.addLeds<NEOPIXEL, 3>(leds, numLedsPerString);
        break;
    case 4: FastLED.addLeds<NEOPIXEL, 4>(leds, numLedsPerString);
        break;
    case 5: FastLED.addLeds<NEOPIXEL, 5>(leds, numLedsPerString);
        break;
    // case 6: FastLED.addLeds<NEOPIXEL, 6>(leds, numLedsPerString);
        // break;
    // case 7: FastLED.addLeds<NEOPIXEL, 7>(leds, numLedsPerString);
        // break;
    // case 8: FastLED.addLeds<NEOPIXEL, 8>(leds, numLedsPerString);
        // break;
    // case 9: FastLED.addLeds<NEOPIXEL, 9>(leds, numLedsPerString);
        // break;
    // case 10: FastLED.addLeds<NEOPIXEL, 10>(leds, numLedsPerString);
        // break;
    case 11: FastLED.addLeds<NEOPIXEL, 11>(leds, numLedsPerString);
        break;
    case 12: FastLED.addLeds<NEOPIXEL, 12>(leds, numLedsPerString);
        break;
    case 13: FastLED.addLeds<NEOPIXEL, 13>(leds, numLedsPerString);
        break;
    case 14: FastLED.addLeds<NEOPIXEL, 14>(leds, numLedsPerString);
        break;
    case 15: FastLED.addLeds<NEOPIXEL, 15>(leds, numLedsPerString);
        break;
    case 16: FastLED.addLeds<NEOPIXEL, 16>(leds, numLedsPerString);
        break;
    case 17: FastLED.addLeds<NEOPIXEL, 17>(leds, numLedsPerString);
        break;
    case 18: FastLED.addLeds<NEOPIXEL, 18>(leds, numLedsPerString);
        break;
    case 19: FastLED.addLeds<NEOPIXEL, 19>(leds, numLedsPerString);
        break;
    // case 20: FastLED.addLeds<NEOPIXEL, 20>(leds, numLedsPerString);
        // break;
    case 21: FastLED.addLeds<NEOPIXEL, 21>(leds, numLedsPerString);
        break;
    case 22: FastLED.addLeds<NEOPIXEL, 22>(leds, numLedsPerString);
        break;
    case 23: FastLED.addLeds<NEOPIXEL, 23>(leds, numLedsPerString);
        break;
    // case 24: FastLED.addLeds<NEOPIXEL, 24>(leds, numLedsPerString);
        // break;
    case 25: FastLED.addLeds<NEOPIXEL, 25>(leds, numLedsPerString);
        break;
    case 26: FastLED.addLeds<NEOPIXEL, 26>(leds, numLedsPerString);
        break;
    case 27: FastLED.addLeds<NEOPIXEL, 27>(leds, numLedsPerString);
        break;
    // case 28: FastLED.addLeds<NEOPIXEL, 28>(leds, numLedsPerString);
        // break;
    // case 29: FastLED.addLeds<NEOPIXEL, 29>(leds, numLedsPerString);
        // break;
    default:
        // Handle default case or do nothing for pins not listed
        break;
  }
}
