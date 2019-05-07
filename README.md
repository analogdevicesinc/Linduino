# Linduino Read Me
## Overview ##
The Linduino (Demonstration Circuit DC2026) is an Arduino-based controller board used for developing and distributing firmware libraries and example code for Analog Devices products. As shipped, the Linduino functions as a bridge between evaluation GUI software and supported product evaluation boards. It can also be reprogrammed with product-specific example code, included in the Linduino Github repository.

Each Linduino-supported product includes an example main program, defined in the LTSketchbook/Part Number folder. Driver code is defined in the LTSketchbook/libraries folder, and is intended to be used directly in the customer's platform. 

## User Guide & Downloads ##
A Detailed User Guide for the Linduino, showing how to set up the hardware and use the software tools is available at https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/dc2026c.html

## Purchase ##
For details on where and how to purchase the Linduino, please visit the Analog Devices website (https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/dc2026c.html#eb-relatedhardware ) or any of Analog Devices authorized distribution partners.

## How to run an example code ##
1. Download and run Arduino.exe (Version 1.8.5). You can find the installer here:
https://www.arduino.cc/en/Main/Software
2. Download LTSketchbook.
3. Open the Arduino IDE and go to Files → Preferences and add the LTSketchbook location. 
4. It is the location where you have saved the LTSketchbook. Check the ‘Use External editor’ option to edit your code files in an external text editor like Notepad++ (optional).
Click OK and reopen the Arduino IDE. The Preferences will not be set unless the IDE is reopened.
5. Connect Linduino to your PC through a USB port. Wait till your computer identifies the board as a COM Port.
6. Select the Serial Port on the Arduino IDE ­from File → Port. 
7. Navigate to the example code in Part Number folder from File options.
8. Click the Compile button to compile the code. Click the Upload button to upload the code into Linduino.
