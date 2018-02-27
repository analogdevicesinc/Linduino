/*!
Linear Technology DC1962C Demonstration Board
LTC3880, LTC2974, LTC2977: Power Management Solution for Application Processors
Setups for training.

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
#include <avr/pgmspace.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_Wire.h"
#include "LT_SMBusNoPec.h"
#include "LT_SMBusPec.h"
#include "LT_PMBus.h"
#include "LT_2977FaultLog.h"
#include "LT_2974FaultLog.h"
#include "LT_3880FaultLog.h"


#define FILE_TEXT_LINE_MAX 132

#define LTC3880_I2C_ADDRESS 0x30
#define LTC2974_I2C_ADDRESS 0x32
#define LTC2977_I2C_ADDRESS 0x33

// Global variables
static uint8_t ltc3880_i2c_address;
static uint8_t ltc2974_i2c_address;
static uint8_t ltc2977_i2c_address;

static LT_SMBus *smbus = new LT_SMBusPec();
static LT_PMBus *pmbus = new LT_PMBus(smbus);

static LT_2977FaultLog *faultLog2977 = new LT_2977FaultLog(pmbus);
static LT_2974FaultLog *faultLog2974 = new LT_2974FaultLog(pmbus);
static LT_3880FaultLog *faultLog3880 = new LT_3880FaultLog(pmbus);

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
  uint8_t *addresses = NULL;
  struct LT_3880FaultLog::FaultLogLtc3880 *ltc3880_fault_log;
  struct LT_2977FaultLog::FaultLogLtc2977 *ltc2977_fault_log;
  struct LT_2974FaultLog::FaultLogLtc2974 *ltc2974_fault_log;
  uint16_t timeout;

  if (Serial.available())                          //! Checks for user input
  {
    while ((user_command = read_int()) == 0);     //! Reads the user command
    if (user_command != 'm')
      Serial.println(user_command);

    switch (user_command)                          //! Prints the appropriate submenu
    {
      case 1:
        Serial.println();
        if (faultLog3880->hasFaultLog(ltc3880_i2c_address))
        {
          faultLog3880->read(ltc3880_i2c_address);
          faultLog3880->print(&Serial);
          faultLog3880->release();
        }
        else
          Serial.println(F("No LTC3880 Fault Log"));

        Serial.println();
        if (faultLog2974->hasFaultLog(ltc2974_i2c_address))
        {
          faultLog2974->read(ltc2974_i2c_address);
          faultLog2974->print(&Serial);
          faultLog2974->release();
        }
        else
          Serial.println(F("No LTC2974 Fault Log"));

        Serial.println();
        if (faultLog2977->hasFaultLog(ltc2977_i2c_address))
        {
          faultLog2977->read(ltc2977_i2c_address);
          faultLog2977->print(&Serial);
          faultLog2977->release();
        }
        else
          Serial.println(F("No LTC2977 Fault Log"));

        break;
      case 2:
        faultLog3880->clearFaultLog(ltc3880_i2c_address);
        delay(2);
        smbus->waitForAck(ltc3880_i2c_address, 0x00);
        pmbus->waitForNotBusy(ltc3880_i2c_address);

        timeout = 4096;
        while (timeout-- > 0)
        {
          if (0 == pmbus->readMfrStatusByte(ltc3880_i2c_address) & ~(1 << 3))
            break;
        }

        faultLog2974->clearFaultLog(ltc2974_i2c_address);
        delay(2);
        pmbus->waitForNotBusy(ltc2974_i2c_address);
        faultLog2977->clearFaultLog(ltc2977_i2c_address);
        delay(2);
        pmbus->waitForNotBusy(ltc2977_i2c_address);
        break;
      case 3:
        pmbus->clearAllFaults(ltc3880_i2c_address);
        pmbus->clearAllFaults(ltc2974_i2c_address);
        pmbus->clearAllFaults(ltc2977_i2c_address);
        break;
      case 4:
        addresses = smbus->probe(0);
        while (*addresses != 0)
        {
          Serial.print(F("ADDR 0x"));
          Serial.println(*addresses++, HEX);
        }
        break;
      case 5:
        pmbus->startGroupProtocol();
        pmbus->reset(ltc3880_i2c_address);
        pmbus->restoreFromNvm(ltc2974_i2c_address);
        pmbus->restoreFromNvm(ltc2977_i2c_address);
        pmbus->executeGroupProtocol();
        break;
      case 6:
        smbus->sendByte(ltc3880_i2c_address, MFR_FAULT_LOG_STORE);
        delay(2);
        smbus->waitForAck(ltc3880_i2c_address, 0x00);
        pmbus->waitForNotBusy(ltc3880_i2c_address);

        timeout = 4096;
        while (timeout-- > 0)
        {
          if (0 != pmbus->readMfrStatusByte(ltc3880_i2c_address) & ~(1 << 3))
            break;
        }

        smbus->sendByte(ltc2974_i2c_address, MFR_FAULT_LOG_STORE);
        delay(2);
        pmbus->waitForNotBusy(ltc2974_i2c_address);

        smbus->sendByte(ltc2977_i2c_address, MFR_FAULT_LOG_STORE);
        delay(2);
        pmbus->waitForNotBusy(ltc2977_i2c_address);

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
  Serial.print(F("\n***************************************************************\n"));
  Serial.print(F("* DC1962C Fault Log Program                                     *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program dumps fault logs                                 *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
//! @return void
void print_prompt()
{
  Serial.print(F("\n  1-Dump Fault Logs\n"));
  Serial.print(F("  2-Clear Fault Logs\n"));
  Serial.print(F("  3-Clear Faults\n"));
  Serial.print(F("  4-Bus Probe\n"));
  Serial.print(F("  5-Reset\n"));
  Serial.print(F("  6-Store Fault Log\n"));
  Serial.print(F("\nEnter a command:"));
}


