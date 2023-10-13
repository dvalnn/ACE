#include "ledHourglass.h"

LedHourglass::LedHourglass(ulong numSteps, ulong timeStepMS, int ledCount)
    : Hourglass(numSteps, timeStepMS) {
    for (int i = 0; i < ledCount; i++) {
        this->ledVec.push_back(*(new Led()));
    }
}

LedHourglass::~LedHourglass() {
    while (!this->ledVec.empty()) {
        delete &(this->ledVec.back());
        this->ledVec.pop_back();
    }
}

void LedHourglass::update() {
    this->update();
    for (int i = 0; i < this->ledVec.size(); i++) {
        this->update();
        if ((i + 1) <= this->getCurrentStep()) {
            if (!this->ledVec[i].isActive()) this->ledVec[i].setActive(true);
        } else if (this->ledVec[i].isActive())
            this->ledVec[i].setActive(false);
    }

    // TODO: neopixel display led strip
}