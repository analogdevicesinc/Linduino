/*
I2C Address Scan

This program will scan all 128 I2C addresses on the QuikEval connector
and report all addresses that acknowledge an address+!w command.

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

// set I2C frequency in kHz
#define SW_I2C_FREQUENCY  400

#include <Arduino.h>
#include <stdint.h>
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "UserInterface.h"
#include "Linduino.h"

void print_title()
// Print the title block
{
  Serial.println("");
  Serial.println("*****************************************************************");
  Serial.println("* I2C Address Scan                                              *");
  Serial.println("*                                                               *");
  Serial.println("* This program will scan all 128 I2C addresses and report those *");
  Serial.println("* that acknowledge a address+!w command.                        *");
  Serial.println("*                                                               *");
  Serial.println("* Set the baud rate to 115200 select the newline terminator.    *");
  Serial.println("*                                                               *");
  Serial.println("*****************************************************************");
}

void setup()
// Setup the program
{
  quikeval_I2C_init();                // Enable the Software I2C
  quikeval_I2C_connect();         // Connects to main I2C port
  Serial.begin(115200);        // Initialize the serial port to the PC
  print_title();
}

void loop()
{
  unsigned char ack;
  unsigned char address;
  unsigned char ack_count;
  Serial.println("\nSend any character to start the I2C address scan. (Press enter in Arduino Serial Monitor.)");
  read_int();
  ack_count = 0;
  for (address = 0; address < 128; address++)
  {
    i2c_start();
    ack = i2c_write(address << 1);
    i2c_stop();
    if (!ack)
    {
      ack_count++;
      Serial.print("Acknowledge received from address : 0x");
      Serial.print(address, HEX);
      Serial.print(" (7 bit) 0x:");
      Serial.print(address<<1, HEX);
      Serial.println(" (8-bit)");
    }
  }
  if (ack_count == 0) Serial.println("No addresses acknowledged.");
}


