/*!
Linear Technology DC1989A Demonstration Board
LTM4676: Dual Output PolyPhase Step-Down DC/DC Module with Digital Power System Management

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim

http://www.linear.com/product/LTM4676
http://www.linear.com/demo/DC1989A

REVISION HISTORY
$Revision: 3214 $
$Date: 2015-03-03 10:54:52 -0700 (Tue, 03 Mar 2015) $

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

#include <Arduino.h>
#include <Linduino.h>
#include <UserInterface.h>
#include <stdint.h>

#include <LT_Wire.h>
//#include <LT_twi.h>
#include <LT_SMBusPec.h>
#include <LT_PMBusMath.h>
#include <LT_SMBus.h>
#include <LT_PMBusRail.h>
#include <LT_I2CBus.h>
#include <LT_SMBusGroup.h>
#include <LT_PMBusDetect.h>
#include <LT_PMBus.h>
#include <LT_FaultLog.h>
#include <LT_PMBusDevice.h>
#include <LT_SMBusNoPec.h>
#include <LT_SMBusBase.h>
#include <LT_PMBusDetect.h>
#include <LT_PMBusSpeedTest.h>
#include <LT_PMBusDeviceLTC3880.h>
#include <LT_3880FaultLog.h> // For compilation of library

#define LTM4676_I2C_ADDRESS_0 0x45
#define LTM4676_I2C_ADDRESS_1 0x46
#define LTM4676_I2C_ADDRESS_2 0x47
#define LTM4676_I2C_ADDRESS_3 0x48
#define LTM4676_I2C_ADDRESS_R 0x31

// Global variables
static uint8_t ltm4676_i2c_address_0;
static uint8_t ltm4676_i2c_address_1;
static uint8_t ltm4676_i2c_address_2;
static uint8_t ltm4676_i2c_address_3;
static uint8_t ltm4676_i2c_address_r;
static uint8_t pages[2];

static LT_SMBus *smbus = new LT_SMBusPec();
static LT_PMBus *pmbus = new LT_PMBus(smbus);
static LT_PMBusDetect *detector = new LT_PMBusDetect(pmbus);
static LT_PMBusRail *rail;

//! Initialize Linduino
void setup()
{
  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  ltm4676_i2c_address_0 = LTM4676_I2C_ADDRESS_0;
  ltm4676_i2c_address_1 = LTM4676_I2C_ADDRESS_1;
  ltm4676_i2c_address_2 = LTM4676_I2C_ADDRESS_2;
  ltm4676_i2c_address_3 = LTM4676_I2C_ADDRESS_3;
  ltm4676_i2c_address_r = LTM4676_I2C_ADDRESS_R;

  detector->detect();
  rail = detector->getRails()[0];

  print_prompt();
}

//! Repeats Linduino loop
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
        menu_1_basic_commands();                 // Print single-ended voltage menu
        break;
      case 2:
        pmbus->readModel(ltm4676_i2c_address_0, model);
        Serial.print(F("MODEL "));
        Serial.print((char *)model);
        Serial.println();
        break;
      case 3:
        res = pmbus->readPmbusRevision(ltm4676_i2c_address_0);
        Serial.print(F("REVISION "));
        Serial.println(res, HEX);
        break;
      case 4:
        pmbus->enablePec(ltm4676_i2c_address_0);
        delete rail;
        delete smbus;
        delete pmbus;
        delete detector;
        smbus = new LT_SMBusPec();
        pmbus = new LT_PMBus(smbus);
        detector = new LT_PMBusDetect(pmbus);
        detector->detect();
        rail =detector->getRails()[0];
        break;
      case 5:
        pmbus->disablePec(ltm4676_i2c_address_0);
        delete rail;
        delete smbus;
        delete pmbus;
        delete detector;
        smbus = new LT_SMBusNoPec();
        pmbus = new LT_PMBus(smbus);
        detector = new LT_PMBusDetect(pmbus);
        detector->detect();
        rail = detector->getRails()[0];
        break;
      case 6:
        addresses = smbus->probe(0);
        while (*addresses != 0)
        {
          Serial.print(F("ADDR 0x"));
          Serial.println(*addresses++, HEX);
        }
        break;
      case 7 :
        pmbus->resetGlobal();
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
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC1590B Demonstration Program                                 *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send and receive data from   *\n"));
  Serial.print(F("* the DC1590B demo board.                                       *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("\n  1-Basic Commands\n"));
  Serial.print(F("  2-Model Number\n"));
  Serial.print(F("  3-Revision Number\n"));
  Serial.print(F("  4-PEC On\n"));
  Serial.print(F("  5-PEC Off\n"));
  Serial.print(F("  6-Bus Probe\n"));
  Serial.print(F("  7-Reset\n"));
  Serial.print(F("\nEnter a command:"));
}

//! Prints a warning if the demo board is not detected.
void print_warning_prompt()
{
  Serial.println(F("\nWarning: Demo board not detected. Linduino will attempt to proceed."));
}

void menu_1_basic_commands()
{
  uint8_t user_command;
  float   result;
  uint16_t status;

  do
  {
    //! Displays the Read/Write menu
    Serial.print(F("\nRead/Write\n\n"));
    Serial.print(F("  1-Read Voltages\n"));
    Serial.print(F("  2-Read Currents\n"));
    Serial.print(F("  3-Read Power\n"));
    Serial.print(F("  4-Read Status\n"));
    Serial.print(F("  5-Sequence Off/On\n"));
    Serial.print(F("  6-Margin High\n"));
    Serial.print(F("  7-Margin Low\n"));
    Serial.print(F("  8-Margin Off\n"));
    Serial.print(F("  9-Clear Faults\n"));
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
        result = rail->readVin(false);
        Serial.print(F("LTM4676 VIN "));
        Serial.println(result, DEC);
        result = rail->readVout(false);
        Serial.print(F("LTM4676 VOUT "));
        Serial.println(result, DEC);
        break;
      case 2:
        result = rail->readIin(false);
        Serial.print(F("LTM4676 IIN "));
        Serial.println(result, DEC);
        result = rail->readIout(false);
        Serial.print(F("LTM4676 IOUT "));
        Serial.println(result, DEC);
        break;
      case 3:
        result = rail->readPout(false);
        Serial.print(F("LTM4676 POUT "));
        Serial.println(result, DEC);
        break;
      case 4:
        status = rail->readStatusWord();
        Serial.print(F("LTM4676 STATUS WORD 0x"));
        Serial.println(status, HEX);
        break;
      case 5:
        rail->sequenceOff();
        delay (2000);
        rail->sequenceOn();
        break;
      case 6:
        rail->marginHigh();
        break;
      case 7:
        rail->marginLow();
        break;
      case 8:
        rail->marginOff();
        break;
      case 9:
        rail->clearFaults();
        break;
      default:
        if (user_command != 'm')
          Serial.println(F("Invalid Selection"));
        break;
    }
  }
  while (user_command != 'm');
}


