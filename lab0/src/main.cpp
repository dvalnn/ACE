#include <Arduino.h>

#include <elapsedMillis.h>

#define pinLedBuiltIn 25
#define ledBlinkPeriodMs 1000

#define pinButtonS1 7
#define pinButtonS2 8
#define pinButtonS3 9

#define pinExternalLedGreen 13
#define pinExternalLedRed 18

#define ledGreenBlinkClickPeriodMs 1500
#define ledRedBlinkClickPeriodMs 3000

class led {
   public:
    led(int pin) {
        this->pin = pin;
        pinMode(pin, OUTPUT);
    }

    void update(bool state) {
        digitalWrite(pin, state);
    }

    bool getPinStatus() {
        return digitalRead(pin);
    }

   private:
    int pin;
};

class ledBlinker : led {
   public:
    ledBlinker(int pin, int periodMs, float dutCycle = .5)
        : led(pin) {
        this->periodMs = periodMs;
        this->dutCycle = dutCycle;
    }

    void update() {
        if (blinkTimer > periodMs * dutCycle) {
            blinkTimer = 0;
            led::update(!led::getPinStatus());
        }
    }

   private:
    float dutCycle;
    long unsigned int periodMs;
    elapsedMillis blinkTimer;
};

class ledClicker : led {
   public:
    ledClicker(int ledPin, int buttonPin, int toggleButtonPin = -1)
        : led(ledPin) {
        this->buttonPin = buttonPin;
        this->toggleButtonPin = toggleButtonPin;

        pinMode(buttonPin, INPUT_PULLUP);
        if (toggleButtonPin != -1)
            pinMode(toggleButtonPin, INPUT_PULLUP);
    }

    void update() {
        if (toggleButtonPin != -1 and digitalRead(toggleButtonPin) == LOW)
            toggle = !toggle;

        if (toggle or digitalRead(buttonPin) == LOW)
            led::update(HIGH);
        else
            led::update(LOW);
    }

   private:
    int buttonPin;
    int toggleButtonPin;
    bool toggle = false;
};

// !Bugado <3 ~~>
/* class ledClickerBlinker : ledClicker, ledBlinker {
   public:
    ledClickerBlinker(int ledPin, int buttonPin, int periodMs, float dutyCycle = .5)
        : ledClicker(ledPin, buttonPin), ledBlinker(ledPin, periodMs, dutyCycle) {
    }

    void update() {
        ledClicker::update();
        ledBlinker::update();
    }
}; */

class ledClickerBlinker {
   public:
    ledClickerBlinker(int ledPin, int buttonPin, int periodMs, float dutyCycle = .5) {
        this->ledPin = ledPin;
        this->buttonPin = buttonPin;
        this->periodMs = periodMs;
        this->dutyCycle = dutyCycle;

        pinMode(ledPin, OUTPUT);
        pinMode(buttonPin, INPUT_PULLUP);
    }

    void update() {
        if (digitalRead(buttonPin) == LOW) {
            if (blinkTimer > periodMs * dutyCycle) {
                blinkTimer = 0;
                digitalWrite(ledPin, !digitalRead(ledPin));
            }
        } else {
            digitalWrite(ledPin, LOW);
        }
    }

   private:
    int ledPin;
    int buttonPin;
    float dutyCycle;
    long unsigned int periodMs;
    elapsedMillis blinkTimer;
};

ledBlinker ledBuiltIn(pinLedBuiltIn, ledBlinkPeriodMs);
ledClicker ledGreen(pinExternalLedGreen, pinButtonS1, pinButtonS2);
ledClickerBlinker ledRed(pinExternalLedRed, pinButtonS3, ledRedBlinkClickPeriodMs);

void setup() {
    Serial.begin(9600);
}

void loop() {
    ledBuiltIn.update();
    ledGreen.update();
    ledRed.update();
}
