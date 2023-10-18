#include "ledHourglass.h"

LedHourglass::LedHourglass(ulong numSteps, ulong timeStepMS, int ledCount,
                           int pin)
    : Hourglass(numSteps, timeStepMS) {
    for (int i = 0; i < ledCount; i++) {
        this->ledVec.push_back(*(new Led()));

        // some default settings
        this->ledVec[i].setColorSettings(0xF0, 0x00, 0xF0);
        this->ledVec[i].setBrightness(1);
    }
}

LedHourglass::~LedHourglass() {
    while (!this->ledVec.empty()) {
        delete &(this->ledVec.back());
        this->ledVec.pop_back();
    }
}

void LedHourglass::setLedColor(int index, uint32_t color) {
    uint8_t r, g, b;
    r = (uint8_t)((color >> 16) & 0xFF);
    g = (uint8_t)((color >> 8) & 0xFF);
    b = (uint8_t)(color & 0xFF);

    this->ledVec[index].setColorSettings(r, g, b);
}

void LedHourglass::setLedColor(int index, uint8_t r, uint8_t g, uint8_t b) {
    this->ledVec[index].setColorSettings(r, g, b);
}

void LedHourglass::setAllLedsColor(uint32_t color) {
    for (int i = 0; i < this->ledVec.size(); i++) setLedColor(i, color);
}

void LedHourglass::setLedDutyCycle(int index, float dutyCycle) {
    this->ledVec[index].setDutyCycle(dutyCycle);
}

void LedHourglass::setLedPeriod(int index, uint32_t period) {
    this->ledVec[index].setPeriod(period);
}

void LedHourglass::setLedBrightness(int index, float brightness) {
    this->ledVec[index].setBrightness(brightness);
}

void LedHourglass::update() {
    Hourglass::update();

    for (int i = 0; i < this->ledVec.size(); i++) {
        this->ledVec[i].update();
        if ((i + 1) <= this->getCurrentStep()) {
            if (!this->ledVec[i].isActive()) this->ledVec[i].setActive(true);
        } else if (this->ledVec[i].isActive())
            this->ledVec[i].setActive(false);
    }
}

int LedHourglass::getLedCount() { return this->ledVec.size(); }

uint32_t LedHourglass::getLedColor(int index) {
    return (uint32_t)this->ledVec[index].getActiveColor();
}

float LedHourglass::getLedDutyCycle(int index) {
    return this->ledVec[index].getDutyCycle();
}

float LedHourglass::getLedBrightness(int index) {
    return this->ledVec[index].getBrightness();
}

uint32_t LedHourglass::getLedPeriod(int index) {
    return this->ledVec[index].getPeriod();
}

uint32_t LedHourglass::getLedBuiltInTime(int index) {
    return this->ledVec[index].getTime();
}