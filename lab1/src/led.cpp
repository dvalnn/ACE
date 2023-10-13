#include "led.h"

Led::Led() {
    float dutyCycle = .5;
    unsigned long period = 200;
    uint8_t savedColor[3] = {0, 0, 0};

    this->dutyCycle = dutyCycle;
    this->period = period;

    this->savedColor[0] = savedColor[0];
    this->savedColor[1] = savedColor[1];
    this->savedColor[2] = savedColor[2];

    this->active = true;
}

Led::Led(float dutyCycle, unsigned long period, int r, int g, int b) {
    this->dutyCycle = dutyCycle;
    this->period = period;

    this->savedColor[0] = r;
    this->savedColor[1] = g;
    this->savedColor[2] = b;

    this->active = true;
}

void Led::update() {
    if (time > period) {
        time = 0;
    }

    this->displayColor[0] = 0;
    this->displayColor[1] = 0;
    this->displayColor[2] = 0;

    if (active and (time < period * dutyCycle)) {
        this->displayColor[0] = this->savedColor[0];
        this->displayColor[1] = this->savedColor[1];
        this->displayColor[2] = this->savedColor[2];
    }
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

bool Led::isActive() { return this->active; }

void Led::setActive(bool state) { this->active = state; }