#ifndef _LED_H_
#define _LED_H_

#include <elapsedMillis.h>

class Led {
   private:
    float dutyCycle;
    unsigned long period;
    uint8_t displayColor[3];
    uint8_t savedColor[3];
    
    float brightness;
    bool active;

    elapsedMillis time;

   public:
    Led();
    Led(float dutyCycle, unsigned long period, int r, int g, int b);

    float getDutyCycle();
    unsigned long getPeriod();
    int getActiveColor();
    int getColorSettings();
    float getBrightness();
    unsigned long getTime();

    void setDutyCycle(float dutyCycle);
    void setPeriod(unsigned long period);
    void setColorSettings(int r, int g, int b);
    void setBrightness(float brightness);

    bool isActive();
    void setActive(bool state);

    void update();
};

#endif