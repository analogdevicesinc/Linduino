/*!
Linear Technology DC1962C Demonstration Board
LTC3880, LTC2974, LTC2977: Power Management Solution for Application Processors

This sketch has only been tested on a Mega 2560. It is known to fail
on Aarduino Uno and Linduino due to size of RAM.

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
#include <Linduino.h>
#include <UserInterface.h>
#include <LT_PMBus.h>
#include <nvm.h>
#include <avr/boot.h>
#include "data.h"

#define SIGRD 5
#define LTC3880_I2C_ADDRESS 0x30
#define LTC2974_I2C_ADDRESS 0x32
#define LTC2977_I2C_ADDRESS 0x33

// Global variables
static uint8_t ltc3880_i2c_address;
static uint8_t ltc2974_i2c_address;
static uint8_t ltc2977_i2c_address;

static LT_SMBusNoPec *smbusNoPec = new LT_SMBusNoPec();
static LT_SMBusPec *smbusPec = new LT_SMBusPec();
static NVM *nvm = new NVM(smbusNoPec, smbusPec);

static LT_SMBus *smbus = smbusNoPec;
static LT_PMBus *pmbus = new LT_PMBus(smbus);

static bool pec = false;

static uint8_t dc1613_addresses[] = { LTC3880_I2C_ADDRESS, LTC2974_I2C_ADDRESS, LTC2977_I2C_ADDRESS };

//! Wait for nvm operation to complete
//! @return void
void wait_for_nvm()
{
  delay(2); // Allow time for action to start.

  smbus->waitForAck(ltc3880_i2c_address, 0x00);
  pmbus->waitForNotBusy(ltc3880_i2c_address);
  pmbus->waitForNvmDone(ltc3880_i2c_address);

  pmbus->waitForNotBusy(ltc2974_i2c_address);

  pmbus->waitForNotBusy(ltc2977_i2c_address);
}

//! Program the nvm
//! @return void
void program_nvm ()
{
  bool worked;

  Serial.println(F("Please wait for programming EEPROM..."));
  worked = nvm->programWithData(isp_data);
  wait_for_nvm();
  // Programming creates a fault when a Poll on Ack generates a Busy Fault just after
  // a Clear Fault Log.
  pmbus->clearFaults(ltc2977_i2c_address);
  Serial.println(F("Programming Complete (RAM and EEPROM may not match until reset)"));
}

//! Verify the nvm
//! @return void
void verify_nvm ()
{
  bool worked;

  Serial.println(F("Please wait for verification of EEPROM..."));
  worked = nvm->verifyWithData(isp_data);
  wait_for_nvm();

  if (worked == 0)
    Serial.println(F("Verification complete: Invalid EEPROM data"));
  else
  {
    Serial.println(F("Verification complete: Valid EEPROM data"));
  }

  // Ensure there is no write protect so that the clear faults option works.
  pmbus->disableWriteProtectGlobal();
}

//! Restore nvm to ram
//! @return void
void restore_nvm ()
{
  smbus->writeByte(LTC3880_I2C_ADDRESS, MFR_EEPROM_STATUS, 0x00);

  Serial.println(F("Restore User All"));
  pmbus->restoreFromNvmGlobal();
  wait_for_nvm();
  Serial.println(F("Restore Complete (EEPROM written to RAM)"));
}

//! Reset all devices
//! @return void
void reset_all_devices ()
{
  Serial.println(F("Resseting all devices"));
  pmbus->startGroupProtocol();
  pmbus->reset(ltc3880_i2c_address);
  pmbus->restoreFromNvm(ltc2974_i2c_address);
  pmbus->restoreFromNvm(ltc2977_i2c_address);
  pmbus->executeGroupProtocol();

  smbus->waitForAck(ltc3880_i2c_address, 0x00);
  pmbus->waitForNotBusy(ltc3880_i2c_address);

  pmbus->waitForNotBusy(ltc2974_i2c_address);

  pmbus->waitForNotBusy(ltc2977_i2c_address);
  Serial.println(F("All devices reset (RAM == EEPROM)"));
}

//! Initialize Linduino
//! @return void
void setup()
{
  Serial.begin(115200);         //! Initialize the serial port to the PC
  /*
  byte sig0;
  byte sig2;
  byte sig4;
  sig0 = boot_signature_byte_get (0);
  sig2 = boot_signature_byte_get (2);
  sig4 = boot_signature_byte_get (4);
  if (sig0 != 0x1E || sig2 != 0x98 | sig4 != 0x01)
  {
    Serial.println("Sketch only runs on Mega 2560");
    return;
  }
  */
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
        program_nvm();
        break;
      case 2:
        verify_nvm();
        // Lock the NVM in case the verify failed.
        pmbus->smbus()->writeByte(0x5b, MFR_EE_UNLOCK, 0x00);
        break;
      case 3:
        restore_nvm();
        break;
      case 4:
        program_nvm();
        reset_all_devices();
        break;
      case 5:
        pmbus->clearFaultsGlobal();
        break;
      case 6:
        pmbus->enablePec(ltc3880_i2c_address);
        pmbus->enablePec(ltc2974_i2c_address);
        pmbus->enablePec(ltc2977_i2c_address);
        smbus = smbusPec;
        pmbus->smbus(smbus);
        pec = true;
        break;
      case 7:
        pmbus->disablePec(ltc3880_i2c_address);
        pmbus->disablePec(ltc2974_i2c_address);
        pmbus->disablePec(ltc2977_i2c_address);
        smbus = smbusNoPec;
        pmbus->smbus(smbus);
        pec = false;
        break;
      case 8:
        addresses = smbus->probe(0);
        while (*addresses != 0)
        {
          Serial.print(F("ADDR 0x"));
          Serial.println(*addresses++, HEX);
        }
        break;
      case 9:
        reset_all_devices();
        break;
      default:
        Serial.println(F("Incorrect Option"));
        break;
    }
    print_prompt();
  }
}

//! Prints the title block when program first starts.
//! @return void
void print_title()
{
  Serial.print(F("\n********************************************************************\n"));
  Serial.print(F("* DC1962C In Flight Update Demonstration Program (MEGA 2560 Only)  *\n"));
  Serial.print(F("*                                                                  *\n"));
  Serial.print(F("* This program demonstrates how to program EEPROM from hex data.   *\n"));
  Serial.print(F("*                                                                  *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.   *\n"));
  Serial.print(F("*                                                                  *\n"));
  Serial.print(F("********************************************************************\n"));
}

//! Prints main menu.
//! @return void
void print_prompt()
{
  Serial.print(F("\n  1-Program\n"));
  Serial.print(F("  2-Verify\n"));
  Serial.print(F("  3-Restore\n"));
  Serial.print(F("  4-Program and Apply\n"));
  Serial.print(F("  5-Clear Faults\n"));
  Serial.print(F("  6-PEC On\n"));
  Serial.print(F("  7-PEC Off\n"));
  Serial.print(F("  8-Bus Probe\n"));
  Serial.print(F("  9-Reset\n"));
  Serial.print(F("\nEnter a command:"));
}
