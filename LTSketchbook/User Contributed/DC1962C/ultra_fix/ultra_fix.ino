/*!
Linear Technology DC1962C Demonstration Board
LTC3880, LTC2974, LTC2977: Power Management Solution for Application Processors

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim

http://www.linear.com/product/LTC3880

http://www.linear.com/product/LTC2974

http://www.linear.com/product/LTC2977

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
static uint8_t ltc_i2c_addresses[3];
static LT_SMBus *smbus = new LT_SMBusNoPec();
static LT_PMBus *pmbus = new LT_PMBus(smbus);

//! Initialize Linduino
//! @return void
void setup()
{
  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  ltc3880_i2c_address = LTC3880_I2C_ADDRESS;
  ltc2974_i2c_address = LTC2974_I2C_ADDRESS;
  ltc2977_i2c_address = LTC2977_I2C_ADDRESS;
  ltc_i2c_addresses[0] = ltc3880_i2c_address;
  ltc_i2c_addresses[1] = ltc2974_i2c_address;
  ltc_i2c_addresses[2] = ltc2977_i2c_address;
  print_prompt();
}

//! Initialize Linduino
//! @return void
void loop()
{
  uint8_t user_command;
  uint8_t res;
  uint8_t model[7];
  uint8_t *addresses = NULL;

  if (Serial.available())                          //! Checks for user input
  {
    user_command = read_int();                     //! Reads the user command
    if (user_command != 'm')
      Serial.println(user_command);

    switch (user_command)                          //! Prints the appropriate submenu
    {
      case 1:

        smbus->writeByte(ltc3880_i2c_address, 0xBD, 0x2B); // Unlock byte 1
        smbus->writeByte(ltc3880_i2c_address, 0xBD, 0xD4); // Unlock byte 2
        smbus->writeByte(ltc3880_i2c_address,0xBE, 0x2B);  // Erase EEPROM

        smbus->writeByte(ltc2974_i2c_address, 0xBD, 0x2B); // Unlock byte 1
        res = 0;
        while (res == 0)
          res = smbus->readByte(ltc2974_i2c_address, 0xEF) & 0x40;
        smbus->writeByte(ltc2974_i2c_address, 0xBD, 0xD4); // Unlock byte 2
        res = 0;
        while (res == 0)
          res = smbus->readByte(ltc2974_i2c_address, 0xEF) & 0x40;
        smbus->writeByte(ltc2974_i2c_address,0xBE, 0x2B);  // Erase EEPROM
        res = 0;
        while (res == 0)
          res = smbus->readByte(ltc2974_i2c_address, 0xEF) & 0x40;

        smbus->writeByte(ltc2977_i2c_address, 0xBD, 0x2B); // Unlock byte 1
        res = 0;
        while (res == 0)
          res = smbus->readByte(ltc2977_i2c_address, 0xEF) & 0x40;
        smbus->writeByte(ltc2977_i2c_address, 0xBD, 0xD4); // Unlock byte 2
        res = 0;
        while (res == 0)
          res = smbus->readByte(ltc2977_i2c_address, 0xEF) & 0x40;
        smbus->writeByte(ltc2977_i2c_address,0xBE, 0x2B);  // Erase EEPROM
        while (res == 0)
          res = smbus->readByte(ltc2977_i2c_address, 0xEF) & 0x40;

        break;
      case 2:
        smbus->writeWord(0x5B, 0x10, 0xC000); // Disable Write Protect
        smbus->writeWord(0x5B, 0xD1, 0xC049); // Set MFR_CONFIG_ALL
        smbus->writeByte(0x5B, 0xE6, 0x30);   // Set base address
        break;
      case 3:
        addresses = pmbus->bricks(ltc_i2c_addresses, 3);
        break;
      case 4:
        smbus->sendByte(0x5B, 0x15); // Store user all
        break;
      case 5:
        pmbus->enablePec(ltc3880_i2c_address);
        pmbus->enablePec(ltc2974_i2c_address);
        pmbus->enablePec(ltc2977_i2c_address);
        delete smbus;
        delete pmbus;
        smbus = new LT_SMBusPec();
        pmbus = new LT_PMBus(smbus);
        break;
      case 6:
        pmbus->disablePec(ltc3880_i2c_address);
        pmbus->disablePec(ltc2974_i2c_address);
        pmbus->disablePec(ltc2977_i2c_address);
        delete smbus;
        delete pmbus;
        smbus = new LT_SMBusNoPec();
        pmbus = new LT_PMBus(smbus);
        break;
      case 7:
        addresses = smbus->probe(0);
        while (*addresses != 0)
        {
          Serial.print(F("ADDR 0x"));
          Serial.println(*addresses++, HEX);
        }
        break;
      case 8 :
        pmbus->startGroupProtocol();
        pmbus->reset(ltc3880_i2c_address);
        pmbus->restoreFromNvm(ltc2974_i2c_address);
        pmbus->restoreFromNvm(ltc2977_i2c_address);
        pmbus->executeGroupProtocol();
        break;
      default:
        Serial.println(F("Incorrect Option"));
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
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC1962C Ultra Fix Demonstration Program                       *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to brick and unbrick an LTC3880 *\n"));
  Serial.print(F("* using the LTC1962C demo board.                                *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
//! @return void
void print_prompt()
{
  Serial.print(F("\n  1-Brick\n"));
  Serial.print(F("  2-Unbrick\n"));
  Serial.print(F("  3-Detect Bricks\n"));
  Serial.print(F("  4-Store User All\n"));
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


