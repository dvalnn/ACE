#include "led.h"

Led::Led(float dutyCycle, unsigned long period, int r, int g, int b) {
    this->dutyCycle = dutyCycle;
    this->period = period;

    this->savedColor[0] = r;
    this->savedColor[1] = g;
    this->savedColor[2] = b;
}

void Led::update() {
    if (time > period) {
        time = 0;
    }

    //* LED is turned ON
    if (time < period * dutyCycle) {
        this->displayColor[0] = this->savedColor[0];
        this->displayColor[1] = this->savedColor[1];
        this->displayColor[2] = this->savedColor[2];
        return;
    }

    //* LED is turned OFF
    this->displayColor[0] = 0;
    this->displayColor[1] = 0;
    this->displayColor[2] = 0;
}

// getters and setters
float Led::getDutyCycle() { return this->dutyCycle; }

unsigned long Led::getPeriod() { return this->period; }

int Led::getActiveColor() {
    return (int)((this->displayColor[0] << 16) | (this->displayColor[1] << 8) |
                 (this->displayColor[2]));
}

int Led::getColorSettings() {
    return (int)((this->savedColor[0] << 16) | (this->savedColor[1] << 8) |
                 (this->savedColor[2]));
}

void Led::setDutyCycle(float dutyCycle) { this->dutyCycle = dutyCycle; }

void Led::setPeriod(unsigned long period) { this->period = period; }

void Led::setColorSettings(int r, int g, int b) {
    this->savedColor[0] = (uint8_t)(r & 0xFF);
    this->savedColor[1] = (uint8_t)(g & 0xFF);
    this->savedColor[2] = (uint8_t)(b & 0xFF);
}
