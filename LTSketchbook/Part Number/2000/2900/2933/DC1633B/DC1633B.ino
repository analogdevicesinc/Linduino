/*!
Linear Technology DC1633B Demonstration Board.
LTC2933: Programable Hex Voltage Supervisor with EEPROM

@verbatim

  Setup:
   Power the demo board through the Linduino connector (12-pin adapter required).

   Set the Linduino terminal baud rate to 115200 and select the newline terminator.

@endverbatim

http://www.linear.com/product/LTC2933

http://www.linear.com/demo/DC1633B


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
    @ingroup LTC2933
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_I2CBus.h"
#include "Wire.h"
#include "LT_SMBusNoPec.h"
#include "LTC2933.h"

// LTC2637 DAC on the DC1633B board
#define DC1633_DAC_ADDRESS 0x22 //pin-strapped address for DAC

//LTC2933 I2C address on the board (selectable by a jumper)
#define LTC2933_I2C_ADDRESS LTC2933_I2C_GLOBAL_ADDRESS

// Global variables
static uint8_t ltc2933_i2c_address;
static uint8_t dc1633_dac_address;
static LT_SMBus *smbus = new LT_SMBusNoPec();

//! Initialize Linduino
//! @return void
void setup()
{
  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  ltc2933_i2c_address = LTC2933_I2C_ADDRESS;
  dc1633_dac_address = DC1633_DAC_ADDRESS;
  print_prompt();
}

//! Repeats Linduino loop
//! @return void
void loop()
{
  uint8_t user_command;
  uint8_t *addresses = NULL;
  uint16_t res;

  if (Serial.available())                          //! Checks for user input
  {
    user_command = read_int();                     //! Reads the user command
    if (user_command != 'm')
      Serial.println(user_command);

    switch (user_command)                          //! Prints the appropriate submenu
    {
      case 0:
        break;
      case 1:
        //! write all LTC2933 registers with demo-board default settings
        //! all channels except 1 expect 2.0v from the DAC
        ltc2933_demo_board_defaults(ltc2933_i2c_address);
        break;
      case 2:
        //! read the LTC2933 STATUS_WORD register
        res = smbus->readWord(ltc2933_i2c_address, LTC2933_STATUS_WORD);
        Serial.println(res, HEX);
        break;
      case 3:
        //! read all LTC2933 registers
        ltc2933_read_registers(ltc2933_i2c_address);
        break;
      case 4:
        //! write to the LTC2637 DACs on the DC1633 board to interesting voltages
        //!  other than the 2.0v DAC defaults
        dc1633_write_dac_voltage(dc1633_dac_address, 0, 3.3);
        dc1633_write_dac_voltage(dc1633_dac_address, 1, 2.5);
        dc1633_write_dac_voltage(dc1633_dac_address, 2, 1.8);
        dc1633_write_dac_voltage(dc1633_dac_address, 3, 1.5);
        dc1633_write_dac_voltage(dc1633_dac_address, 4, 1.2);
        dc1633_write_dac_voltage(dc1633_dac_address, 5, 1.0);
        break;
      case 5:
        //! write all LTC2933 registers with more interesting voltage settings
        ltc2933_demo_board_demo_thresholds(ltc2933_i2c_address);
        break;
      case 6:
        // margin the supplies to the 20% high side
        // write to the LTC2637 DACs on the DC1633 board
        dc1633_write_dac_voltage(dc1633_dac_address, 0, 3.96);
        dc1633_write_dac_voltage(dc1633_dac_address, 1, 3.0);
        dc1633_write_dac_voltage(dc1633_dac_address, 2, 2.16);
        dc1633_write_dac_voltage(dc1633_dac_address, 3, 1.8);
        dc1633_write_dac_voltage(dc1633_dac_address, 4, 1.44);
        dc1633_write_dac_voltage(dc1633_dac_address, 5, 1.2);
        break;
      case 7:
        //! write the LTC2933 CLEAR_ALERT
        ltc2933_clear_alertb(ltc2933_i2c_address);
        break;
      case 8:
        addresses = smbus->probe(0);
        while (*addresses != 0)
        {
          Serial.print(F("ADDR 0x"));
          Serial.println(*addresses++, HEX);
        }
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
  Serial.print(F("* DC1633B Demonstration Program                                 *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send and receive data from   *\n"));
  Serial.print(F("* the DC1633B demo board.                                       *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("  1-Write LTC2933 registers with default settings\n"));
  Serial.print(F("  2-Read Status (STATUS_WORD)\n"));
  Serial.print(F("  3-Read All Registers\n"));
  Serial.print(F("  4-Set DAC voltages on Vn pins to interesting values\n"));
  Serial.print(F("  5-Change LTC2933 voltage thresholds to match interesting DAC voltages\n"));
  Serial.print(F("  6-Set DAC voltages on Vn pins to +20% high values\n"));
  Serial.print(F("  7-Clear LTC2933 ALERTB\n"));
  Serial.print(F("  8-Bus Probe\n"));
  Serial.print(F("\nEnter a command:"));
}

//! Prints a warning if the demo board is not detected.
void print_warning_prompt()
{
  Serial.println(F("\nWarning: Demo board not detected. Linduino will attempt to proceed."));
}


//! Return 1 if the LTC2933 is write-protected
//  0 otherwise
int ltc2933_is_write_protected(uint8_t ltc2933_i2c_address)
{
  uint16_t res;

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_STATUS_WORD);

  //b[0] is the write-protect bit
  return ((res&0x0001) == 0x0001) ? 1 : 0;
}

//! Read all registers from RAM
void ltc2933_read_registers(uint8_t ltc2933_i2c_address)
{
  uint16_t res;
  res = smbus->readWord(ltc2933_i2c_address, LTC2933_WRITE_PROTECT);
  Serial.print(F("\n LTC2933_WRITE_PROTECT:"));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_GPI_CONFIG);
  Serial.print(F("\n LTC2933_GPI_CONFIG: "));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_GPIO1_CONFIG);
  Serial.print(F("\n LTC2933_GPIO1_CONFIG: "));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address,  LTC2933_GPIO2_3_CONFIG);
  Serial.print(F("\n LTC2933_GPIO2_3_CONFIG: "));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_V1_THR);
  Serial.print(F("\n LTC2933_V1_THR: "));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_V2_THR);
  Serial.print(F("\n LTC2933_V2_THR: "));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_V3_THR);
  Serial.print(F("\n LTC2933_V3_THR: "));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_V4_THR);
  Serial.print(F("\n LTC2933_V4_THR: "));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_V5_THR);
  Serial.print(F("\n LTC2933_V5_THR: "));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_V6_THR);
  Serial.print(F("\n LTC2933_V6_THR: "));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_V1_CONFIG);
  Serial.print(F("\n LTC2933_V1_CONFIG: "));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_V2_CONFIG);
  Serial.print(F("\n LTC2933_V2_CONFIG: "));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_V3_CONFIG);
  Serial.print(F("\n LTC2933_V3_CONFIG: "));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_V4_CONFIG);
  Serial.print(F("\n LTC2933_V4_CONFIG: "));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_V5_CONFIG);
  Serial.print(F("\n LTC2933_V5_CONFIG: "));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_V6_CONFIG);
  Serial.print(F("\n LTC2933_V6_CONFIG: "));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_HISTORY_WORD);
  Serial.print(F("\n LTC2933_HISTORY_WORD: "));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_BACKUP_WORD);
  Serial.print(F("\n LTC2933_BACKUP_WORD: "));
  Serial.println(res, HEX);

  res = smbus->readWord(ltc2933_i2c_address, LTC2933_STATUS_WORD);
  Serial.print(F("\n LTC2933_STATUS_WORD: "));
  Serial.println(res, HEX);
  Serial.print(F("\n"));

}

//! Load demo-board default settings into RAM
void ltc2933_demo_board_defaults(uint8_t ltc2933_i2c_address)
{
  if (ltc2933_is_write_protected(ltc2933_i2c_address) != 1)
  {
    smbus->writeWord(ltc2933_i2c_address, LTC2933_WRITE_PROTECT, 0xAAA8);
    smbus->writeWord(ltc2933_i2c_address, LTC2933_GPI_CONFIG, 0x1040);
    smbus->writeWord(ltc2933_i2c_address, LTC2933_GPIO1_CONFIG, 0x002E);
    smbus->writeWord(ltc2933_i2c_address, LTC2933_GPIO2_3_CONFIG, 0x2E07);
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V1_THR, 0x412D); // ov = 5.5, uv = 4.5
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V2_THR, 0xAF87); // ov = 2.2, uv = 1.8
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V3_THR, 0xAF87); // ov = 2.2, uv = 1.8
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V4_THR, 0xAF87); // ov = 2.2, uv = 1.8
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V5_THR, 0xAF87); // ov = 2.2, uv = 1.8
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V6_THR, 0xAF87); // ov = 2.2, uv = 1.8
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V1_CONFIG, 0x009C); // high range
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V2_CONFIG, 0x019C); // low range
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V3_CONFIG, 0x019C); // low range
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V4_CONFIG, 0x019C); // low range
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V5_CONFIG, 0x019C); // low range
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V6_CONFIG, 0x019C); // low range
  }
  else
  {
    // error, LTC2933 is write-protected
    Serial.println(F("\nERROR: LTC2933 is write-protected. Cannot write to registers"));
  }
}

//! Load different voltage threshold settings into RAM
void ltc2933_demo_board_demo_thresholds(uint8_t ltc2933_i2c_address)
{
  if (ltc2933_is_write_protected(ltc2933_i2c_address) != 1)
  {
    smbus->writeWord(ltc2933_i2c_address, LTC2933_WRITE_PROTECT, 0xAAA8);
    smbus->writeWord(ltc2933_i2c_address, LTC2933_GPI_CONFIG, 0x1040);
    smbus->writeWord(ltc2933_i2c_address, LTC2933_GPIO1_CONFIG, 0x002E);
    smbus->writeWord(ltc2933_i2c_address, LTC2933_GPIO2_3_CONFIG, 0x2E07);
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V1_THR, 0x412D); // ov = 5.5, uv = 4.5
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V2_THR, 0x554B); // ov = 2.6, uv = 2.4
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V3_THR, 0x917D); // ov = 1.9, uv = 1.7
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V4_THR, 0x735F); // ov = 1.6, uv = 1.4
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V5_THR, 0x5541); // ov = 1.3, uv = 1.1
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V6_THR, 0x3C32); // ov = 1.05, uv = 0.95
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V1_CONFIG, 0x009C); // high range
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V2_CONFIG, 0x009C); // medium range
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V3_CONFIG, 0x019C); // low range
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V4_CONFIG, 0x019C); // low range
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V5_CONFIG, 0x019C); // low range
    smbus->writeWord(ltc2933_i2c_address, LTC2933_V6_CONFIG, 0x019C); // low range
  }
  else
  {
    // error, LTC2933 is write-protected
    Serial.println(F("\nERROR: LTC2933 is write-protected. Cannot write to registers"));
  }
}

//! Clear ALERTB
void ltc2933_clear_alertb(uint8_t ltc2933_i2c_address)
{
  smbus->writeWord(ltc2933_i2c_address, LTC2933_CLEAR_HISTORY, 0x0000);
}


//! program the DAC on the DC1633B demo board to a voltage
//  refer to the LTC2637 datasheet
void dc1633_write_dac_voltage(uint8_t dac_address, int channel, float voltage)
{
  uint8_t *data = (uint8_t *)malloc(3*sizeof(uint8_t));
  uint8_t cmd, ch_addr;
  uint16_t v_data;
  float v;

  // pack the data bytes with the necessary bits
  // channel numbers 0 - 7 correspond to letters A - H in the datasheet
  if ((channel < 8) && (channel >= 0))
  {
    ch_addr = (uint8_t)channel;
  }
  else
  {
    //address all channels
    ch_addr = 0x0F;
  }
  cmd = 0x30; // the write to and update command
  data[0] = cmd | ch_addr;

  if ((voltage > 0) && (voltage < 4.096))
  {
    v = (voltage/4.096);
    v_data = (uint16_t)(v*4096);
  }
  else
  {
    Serial.println(F("\nERROR: Voltage out of DAC range"));
  }
  data[1] = (uint8_t)(v_data >> 4); // most significant bits
  data[2] = (uint8_t)(v_data << 4); // least significant bits

  //write the command and data to the DAC
  Wire.beginTransmission(dac_address);
  Wire.write(data[0]);
  Wire.write(data[1]);
  Wire.write(data[2]);
  Wire.endTransmission(1);

  free(data);
  return;

}
