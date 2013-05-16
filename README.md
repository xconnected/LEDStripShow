LEDStripShow
============
Is a Arduino Sketch that drives a LED Strip with various color effects.

Selection of the effects and varying some parameters is done with a rotary encoder 
and two switches (I used a rotary encoder with a built in switch and a separate switch)

Features
========
9 Effects + 1 Demo Mode (cycling through all effects)

Additional effects can be added quite easily and supported by led strip primitives such as 
fill, copy, shift, rotate, mirror, filter all based on a HSV color model.

Effect settings are stored in EEPROM after 10 seconds of the last change 
(Note changes to the application might require a reinitialization of the EEPROM which can be 
done by changing the EEPROM_MARKER constant (see LedStripShow.h).

Last selected effect is stored in EEPROM after 10 seconds 

On start and effect selection last effect and settings are loaded

Installation
============
In order to compile this Sketch additional libraries have to be installed in the Arduino IDE:

- TTimer   (on Git Hub by me - link follows)
- TSwitch  (on Git Hub by me - link follows)
- TCounter (on Git Hub by me - link follows)

and the two amazing useful libraries:

- FastSPI  found at https://code.google.com/p/fastspi/
- Rotary   found at http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html

Configuration
=============
LedStripShow.ino (look for CONFIGURATION ITEMS)
Switch 1				- Default on PIN 4
Rotary Encoder 			- Default on PIN 2 & 3
Switch 2 (optional) 	- Default on PIN 5
Ledstrip 				- WS2801  on Pin 7 & 11

LedStripShow.h (look for CONFIGURATION ITEMS)
Number of LEDS			_LED_COUNT_ 	25
Number of effects		_EFFECT_COUNT_  10 (including the demo)
Cycle Time (ms)			_CYCLE_TIME_    30000
Wait before save (ms)	_WAIT_SAVE_     10000


Requirements
============
Sketch size: approx 12kByte
The sketch has been tested on Arduino UNO and Arduino Mini Pro 5v/16Mhz
