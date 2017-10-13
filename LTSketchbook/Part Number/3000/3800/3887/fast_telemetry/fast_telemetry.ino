/*!
Linear Technology LTC3887 Telemetry
LTC3887: Power Management Solution for Application Processors

@verbatim

Demonstrates Fast Telemetry

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim

http://www.linear.com/product/LTC3887

REVISION HISTORY
$Revision: 4467 $
$Date: 2015-12-02 11:28:55 -0800 (Wed, 02 Dec 2015) $

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
    @ingroup LTC3887
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_I2CBus.h"
#include "LT_SMBusNoPec.h"
#include "LT_SMBusPec.h"
#include "LT_PMBus.h"

#define LTC3887_I2C_ADDRESS 0x4E
#define MFR_ADC_CONTROL          0xD8
#define ADC_MFR_TELEMETRY_STATUS 0xDA
#define MFR_REAL_TIME            0xFB
#define NUM_MEAS 10
// Global variables
static uint8_t ltc3887_i2c_address;
static LT_SMBus *smbus = new LT_SMBusNoPec();
static LT_PMBus *pmbus = new LT_PMBus(smbus);

//! Initialize Linduino
//! @return void
void setup()
{
  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  ltc3887_i2c_address = LTC3887_I2C_ADDRESS;
  print_prompt();

  pmbus->setPage(ltc3887_i2c_address, 0);
  pmbus->setVout(ltc3887_i2c_address, 5.0);
  smbus->writeByte(ltc3887_i2c_address, 0xD8, 0x00); // Slow
}

//! Repeats Linduino loop
//! @return void
void loop()
{
  uint8_t user_command;
  uint8_t model[7];
  uint8_t *addresses = NULL;
  float voltage0[NUM_MEAS], voltage1[NUM_MEAS];
  float current0[NUM_MEAS], current1[NUM_MEAS];
  float meas[NUM_MEAS];
  uint64_t start_time, current_time;
  uint64_t max_time = 2^64-1; // Suspect
  uint16_t voltage0_time[NUM_MEAS], voltage1_time[NUM_MEAS];
  uint16_t current0_time[NUM_MEAS], current1_time[NUM_MEAS];
  int i,j;
  uint8_t b;

  if (Serial.available())                          //! Checks for user input
  {
    user_command = read_int();                     //! Reads the user command
    if (user_command != 'm')
      Serial.println(user_command);
    uint8_t b;

    switch (user_command)                          //! Prints the appropriate submenu
    {
      case 0:
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x0D); // Fast
        smbus->writeByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS, 0x0F); // Clear status
        start_time = micros();
        for (i = 0; i < NUM_MEAS; i++)
        {
          pmbus->setPage(ltc3887_i2c_address, 0);
          while ((smbus->readByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS) & 0x01) == 0);
          current_time = micros();
          voltage0[i] = pmbus->readVout(ltc3887_i2c_address, false);
          voltage0_time[i] = current_time > start_time ? current_time - start_time : max_time - start_time + current_time;
          start_time = current_time;
          smbus->writeByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS, 0x0E); // Clear remaining status
          while ((smbus->readByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS) & 0x02) == 0);
          current_time = micros();
          current0[i] = pmbus->readIout(ltc3887_i2c_address, false);
          current0_time[i] = current_time > start_time ? current_time - start_time : max_time - start_time + current_time;
          start_time = current_time;
          pmbus->setPage(ltc3887_i2c_address, 1);
          while ((smbus->readByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS) & 0x04) == 0);
          current_time = micros();
          voltage1[i] = pmbus->readVout(ltc3887_i2c_address, false);
          voltage1_time[i] = current_time > start_time ? current_time - start_time : max_time - start_time + current_time;
          start_time = current_time;
          while ((smbus->readByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS) & 0x08) == 0);
          current_time = micros();
          current1[i] = pmbus->readIout(ltc3887_i2c_address, false);
          current1_time[i] = current_time > start_time ? current_time - start_time : max_time - start_time + current_time;
          start_time = current_time;

          smbus->writeByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS, 0x0F); // Clear status
        }
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x00); // Standard
        delay(110);

        for (i = 0; i < NUM_MEAS; i++)
        {
          Serial.print(voltage0_time[i]);
          Serial.print(F("us\t"));
          Serial.print(voltage0[i]);
          Serial.print(F("\t"));
          Serial.print(current0_time[i]);
          Serial.print(F("us\t"));
          Serial.print(current0[i]);
          Serial.print(F("\t"));
          Serial.print(voltage1_time[i]);
          Serial.print(F("us\t"));
          Serial.print(voltage1[i]);
          Serial.print(F("\t"));
          Serial.print(current1_time[i]);
          Serial.print(F("us\t"));
          Serial.println(current1[i]);
        }
        break;
      case 1:
        smbus->writeByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS, 0x0F); // Clear status
        start_time = micros();
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x01); // Vin
        for (i = 0; i < NUM_MEAS; i++)
        {
          meas[i] = pmbus->readVin(ltc3887_i2c_address, false);
        }
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x00); // Standard
        delay(110);

        for (i = 0; i < NUM_MEAS; i++)
        {
          Serial.println(meas[i]);
        }
        break;
      case 2:
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x05); // Vout0
        pmbus->setPage(ltc3887_i2c_address, 0);

        smbus->writeByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS, 0x0F); // Clear status
        start_time = micros();
        for (i = 0; i < NUM_MEAS; i++)
        {
          while ((smbus->readByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS) & 0x01) == 0);
          current_time = micros();
          voltage0[i] = pmbus->readVout(ltc3887_i2c_address, false);
          voltage0_time[i] = current_time > start_time ? current_time - start_time : max_time - start_time + current_time;
          start_time = current_time;
          smbus->writeByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS, 0x0F);
        }
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x00); // Standard
        delay(110);

        for (i = 0; i < NUM_MEAS; i++)
        {
          Serial.print(voltage0_time[i]);
          Serial.print("us\t");
          Serial.println(voltage0[i]);
        }
        break;
      case 3:
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x09); // Vout1
        pmbus->setPage(ltc3887_i2c_address, 1);

        smbus->writeByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS, 0x0F); // Clear status
        start_time = micros();
        for (i = 0; i < NUM_MEAS; i++)
        {
          while ((smbus->readByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS) & 0x04) == 0);
          current_time = micros();
          voltage1[i] = pmbus->readVout(ltc3887_i2c_address, false);
          voltage1_time[i] = current_time > start_time ? current_time - start_time : max_time - start_time + current_time;
          start_time = current_time;
          smbus->writeByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS, 0x0F);
        }
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x00); // Standard
        delay(110);

        for (i = 0; i < NUM_MEAS; i++)
        {
          Serial.print(voltage1_time[i]);
          Serial.print("us\t");
          Serial.println(voltage1[i]);
        }
        break;
      case 4:
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x06); // Iout0
        pmbus->setPage(ltc3887_i2c_address, 0);

        smbus->writeByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS, 0x0F); // Clear status
        start_time = micros();
        for (i = 0; i < NUM_MEAS; i++)
        {
          while ((smbus->readByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS) & 0x02) == 0);
          current_time = micros();
          current0[i] = pmbus->readIout(ltc3887_i2c_address, false);
          current0_time[i] = current_time > start_time ? current_time - start_time : max_time - start_time + current_time;
          start_time = current_time;
          smbus->writeByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS, 0x0F);
        }
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x00); // Standard
        delay(110);

        for (i = 0; i < NUM_MEAS; i++)
        {
          Serial.print(current0_time[i]);
          Serial.print("us\t");
          Serial.println(current0[i]);
        }
        break;
      case 5:
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x0A); // Iout1
        pmbus->setPage(ltc3887_i2c_address, 1);

        smbus->writeByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS, 0x0F); // Clear status
        start_time = micros();
        for (i = 0; i < NUM_MEAS; i++)
        {
          while ((smbus->readByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS) & 0x08) == 0);
          current_time = micros();
          current1[i] = pmbus->readIout(ltc3887_i2c_address, false);
          current1_time[i] = current_time > start_time ? current_time - start_time : max_time - start_time + current_time;
          start_time = current_time;
          smbus->writeByte(ltc3887_i2c_address, ADC_MFR_TELEMETRY_STATUS, 0x0F);
        }
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x00); // Standard
        delay(110);

        for (i = 0; i < NUM_MEAS; i++)
        {
          Serial.print(current1_time[i]);
          Serial.print("us\t");
          Serial.println(current1[i]);
        }
        break;
      case 6:
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x08); // Temp Ext 0
        pmbus->setPage(ltc3887_i2c_address, 0);
        for (i = 0; i < NUM_MEAS; i++)
        {
          meas[i] = pmbus->readExternalTemperature(ltc3887_i2c_address, false);
        }
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x00); // Standard

        for (i = 0; i < NUM_MEAS; i++)
        {
          Serial.println(meas[i]);
        }
        break;
      case 7:
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x0C); // Temp Ext 1
        pmbus->setPage(ltc3887_i2c_address, 1);
        for (i = 0; i < NUM_MEAS; i++)
        {
          meas[i] = pmbus->readExternalTemperature(ltc3887_i2c_address, false);
        }
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x00); // Standard
        delay(110);

        for (i = 0; i < NUM_MEAS; i++)
        {
          Serial.println(meas[i]);
        }
        break;
      case 8:
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x04); // Temp Int
        for (i = 0; i < NUM_MEAS; i++)
        {
          meas[i] = pmbus->readInternalTemperature(ltc3887_i2c_address, false);
        }
        smbus->writeByte(ltc3887_i2c_address, MFR_ADC_CONTROL, 0x00); // Standard
        delay(110);

        for (i = 0; i < NUM_MEAS; i++)
        {
          Serial.println(meas[i]);
        }
        break;
      case 9:
        addresses = smbus->probe(0);
        while (*addresses != 0)
        {
          Serial.print(F("ADDR 0x"));
          Serial.println(*addresses++, HEX);
        }
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
  Serial.print(F("* LTC3887 Fast Telemetry Program                                *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how program fast telemetry          *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
//! @return void
void print_prompt()
{
  Serial.print(F("\n  0-Loop Fast Out\n"));
  Serial.print(F("  1-Loop Fast Vin 0\n"));
  Serial.print(F("  2-Loop Fast Vout 0\n"));
  Serial.print(F("  3-Loop Fast Vout 1\n"));
  Serial.print(F("  4-Loop Fast Iout 0\n"));
  Serial.print(F("  5-Loop Fast Iout 1\n"));
  Serial.print(F("  6-Loop Fast Temp Ext 0\n"));
  Serial.print(F("  7-Loop Fast Temp Ext 1\n"));
  Serial.print(F("  8-Loop Fast Temp Int\n"));
  Serial.print(F("  9-Bus Probe\n"));
  Serial.print(F("\nEnter a command:"));
}

