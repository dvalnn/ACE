#include "ledHourglass.h"

LedHourglass::LedHourglass(ulong numSteps, ulong timeStepMS, int ledCount,
                           int pin)
    : Hourglass(numSteps, timeStepMS) {
    for (int i = 0; i < ledCount; i++) {
        this->ledVec.push_back(*(new Led()));
    }
    this->npStrip = Adafruit_NeoPixel(ledCount, pin, NEO_GRB + NEO_KHZ800);
}

LedHourglass::~LedHourglass() {
    while (!this->ledVec.empty()) {
        delete &(this->ledVec.back());
        this->ledVec.pop_back();
    }
}

void LedHourglass::begin() {
    this->npStrip.begin();

    for (int i = 0; i < this->ledVec.size(); i++) {
        this->ledVec[i].setColorSettings(0x7B, 0xB5, 0xDC);
        this->ledVec[i].setBrightness(0.5);
    }
}

void LedHourglass::update() {
    Hourglass::update(); 
    this->npStrip.clear();
    // TODO - this->npStrip.clear() breaks / cannot exit INIT state

    for (int i = 0; i < this->ledVec.size(); i++) {
        this->ledVec[i].update();
        if ((i + 1) <= this->getCurrentStep()) {
            if (!this->ledVec[i].isActive()) this->ledVec[i].setActive(true);
        } else if (this->ledVec[i].isActive())
            this->ledVec[i].setActive(false);
    
        this->npStrip.setPixelColor(i, this->ledVec[i].getActiveColor()); // parte
    }

    this->npStrip.show();
}

void LedHourglass::setLedColor(int index, uint32_t color) {
    uint8_t r, g, b;
    r = (color >> 16) & 0xFF;
    g = (color >> 8) & 0xFF;
    b = color & 0xFF;

    this->ledVec[index].setColorSettings(r, g, b);
}

void LedHourglass::setLedColor(int index, uint8_t r, uint8_t g, uint8_t b) {
    this->ledVec[index].setColorSettings(r, g, b);
}