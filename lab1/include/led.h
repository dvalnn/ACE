#ifndef _LED_H_
#define _LED_H_

#include <elapsedMillis.h>

class Led {
   private:
    float dutyCycle;
    unsigned long period;
    uint8_t displayColor[3] = {0, 0, 0};
    uint8_t savedColor[3] = {0, 0, 0};

    elapsedMillis time;

   public:
    Led(float dutyCycle, unsigned long period, int r, int g, int b);

    float getDutyCycle();
    unsigned long getPeriod();
    int getActiveColor();
    int getColorSettings();

    void setDutyCycle(float dutyCycle);
    void setPeriod(unsigned long period);
    void setColorSettings(int r, int g, int b);

    void update();
};

#endif