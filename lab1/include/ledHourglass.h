#ifndef _LED_HOURGLASS_H_
#define _LED_HOURGLASS_H_

#include <vector>

#include "hourglass.h"
#include "led.h"

class LedHourglass : public Hourglass {
   public:
    LedHourglass(ulong numSteps, ulong timeStepMs, int ledCount, int pin);
    ~LedHourglass();

    void update();

    void setLedColor(int index, uint32_t color);
    void setLedColor(int index, uint8_t r, uint8_t g, uint8_t b);
    void setAllLedsColor(uint32_t color);
    void setAllLedsColor(uint8_t r, uint8_t g, uint8_t b);
    void setLedBrightness(int index, float brightness);
    void setAllLedsBrightness(float brightness);
    void setLedDutyCycle(int index, float dutyCycle);
    void setLedFrequency(int index, uint32_t freq);

    int getLedCount();
    uint32_t getLedColor(int index);
    float getLedBrightness(int index);
    float getLedDutyCycle(int index);
    uint32_t getLedFrequency(int index);

   private:
    std::vector<Led> ledVec;
};

#endif