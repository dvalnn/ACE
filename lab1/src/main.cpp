#include <Arduino.h>
#include <Bounce2.h>
#include <Serial.h>

#include "hourglass.h"

#define SEGMENTS 5
#define SEGMENT_TIME 2000

// todo: VERIFY PIN NUMBERS ARE CORRECT
#define S_UP 2
#define S_DOWN 3
#define S_GO 4

Bounce sGo = Bounce();
Bounce sUp = Bounce();
Bounce sDown = Bounce();

typedef enum state {
    INIT,
    IDLE,
    COUNTING,
    PAUSED,
    FINISHED,
} state;

void setup() {
    sGo.attach(S_GO, INPUT_PULLUP);
    sUp.attach(S_UP, INPUT_PULLUP);
    sDown.attach(S_DOWN, INPUT_PULLUP);
    Serial.begin(9600);
}

void loop() {
    state currentState = INIT;

    hourglass hg(SEGMENT_TIME,
                 SEGMENTS);  // 5 segments, 2 seconds per segment
    for (;;) {
        // TODO: add missing serial print information
        if (Serial) Serial.println(std::string(hg).c_str());

        hg.update();


        switch (state)
        {
        case /* constant-expression */:
            /* code */
            break;
        
        default:
            break;
        }




        if (hg.isFinished()) {
            if (Serial) Serial.println("Finished!");

            hg.reset();
        }

        delay(100);
    }
}