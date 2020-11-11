/*!
Analog Devices DC2978A Demonstration

LTC7841: Six Channel Sequencer and Voltage Supervisor with EEPROM

@verbatim
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim

http://www.analog.com/products/LTC7841

http://www.analog.com/design-center/evaluation-hardware-and-software/evaluation-boards-kits/dc1798a-a.html


Copyright 2020(c) Analog Devices, Inc.

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
    @ingroup LTC7841
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
//#include "LT_I2CBus.h"
#include "LT_SMBusNoPec.h"
//#include "LT_SMBusPec.h"
//#include "LT_PMBUS.h"
//#include "LT_I2C.h"
#include "LTC7841.h"

#define LTC7841_I2C_ADDRESS       0x20

/****************************************************************************/
// Global variables
static uint8_t ltc7841_i2c_address;

//static LT_I2CBus *i2cbus = new LT_I2CBus();
//static LT_I2CBus *i2cbus = new LT_I2CBus();
//static LT_SMBusNoPec *smbus = new LT_SMBusNoPec(i2cbus);
static LT_SMBusNoPec *smbus = new LT_SMBusNoPec();

/****************************************************************************/
//! Initialize Linduino
//! @return void
void setup()
{
  uint16_t return_val;

  ltc7841_i2c_address = LTC7841_I2C_ADDRESS;

  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  print_prompt();

//  ltc7841_config0();
}

/****************************************************************************/
//! Main Linduino Loop
//! @return void
void loop()
{
  uint8_t user_command;
  double voltage = 0.0;

  if (Serial.available())                //! Checks for user input
  {
    user_command = read_int();         //! Reads the user command

    switch (user_command)
    {

      case 1 :
        reset();
        break;

      case 2 :
        clear_faults();
        break;

      case 3 :
        turn_off();
        break;

      case 4 :
        turn_on();
        break;

      case 5 :
        margin_high();
        break;

      case 6 :
        margin_low();
        break;

      case 7 :
        Serial.print(F("Enter voltage: "));
        voltage = read_float();
        set_output_voltage(voltage);
        break;

      case 8 :
        Serial.print(F("Enter voltage: "));
        voltage = read_float();
        set_output_high_voltage(voltage);
        break;

      case 9 :
        Serial.print(F("Enter voltage: "));
        voltage = read_float();
        set_output_low_voltage(voltage);
        break;

      case 10 :
        read_output_voltage();
        break;

      case 11 :
        read_input_voltage();
        break;

      case 12 :
        read_output_current();
        break;

      case 13 :
        read_input_current();
        break;

      case 14 :
        set_slew(LTC7841_CONFIG_FAST_SLEW);
        break;

      case 15 :
        set_slew(LTC7841_CONFIG_NORMAL_SLEW);
        break;

      case 16 :
        set_slew(LTC7841_CONFIG_SLOW_SLEW);
        break;

      case 17 :
        read_status();
        break;


      default:
        Serial.print(F("15\n"));
        Serial.println(F("Incorrect Option"));
        break;
    }
    print_prompt();
  }
}


/************************************************************************/
// Function Definitions

//! Prints the title block when program first starts.
//! @return void
void print_title()
{
  Serial.print(F("\n***************************************************************\n"));
  Serial.print(F("* DC2978A Control Program                                       *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program provides a simple interface to control the       *\n"));
  Serial.print(F("* the LTC7841 on the DC2978A Demo Board                         *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
//! @return void
void print_prompt()
{
  Serial.print(F("\n"));
  Serial.print(F("  1  - Reset\n"));
  Serial.print(F("  2  - Clear faults\n"));
  Serial.print(F("  3  - Turn off output\n"));
  Serial.print(F("  4  - Turn on output\n"));
  Serial.print(F("  5  - Margin high output\n"));
  Serial.print(F("  6  - Margin low output\n"));
  Serial.print(F("  7  - Set output voltage\n"));
  Serial.print(F("  8  - Set output high voltage\n"));
  Serial.print(F("  9  - Set output low voltage\n"));
  Serial.print(F("  10 - Read output voltage\n"));
  Serial.print(F("  11 - Read input voltage\n"));
  Serial.print(F("  12 - Read output current\n"));
  Serial.print(F("  13 - Read input current\n"));
  Serial.print(F("  14 - Fast slew\n"));
  Serial.print(F("  15 - Normal slew\n"));
  Serial.print(F("  16 - Slow slew\n"));

  Serial.print(F("  17 - Read status\n"));

  Serial.print(F("\n Enter a command number: "));
}

//! Reset.
//! @return void
void reset()
{
  smbus->sendByte(LTC7841_I2C_ADDRESS, LTC7841_MFR_RESET);
}

//! Clear faults.
//! @return void
void clear_faults()
{
  smbus->writeWord(LTC7841_I2C_ADDRESS, LTC7841_STATUS_WORD, 0xFFFF);
}

//! Turn off output.
//! @return void
void turn_off()
{

  smbus->writeByte(LTC7841_I2C_ADDRESS, LTC7841_OPERATION, LTC7841_OPERATION_OFF);
}

//! Turn on output.
//! @return void
void turn_on()
{

  smbus->writeByte(LTC7841_I2C_ADDRESS, LTC7841_OPERATION, LTC7841_OPERATION_ON);
}

//! Margin high output.
//! @return void
void margin_high()
{

  smbus->writeByte(LTC7841_I2C_ADDRESS, LTC7841_OPERATION, LTC7841_OPERATION_MARGIN_HIGH);
}

//! Margin low output.
//! @return void
void margin_low()
{

  smbus->writeByte(LTC7841_I2C_ADDRESS, LTC7841_OPERATION, LTC7841_OPERATION_MARGIN_LOW);
}

//! Set output voltage.
//! @return void
void set_output_voltage(double v)
{
  double v_clamp = max(0.0, min(v, 48.0));
  double v_scale = (100.0 * v_clamp / 48.0) / 0.2;
  uint16_t w = (uint16_t) v_scale;

  smbus->writeWord(LTC7841_I2C_ADDRESS, LTC7841_MFR_VOUT_COMMAND, w);
}

//! Set output high voltage.
//! @return void
void set_output_high_voltage(double v)
{
  double v_clamp = max(0.0, min(v, 48.0));
  double v_scale = (100.0 * v_clamp / 48.0) / 0.2;
  uint16_t w = (uint16_t) v_scale;

  smbus->writeWord(LTC7841_I2C_ADDRESS, LTC7841_MFR_VOUT_MARGIN_HIGH, w);
}

//! Set output low voltage.
//! @return void
void set_output_low_voltage(double v)
{
  double v_clamp = max(0.0, min(v, 48.0));
  double v_scale = (100.0 * v_clamp / 48.0) / 0.2;
  uint16_t w = (uint16_t) v_scale;

  smbus->writeWord(LTC7841_I2C_ADDRESS, LTC7841_MFR_VOUT_MARGIN_LOW, w);
}

//! Read output voltage.
//! @return void
void read_output_voltage()
{
  uint16_t w;
  double v;

  w = smbus->readWord(LTC7841_I2C_ADDRESS, LTC7841_READ_VOUT);

  v = 10.0E-3 * ((double) w);
  Serial.print(F("VOUT: "));
  Serial.println(v);
}

//! Read input voltage.
//! @return void
void read_input_voltage()
{
  uint16_t w;
  double v;

  w = smbus->readWord(LTC7841_I2C_ADDRESS, LTC7841_READ_VIN);

  v = 10.0E-3 * ((double) w);
  Serial.print(F("VIN: "));
  Serial.println(v);
}

//! Read input current.
//! @return void
void read_input_current()
{
  uint16_t w;
  double v;
  double R = 0.002;

  w = smbus->readWord(LTC7841_I2C_ADDRESS, LTC7841_READ_IIN);

  v = 50.0E-6 * R * ((double) w);
  Serial.print(F("IIN: "));
  Serial.println(v);
}

//! Read output current.
//! @return void
void read_output_current()
{
  uint16_t w;
  double v;
  double R = 0.002;

  w = smbus->readWord(LTC7841_I2C_ADDRESS, LTC7841_READ_IOUT);

  v = 50.0E-6 * R * ((double) w);
  Serial.print(F("IOUT: "));
  Serial.println(v);
}

//! Set slew.
//! @return void
void set_slew(uint8_t s)
{
  uint8_t v;

  v = smbus->readByte(LTC7841_I2C_ADDRESS, LTC7841_MFR_CONFIG);
  v &= 0xFC;
  v |= s;
  smbus->writeByte(LTC7841_I2C_ADDRESS, LTC7841_MFR_CONFIG, v);
}

//! Read status.
//! @return void
void read_status()
{
  uint16_t w;

  w = smbus->readWord(LTC7841_I2C_ADDRESS, LTC7841_STATUS_WORD);
  Serial.print(F("STATUS WORD: "));
  if (w & LTC7841_STATUS_NONE) Serial.print(F("NONE "));
  if (w & LTC7841_STATUS_COM) Serial.print(F("COM "));
  if (w & LTC7841_STATUS_TEMP) Serial.print(F("TEMP "));
  if (w & LTC7841_STATUS_VOUT_OV) Serial.print(F("VOUT_OV "));
  if (w & LTC7841_STATUS_OFF) Serial.print(F("OFF "));
  if (w & LTC7841_STATUS_PGOODB) Serial.print(F("PGOODB "));
  if (w & LTC7841_STATUS_VOUT) Serial.print(F("VOUT "));
  Serial.println();
}