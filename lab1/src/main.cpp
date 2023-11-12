#include <Arduino.h>
#include <Bounce2.h>
#include <NeoPixelConnect.h>
#include <Serial.h>
#include <elapsedMillis.h>

#include <cmath>

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
    1000, // 1 second
    2000, // 2 seconds
    5000, // 5 seconds
    10000 // 10 seconds
};

#define NUM_COLOR_OPTIONS 6
volatile int selectedColor = 0;
const int COLOR_OPTIONS[] = {
    0xF000F0, // purple
    0x0000FF, // blue
    0x00FFFF, // cyan
    0x00FF00, // green
    0xFFFF00, // yellow
    0xFFFFFF, // white
};

// color string
const char *COLOR_STRINGS[] = {
    "white", "purple", "blue", "cyan", "green", "yellow",
};

#define NUM_EFFECT_OPTIONS 3
volatile int selectedEffect = 0;
typedef enum countingEffect {
  DEFAULT,
  BLINK,
  FADE,
} countingEffect;

//* 5 segments, 2 seconds per segment
LedHourglass hg(N_SEGMENTS, SEGMENT_TIME, N_LEDS, CONTROL_PIN);

elapsedMillis idleTimer;

// TODO: set magic numbers as macros
void defaultEffect(int index) {
  if (hg.getLedBrightness(index) != 1)
    hg.setLedBrightness(index, 1);
  if (hg.getLedDutyCycle(index) != 1)
    hg.setLedDutyCycle(index, 1);
}

void blinkEffect(int ledIndex) {
  if (hg.getLedBrightness(ledIndex) != 1)
    hg.setLedBrightness(ledIndex, 1);
  if (hg.getLedDutyCycle(ledIndex) != 1)
    hg.setLedDutyCycle(ledIndex, 1);

  if ((ledIndex + 1) == hg.getCurrentStep()) {
    uint32_t elapsedTime =
        hg.getTimeStep() * hg.getCurrentStep() - hg.getTimeRemaining();

    if (elapsedTime >= 0.5 * hg.getTimeStep())
      hg.setLedDutyCycle(ledIndex, 0.5);
  }
}

void fadeEffect(int ledIndex) {
  if (hg.getLedBrightness(ledIndex) != 1)
    hg.setLedBrightness(ledIndex, 1);
  if (hg.getLedDutyCycle(ledIndex) != 1)
    hg.setLedDutyCycle(ledIndex, 1);

  if ((ledIndex + 1) == hg.getCurrentStep()) {
    uint32_t rTimeInStep =
        hg.getTimeStep() * hg.getCurrentStep() - hg.getTimeRemaining();

    // 1 -- timeStep
    // x -- rTimeInStep
    // x =  rTimeInStep / timeStep

    // since we want to fade from 100% to 0%
    // we need to invert the brightness
    // 1 - x = 1 - rTimeInStep / timeStep

    hg.setLedBrightness(ledIndex, 1 - (float)rTimeInStep / hg.getTimeStep());
  }
}

void applyCountingEffect(int i) {
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
    break;
  }
}

void applyCountingEffectAllLeds() {
  if (currentState != COUNTING)
    return;

  for (int i = 0; i < hg.getLedCount(); i++) {
    applyCountingEffect(i);
  }
}

void handleConfigVFX(int led) {
  switch (selectedEffect) {
  case DEFAULT:
    //* Brightness is set to 1 to remove previous settings
    hg.setLedBrightness(led, 1);

    //* For this effect the Led only needs to turn off and on normally
    //* so we will be using the duty cycle to 0.7 so that
    //* the led is on for timeStep seconds and off for 0.3*timeStep
    //* seconds.
    //* This is enough for the user to see the effect.
    hg.setLedDutyCycle(led, 0.70);
    hg.setLedPeriod(led, (uint32_t)((TIME_STEP_OPTIONS[1]) / 0.70));
    break;

  case BLINK:
    //* Brightness is set to 1 to remove previous settings
    hg.setLedBrightness(led, 1);

    //* For this effect the Led needs to blink when half the
    //* timeStep has passed.

    //* This can be achieved by manipulating the LED's duty cycle,
    //* period, brightness and making usage of it's own built in timer.

    //* The duty cycle will be set to 1 so that the led is on for
    //* the whole timeStep.
    hg.setLedDutyCycle(led, 1);

    //* The period will be set to 2 seconds.
    //* This is enought for the user to see the effect.
    hg.setLedPeriod(led, TIME_STEP_OPTIONS[1]);

    //* During the first half of the timeStep
    //* we dont want the led to blink.
    //* So we set the brightness to 1.
    if (hg.getLedBuiltInTime(led) >= TIME_STEP_OPTIONS[1] / 2)
      hg.setLedBrightness(led, 1);

    //* During the second half of the timeStep
    //* we want the led to blink.
    //* To make it blink every 100ms without changing the period
    //* we can alternate the brightness between 1 and 0.
    else
      hg.setLedBrightness(led, (hg.getLedBuiltInTime(led) % 200) < 100);

    break;

  case FADE:
    //* Brightness is set to 1 to remove previous settings
    hg.setLedBrightness(led, 1);

    //* For this effect the Led needs to fade from 100% to 0%
    //* Like before, we set the duty cycle to 1 so that the led is on

    //* for the whole timeStep and we can manipulate the brightness
    //* to make it fade.
    hg.setLedDutyCycle(led, 1);

    //* The period will be set to 2 seconds.
    //* This is enought for the user to see the effect.
    hg.setLedPeriod(led, TIME_STEP_OPTIONS[1]);

    //* Now we just need to calculate the brightness for the fade effect
    //* and set it to the led.

    //* the led timer counts up from 0 -> timeStep
    //* when the timer is 0 we want the brightness to be 100%
    //* when the timer is timeStep we want the brightness to be 0%
    hg.setLedBrightness(led, 1 - (float)hg.getLedBuiltInTime(led) /
                                     TIME_STEP_OPTIONS[1]);
    break;

  default:
    break;
  }
}

void configEffect() {
  for (int index = 0; index < hg.getLedCount(); index++) {
    // if (hg.getLedDutyCycle(index) != 1) hg.setLedDutyCycle(index, 1);
    // if (hg.getLedBrightness(index) != 1) hg.setLedBrightness(index, 1);
    hg.setLedDutyCycle(index, 0);
    hg.setLedBrightness(index, 1);
  }

  // enable the led corresponding to the configuration mode
  // to blink
  Serial.println(confMode);
  hg.setLedDutyCycle(confMode, 0.5);
  hg.setLedColor(confMode, 0xFF0000);
  hg.setLedBrightness(confMode, 1);

  int displayLed = hg.getLedCount() - 1;
  hg.setLedBrightness(displayLed, 1);

  switch (confMode) {
  case TIME_STEP:
    // 0.9 ---- timeStep
    // 1   ---- x
    hg.setLedDutyCycle(displayLed, 0.85);
    hg.setLedPeriod(displayLed,
                    (uint32_t)((TIME_STEP_OPTIONS[selectedTimeStep]) / 0.85));
    break;

  case EFFECT:
    handleConfigVFX(displayLed);
    break;

  case COLOR:
    hg.setLedDutyCycle(displayLed, 1);
    hg.setLedBrightness(displayLed, 1);
    hg.setLedColor(displayLed, COLOR_OPTIONS[selectedColor]);
    break;

  default:
    break;
  }
}

void idleEffect() {
  if (idleTimer <= 750)
    return;

  idleTimer = 0;

  static int offset = 0;

  for (int index = 0; index < hg.getLedCount(); index++) {
    if (hg.getLedBrightness(index) != 0.5)
      hg.setLedBrightness(index, 0.5);
    if (hg.getLedPeriod(index) != 750)
      hg.setLedPeriod(index, 750);

    double dutyCycle = (float)(index + 1) / hg.getLedCount();
    hg.setLedDutyCycle(index, dutyCycle / 2 + 0.25);
    hg.setLedColor(index, COLOR_OPTIONS[(index + offset) % NUM_COLOR_OPTIONS]);
  }
  offset++;
}

void pauseEffect() {
  for (int index = 0; index < hg.getLedCount(); index++) {
    if (hg.getLedBrightness(index) != 1)
      hg.setLedBrightness(index, 1);
    if (hg.getLedDutyCycle(index) != 0.5)
      hg.setLedDutyCycle(index, 0.5);
    if (hg.getLedPeriod(index) != 500)
      hg.setLedPeriod(index, 500);
  }
}

void finishedEffect() {
  for (int index = 0; index < hg.getLedCount(); index++) {
    if (hg.getLedBrightness(index) != 1)
      hg.setLedBrightness(index, 1);
    if (hg.getLedDutyCycle(index) != 0.5)
      hg.setLedDutyCycle(index, 0.5);
    if (hg.getLedPeriod(index) != 200)
      hg.setLedPeriod(index, 200);
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

void hgStateMachine() {

  switch (currentState) {
  case INIT:
    if (Serial)
      Serial.println("INIT");
    if (!hg.isPaused())
      hg.pause();
    if (hg.getTimeRemaining() < hg.getTotalTime())
      hg.reset();
    if (sGo.rose())
      currentState = COUNTING;
    if (sUp.read() == LOW and sUp.currentDuration() >= 3000) {
      currentState = ENTER_CONFIG;
      statePreConfig = INIT;
      break;
    }
    if (idleTimer >= 30000)
      currentState = IDLE;
    break;

  case IDLE:
    if (Serial)
      Serial.println("IDLE");
    if (!hg.isPaused())
      hg.pause();
    if (hg.getTimeRemaining() < hg.getTotalTime())
      hg.reset();
    if (sGo.rose()) {
      hg.setAllLedsColor(COLOR_OPTIONS[selectedColor]);
      currentState = COUNTING;
    }
    if (sUp.read() == LOW and sUp.currentDuration() >= 3000) {
      currentState = ENTER_CONFIG;
      statePreConfig = INIT;
      hg.setAllLedsColor(COLOR_OPTIONS[selectedColor]);

      break;
    }
    idleEffect();

    break;

  case COUNTING:
    applyCountingEffectAllLeds();

    if (Serial)
      Serial.println("COUNTING");
    if (hg.isPaused())
      hg.resume();
    if (sGo.rose())
      hg.reset();
    hg.addTime(hg.getTimeStep());

    if (sUp.read() == LOW and sUp.currentDuration() >= 3000) {
      currentState = ENTER_CONFIG;
      statePreConfig = COUNTING;
      break;
    }

    if (sDown.rose())
      currentState = PAUSED;
    if (hg.isFinished()) {
      idleTimer = 0;
      currentState = FINISHED;
    }
    break;

  case PAUSED:
    if (Serial)
      Serial.println("PAUSED");
    if (!hg.isPaused())
      hg.pause();
    if (sGo.rose())
      hg.reset();
    if (sDown.rose())
      currentState = COUNTING;
    if (sUp.read() == LOW and sUp.currentDuration() >= 3000) {
      currentState = ENTER_CONFIG;
      statePreConfig = COUNTING;
      break;
    }
    pauseEffect();
    break;

  //* buffer state to ensure the button is released before entering
  case ENTER_CONFIG:
    configEffect();
    if (Serial)
      Serial.println("ENTER_CONFIG");
    if (!hg.isPaused())
      hg.pause();
    if (sUp.rose()) {
      confMode = TIME_STEP;
      currentState = CONFIG;
      hg.reset();
      break;
    }
    break;

  case CONFIG:
    configEffect();
    if (Serial)
      Serial.println("CONFIG");
    if (sUp.read() == LOW and sUp.currentDuration() >= 3000) {
      currentState = EXIT_CONFIG;
      confMode = NONE;
      break;
    }
    // Simple hack to make rotate through config modes
    // This works because the enum values are 0-3
    if (sUp.rose())
      confMode = (configMode)((((int)confMode) + 1) % 3);
    break;

  case EXIT_CONFIG:
    if (Serial)
      Serial.println("EXIT_CONFIG");
    if (!hg.isPaused())
      hg.pause();
    if (sUp.rose()) {
      currentState = statePreConfig;
      applyCountingEffectAllLeds();
      hg.setAllLedsColor(COLOR_OPTIONS[selectedColor]);
      break;
    }
    break;

  case FINISHED:
    if (Serial)
      Serial.println("FINISHED");

    finishedEffect();
    hg.setAllLedsColor(0xFF0000);

    if (!hg.isPaused())
      hg.pause();
    if (hg.getTimeRemaining() < hg.getTotalTime())
      hg.reset();

    if (sUp.read() == LOW and sUp.currentDuration() >= 3000) {
      currentState = ENTER_CONFIG;
      statePreConfig = FINISHED;
      hg.setAllLedsColor(COLOR_OPTIONS[selectedColor]);
      break;
    }

    if (sGo.rose()) {
      currentState = COUNTING;
      hg.setAllLedsColor(COLOR_OPTIONS[selectedColor]);
    }

    if (idleTimer > 30000) {
      currentState = IDLE;
    }

    break;

  default:
    if (Serial)
      Serial.println("Invalid state!");
    break;
  }

  switch (confMode) {
  case NONE:
    break;

  case TIME_STEP:
    if (Serial)
      Serial.println("TIME_STEP CONFIG");
    if (sDown.rose()) {
      selectedTimeStep = (selectedTimeStep + 1) % NUM_TIME_STEP_OPTIONS;
      hg.setTimeStep(TIME_STEP_OPTIONS[selectedTimeStep]);
    }
    break;

  case EFFECT:
    if (Serial)
      Serial.println("EFFECT CONFIG");
    if (sDown.rose())
      selectedEffect = (selectedEffect + 1) % NUM_EFFECT_OPTIONS;
    Serial.print("Selected countingEffect: ");
    Serial.println(selectedEffect);
    break;

  case COLOR:
    if (Serial)
      Serial.println("COLOR CONFIG");
    Serial.print("Selected color: ");
    Serial.println(selectedColor);
    if (sDown.rose()) {
      selectedColor = (selectedColor + 1) % NUM_COLOR_OPTIONS;
      hg.setAllLedsColor(COLOR_OPTIONS[selectedColor]);
    }
    break;

  default:
    if (Serial)
      Serial.println("Invalid config mode!");
    break;
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
    // update buttons
    sGo.update();
    sUp.update();
    sDown.update();

    // update hourglass internal logic
    hg.update();

    updateLedStrip();

    // run the state machine logic
    hgStateMachine();

    // 10ms delay used to prevent freewheeling
    delay(10);
  }
}
