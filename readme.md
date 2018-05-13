# RPM_Converter
This project is based on the ATTiny85 and developped with Atmel Studio. 
I also use avrdude (fantastic!) program to flash the chip, which is triggered by a simple .bat file (this structure is 
easily reproduceable with Linux or any other OS on which avrdude is available)

---

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
[Thingiverse link](https://www.thingiverse.com/thing:2860653)

Then the Hall Sensor (which is a unidirectional switch with incorporated hysteresis) detects a magnet (signal pulled to low) and the latch is released when the magnetic field decreases (signal pulled up).

---

## Flashing the code to the chip

I used AVRDUDE to flash directly the binary onto the chip. I used an arduino Nano as a chip programmer (using ArduinoISP arduino sketch).
The hexfile can be found in the ATTiny_Binaries directory. This folder simply contains one hexfile, which has been built using Atmel Studio and the AVR-GCC toolchain (under Release profile).
To flash the hexfile, I've written a simple .batch script that one can use to write the chip with the hexfile. Simply follow the template to meet your needs

**_Note_** : I strongly encourage to test the Stimuli file and tweak it as you need. **This is a good exercise if you don't feel comfortable with the Stimuli files** and Atmel simulating tools. Here is a usefull link to use a Stimuli file while debugging :
[Youtube - How To: Using Stimuli in Simulations with Atmel Studio](https://www.youtube.com/watch?v=5kF-Y8q7e9k)
To do so, remember to build your project under the <Debug> configuration, then launch the debug session and attach the stimuli file under **_>debug>use stimuli file_**

#### TODO : add schematics and documentation
- [ ] Add electronic schematics
