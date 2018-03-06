/*!
Linear Technology DC2675A + DC2363A Demonstration Board
LTC2972 + LTM4644: Power Management Solution for Application Processors

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim

http://www.linear.com/product/LTC2972

http://www.linear.com/demo/DC2675A


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
    @ingroup LTC2972
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_I2CBus.h"
#include "LT_SMBusNoPec.h"
#include "LT_SMBusPec.h"
#include "LT_PMBusMath.h"
#include "LT_PMBus.h"

#define LTC2972_0_I2C_ADDRESS 0x5C
#define LTC2972_1_I2C_ADDRESS 0x5D
#define MFR_PG_CONFIG_ORIGINAL 0xC046
#define MFR_PG_CONFIG_WRITE_PG 0xC044

// Global variables
static uint8_t ltc2972_0_i2c_address;
static uint8_t ltc2972_1_i2c_address;

static LT_PMBusMath *math = new LT_PMBusMath();
static LT_SMBus *smbus = new LT_SMBusPec();
static LT_PMBus *pmbus = new LT_PMBus(smbus);

//! Initialize Linduino
//! @return void
void setup()
{
  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  ltc2972_0_i2c_address = LTC2972_0_I2C_ADDRESS;
  ltc2972_1_i2c_address = LTC2972_1_I2C_ADDRESS;

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
        menu_1_input_telemetry();                 // Print single-ended voltage menu
        break;
      case 2:
        menu_2_output_telemetry();
        break;
      case 3:
        print_all_status();
        break;
      case 4:
        menu_3_pg_commands();
        break;
      case 5:
        menu_4_margin_commands();
        break;
      case 6:
        menu_5_pec_commands();
        break;
      case 7:
        sequence_off_on();
        break;
      case 8:
        addresses = smbus->probe(0);
        while (*addresses != 0)
        {
          Serial.print(F("ADDR 0x"));
          Serial.println(*addresses++, HEX);
        }
        break;
      case 9 :
        pmbus->startGroupProtocol();
        pmbus->restoreFromNvm(ltc2972_0_i2c_address);
        pmbus->restoreFromNvm(ltc2972_1_i2c_address);
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
  Serial.print(F("* DC2675A Hello World Demonstration Program                     *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send and receive data from   *\n"));
  Serial.print(F("* the LTC2675A  demo board.                                     *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
//! @return void
void print_prompt()
{
  Serial.println();
  Serial.print(F("  1-Read Input Telemetry\n"));
  Serial.print(F("  2-Read Output Telemetry\n"));
  Serial.print(F("  3-Read All Status\n"));
  Serial.print(F("  4-PG Commands\n"));
  Serial.print(F("  5-Margin Commands\n"));
  Serial.print(F("  6-PEC Options\n"));
  Serial.print(F("  7-Sequence Off/On\n"));
  Serial.print(F("  8-Bus Probe\n"));
  Serial.print(F("  9-Reset\n"));
  Serial.print(F("\nEnter a command:"));
}

//! Prints a warning if the demo board is not detected.
//! @return void
void print_warning_prompt()
{
  Serial.println(F("\nWarning: Demo board not detected. Linduino will attempt to proceed."));
}

//! Print all output voltages
//! @return void
void print_all_output_voltages()
{
  float voltage;
  uint8_t page;

  for (page = 0; page < 2; page++)
  {
    pmbus->setPage(ltc2972_0_i2c_address, page);
    voltage = pmbus->readVout(ltc2972_0_i2c_address, false);
    Serial.print(F("  LTC2972:U0:CH"));
    Serial.print(page, DEC);
    Serial.print(F(" VOUT = "));
    Serial.println(voltage, DEC);

  }

  for (page = 0; page < 2; page++)
  {
    pmbus->setPage(ltc2972_1_i2c_address, page);
    voltage = pmbus->readVout(ltc2972_1_i2c_address, false);
    Serial.print(F("  LTC2972:U1:CH"));
    Serial.print(page, DEC);
    Serial.print(F(" VOUT = "));
    Serial.println(voltage, DEC);
  }
}

//! Print all input voltages
//! @return void
void print_all_input_voltages()
{
  float voltage;

  voltage = pmbus->readVin(ltc2972_0_i2c_address, false);
  Serial.print(F("  LTC2972:U0 VIN = "));
  Serial.println(voltage, DEC);
  voltage = pmbus->readVin(ltc2972_1_i2c_address, false);
  Serial.print(F("  LTC2972:U1 VIN = "));
  Serial.println(voltage, DEC);

}

//! Print all output currents
//! @return void
void print_all_output_currents()
{
  float current;
  uint8_t page;

  for (page = 0; page < 2; page++)
  {
    pmbus->setPage(ltc2972_0_i2c_address, page);
    current = pmbus->readIout(ltc2972_0_i2c_address, false);
    Serial.print(F("  LTC2972:U0:CH"));
    Serial.print(page, DEC);
    Serial.print(F(" IOUT = "));
    Serial.println(current, DEC);
  }

  for (page = 0; page < 2; page++)
  {
    pmbus->setPage(ltc2972_1_i2c_address, page);
    current = pmbus->readIout(ltc2972_1_i2c_address, false);
    Serial.print(F("  LTC2972:U1:CH"));
    Serial.print(page, DEC);
    Serial.print(F(" IOUT = "));
    Serial.println(current, DEC);
  }
}

//! Print all input currents
//! @return void
void print_all_input_currents()
{
  float current;

  current = pmbus->readIin(ltc2972_0_i2c_address, false);
  Serial.print(F("  LTC2972:U0 Iin = "));
  Serial.println(current, DEC);
  current = pmbus->readIin(ltc2972_1_i2c_address, false);
  Serial.print(F("  LTC2972:U1 Iin = "));
  Serial.println(current, DEC);

}

//! Print all output power
//! @return void
void print_all_output_power()
{
  float power;
  uint8_t page;

  for (page = 0; page < 2; page++)
  {
    pmbus->setPage(ltc2972_0_i2c_address, page);
    power = pmbus->readPout(ltc2972_0_i2c_address, false);
    Serial.print(F("  LTC2972:U0:CH"));
    Serial.print(page, DEC);
    Serial.print(F(" POUT = "));
    Serial.println(power, DEC);
  }

  for (page = 0; page < 2; page++)
  {
    pmbus->setPage(ltc2972_1_i2c_address, page);
    power = pmbus->readPout(ltc2972_1_i2c_address, false);
    Serial.print(F("  LTC2972:U1:CH"));
    Serial.print(page, DEC);
    Serial.print(F(" POUT = "));
    Serial.println(power, DEC);
  }
}

//! Print all input power
//! @return void
void print_all_input_power()
{
  float power;

  power = pmbus->readPin(ltc2972_0_i2c_address, false);
  Serial.print(F("  LTC2972:U0 Pin = "));
  Serial.println(power, DEC);
  power = pmbus->readPin(ltc2972_1_i2c_address, false);
  Serial.print(F("  LTC2972:U1 Pin = "));
  Serial.println(power, DEC);

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
    Serial.print(F("U0:CH"));
    Serial.println(page, DEC);
    pmbus->setPage(ltc2972_0_i2c_address, page);
    b = pmbus->readStatusByte(ltc2972_0_i2c_address);
    Serial.print(F("   LTC2972 STATUS BYTE 0x"));
    Serial.println(b, HEX);
    w = pmbus->readStatusWord(ltc2972_0_i2c_address);
    Serial.print(F("   LTC2972 STATUS WORD 0x"));
    Serial.println(w, HEX);
  }

  for (page = 0; page < 2; page++)
  {
    Serial.print(F("U1:CH"));
    Serial.println(page, DEC);
    pmbus->setPage(ltc2972_1_i2c_address, page);
    b = pmbus->readStatusByte(ltc2972_1_i2c_address);
    Serial.print(F("   LTC2972 STATUS BYTE 0x"));
    Serial.println(b, HEX);
    w = pmbus->readStatusWord(ltc2972_1_i2c_address);
    Serial.print(F("   LTC2972 STATUS WORD 0x"));
    Serial.println(w, HEX);
  }
}

//! Print all PG states
//! @return void
void print_all_pg()
{
  uint16_t pg_status;

  pg_status = smbus->readWord(ltc2972_0_i2c_address, 0xE5);
  pg_status &= 0x03;
  Serial.print(F("  PG0 = LOGIC"));
  if (pg_status & 0x01)
  {
    Serial.print(F(" HIGH\n"));
  }
  else
  {
    Serial.print(F(" LOW\n"));
  }
  Serial.print(F("  PG1 = LOGIC"));
  if (pg_status & 0x02)
  {
    Serial.print(F(" HIGH\n"));
  }
  else
  {
    Serial.print(F(" LOW\n"));
  }

  pg_status = smbus->readWord(ltc2972_1_i2c_address, 0xE5);
  pg_status &= 0x03;
  Serial.print(F("  PG2 = LOGIC"));
  if (pg_status & 0x01)
  {
    Serial.print(F(" HIGH\n"));
  }
  else
  {
    Serial.print(F(" LOW\n"));
  }
  Serial.print(F("  PG3 = LOGIC"));
  if (pg_status & 0x02)
  {
    Serial.print(F(" HIGH\n"));
  }
  else
  {
    Serial.print(F(" LOW\n"));
  }
}


//! Toggle PG0
//! @return void
void toggle_pg0()
{

  pmbus->setPage(ltc2972_0_i2c_address, 0);
  smbus->writeWord(ltc2972_0_i2c_address, 0xCB, MFR_PG_CONFIG_WRITE_PG); //! enable writing to pg register
  smbus->writeByte(ltc2972_0_i2c_address, 0xCE, 0x00); //! force pg low
  delay(1000);
  smbus->writeByte(ltc2972_0_i2c_address, 0xCE, 0x01); //! return pg to hi-z
  smbus->writeWord(ltc2972_0_i2c_address, 0xCB, MFR_PG_CONFIG_ORIGINAL);  //! return register to original state
}

//! Toggle PG0
//! @return void
void toggle_pg1()
{
  pmbus->setPage(ltc2972_0_i2c_address, 1);
  smbus->writeWord(ltc2972_0_i2c_address, 0xCB, MFR_PG_CONFIG_WRITE_PG);
  smbus->writeByte(ltc2972_0_i2c_address, 0xCE, 0x00);
  delay(1000);
  smbus->writeByte(ltc2972_0_i2c_address, 0xCE, 0x01);
  smbus->writeWord(ltc2972_0_i2c_address, 0xCB, MFR_PG_CONFIG_ORIGINAL);
}

//! Toggle PG2
//! @return void
void toggle_pg2()
{
  pmbus->setPage(ltc2972_1_i2c_address, 0);
  smbus->writeWord(ltc2972_1_i2c_address, 0xCB, MFR_PG_CONFIG_WRITE_PG);
  smbus->writeByte(ltc2972_1_i2c_address, 0xCE, 0x00);
  delay(1000);
  smbus->writeByte(ltc2972_1_i2c_address, 0xCE, 0x01);
  smbus->writeWord(ltc2972_1_i2c_address, 0xCB, MFR_PG_CONFIG_ORIGINAL);
}

//! Toggle PG3
//! @return void
void toggle_pg3()
{
  pmbus->setPage(ltc2972_1_i2c_address, 1);
  smbus->writeWord(ltc2972_1_i2c_address, 0xCB, MFR_PG_CONFIG_WRITE_PG);
  smbus->writeByte(ltc2972_1_i2c_address, 0xCE, 0x00);
  delay(1000);
  smbus->writeByte(ltc2972_1_i2c_address, 0xCE, 0x01);
  smbus->writeWord(ltc2972_1_i2c_address, 0xCB, MFR_PG_CONFIG_ORIGINAL);
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
//! @return void
void menu_1_input_telemetry()
{
  uint8_t user_command;

  do
  {
    //! Displays the Input Telemetry menu
    Serial.println();
    Serial.print(F("  1-Read Input Voltages\n"));
    Serial.print(F("  2-Read Input Currents\n"));
    Serial.print(F("  3-Read Input Power\n"));
    Serial.print(F("  4-Read All Input Telemetry\n"));
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
        print_all_input_voltages();
        break;
      case 2:
        print_all_input_currents();
        break;
      case 3:
        print_all_input_power();
        break;
      case 4:
        Serial.println();
        print_all_input_voltages();
        Serial.println();
        print_all_input_currents();
        Serial.println();
        print_all_input_power();
        break;
      default:
        if (user_command != 'm')
          Serial.println(F("Invalid Selection"));
        break;
    }
  }
  while (user_command != 'm');
}

//! Display menu 2
//! @return void
void menu_2_output_telemetry()
{
  uint8_t user_command;

  do
  {
    //! Displays the Ouput Telemetry menu
    Serial.println();
    Serial.print(F("  1-Read Output Voltages\n"));
    Serial.print(F("  2-Read Output Currents\n"));
    Serial.print(F("  3-Read Output Power\n"));
    Serial.print(F("  4-Read All Output Telemetry\n"));
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
        print_all_output_voltages();
        break;
      case 2:
        print_all_output_currents();
        break;
      case 3:
        print_all_output_power();
        break;
      case 4:
        Serial.println();
        print_all_output_voltages();
        Serial.println();
        print_all_output_currents();
        Serial.println();
        print_all_output_power();
        break;
      default:
        if (user_command != 'm')

          Serial.println(F("Invalid Selection"));
        break;
    }
  }
  while (user_command != 'm');
}

//! Display menu 3
//! @return void
void menu_3_pg_commands()
{
  uint8_t user_command;

  do
  {
    //! Displays the PG Command menu
    Serial.println();
    Serial.print(F("  1-Read All PG States\n"));
    Serial.print(F("  2-Force PG0 Low (1 sec)\n"));
    Serial.print(F("  3-Force PG1 Low (1 sec)\n"));
    Serial.print(F("  4-Force PG2 Low (1 sec)\n"));
    Serial.print(F("  5-Force PG3 Low (1 sec)\n"));
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
        print_all_pg();
        break;
      case 2:
        toggle_pg0();
        break;
      case 3:
        toggle_pg1();
        break;
      case 4:
        toggle_pg2();
        break;
      case 5:
        toggle_pg3();
        break;
      default:
        if (user_command != 'm')
          Serial.println(F("Invalid Selection"));
        break;
    }
  }
  while (user_command != 'm');
}


//! Display menu 4
//! @return void
void menu_4_margin_commands()
{
  uint8_t user_command;

  do
  {
    //! Displays the Margin menu
    Serial.println();
    Serial.print(F("  1-Margin High\n"));
    Serial.print(F("  2-Margin Low\n"));
    Serial.print(F("  3-Margin Off\n"));
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
        margin_high();
        delay(500);
        print_all_output_voltages();
        break;
      case 2:
        margin_low();
        delay(500);
        print_all_output_voltages();
        break;
      case 3:
        margin_off();
        delay(500);
        print_all_output_voltages();
        break;
      default:
        if (user_command != 'm')
          Serial.println(F("Invalid Selection"));
        break;
    }
  }
  while (user_command != 'm');
}

//! Display menu 5
//! @return void
void menu_5_pec_commands()
{
  uint8_t user_command;

  do
  {
    //! Displays the PEC menu
    Serial.println();
    Serial.print(F("  1-PEC On\n"));
    Serial.print(F("  2-PEC Off\n"));
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
        pmbus->enablePec(ltc2972_0_i2c_address);
        pmbus->enablePec(ltc2972_1_i2c_address);
        delete smbus;
        delete pmbus;
        smbus = new LT_SMBusPec();
        pmbus = new LT_PMBus(smbus);
        Serial.print(F("\n  PEC Enabled"));
        break;
      case 2:
        pmbus->enablePec(ltc2972_0_i2c_address);
        pmbus->enablePec(ltc2972_1_i2c_address);
        delete smbus;
        delete pmbus;
        smbus = new LT_SMBusNoPec();
        pmbus = new LT_PMBus(smbus);
        Serial.print(F("\n  PEC Disabled"));
        break;
      default:
        if (user_command != 'm')
          Serial.println(F("Invalid Selection"));
        break;
    }
  }
  while (user_command != 'm');
}

