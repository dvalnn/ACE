#include <Arduino.h>
#include <Bounce2.h>
#include <NeoPixelConnect.h>
#include <Serial.h>

#include "ledHourglass.h"

#define N_SEGMENTS 5
#define SEGMENT_TIME 2000

#define N_LEDS 5
#define CONTROL_PIN 6

#define S_UP 3
#define S_DOWN 4
#define S_GO 2

Bounce sGo = Bounce();
Bounce sUp = Bounce();
Bounce sDown = Bounce();

NeoPixelConnect npStrip(CONTROL_PIN, N_LEDS);

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

// const int EFFECT_OPTIONS[] = {
//     DEFAULT,  // switch of at the end of the interval
//     BLINK,    // when at the second half of the interval, blink
//     FADE,     // fade from 100% to 0%
// };

//* 5 segments, 2 seconds per segment
LedHourglass hg(N_SEGMENTS, SEGMENT_TIME, N_LEDS, CONTROL_PIN);

void hgStateMachine() {
    // Serial.println(std::string(hg).c_str());

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
                //! timeInc = Tstep + Tstep*CurrentStep - rTime
                //* increment a full time step
                //* + time remaining in current step
                uint32_t timeInc = hg.getTimeStep() +
                                   hg.getTimeStep() * hg.getCurrentStep() -
                                   hg.getTimeRemaining();

                hg.addTime(timeInc);
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
            // TODO: finished counting effect
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
                // hg.setTimeStep(TIME_STEP_OPTIONS[selectedTimeStep]);
            }
            break;

        case EFFECT:
            if (Serial) Serial.println("EFFECT CONFIG");
            if (sDown.rose())
                selectedEffect = (selectedEffect + 1) % NUM_EFFECT_OPTIONS;
            Serial.print("Selected effect: ");
            Serial.println(selectedEffect);
            break;

        case COLOR:
            if (Serial) Serial.println("COLOR CONFIG");
            if (sDown.rose()) {
                selectedColor = (selectedColor + 1) % NUM_COLOR_OPTIONS;
            }
            break;

        default:
            if (Serial) Serial.println("Invalid config mode!");
            break;
    }
}

// TODO: set magic numbers as macros
void defaultEffect(int index) {
    if (hg.getLedDutyCycle(index) != 1) hg.setLedDutyCycle(index, 1);
}

void blinkEffect(int index) {
    if (hg.getLedDutyCycle(index) != 1) hg.setLedDutyCycle(index, 1);

    if ((index + 1) == hg.getCurrentStep()) {
        uint32_t elapsedTime =
            hg.getTimeStep() * hg.getCurrentStep() - hg.getTimeRemaining();

        if (elapsedTime >= 0.5 * hg.getTimeStep())
            hg.setLedDutyCycle(index, 0.5);
    }
}

void fadeEffect(int index) {
    if (hg.getLedBrightness(index) != 1) hg.setLedBrightness(index, 1);
    if (hg.getLedDutyCycle(index) != 1) hg.setLedDutyCycle(index, 1);

    if ((index + 1) == hg.getCurrentStep()) {
        uint32_t rTimeInStep =
            hg.getTimeStep() * hg.getCurrentStep() - hg.getTimeRemaining();
        
        // 1 -- timeStep
        // x -- rTimeInStep
        // x =  rTimeInStep / timeStep

        // since we want to fade from 100% to 0%
        // we need to invert the brightness
        // 1 - x = 1 - rTimeInStep / timeStep

        Serial.print("ElapsedTime: ");
        Serial.println(rTimeInStep);
        Serial.print("New brightness: ");
        Serial.println(1 -(float)rTimeInStep / hg.getTimeStep());

        hg.setLedBrightness(index, 1 - (float)rTimeInStep / hg.getTimeStep());
    }
}

void applyEffects() {
    if (currentState != COUNTING) return;

    for (int i = 0; i < hg.getLedCount(); i++) {
        switch (selectedEffect) {
            case DEFAULT:
                defaultEffect(i);
                break;

            case BLINK:
                blinkEffect(i);
                break;

            case FADE:
                fadeEffect(i);
                break;

            default:
                if (Serial) Serial.println("Invalid effect!");
                delay(1000);
                break;
        }
    }
}

void updateLedStrip() {
    uint8_t r, g, b;

    for (int i = 0; i < hg.getLedCount(); i++) {
        r = (uint8_t)((hg.getLedColor(i) >> 16) & 0xFF);
        g = (uint8_t)((hg.getLedColor(i) >> 8) & 0xFF);
        b = (uint8_t)((hg.getLedColor(i)) & 0xFF);

        npStrip.neoPixelSetValue(i, r, g, b, true);
    }
}

void setup() {
    Serial.begin(9600);

    sGo.attach(S_GO, INPUT_PULLUP);
    sUp.attach(S_UP, INPUT_PULLUP);
    sDown.attach(S_DOWN, INPUT_PULLUP);

    sGo.interval(5);
    sUp.interval(5);
    sDown.interval(5);
}

void loop() {
    for (;;) {
        sGo.update();
        sUp.update();
        sDown.update();

        hg.update();

        updateLedStrip();

        hgStateMachine();

        applyEffects();

        delay(10);
    }
}