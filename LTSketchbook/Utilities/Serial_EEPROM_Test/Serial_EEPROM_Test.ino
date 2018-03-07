/*
Serial EEPROM Test

This program will demonstrate how to communicate with a serial EEPROM
such as the 24LC025 device found on all QuikEval compatible demo board.

The routines require the use of the Arduino's hardware I2C port.

Pin Assignments:
SDA   PINC4  ATMega328 pin 27. Arduino pin A4. QuikEval pin 9.
SCL   PINC5  ATMega328 pin 28. Arduino pin A5. QuikEval pin 11.

DATA TYPES:
char  = 1 byte
int   = 2 bytes
long  = 4 bytes
float = 4 bytes


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

#define EEPROM_PAGE_SIZE  16    // EEPROM Page size in bytes
#define EEPROM_DATA_SIZE  256   // EEPROM size in bytes

#include <Arduino.h>        // Arduino specific header file
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"         // Hardware I2C driver
#include "QuikEval_EEPROM.h"
#include "UserInterface.h"  // serial interface routines to communicate with the user

// global variables
uint8_t i2c_address = 0xA0;    // The I2C address of the QuikEval EEPROM
unsigned char id_string_size = 50;   // The QuikEval ID string size
uint16_t eeprom_address = 0x00;    // The eeprom data address
byte terminator = 0x0a;              // The termination char for buffer reads
byte read_count = id_string_size;    // Byte read count for buffer reads
int int_data;                      // Integer test variable
long long_data;                    // Long test variable
float float_data;                  // Float test variable

// user command
unsigned char user_command;

void print_title()
// Print the title block
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* QuikEval Demo Board EEPROM Reader                            *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program will read the ID string from a QuikEval         *\n"));
  Serial.print(F("* compatible demo board using the Arduino's Hardware I2C port.  *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Input Data Format :                                           *\n"));
  Serial.print(F("*   decimal : 1024                                              *\n"));
  Serial.print(F("*   hex     : 0x400                                             *\n"));
  Serial.print(F("*   octal   : 02000  (leading 0 'Zero')                         *\n"));
  Serial.print(F("*   binary  : B10000000000                                      *\n"));
  Serial.print(F("*   float   : 1024.0                                            *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 select the newline terminator.    *\n"));
  Serial.print(F("*                                                               *\n"));
}

void print_prompt()
// Prompt the user for an input command
{
  Serial.print(F("*****************************************************************\n"));
  Serial.println("Present Values:");
  // I2C address
  Serial.print("  I2C Address: 0x");
  Serial.println(i2c_address, HEX);
  // EEPROM data address
  Serial.print("  EEPROM Address: 0x");
  Serial.println(eeprom_address, HEX);
  // EEPROM terminator
  Serial.print("  Buffer Read Terminator: 0x");
  Serial.println(terminator, HEX);
  // EEPROM buffer Read Count
  Serial.print("  Buffer Read Count: ");
  Serial.println(read_count);
  // Print the Command Summary
  Serial.println("\nCommand Summary:");
  Serial.println("  0-Buffer Read");
  Serial.println("  1-Buffer Read with Terminator");
  Serial.println("  2-Byte Read");
  Serial.println("  3-Buffer Write");
  Serial.println("  4-Byte Write");
  Serial.println("  5-Set I2C Address");
  Serial.println("  6-Set EEPROM Address");
  Serial.println("  7-Set Terminator");
  Serial.println("  8-Set Read Count");
  Serial.println("  9-Integer Write");
  Serial.println("  10-Integer Read");
  Serial.println("  11-Long Write");
  Serial.println("  12-Long Read");
  Serial.println("  13-Float Write");
  Serial.println("  14-Float Read");
  Serial.println("  15-Clear Calibration Settings On Demo Boards");
  Serial.println("");
  Serial.print("Enter a command: ");
}

void setup()
// Setup the program
{
  i2c_enable();          // Configure the Linduino for 100kHz I2C communications
  Serial.begin(115200);      // Initialize the serial port to the PC
  print_title();
  print_prompt();
}

void loop()
{
  unsigned char user_command;
  char data;
  byte byte_count;
  if (Serial.available())
  {
    user_command = read_int();
    Serial.println(user_command);
    switch (user_command)
    {
      case 0:
        // buffer read
        byte_count = eeprom_read_buffer(i2c_address, &ui_buffer[0], eeprom_address, read_count);
        if (byte_count != 0)
        {
          Serial.print("Data Received: ");
          Serial.println(ui_buffer);
        }
        else
          Serial.println("Error: No Data Received");
        break;
      case 1:
        // buffer read with terminator
        byte_count = eeprom_read_buffer_with_terminator(i2c_address, &ui_buffer[0], eeprom_address, terminator, read_count);
        if (byte_count != 0)
        {
          Serial.print("Data Received: ");
          Serial.println(ui_buffer);
        }
        else
          Serial.println("Error: No Data Received");
        break;
      case 2:
        // byte read
        byte_count = eeprom_read_byte(i2c_address, &data, eeprom_address);
        Serial.println("test");
        if (byte_count != 0)
        {
          Serial.print("Byte Received: 0x");
          Serial.print(data, HEX);
          Serial.print("(");
          Serial.print(char(ui_buffer[0]));
          Serial.println(")");
        }
        else
          Serial.println("Error: No Data Received");
        break;
      case 3:
        // buffer write
        Serial.print("Enter string to Write to EEPROM: ");
        read_string();
        byte_count = eeprom_write_buffer(i2c_address, &ui_buffer[0], eeprom_address);
        if (byte_count == strlen(ui_buffer))
        {
          Serial.println(ui_buffer);
        }
        else
          Serial.println("Data Write Error");
        break;
      case 4:
        // byte write
        Serial.print("Enter Data Byte to Write to EEPROM: ");
        data = read_int();
        byte_count = eeprom_write_byte(i2c_address, data, eeprom_address);
        if (byte_count != 0)
        {
          Serial.print("0x");
          Serial.println(data, HEX);
        }
        else
          Serial.println("Data Write Error");
        break;
      case 5:
        // I2C Address
        Serial.print("Enter I2C Address: ");
        i2c_address = read_int();
        Serial.println(i2c_address, HEX);
        break;
      case 6:
        // EEPROM Address
        Serial.print("Enter EEPROM Address: ");
        eeprom_address = read_int();
        if (eeprom_address >= EEPROM_DATA_SIZE) eeprom_address = EEPROM_DATA_SIZE-1;
        Serial.println(eeprom_address, HEX);
        break;
      case 7:
        // Terminator
        Serial.print("Enter Terminator: ");
        terminator = read_int();
        Serial.println(terminator, HEX);
        break;
      case 8:
        // Read Count
        Serial.print("Enter Read Count: ");
        read_count = read_int();
        Serial.println(read_count);
        break;
      case 9:
        // Write Integer
        Serial.print("Enter Integer To Write (-32,768 to 32,767): ");
        int_data = read_int();
        byte_count = eeprom_write_int16(i2c_address, int_data, eeprom_address);
        if (byte_count != 0)
        {
          Serial.println(int_data);
        }
        else
          Serial.println("Data Write Error");
        break;
      case 10:
        // Read Integer from EEPROM
        byte_count = eeprom_read_int16(i2c_address, &int_data, eeprom_address);
        if (byte_count != 0)
        {
          Serial.print("Integer Read From EEPROM:");
          Serial.println(int_data);
        }
        else
          Serial.println("Data Read Error");
        break;
      case 11:
        // Write Long
        Serial.print("Enter Long To Write (-2,147,483,648 to 2,147,483,647): ");
        long_data = read_int();
        byte_count = eeprom_write_int32(i2c_address, long_data, eeprom_address);
        if (byte_count != 0)
        {
          Serial.println(long_data);
        }
        else
          Serial.println("Data Write Error");
        break;
      case 12:
        // Read Long from EEPROM
        byte_count = eeprom_read_int32(i2c_address, &long_data, eeprom_address);
        if (byte_count != 0)
        {
          Serial.print("Long Read From EEPROM:");
          Serial.println(long_data);
        }
        else
          Serial.println("Data Read Error");
        break;
      case 13:
        // Write Float
        Serial.print("Enter Float To Write: ");
        float_data = read_float();
        byte_count = eeprom_write_float(i2c_address, float_data, eeprom_address);
        if (byte_count != 0)
        {
          Serial.println(float_data, 6);
        }
        else
          Serial.println("Data Write Error");
        break;
      case 14:
        // Read Float from EEPROM
        byte_count = eeprom_read_float(i2c_address, &float_data, eeprom_address);
        if (byte_count != 0)
        {
          Serial.print("Float Read From EEPROM: ");
          Serial.println(float_data, 6);
        }
        else
          Serial.println("Data Read Error");
        break;
      case 15:
        // Clear Calibration Settings That are stored on the Demo boards EEPROM
        Serial.println("Do You Want to Clear Calibration on the Demo Board?");
        Serial.println("1-Clear Calibration\n");
        Serial.println("0-Keep Calibration\n");
        Serial.print("Enter a Command: ");
        user_command = read_int();

        if (user_command == 1)
        {
          // Clear ALL
          for (int i = 0; i < 0x7FF; i++)
            eeprom_write_byte(EEPROM_I2C_ADDRESS, 0x0FF, (EEPROM_CAL_STATUS_ADDRESS + i));
          Serial.print("Calibration Cleared\n");
        }
        else
        {
          Serial.print("Calibration NOT Cleared\n");
        }
    }
    print_prompt();
  }
}
