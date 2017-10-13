/*!
Linear Technology DC1962C Demonstration Board
LTC3880, LTC2974, LTC2977: Power Management Solution for Application Processors

@verbatim

Demonstrates SVID using PMBus and LTC3880 for QorIQ. The SVID values
are 0, 1, 2, 3. 3 is treated as VBOOT. When run, the script sets VBOOT
and then the voltage can be lowered in 12.5mV steps by selecting a number.

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim

http://www.linear.com/product/LTC3880

http://www.linear.com/demo/DC1962C

REVISION HISTORY
$Revision: 3033 $
$Date: 2014-12-05 14:58:30 -0800 (Fri, 05 Dec 2014) $

Copyright (c) 2014, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/

/*! @file
    @ingroup DC1962
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_I2CBus.h"
#include "LT_SMBusNoPec.h"
#include "LT_SMBusPec.h"
#include "LT_PMBus.h"

#define LTC3880_I2C_ADDRESS 0x30
#define LTC2974_I2C_ADDRESS 0x32
#define LTC2977_I2C_ADDRESS 0x33

// Global variables
static uint8_t ltc3880_i2c_address;
static uint8_t ltc2974_i2c_address;
static uint8_t ltc2977_i2c_address;
static LT_SMBus *smbusNoPec = new LT_SMBusNoPec();
static LT_SMBus *smbusPec = new LT_SMBusPec();
static LT_PMBus *pmbusNoPec = new LT_PMBus(smbusNoPec);
static LT_PMBus *pmbusPec = new LT_PMBus(smbusPec);
static LT_SMBus *smbus = smbusNoPec;
static LT_PMBus *pmbus = pmbusNoPec;

//! Initialize Linduino
//! @return void
void setup()
{
  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  ltc3880_i2c_address = LTC3880_I2C_ADDRESS;
  ltc2974_i2c_address = LTC2974_I2C_ADDRESS;
  ltc2977_i2c_address = LTC2977_I2C_ADDRESS;
  print_prompt();

  pmbus->setPage(ltc3880_i2c_address, 0);
  pmbus->setVout(ltc3880_i2c_address, 0.85);
}

//! Repeats Linduino loop
//! @return void
void loop()
{
  uint8_t user_command;
  uint8_t model[7];
  uint8_t *addresses = NULL;
  float voltage;

  if (Serial.available())                          //! Checks for user input
  {
    user_command = read_int();                     //! Reads the user command
    if (user_command != 'm')
      Serial.println(user_command);
    voltage = 0.85;
    switch (user_command)                          //! Prints the appropriate submenu
    {
      case 0:
        pmbus->setVout(ltc3880_i2c_address, voltage - 0.060);
        break;
      case 1:
        pmbus->setVout(ltc3880_i2c_address, voltage - 0.045);
        break;
      case 2:
        pmbus->setVout(ltc3880_i2c_address, voltage - 0.030);
        break;
      case 3:
        pmbus->setVout(ltc3880_i2c_address, voltage - 0.015);
        break;
      case 4:
        pmbus->setVout(ltc3880_i2c_address, voltage);
        break;
      case 5:
        pmbus->enablePec(ltc3880_i2c_address);
        pmbus->enablePec(ltc2974_i2c_address);
        pmbus->enablePec(ltc2977_i2c_address);
        smbus = smbusPec;
        pmbus = pmbusPec;
        break;
      case 6:
        pmbus->disablePec(ltc3880_i2c_address);
        pmbus->disablePec(ltc2974_i2c_address);
        pmbus->disablePec(ltc2977_i2c_address);
        smbus = smbusNoPec;
        pmbus = pmbusNoPec;
        break;
      case 7:
        addresses = smbus->probe(0);
        while (*addresses != 0)
        {
          Serial.print("ADDR 0x");
          Serial.println(*addresses++, HEX);
        }
        break;
      case 8 :
        pmbus->resetGlobal();
        break;
      default:
        Serial.println("Incorrect Option");
        break;
    }
    print_prompt();
  }
}

// Function Definitions

//! Prints the title block when program first starts.
//! @return void
void print_title()
{
  Serial.print(F("\n***************************************************************\n"));
  Serial.print(F("* DC1962C SVID Demonstration Program                            *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how program SVID for LTC3880        *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
//! @return void
void print_prompt()
{
  Serial.print(F("\n  0-SVID 0 - VBOOT - 60mV\n"));
  Serial.print(F("  1-SVID 1 - VBOOT - 45mV\n"));
  Serial.print(F("  2-SVID 2 - VBOOT - 30mV\n"));
  Serial.print(F("  3-SVID 3 - VBOOT - 15mV \n"));
  Serial.print(F("  4-SVID 4 - VBOOT\n"));
  Serial.print(F("  5-PEC On\n"));
  Serial.print(F("  6-PEC Off\n"));
  Serial.print(F("  7-Bus Probe\n"));
  Serial.print(F("  8-Reset\n"));
  Serial.print(F("\nEnter a command:"));
}

//! Prints a warning if the demo board is not detected.
//! @return void
void print_warning_prompt()
{
  Serial.println(F("\nWarning: Demo board not detected. Linduino will attempt to proceed."));
}

