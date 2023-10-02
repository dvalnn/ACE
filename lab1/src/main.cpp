#include <Arduino.h>
#include <Serial.h>

#include "hourglass.h"

void setup() {
    Serial.begin(9600);
}

void loop() {
    hourglass hg(5, 2000);  // 5 segments, 2 seconds per segment
    for (;;) {
        if (Serial)
            Serial.println(std::string(hg).c_str());

        hg.update();

        if (hg.isFinished()) {
            if (Serial)
                Serial.println("Finished!");

            hg.reset();
        }

        delay(100);
    }
}