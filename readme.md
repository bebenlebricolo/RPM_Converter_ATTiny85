# RPM_Converter
This project is based on the ATTiny85 and developped with Atmel Studio. 
I also use avrdude (fantastic!) program to flash the chip, which is triggered by a simple .bat file (this structure is 
easily reproduceable with Linux or any other OS on which avrdude is available)

## What this project is all about
It is aimed to provide a simple toolset to produce a clean voltage which is the linear image of the car's engine speed.

### Motivations
I essentially made this project to repair the RPM meter of a 70's italian car, which was not working anymore as we replaced the standard ignition system with a transistor ignition setup. 
The galvanometer's internal electronic was not healthy anyway and didn't work well at this time, even in the old configuration.

### How it works
The chip interpretes a square wave pattern coming from a Hall Sensor, mounted in front of a car engine's cranckshaft.
This pattern is then translated into RPM values, which are fed into a PWM synthesiser (using Timers) to emulate a Digital to Analog Converter.
The output signal is then fed into a single transistor, regulated with a capacitor, and finally enters the car's galvanometer.

#### Mechanical setup
The cranckshaft has two magnets mounted in the center of the cranckshaft's pulley.
I designed a special mounting system to fill that purpose, but it may largely differ from a car to another.
This 3D printed piece could be find here :
https://www.thingiverse.com/thing:2860653

Then the Hall Sensor (which is a unidirectional switch with incorporated hysteresis) detects a magnet (signal pulled to low) and the latch is released when the magnetic field decreases (signal pulled up).

#### TODO : add schematics and documentation
- [ ] Add electronic schematics