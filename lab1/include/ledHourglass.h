#ifndef _LED_HOURGLASS_H_
#define _LED_HOURGLASS_H_

#include <vector>

#include "hourglass.h"
#include "led.h"

class LedHourglass : public Hourglass {
   public:
    LedHourglass(ulong numSteps, ulong timeStepMs, int ledCount);
    ~LedHourglass();
    void update();

   private:
    std::vector<Led> ledVec;
};

#endif