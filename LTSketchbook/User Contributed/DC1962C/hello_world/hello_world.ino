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


Copyright 2018(c) Analog Devices, Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
 - Neither the name of Analog Devices, Inc. nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.
 - The use of this software may or may not infringe the patent rights
   of one or more patent holders.  This license does not release you
   from the requirement that you obtain separate licenses from these
   patent holders to use this software.
 - Use of the software either in source or binary form, must be run
   on or directly connected to an Analog Devices Inc. component.

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*! @file
    @ingroup DC1962
*/

#include <Arduino.h>
#include <stdint.h>
#include <Linduino.h>
#include <UserInterface.h>
#include <LT_Wire.h>
#include <LT_PMBus.h>
#include <LT_SMBusPec.h>
#include <LT_PMBusMath.h>
#include <LT_SMBus.h>
#include <LT_I2CBus.h>
#include <LT_SMBusGroup.h>
#include <LT_FaultLog.h>
#include <LT_SMBusNoPec.h>
#include <LT_SMBusBase.h>

#define LTC3880_I2C_ADDRESS 0x30
#define LTC2974_I2C_ADDRESS 0x32
#define LTC2977_I2C_ADDRESS 0x33

// Global variables
static uint8_t ltc3880_i2c_address;
static uint8_t ltc2974_i2c_address;
static uint8_t ltc2977_i2c_address;
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
  print_prompt();
}

//! Repeats Linduino loop
//! @return void
void loop()
{
  uint8_t user_command;
  uint8_t res;
  uint8_t model[7];
  uint8_t revision[10];
  uint8_t *addresses = NULL;

  if (Serial.available())                          //! Checks for user input
  {
    user_command = read_int();                     //! Reads the user command
    if (user_command != 'm')
      Serial.println(user_command);

    switch (user_command)                          //! Prints the appropriate submenu
    {
      case 1:
        menu_1_basic_commands();                 // Print single-ended voltage menu
        break;
      case 2:
        pmbus->enablePec(ltc3880_i2c_address);
        pmbus->enablePec(ltc2974_i2c_address);
        pmbus->enablePec(ltc2977_i2c_address);
        delete smbus;
        delete pmbus;
        smbus = new LT_SMBusPec();
        pmbus = new LT_PMBus(smbus);
        break;
      case 3:
        pmbus->disablePec(ltc3880_i2c_address);
        pmbus->disablePec(ltc2974_i2c_address);
        pmbus->disablePec(ltc2977_i2c_address);
        delete smbus;
        delete pmbus;
        smbus = new LT_SMBusNoPec();
        pmbus = new LT_PMBus(smbus);
        break;
      case 4:
        addresses = smbus->probe(0);
        while (*addresses != 0)
        {
          Serial.print(F("ADDR 0x"));
          Serial.println(*addresses++, HEX);
        }
        break;
      case 5 :
        pmbus->startGroupProtocol();
        pmbus->reset(ltc3880_i2c_address);
        pmbus->restoreFromNvm(ltc2974_i2c_address);
        pmbus->restoreFromNvm(ltc2977_i2c_address);
        pmbus->executeGroupProtocol();
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
  Serial.print(F("* DC1962C Hello World Demonstration Program                     *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send and receive data from   *\n"));
  Serial.print(F("* the LTC1962C demo board.                                      *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
//! @return void
void print_prompt()
{
  Serial.print(F("\n  1-Basic Commands\n"));
  Serial.print(F("  2-PEC On\n"));
  Serial.print(F("  3-PEC Off\n"));
  Serial.print(F("  4-Bus Probe\n"));
  Serial.print(F("  5-Reset\n"));
  Serial.print(F("\nEnter a command:"));
}

//! Prints a warning if the demo board is not detected.
//! @return void
void print_warning_prompt()
{
  Serial.println(F("\nWarning: Demo board not detected. Linduino will attempt to proceed."));
}

//! Print all voltages
//! @return void
void print_all_voltages()
{
  float   voltage;
  uint8_t page;

  for (page = 0; page < 2; page++)
  {
    pmbus->setPage(ltc3880_i2c_address, page);
    voltage = pmbus->readVout(ltc3880_i2c_address, false);
    Serial.print(F("LTC3880 VOUT "));
    Serial.println(voltage, DEC);
  }

  for (page = 0; page < 4; page++)
  {
    pmbus->setPage(ltc2974_i2c_address, page);
    voltage = pmbus->readVout(ltc2974_i2c_address, false);
    Serial.print(F("LTC2974 VOUT "));
    Serial.println(voltage, DEC);
  }

  for (page = 0; page < 8; page++)
  {
    pmbus->setPage(ltc2977_i2c_address, page);
    voltage = pmbus->readVout(ltc2977_i2c_address, false);
    Serial.print(F("LTC2977 VOUT "));
    Serial.println(voltage, DEC);
  }

}

//! Print all currents
//! @return void
void print_all_currents()
{
  float   current;
  uint8_t page;

  for (page = 0; page < 2; page++)
  {
    pmbus->setPage(ltc3880_i2c_address, page);
    current = pmbus->readIout(ltc3880_i2c_address, false);
    Serial.print(F("LTC3880 IOUT "));
    Serial.println(current, DEC);
  }

  for (page = 0; page < 4; page++)
  {
    pmbus->setPage(ltc2974_i2c_address, page);
    current = pmbus->readIout(ltc2974_i2c_address, false);
    Serial.print(F("LTC2974 IOUT "));
    Serial.println(current, DEC);
  }

  // LTC2977 does not measure current on a DC1962C

}

//! Print all status bytes and words
//! @return void
void print_all_status()
{
  uint8_t b;
  uint16_t w;
  uint8_t page;

  for (page = 0; page < 2; page++)
  {
    Serial.print(F("PAGE "));
    Serial.println(page, DEC);
    pmbus->setPage(ltc3880_i2c_address, page);
    b = pmbus->readStatusByte(ltc3880_i2c_address);
    Serial.print(F("LTC3880 STATUS BYTE 0x"));
    Serial.println(b, HEX);
    w = pmbus->readStatusWord(ltc3880_i2c_address);
    Serial.print(F("LTC3880 STATUS WORD 0x"));
    Serial.println(w, HEX);
  }

  for (page = 0; page < 4; page++)
  {
    Serial.print(F("PAGE "));
    Serial.println(page, DEC);
    pmbus->setPage(ltc2974_i2c_address, page);
    b = pmbus->readStatusByte(ltc2974_i2c_address);
    Serial.print(F("LTC2974 STATUS BYTE 0x"));
    Serial.println(b, HEX);
    w = pmbus->readStatusWord(ltc2974_i2c_address);
    Serial.print(F("LTC2974 STATUS WORD 0x"));
    Serial.println(w, HEX);
  }

  for (page = 0; page < 8; page++)
  {
    Serial.print(F("PAGE "));
    Serial.println(page, DEC);
    pmbus->setPage(ltc2977_i2c_address, page);
    b = pmbus->readStatusByte(ltc2977_i2c_address);
    Serial.print(F("LTC2977 STATUS BYTE 0x"));
    Serial.println(b, DEC);
    w = pmbus->readStatusWord(ltc2977_i2c_address);
    Serial.print(F("LTC2977 STATUS WORD 0x"));
    Serial.println(w, HEX);
  }
}

//! Sequence off then on
//! @return void
void sequence_off_on()
{
  pmbus->sequenceOffGlobal();
  delay (2000);
  pmbus->sequenceOnGlobal();
}

//! Margin high
//! @return void
void margin_high()
{
  pmbus->marginHighGlobal();
}

//! Margin low
//! @return void
void margin_low()
{
  pmbus->marginLowGlobal();
}

//! Go to nominal
//! @return void
void margin_off()
{
  pmbus->sequenceOnGlobal();
}

//! Display menu 1
void menu_1_basic_commands()
{
  uint8_t user_command;

  do
  {
    //! Displays the Read/Write menu
    Serial.print(F("  1-Read All Voltages\n"));
    Serial.print(F("  2-Read All Currents\n"));
    Serial.print(F("  3-Read All Status\n"));
    Serial.print(F("  4-Sequence Off/On\n"));
    Serial.print(F("  5-Margin High\n"));
    Serial.print(F("  6-Margin Low\n"));
    Serial.print(F("  7-Margin Off\n"));
    Serial.print(F("  8-Set LTC3880 PAGE 0 to 0.87V\n"));
    Serial.print(F("  9-Set LTC3880 PAGE 0 to 0.78V\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
    {
      Serial.print(F("m\n"));
    }
    else
      Serial.println(user_command);                         // Print user command

    switch (user_command)
    {
      case 1:
        print_all_voltages();
        break;
      case 2:
        print_all_currents();
        break;
      case 3:
        print_all_status();
        break;
      case 4:
        sequence_off_on();
        break;
      case 5:
        margin_high();
        break;
      case 6:
        margin_low();
        break;
      case 7:
        margin_off();
        break;
      case 8:
        pmbus->setPage(ltc3880_i2c_address, 0x00);
        pmbus->setVout(ltc3880_i2c_address, 0.87);
        break;
      case 9:
        pmbus->setPage(ltc3880_i2c_address, 0x00);
        pmbus->setVout(ltc3880_i2c_address, 0.78);
        break;
      default:
        if (user_command != 'm')
          Serial.println(F("Invalid Selection"));
        break;
    }
  }
  while (user_command != 'm');
}


