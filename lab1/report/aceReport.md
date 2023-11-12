23202311121620
Status : #idea
Tags:

# Ace - Relatório

TODO

## 1. Introduction

In this report we design an electronic sand glass to function as a timer using a Raspberry Pi Pico. The project also counts with a 5 LED string to act as an hourglass and some buttons to control the system named Sgo, Sup and Sdown.

## 2. Diagrams
### 2.1. Schematic circuit diagram
![[Modelo_Trabalho_1_extra_231112_183350-4.png]]
Figure 1 : Schematic representation of the system.

The figure 1 shows the schematic representation of the system with the following connections: The buttons Sgo, Sup, and Sdown are connected to GPIO pins GP2, GP3, and GP4, respectively. All buttons should connect their pin to GND when pressed. The LED trip is powered by the 5V line, with GPIO pin GP6 used for communication.
### 2.2. State Machines
#### 2.2.1 Core State Machine 
![[Modelo_Trabalho_1_extra_231112_193257-1.png]]
Figure 2: Main State Machine Diagram

The figure 2 illustrates the implemented state machine diagram to the control system. The system initial state is INIT. From this state, the system can transition to other states to start a counting, enter in config mode, or go to idle mode after being inactive for 30 seconds.

Transitioning between states requires certain actions like a button being pressed or an event.

To enter in the configuration mode state (CONFIG), we opted to create a state dedicated to detecting the long press of 3 seconds of the button sUp (ENTER_CONFIG) and another to return to the previous state before entering in the configuration mode (EXIT_CONFIG). While in the configuration mode, the user can cycle between the possible configurations shown by figure 3 by pressing sUp.

After exit the configuration state, the user can start a timer by pressing sGo, transitioning to the counting state. In this state, the timer can be reset with the sGo press or incremented by one LED with the sUp press. Pausing and unpausing the timer is also possible with sDown. If the timer is paused, the reset and the time increment are still functional and all LEDs blinks.

When the timer ends all LEDs blink in the red colour in the finished state. The timer can start a new counting with sGo press or go to idle mode in case the system is inactive.


#### 2.2.2.2 Configuration State Machine
![[Modelo_Trabalho_1_extra_231112_185647-2.png]]
The figure 3 shows the state machine diagram responsible for controlling the appearance of the strip in the counting mode. The diagram has 4 states, being the NONE state important to disable changing the settings. The three changeable settings are: time step, counting effect and colour. The time step setting allows each LED being on for 1, 2, 5 or 10 seconds. The counting effect changes the behaviour over time from no effect to fade or blinking at the half period of each LED time. And the last configuration mode makes possible to change colour of the strip to purple, blue, cyan, green, yellow, or white.

Each setting follows an enumeration, making easier cycling between selected setting by pressing sDown for each setting.


# X. Conclusion

To sum up, the goal of the project was achieved without trouble. Additionally, it helped us understand the principles of embedded systems, including the configuration, programming, debugging and validation of the implemented system.

