# lab report

<!--toc:start-->

- [lab report](#lab-report)
  - [3. Software Implementation](#3-software-implementation) - [3.1 Third Party Libraries and Functions](#31-third-party-libraries-and-functions) - [3.2 Implementation Details](#32-implementation-details) - [3.2.1 Class Structure](#321-class-structure) - [3.2.1.1 Class Led](#3211-class-led) - [3.2.1.2 Class Hourglass](#3212-class-hourglass) - [3.2.1.3 Class LedHourglass](#3213-class-ledhourglass)
  <!--toc:end-->

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
 This class is also dependent on the `elapsedMillis` library (see [3.1](#31-third-party-libraries-and-functions)).

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

#### 3.2.2 State Machine Implementation

The logic of the project was implemented using the state machines described in
section 2.
