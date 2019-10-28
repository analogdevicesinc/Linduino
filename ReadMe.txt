Control the program via the Arduino Serial Monitor or similar serial interface such as PuTTY,
with the baud rate set to 115200.

The main menu is printed out and the serial port monitored for input. The user enters the selection
number for a desired interaction with the LTM9057 device registers

Menus for reading or writing to FAULT1, FAULT2, ALERT1, ALERT2, CTRLA, CTRLB and reading
STATUS1 and STATUS2 registers have been provided, all other registers may be accessed by
selecting either "2-Read Single Register" or "3-Write Single Register" from the main menu
then entering the register's hex value address, i.e. 0x03 etc. (Refer to LTM9057 datasheet Table 3)

Bit values within specific registers may be written to by entering their position 0 to 7,
(Refer to LTM9057 datasheet Tables 4 - 16), using "4-Set Bit" or "Clear Bit"

Customized values for current sense resistance, ADIN scaling and device address may be adjusted
in the DC2819A-2_Linduino.ino file at lines 10 - 12


DISCLAIMER:

The DC2819A-2_Linduino.ino is derived from the DC2423A_Linduino.ino as its template. 
Copyright (c) 2013, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

This software is provided "as is" and any express or implied warranties, including, but not limited to,
the implied warranties of merchantability and fitness for a particular purpose are disclaimed. In no
event shall the copyright owner or contributors be liable for any direct, indirect incidental, special,
exemplary, or consequential damages (including, but not limited to, procurement of substitute goods
or services; loss of use, data, or profits; or business interruption) however caused and on any theory
of liability, whether in contract, strict liability, or tort (including negligence or otherwise)
arising in any way out of the use of this software, even if advised of the possibility of such damage.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Analog Devices Corp.

The Analog Devices Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.