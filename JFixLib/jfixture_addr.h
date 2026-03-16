#ifndef JFIXTURE_ADDR_H
#define JFIXTURE_ADDR_H

#include "jfixture_graphics.h"
#include "jfix_platform.h"
#ifndef JFIX_EMULATION
#include "led_strip.h"
#endif
#include <vector>

class jFixtureAddr : public jFixtureGraphics {
public:
    enum JAddressableMode { J_WS2812B, J_WS2816B };
    enum JDrawMode { TEST, TEST_PERLIN, LIVE };

    jFixtureAddr();
    virtual ~jFixtureAddr();

    void setup(uint8_t numColorChannels = 6, const uint8_t *pins = nullptr,
               uint16_t numLedsPerString = 60, JAddressableMode mode = J_WS2812B,
               uint8_t numStrings = 1);

    static void writeRGB(int id, float r, float g, float b, uint8_t channel,
                         floatColor **leds);

    void writeRGBHard(int id, float r, float g, float b, uint8_t channel,
                      floatColor **leds);

    void testLED();
    void allBlack(bool bWrite = false);
    void writeLeds();
    virtual void update() override;

    void blink(uint8_t num = 1, uint16_t dur = 100, uint16_t delayTime = 100,
               uint8_t channel = 0);

    // Emulation: expose LED buffer for external rendering
    floatColor** getLedBuffer() const { return ledBuffer; }
    uint16_t getNumLedsPerString() const { return numLedsPerString; }
    uint8_t getNumStrings() const { return numStrings; }
    float getBrightnessCurveVal(int i) const { return brightnessCurve[i]; }

protected:
    floatColor **ledBuffer = nullptr;
#ifndef JFIX_EMULATION
    std::vector<led_strip_handle_t> ledStrips;
#endif

    uint16_t numLedsPerString = 1;
    uint8_t numStrings = 1;
    JAddressableMode ledMode = J_WS2812B;
    JDrawMode drawMode = LIVE;
    uint32_t lastUpdatedLive = 0;
    int horizontalPixelDistance = 10;

    void initCurve();
    float brightnessCurve[256];
};

#endif // JFIXTURE_ADDR_H
