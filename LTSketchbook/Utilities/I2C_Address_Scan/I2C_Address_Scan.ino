/*
I2C Address Scan

This program will scan all 128 I2C addresses on the QuikEval connector
and report all addresses that acknowledge an address+!w command.

DATA TYPES:
char  = 1 byte
int   = 2 bytes
long  = 4 bytes
float = 4 bytes

REVISION HISTORY
$Revision: 2649 $
$Date: 2014-07-18 08:09:33 -0700 (Fri, 18 Jul 2014) $

Copyright (c) 2013, Linear Technology Corp.(LTC)
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


