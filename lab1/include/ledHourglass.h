#ifndef _LED_HOURGLASS_H_
#define _LED_HOURGLASS_H_

#include <vector>
#include <Adafruit_NeoPixel.h>

#include "hourglass.h"
#include "led.h"

class LedHourglass : public Hourglass {
   public:
    LedHourglass(ulong numSteps, ulong timeStepMs, int ledCount, int pin);
    ~LedHourglass();

    void begin();
    void update();
    
    void setLedColor(int index, uint32_t color);
    void setLedColor(int index, uint8_t r, uint8_t g, uint8_t b);
    void setLedBrightness(int index, float brightness);
    
    int getLedCount();
    uint32_t getLedColor(int index);
    float getLedBrightness(int index);

    void setAllLedsColor(uint32_t color);
    void setAllLedsColor(uint8_t r, uint8_t g, uint8_t b);
    void setAllLedsBrightness(float brightness);
    
   private:
    std::vector<Led> ledVec;
    Adafruit_NeoPixel npStrip;
};

#endif