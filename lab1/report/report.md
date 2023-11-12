# lab report

<!-- prettier-ignore-start -->

<!--toc:start-->
- [lab report](#lab-report)
  - [1. Introduction](#1-introduction)
  - [2. Diagrams](#2-diagrams)
    - [2.1. Schematic circuit diagram](#21-schematic-circuit-diagram)
    - [2.2. State Machines](#22-state-machines)
      - [2.2.1 Core State Machine](#221-core-state-machine)
      - [2.2.2.2 Configuration State Machine](#2222-configuration-state-machine)
  - [3. Software Implementation](#3-software-implementation)
    - [3.1 Third Party Libraries and Functions](#31-third-party-libraries-and-functions)
    - [3.2 Implementation Details](#32-implementation-details)
      - [3.2.1 Class Structure](#321-class-structure)
        - [3.2.1.1 Class Led](#3211-class-led)
        - [3.2.1.2 Class Hourglass](#3212-class-hourglass)
        - [3.2.1.3 Class LedHourglass](#3213-class-ledhourglass)
      - [3.2.2 State Machine Implementation](#322-state-machine-implementation)
      - [3.2.3 Visual Effects](#323-visual-effects)
        - [3.2.3.1 Countdown Effects](#3231-countdown-effects)
        - [3.2.3.2 State Effects](#3232-state-effects)
      - [3.2.4 Main Loop](#324-main-loop)
- [4. Conclusion](#4-conclusion)
<!--toc:end-->

<!-- prettier-ignore-end -->

## 1. Introduction

The purpose of this lab work was to build a simple hourglass like electronic
timer using a Raspberry Pi Pico and a 5 LED LED strip. This report goes over
the constructive process of the project, the implemented functionalities and
the design choices that were made. Additionally, it also includes an abridged
look at the source code and its design philosophy.

The complete source code for this project can be found at the following link:
[ace lab1 github repository](https://github.com/dvalnn/ACE/tree/main/lab1).

## 2. Diagrams

### 2.1. Schematic circuit diagram

![figure1](/home/dvalinn/github/ACE/lab1/report/images/schematics.png)
Figure 1 : Schematic representation of the system.

Figure 1 shows the schematic representation of the system
The buttons sGo, sUp, and sDown are connected to GPIO pins GP2, GP3, and
GP4, respectively. All buttons should connect their pin to GND when pressed.
The LED trip is powered by the 5V line, with GPIO pin GP6 used for communication.

### 2.2. State Machines

#### 2.2.1 Core State Machine

![figure2](/home/dvalinn/github/ACE/lab1/report/images/coreMachineState.png)
Figure 2: Main State Machine Diagram

Figure 2 illustrates the implemented state machine diagram to the control
system. The system initial state is INIT. From this state, the system can
transition to other states to start a counting, enter in config mode, or go to
idle mode after being inactive for 30 seconds.

Transitioning between states requires certain actions like a button being
pressed or an event.

To enter in the configuration mode state (CONFIG), we opted to create a state
dedicated to detecting the long press of 3 seconds of the button sUp
(ENTER_CONFIG) and another to return to the previous state before entering in
the configuration mode (EXIT_CONFIG). While in the configuration mode, the user
can cycle between the possible configurations shown by figure 3 by pressing sUp.

After exit the configuration state, the user can start a timer by pressing sGo,
transitioning to the counting state. In this state, the timer can be reset with
the sGo press or incremented by one LED with the sUp press. Pausing and
unpausing the timer is also possible with sDown. If the timer is paused, the
reset and the time increment are still functional and all LEDs blinks.

When the timer ends all LEDs blink in the red colour in the finished state.
The timer can start a new counting with sGo press or go to idle mode in case
the system is inactive.

#### 2.2.2.2 Configuration State Machine

![figura3](/home/dvalinn/github/ACE/lab1/report/images/settingsMachineState.png)
Figure: 3 Configuration State Machine Diagram

Figure 3 shows the state machine diagram responsible for controlling the
appearance of the strip in the counting mode. The diagram has 4 states,
being the NONE state important to disable changing the settings. The three
changeable settings are: time step, counting effect and colour. The time step
setting allows each LED being on for 1, 2, 5 or 10 seconds. The counting effect
changes the behaviour over time from no effect to fade or blinking at the half
period of each LED time. And the last configuration mode makes possible to
change colour of the strip to purple, blue, cyan, green, yellow, or white.

Each setting follows an enumeration, making easier cycling between selected
setting by pressing _sDown_ for each setting.

## 3. Software Implementation

### 3.1 Third Party Libraries and Functions

Our project uses the following third party libraries and functions:

- [NeoPixelConnect](https://github.com/MrYsLab/NeoPixelConnect)

  A basic NeoPixel control library for the Arduino Nano RP2040 Connect board.
  We use this library as the RP2040 is the chip on board the Raspberry Pi Pico.
  This dependency is used to communicate with the led strip via the following
  function:

  ```c++
  void NeoPixelConnect::neoPixelSetValue(int ledIndex,
                                         int r,
                                         int g,
                                         int b,
                                         bool autoShow);
  ```

  This function sets the color of the led at the specified index to the specified
  RGB value. The `autoShow` parameter is used to specify whether or not the led
  strip should be updated immediately or not.

- [Bounce2](https://github.com/thomasfredericks/Bounce2)

  A library for debouncing buttons and switches. We use this library to debounce
  the input buttons on the Raspberry Pi Pico. This library also has the additional
  functionalities of being able to detect when a button is being pressed or
  released, has built-int edge detection, and tracks the amount of time a button
  persists in a certain state.\
  As such, we use the following functions:

  ```c++
  // setup and initialization for a button
  Bounce Bounce::Bounce();
  void Bounce::attach(pin, INPUT_PULLUP);
  void Bounce::interval(ms);

  // button reading and state checking
  void Bounce::update();
  void Bouce::read();
  void Bounce::rose();
  void Bounce::fell();
  void Bounce::currentDuration();
  ```

  The `Bounce::Bounce()` function initializes a button object, the `Bounce::attach()`
  function ties the button object to a specific pin on the Raspberry Pi Pico and
  the `Bounce::interval()` sets the debounce interval for the button. \
  `Bounce::update()` is responsible for updating the state of the button object
  and the remaining functions are used to read the state, detect edges and
  keep track of the time the button has been in a certain state.

- [ElapsedMillis](https://github.com/pfeerick/elapsedMillis/tree/master)

  A simple wrapper library around the common `millis()` arduino function for
  increased ease of use. We use this library to keep track of timer associated
  with various functionalities of the project.

### 3.2 Implementation Details

#### 3.2.1 Class Structure

The main design philosophy behind the project implementation was to build up
the functionalities required for the application in a modular fashion, starting
from the more elementary components and using those to build up the more complex
components. As such, the project is structured as a series of classes, each
of which encapsulates a specific functionality of the project.

##### 3.2.1.1 Class Led

This class encapsulates the functionalities of a single LED. It is responsible
for keeping track of the LED's current color, the LED's current brightness, and
computing the LED's current on/off state based on its period and duty cycle.
This is a purely functional class, meaning that it does not interact with any
of the hardware components of the project and is agnostic of how the LED are
arranged and controlled. This means this class can be reused when implementing
other projects that require individual led control.
It is dependent of the `elapsedMillis` library to keep track of time (see
[3.1](#31-third-party-libraries-and-functions)).

When instantiated, the class creates a new led object with a set of default
parameters that can be changed ate any time using the following functions:

```c++
void Led::setColorSettings(int r, int g, int b);
void Led::setBrightness(float brightness);
void Led::setPeriod(unsigned long period);
void Led::setDutyCycle(float dutyCycle);
```

The LEDs can also be manually turned on or off regardless of their period and
duty cycle using the following functions:

```c++
void Led::setActive(bool state);
```

Each loop iteration, the `Led::update()` function should be called to update
the internal state of the led object, which is responsible for updating the
internal timer that keeps track of the LED's period and duty cycle.

##### 3.2.1.2 Class Hourglass

This class implements the basic logic of an hourglass. It is responsible for
keeping track of the current state of the hourglass (running, paused, stopped,
finished), the current time remaining, and the total time. This class is also
self contained in the sense that is does not interact with any of the hardware
components and does not implement any led-related functionalities. As such it,
it comes with a public API that allows the user to interact with the hourglass
and an overload of the `operator std::string()` to allow for easy printing of
the current state of the hourglass\
 This class is also dependent on the `elapsedMillis` library (see [3.1](#3.1-third-party-libraries-and-functions)).

An Hourglass object is instantiated by defining the number of steps and the
time for each step in milliseconds. The corresponding total time is then
computed by multiplying the number of steps by the time for each step.

```c++
Hourglass Hourglass::Hourglass(uint32_t numSteps, uint32_t timeStepMS);
```

Afterwards, the hourglass object automatically initiates the countdown if
its not paused. Managing the hourglass in discrete steps allows for easy and
straightforward implementation of the hourglass logic and allows different
resolutions.\
 Each loop iteration, the `Hourglass::update()` function should be called to
update the internal state of the hourglass object, which is responsible for
updating the internal timer that keeps track of the hourglass's remaining time.

In addition, the Hourglass class API includes a variety of auxiliary functions
that streamline the interaction with the inner logic and state of the hourglass.

```c++
// basic functionalities
void Hourglass::pause();
void Hourglass::resume();
void Hourglass::reset();
void Hourglass::update();
void Hourglass::isPaused();
void Hourglass::isFiniched();

// useful getters and setters
uint32_t Hourglass::getTimeStep();
void Hourglass::setTimeStep(uint32_t newTimeStep);

int Hourglass::getNumSteps();
void Hourglass::setNumSteps(int newNumSteps);

int Hourglass::getCurrentStep();

uint32_t Hourglass::getTotalTime();
uint32_t Hourglass::getTimeRemaining();
```

##### 3.2.1.3 Class LedHourglass

The last and most complex class is the LedHourglass class, which is responsible
for integrating and extending the functionalities of the individual Led and
Hourglass classes into a single, cohesive API. As such, it inherits from Hourglass
and is built around the Led class objects.

```c++
LedHourglass LedHourglass::LedHourglass(uint32_t numSteps,
                                        uint32_t timeStepMS,
                                        int ledCount,
                                        int pin);
```

At is core, the LedHourglass class is nothing more than an elaborate wrapper for
a vector of Led objects that is controlled by the Hourglass class logic.

```c++
private:
  std::vector<Led> leds;
```

This provides the user with an API with all regular features from the Hourglass
class while keeping the granularity provided by the Led class. This means that
it becomes possible to individually control each LED in the led strip without
creating incompatibilities with the hourglass logic.
Methods such as `LedHourglass::setAllLedsColor()` can be used to easily configure
the entire led strip at once while methods such as `LedHourglass::setLedColor()`
allow for minute control.

This layer of abstraction makes it exceedingly easy to implement the functionalities
expected of the project, as we do not have to worry about the inner logic of both
the individual LEDs and the hourglass. Instead, we can simply call the appropriate
methods from the LedHourglass class API to achieve the desired result and
focus on the overall project structure and design. This API also facilitates the
creation of all sorts of visual effects suck as the blinking and fading animations
required for this lab work.

Further conforming to the modular design concept, the LedHourglass class is also
decoupled from any LED control libraries like the NeoPixelConnect library.
The advantage of this approach is that it allows for portability between different
platforms, with the downside of offloading the responsibility of updating the
led strip to the user. In our case, this is achieved by periodically
passing the color and brightness values for each led to the
`NeoPixelConnect::neoPixelSetValue()` function.

This is done in the main loop (see [3.2.3](#3.2.3-main-loop)) and the update logic
is defined as follows:

```c++
void updateLedStrip() {
  uint8_t r, g, b;

  for (int i = 0; i < hg.getLedCount(); i++) {
    r = (uint8_t)((hg.getLedColor(i) >> 16) & 0xFF);
    g = (uint8_t)((hg.getLedColor(i) >> 8) & 0xFF);
    b = (uint8_t)((hg.getLedColor(i)) & 0xFF);

    npStrip.neoPixelSetValue(i, r, g, b, true);
  }
}
```

The for each LED the color is extracted from the LedHourglass object and
converted to the 32bit RGB format required by the NeoPixelConnect library.
each led is then updated using the `NeoPixelConnect::neoPixelSetValue()` function.
We leave the `autoShow` parameter to true to ensure that the led strip is updated
as soon as possible.

#### 3.2.2 State Machine Implementation

The state machines described in section [something] of the project are
implemented in the 'main.cpp' file in a single function by the name of
`void hgStateMachine()`. This function is called in the main loop of the
program and is responsible for both the main and configuration state machines
that are defined in two separate switch statements.
The states themselves are defined with the following _enums_:

```c++
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

typedef enum configMode {
  TIME_STEP,
  EFFECT,
  COLOR,
  NONE,
} configMode;
```

This makes it simple to keep track of the current state of the state machines
and improves code readability. Likewise, the `configMode` _enum_ is used to keep
track of the current configuration mode. The 'NONE' mode is an auxiliary mode
to signal that the program is not running in configuration mode.

#### 3.2.3 Visual Effects

The visual effects required for this lab work were implemented in dedicated
functions that are called according to the current configuration. They were
build using the building blocks provided by the 'LedHourglass' class API. This
made the code very simple and easy to read since only a few lines of code are
required per effect.\
All visual effects belong to one of two categories:

- Countdown effects
- State effects

##### 3.2.3.1 Countdown Effects

Countdown effects, like the name suggests, are effects that are displayed while
the hourglass is counting down. There are a total of 3 distinct countdown effects:

1. **Default countdown effect** \
   In this mode each led on the strip simply turns off when the corresponding
   time has elapsed. This is the default behaviour of the hourglass so no
   code is required to implement this effect.

2. **Blinking countdown effect** \
   In this mode each led on the strip starts blinking during the second half
   of its corresponding time step. This is achieved by setting the duty cycle
   of the LED to 50% when the time remaining is less than half the time step
   that the led is associated with.

   ```c++
     if ((ledIndex + 1) == hg.getCurrentStep()) {
       uint32_t elapsedTime =
           hg.getTimeStep() * hg.getCurrentStep() - hg.getTimeRemaining();

       if (elapsedTime >= 0.5 * hg.getTimeStep())
         hg.setLedDutyCycle(ledIndex, 0.5);
     }
   ```

   P.S.: The `hg` variable is an instance of the `LedHourglass` class.

3. **Fading countdown effect** \
    For the fading effect, the led brightness is gradually decreased as the time
   elapses. Once again recurring to the LedHourglass API we can achieve this by
   setting the brightness of the led to a value proportional to the time remaining
   in its corresponding time step.

   ```c++
     if ((ledIndex + 1) == hg.getCurrentStep()) {
       // calculate the remaining time in the current step
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
   ```

   P.S.: The led brightness is a floating point number between 0 and 1.

##### 3.2.3.2 State Effects

State effects are effects that are displayed when the hourglass is in a specific
state such as paused, finished, idle or in configuration mode.

1. **Idle effect** \
   In this mode, the LED duty cycle and color is set as a function of its index
   in the LED strip to which is added a cyclic offset. This creates a wave-like
   cycle of colors that moves along the led strip.

   ```c++
     double dutyCycle = (float)(index + 1) / hg.getLedCount();
     hg.setLedDutyCycle(index, dutyCycle / 2 + 0.25);
     hg.setLedColor(index, COLOR_OPTIONS[(index + offset) % NUM_COLOR_OPTIONS]);
   ```

2. **Paused effect** \
   When the countdown is paused, all remaining LEDs blink in unison with a period
   of 500 milliseconds. Achieving this behaviour is trivial and only a matter of
   calling the corresponding LED setter methods.

3. **Finished counting effect** \
   This is a variant of the paused effect, only all LEDs are turned on and blink
   with the red color.

4. **Configuration Effect** \
   While the code for the other visual effects is very straightforward and only
   a matter of calling the right LedHourglass class methods, the configurations
   effect, due to its nature, requires more minute LED control and required a
   different approach.

   The configuration effects are created by first bypassing the hourglass
   class standard behaviour and then manipulating the LEDs individually. We
   We can easily do this since the LED and Hourglass logic are decoupled as
   described in the sections above (see [3.2.1](#3.2.1-Class-Structure)).

   By first resetting and freezing the countdown, we ensure all the LEDs are
   available for manipulation. Then we can manually activate the desired LEDs
   to showcase the various configuration parameters such as the time step, the
   counting effect and the LED color.

   When a configuration is selected, the configuration effect is cleared, replaced
   with the new settings and the Hourglass resumes normal operation.

#### 3.2.4 Main Loop

Since all the of the project feature are encapsulated in either their own classed
or in dedicated functions, the main loop is kept very simple. This helps with
code readability and maintainability, making it easier to add new features.

```c++
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
```

## 4. Conclusion

To sum up, the goal of the project was achieved without trouble. Additionally,
it helped us understand the principles of embedded systems, including the
configuration, programming, debugging and validation of the implemented system.
