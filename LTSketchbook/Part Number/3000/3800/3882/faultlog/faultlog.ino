/*!
Linear Technology LTC3882 Fault Logging

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim

http://www.linear.com/product/LTC3882


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
    @ingroup LTC3882
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
#include "LT_3882FaultLog.h"

#define FILE_TEXT_LINE_MAX 132

#define LTC3882_I2C_ADDRESS 0x5C

// Global variables
static uint8_t ltc3882_i2c_address;

static LT_SMBus *smbus = new LT_SMBusPec();
static LT_PMBus *pmbus = new LT_PMBus(smbus);

static LT_3882FaultLog *faultLog3882 = new LT_3882FaultLog(pmbus);

//! Initialize Linduino
//! @return void
void setup()
{
  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  ltc3882_i2c_address = LTC3882_I2C_ADDRESS;
  print_prompt();
}

//! Repeats Linduino loop
//! @return void
void loop()
{
  uint8_t user_command;
  uint8_t *addresses = NULL;
  struct LT_3882FaultLog::FaultLogLtc3882 *ltc3882_fault_log;


  if (Serial.available())                          //! Checks for user input
  {
    while ((user_command = read_int()) == 0);     //! Reads the user command
    if (user_command != 'm')
      Serial.println(user_command);

    uint8_t status;

    switch (user_command)                          //! Prints the appropriate submenu
    {
      case 1:
        status = pmbus->readMfrStatusByte(ltc3882_i2c_address);
        if (status & LTC3882_SMFR_FAULT_LOG)
        {
          faultLog3882->read(ltc3882_i2c_address);

          faultLog3882->dumpBinary(&Serial);
          Serial.println();
          faultLog3882->print(&Serial);

          faultLog3882->release();
        }
        else
          Serial.println(F("No LTC3882 Fault Log"));

        break;
      case 2:
        faultLog3882->clearFaultLog(ltc3882_i2c_address);

        break;
      case 3:
        pmbus->clearAllFaults(ltc3882_i2c_address);

        break;
      case 4:
        pmbus->enablePec(ltc3882_i2c_address);
        delete faultLog3882;
        delete smbus;
        delete pmbus;
        smbus = new LT_SMBusPec();
        pmbus = new LT_PMBus(smbus);
        faultLog3882 = new LT_3882FaultLog(pmbus);
        break;
      case 5:
        pmbus->disablePec(ltc3882_i2c_address);
        delete faultLog3882;
        delete smbus;
        delete pmbus;
        smbus = new LT_SMBusNoPec();
        pmbus = new LT_PMBus(smbus);
        faultLog3882 = new LT_3882FaultLog(pmbus);
        break;
      case 6:
        addresses = smbus->probe(0);
        while (*addresses != 0)
        {
          Serial.print(F("ADDR 0x"));
          Serial.println(*addresses++, HEX);
        }
        break;
      case 7:
        pmbus->restoreFromNvmGlobal();
        delay(2000);
        pmbus->resetGlobal();
        break;
      case 8:
        smbus->sendByte(ltc3882_i2c_address, MFR_FAULT_LOG_STORE);
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
  Serial.print(F("* LTC3882 Fault Log Program                                     *\n"));
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
  Serial.print(F("  4-PEC On\n"));
  Serial.print(F("  5-PEC Off\n"));
  Serial.print(F("  6-Bus Probe\n"));
  Serial.print(F("  7-Reset\n"));
  Serial.print(F("  8-Store Fault Log\n"));
  Serial.print(F("\nEnter a command:"));
}


