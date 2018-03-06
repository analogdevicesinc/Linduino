/*!
DC2217A
LTC4316: Single/Dual I2C SMBUS ADDRESS REMAPPER

@verbatim

LTC4316 Remapper Menu
    1. Scan Bus for Addresses
    2. Control On Board DACs

Control On Board DACs:
    1. Turn on Through DAC
    2. Turn on Remapped DAC
    3. Turn off Through DAC
    4. Turn off Remapped DAC

NOTES
 Setup:
 Set the terminal baud rate to 115200 and select the newline terminator.
 Requires a power supply.
 Refer to demo manual DC2217A.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC4316

http://www.linear.com/product/LTC4316#demoboards


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

//! @ingroup Transceivers
//! @{
//! @defgroup LTC4316 LTC4316: Single/Dual I2C SMBUS ADDRESS REMAPPER
//! @}

/*! @file
    @ingroup LTC4316
    Library for LTC4316: Single/Dual I2C SMBUS ADDRESS REMAPPER
*/

#include <Arduino.h>
#include "LT_I2C.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "Linduino.h"
#include <Wire.h>
#include <stdint.h>



static int8_t demo_board_connected;        //!< Set to 1 if the board is connected

const char ack_error[] = "Error: No Acknowledge. Check I2C Address.";    //!< Error message

const int LTC4316_OFFSET = 0x78;               //!< Demoboard offset. Change to correct value if EXT offset if chosen.
const int LTC2631_GLOBAL_ADDRESS = 0x73;       //!< Global address of LTC2631 DAC
const int LTC2631_JP_ADDRESS_1 = 0x12;         //!< Jumper Setting 1 on Demoboard
const int LTC2631_JP_ADDRESS_2 = 0x11;         //!< Jumper Setting 2 on Demoboard
const int LTC2631_JP_ADDRESS_3 = 0x10;         //!< Jumper Setting 3 on Demoboard
const int EEPROM_ADDRESS = 0x50;               //!< EEPROM Address on Demoboard

//! Initialize Linduino
void setup()
{
  char demo_name[] = "DC2217A";      // Demo Board Name stored in QuikEval EEPROM
  quikeval_I2C_init();              //! Configure the EEPROM I2C port for 100kHz
  quikeval_I2C_connect();           //! Connects to main I2C port
  Serial.begin(115200);             //! Initialize the serial port to the PC
  print_title();                    //! Print Title
  demo_board_connected = 1;
  if (!demo_board_connected)
  {
    Serial.println(F("Demo board not detected, will attempt to proceed"));
    demo_board_connected = true;
  }
  if (demo_board_connected)
  {
    print_prompt();
  }
}

//! Repeats Linduino Loop
void loop()
{

  int8_t ack = 0;                                             //! I2C acknowledge indicator
  static uint8_t user_command;                                //! The user input command
  if (demo_board_connected)                                   //! Do nothing if the demo board is not connected
  {
    if (Serial.available())                                   //! Do nothing if serial is not available
    {
      user_command = read_int();                              //! Read user input command
      if (user_command != 'm')
        Serial.println(user_command);
      Serial.println();
      ack = 0;
      switch (user_command)                                    //! Check user input.
      {
        case 1:
          menu_1_scan_addresses();                              //!< Perform I2C Address Scan
          break;
        case 2:
          ack |= menu_2_DAC();                                  //!< Control on board DACs
          break;
        default:
          Serial.println("Incorrect Option");
      }

      if (ack != 0)
        Serial.println(ack_error);
      Serial.print(F("*************************"));
      print_prompt();
    }
  }
}



// Function Definitions
//! Print the title block
void print_title()
{
  Serial.println(F("\n*****************************************************************"));
  Serial.print(F("* DC2156 Demonstration Program                                  *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates the functionality of the LTC4316    *\n"));
  Serial.print(F("* which is a Single/Dual I2C SMBus Remappper. The program       *\n"));
  Serial.print(F("* scans the addresses present on the bus and controls the       *\n"));
  Serial.print(F("* onboard remapped DACs to light LEDs                           *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Print the main menu prompt
void print_prompt()
{
  Serial.print(F("\n1- Scan Bus for Addresses\n"));
  Serial.print(F("2- Control On Board DACs\n"));
  Serial.print(F("\n"));
  Serial.print(F("Enter a command: "));
}

//! Scan I2C Bus for addresses
void menu_1_scan_addresses()
{

  int maxAddr = 127;                                    //! Max Number for possible 7-bit address values
  int addrCount = 0;                                    //! Initialization of address count.

  for (int addr = 0x00; addr <= maxAddr; addr++)        //! Scan possible address values till max address number
  {
    int8_t ret= 0 ;                                    //! Initialize return bit variable
    if (i2c_start()!=0)                                        //! I2C START
    {
      Serial.print(F("Bus is Busy. Aborting..."));             //! Stop and abort scanning if bus is busy.
      break;
    }

    ret |= i2c_write((addr<<1)|I2C_WRITE_BIT);        //! Write the I2C 7 bit address with W bit
    i2c_stop();                                       //! I2C STOP
    if (ret==0)                                       //! If acknolwege recieved on bus, print address
    {
      addrCount++;                                    //! Increment address count
      Serial.print(F("\n Address Found - #"));
      Serial.print(addrCount);                        //! Print address count
      Serial.print(F(": "));
      Serial.print(addr, HEX);                        //! Print address

      switch (addr)                                   //! Switch statement to identify factory set possible address values. Note: If external address has the same address as any of the jumper settings
        //! it might register as one of the switch statements
      {
        case EEPROM_ADDRESS:
          Serial.print(F(" - Onboard EEPROM Address."));
          break;
        case (LTC2631_GLOBAL_ADDRESS):
          Serial.print(F(" - Through Global Address of LTC2631"));
          break;
        case (LTC2631_GLOBAL_ADDRESS ^ LTC4316_OFFSET):
          Serial.print(F(" - Remapped Global Address of LTC2631"));
          break;
        case (LTC2631_JP_ADDRESS_1):
          Serial.print(F(" - Through Jumper Setting of Onboard LTC2631"));
          break;
        case (LTC2631_JP_ADDRESS_2):
          Serial.print(F(" - Through Jumper Setting of Onboard LTC2631"));
          break;
        case (LTC2631_JP_ADDRESS_3):
          Serial.print(F(" - Through Jumper Setting of Onboard LTC2631"));
          break;
        case (LTC2631_JP_ADDRESS_1 ^ LTC4316_OFFSET):
          Serial.print(F(" - Remapped Address of Onboard LTC2631 Based On Jumper Setting 12h"));
          break;
        case (LTC2631_JP_ADDRESS_2 ^ LTC4316_OFFSET):
          Serial.print(F(" - Remapped Address of Onboard LTC2631 Based On Jumper Setting 11h"));
          break;
        case (LTC2631_JP_ADDRESS_3 ^ LTC4316_OFFSET):
          Serial.print(F(" - Remapped Address of Onboard LTC2631 Based On Jumper Setting 11h"));
          break;
        default:
          Serial.print(F(" - Unknown Address Found. Possible external device connected and/or Non-Factory address selected."));
          break;
      }
      Serial.print("\n\n");
    }
  }

  if (addrCount == 0)
    Serial.println(F("Nothing Found on Bus"));
}

//! DAC Control Menu
uint8_t menu_2_DAC()
{
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.

  int ack = 0;                                                    //! I2C acknowledge indicator
  static uint8_t user_command;                                    //! The user input command
  int maxAddr = 127;                                              //! Max Number for possible 7-bit address values
  int addrCount = 0;                                              //! Initialization of address count.
  uint8_t LTC2631_CMD_WRITE_UPDATE = 0x30;                        //! LTC2631 Write and Update Command Byte
  uint16_t LTC2631_FULLSCALE_OUTPUT = 0xFFC0;                     //! LTC2631 10 bit fullscale input value
  uint8_t through_address = 0;                                    //! Through address value
  uint8_t remapped_address = 0;                                   //! Remapped address value

  for (int addr = 0x00; addr <= maxAddr; addr++)
  {
    int8_t ret= 0 ;

    if (i2c_start()!=0)                                        //I2C START
    {
      Serial.print(F("Bus is Busy. Aborting..."));             //Stop and abort scanning if bus is busy.
      break;
    }

    ret |= i2c_write((addr<<1)|I2C_WRITE_BIT);        //Write the I2C 7 bit address with W bit
    i2c_stop();                                       //I2C STOP
    if (ret==0)                                       // Returns 1 if failed
    {
      switch (addr)                                   //Switch statement to assign addresses based on default demoboard configuration. External addresses are not supported and could cause conflict.
      {
        case LTC2631_JP_ADDRESS_1:
        case LTC2631_JP_ADDRESS_2:
        case LTC2631_JP_ADDRESS_3:
          through_address = addr;
          break;
        case LTC2631_JP_ADDRESS_1 ^ LTC4316_OFFSET:
        case LTC2631_JP_ADDRESS_2 ^ LTC4316_OFFSET:
        case LTC2631_JP_ADDRESS_3 ^ LTC4316_OFFSET:
          remapped_address = addr;
          break;
        case EEPROM_ADDRESS:
        case LTC2631_GLOBAL_ADDRESS:
        case LTC2631_GLOBAL_ADDRESS ^ LTC4316_OFFSET:
          break;
        default:
          Serial.print(F(" - Unknown Address Found. Possible external device connected and/or Non-Factory address selected."));
          break;
      }
      Serial.print("\n\n");
    }
  }

  do
  {
    Serial.print(F("*************************\n\n"));
    Serial.print(F("Turn on DACs\n"));
    Serial.print(F("1-Turn on Through DAC\n"));
    Serial.print(F("2-Turn on Remapped DAC\n"));
    Serial.print(F("3-Turn off Through DAC\n"));
    Serial.print(F("4-Turn off Through DAC\n"));
    Serial.print(F("m-Main Menu\n\n"));
    Serial.print(F("Enter a command: "));

    user_command = read_int();                              //! Read user input command
    if (user_command != 'm')
      Serial.println(user_command);
    if (user_command == 'm')
      Serial.println("m");
    Serial.println();
    ack = 0;
    switch (user_command)                                         //! Switch statement for DAC menu
    {
      case 1:
        ack |= i2c_write_word_data(through_address,LTC2631_CMD_WRITE_UPDATE, LTC2631_FULLSCALE_OUTPUT);       //! I2C write command to turn Through DAC on
        break;
      case 2:
        ack |= i2c_write_word_data(remapped_address,LTC2631_CMD_WRITE_UPDATE, LTC2631_FULLSCALE_OUTPUT);      //! I2C write command to turn Remapped DAC on
        break;
      case 3:
        ack |= i2c_write_word_data(through_address, LTC2631_CMD_WRITE_UPDATE, 0x0000);                        //! I2c write command to turn through DAC off
        break;
      case 4:
        ack |= i2c_write_word_data(remapped_address, LTC2631_CMD_WRITE_UPDATE, 0x0000);                        //! I2C write command to turn remapped DAC off
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
    }
  }
  while (!(user_command == 'm' || (ack)));
  if (ack != 0)
    Serial.println(ack_error);
  return ack;
}
