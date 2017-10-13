Welcome to the Linduino Source Code Home Page       {#mainpage}
=============================================

For The Latest Information See [Linduino Home Page](http://www.linear.com/designtools/linduino.php)

![](..\Doxygen\Include\dc2026.jpg)


What is a Linduino?
-------------------

Linduino is Linear Technology's Arduino-compatible system for developing and distributing firmware libraries 
and example code for Linear Technology's integrated circuits. The code is designed to be highly portable to 
other microcontroller platforms, and is written in C using as few processor specific functions as possible. 
The code libraries can be downloaded below and used as-is in your project. The Linduino One board 
(Demonstration Circuit DC2026B) allows you to test out the code directly, using the standard demo board for 
the particular IC.

The Linduino One board is compatible with the Arduino Uno, using the Atmel ATmega28 processor. This board 
features a 14-pin "QuikEval" connector that can be plugged into nearly 100 daughter boards for various Linear 
Technology parts, including Analog to Digital converters, Digital to Analog Converters, high-voltage power 
monitors, temperature measurement devices, RF synthesizers, battery stack monitors, and more.

An LTM2884 USB Isolator breaks the ground connection to the PC, allowing projects to operate at a different 
ground potential than the computer that is controlling it.


Setup is Simple:
----------------
[Download the Demo Manual](http://cds.linear.com/docs/en/demo-board-manual/DC2026CFD.PDF) for 
detailed instructions. Three files are required to follow the complete procedure in the demo manual:

1) The latest [Linduino sketchbook](http://www.linear.com/docs/43958). This file contains the Linduino code base, demo board schematics and board files. If you are only interested in the code, stop here.

2) The [QuikEval](http://www.linear.com/quikeval) program. The Linduino board ships with firmware that allows it to communicate with the QuikEval program, which provides all of the GUIs for compatible demo boards. Installing QuikEval is also the recommended way to install the correct USB drivers for the Linduino One board.

3) The [Arduino 1.6.4 IDE](http://arduino.cc/download_handler.php?f=/arduino-1.6.4-windows.zip). This is the development environment for the Arduino, and is required to modify the code, and to load programs into the Linduino One.

[Schematic of DC2026 PDF](http://cds.linear.com/docs/en/demo-board-schematic/DC2026C1-SCH.PDF)

We also recommend: Notepad Plus Plus, an excellent code editor: http://notepad-plus-plus.org/

Atmel Studio, for more advanced development and debugging with the Linduino hardware, and Atmel processors in general: http://www.atmel.com/microsite/atmel_studio6/