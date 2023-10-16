#include <Arduino.h>
#include <Bounce2.h>
#include <Serial.h>

#include "ledHourglass.h"

#define SEGMENTS 5
#define SEGMENT_TIME 2000

#define S_UP 3
#define S_DOWN 4
#define S_GO 2

Bounce sGo = Bounce();
Bounce sUp = Bounce();
Bounce sDown = Bounce();

typedef enum state {
    INIT,
    IDLE,
    COUNTING,
    PAUSED,
    FINISHED,
    ENTER_CONFIG,
    CONFIG,
    EXIT_CONFIG,
} state;

typedef enum configMode { TIME_STEP, EFFECT, COLOR, NONE } configMode;

state currentState = INIT;
state statePreConfig = INIT;
configMode confMode = NONE;

#define NUM_TIME_STEP_OPTIONS 4
volatile int selectedTimeStep = 0;
const int TIME_STEP_OPTIONS[] = {
    1000,  // 1 second
    2000,  // 2 seconds
    5000,  // 5 seconds
    10000  // 10 seconds
};

#define NUM_COLOR_OPTIONS 6
volatile int selectedColor = 0;
const int COLOR_OPTIONS[] = {
    0xF000F0,  // purple
    0x0000FF,  // blue
    0x00FFFF,  // cyan
    0x00FF00,  // green
    0xFFFF00,  // yellow
    0xFFFFFF,  // white
};

// color string
const char* COLOR_STRINGS[] = {
    "purple", "blue", "cyan", "green", "yellow", "white",
};

// TODO: add effect options
#define NUM_EFFECT_OPTIONS 3
volatile int selectedEffect = 0;
typedef enum effect {
    DEFAULT,
    BLINK,
    FADE,
} effect;

const int EFFECT_OPTIONS[] = {
    DEFAULT,  // switch of at the end of the interval
    BLINK,    // when at the second half of the interval, blink
    FADE,     // fade from 100% to 0%

};

//* 5 segments, 2 seconds per segment
LedHourglass hg(SEGMENTS, SEGMENT_TIME, 5);

void hgStateMachine() {
    // TODO: add missing serial print information
    switch (currentState) {
        case INIT:
            if (Serial) Serial.println("INIT");
            if (!hg.isPaused()) hg.pause();
            if (hg.getTimeRemaining() < hg.getTotalTime()) hg.reset();
            if (sGo.rose()) currentState = COUNTING;
            if (sUp.read() == LOW and sUp.currentDuration() >= 3000) {
                currentState = ENTER_CONFIG;
                statePreConfig = INIT;
                break;
            }
            break;

        case COUNTING:
            if (Serial) Serial.println("COUNTING");
            if (hg.isPaused()) hg.resume();
            if (sGo.rose()) hg.reset();
            if (sUp.rose()) {
                // calculate time remaining in current step
                int rTime = hg.getTimeRemaining() -
                            hg.getCurrentStep() * hg.getTimeStep();
                // increment a full time step + time remaining in current
                // step
                hg.addTime(hg.getTimeStep() + rTime);
            }
            if (sUp.read() == LOW and sUp.currentDuration() >= 3000) {
                currentState = ENTER_CONFIG;
                statePreConfig = COUNTING;
                break;
            }
            if (sDown.rose()) currentState = PAUSED;
            if (hg.isFinished()) currentState = FINISHED;
            break;

        case PAUSED:
            if (Serial) Serial.println("PAUSED");
            if (!hg.isPaused()) hg.pause();
            if (sGo.rose()) hg.reset();
            if (sDown.rose()) currentState = COUNTING;
            if (sUp.read() == LOW and sUp.currentDuration() >= 3000) {
                currentState = ENTER_CONFIG;
                statePreConfig = COUNTING;
                break;
            }
            break;

        //* buffer state to ensure the button is released before entering
        case ENTER_CONFIG:
            if (Serial) Serial.println("ENTER_CONFIG");
            if (!hg.isPaused()) hg.pause();
            if (sUp.rose()) {
                confMode = TIME_STEP;
                currentState = CONFIG;
                break;
            }
            break;

        case CONFIG:
            if (Serial) Serial.println("CONFIG");
            if (sUp.read() == LOW and sUp.currentDuration() >= 3000) {
                currentState = EXIT_CONFIG;
                confMode = NONE;
                break;
            }
            // Simple hack to make rotate through config modes
            // This works because the enum values are 0-3
            if (sUp.rose()) confMode = (configMode)((((int)confMode) + 1) % 4);
            break;

        case EXIT_CONFIG:
            if (Serial) Serial.println("EXIT_CONFIG");
            if (!hg.isPaused()) hg.pause();
            if (sUp.rose()) {
                currentState = statePreConfig;
                break;
            }
            break;

        case FINISHED:
            if (Serial) Serial.println("FINISHED");
            if (!hg.isPaused()) hg.pause();
            if (sGo.rose()) hg.reset();
            if (sUp.read() == LOW and sUp.currentDuration() >= 3000) {
                currentState = ENTER_CONFIG;
                statePreConfig = FINISHED;
                break;
            }
            if (hg.getTimeRemaining() > 0) currentState = COUNTING;
            break;

        default:
            if (Serial) Serial.println("Invalid state!");
            break;
    }

    switch (confMode) {
        case NONE:
            break;

        case TIME_STEP:
            if (Serial) Serial.println("TIME_STEP CONFIG");
            if (sDown.rose()) {
                selectedTimeStep =
                    (selectedTimeStep + 1) % NUM_TIME_STEP_OPTIONS;
                hg.setTimeStep(TIME_STEP_OPTIONS[selectedTimeStep]);
            }
            break;

        case EFFECT:
            if (Serial) Serial.println("EFFECT CONFIG");
            if (sDown.rose())
                selectedEffect = (selectedEffect + 1) % NUM_EFFECT_OPTIONS;
            Serial.print("Selected effect: ");
            Serial.println(EFFECT_OPTIONS[selectedEffect]);
            break;

        case COLOR:
            if (Serial) Serial.println("COLOR CONFIG");
            if (sDown.rose())
                selectedColor = (selectedColor + 1) % NUM_COLOR_OPTIONS;
            // TODO: add color change
            break;

        default:
            if (Serial) Serial.println("Invalid config mode!");
            break;
    }
}

void setup() {
    Serial.begin(9600);
    sGo.attach(S_GO, INPUT_PULLUP);
    sUp.attach(S_UP, INPUT_PULLUP);
    sDown.attach(S_DOWN, INPUT_PULLUP);
}

void loop() {
    for (;;) {
        // TODO: switch to LED Hourglass child class
        // Serial.println("Hallo, guten Morgen!");

        hg.update();

        sGo.update();
        sUp.update();
        sDown.update();

        hgStateMachine();
        delay(100);
    }
}