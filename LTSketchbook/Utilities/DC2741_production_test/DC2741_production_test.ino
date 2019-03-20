/*
DC2741 production program test

This program will run through a battery of tests to verify DC2741 connectivity.
Test Jig consists of an SDP breakout board (Part Number ADZS-BRKOUT-EX3)

With the following direct connections:
43 - 45, 10k pulldown (IO2 - AD0)
44 - 47, 10k pulldown (IO4 - AD2)
76 - 78, 10k pulldown (IO3~ - AD1)
74 - 77, 10k pulldown (AD3 - IO5~)

And the following connections with BAT85 Schottky diodes:
82 - 83 (SCK  ->|- MOSI)
84 - 83 (MOSI ->|- MISO)
84 - 83 (CS   ->|- MISO)

**********************************************
IMPORTANT Note: When testing Assembly Type B,
R44 on the Linduino must be removed. Keep this
Linduino set aside, or re-install R44.
**********************************************

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
#include "LT_SPI.h"
#include "QuikEval_EEPROM.h"
#include "UserInterface.h"
#include "Linduino.h"

// String descriptions for each test:
char* descr[] = {"SCK - MISO", "MOSI - MISO", "CS - MISO", "IO2 - AD0", "IO3~ - AD1", "IO4 - AD2", "IO5 - AD3"};
// descr[0] = "SCK - MISO";
// descr[1] = "MOSI - MISO";
// descr[2] = "CS - MISO";
// descr[3] = "IO2 - AD0";
// descr[4] = "IO3~ - AD1";
// descr[5] = "IO4 - AD2";
// descr[6] = "IO5 - AD3";

const uint8_t stimulus[7] = {13, 11, QUIKEVAL_CS, 2, 3, 4, 5};
const uint8_t readback[7] = {12, 12, 12         , 0, 1, 2, 3};

void print_title()
// Print the title block
{
  Serial.println("");
  Serial.println("*****************************************************************");
  Serial.println("* DC2741 production program test                                *");
  Serial.println("*                                                               *");
  Serial.println("* Set the baud rate to 115200 select the newline terminator.    *");
  Serial.println("*                                                               *");
  Serial.println("*****************************************************************");
}

void setup()
// Setup the hardware
{
  quikeval_I2C_init();                // Enable the Software I2C
  quikeval_I2C_connect();         // Connects to main I2C port
  pinMode(2, INPUT); //GPIOs
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(12, INPUT);
  pinMode(13, OUTPUT); //SCK
  pinMode(11, OUTPUT); //MOSI
  pinMode(QUIKEVAL_CS, OUTPUT); //
  output_high(13);
  output_high(11);
  output_high(QUIKEVAL_CS);


  Serial.begin(115200);        // Initialize the serial port to the PC
  print_title();
}

void loop()
{
  unsigned char ack;
  unsigned char address;
  unsigned char ackaddr;
  unsigned char ack_count;
  unsigned char fail_count;
  unsigned char i;
  uint16_t voltage;
  Serial.println("\nSend any character to start the test. (Press enter in Arduino Serial Monitor.)");
  read_int();
  Serial.println("Testing...");
  delay(500); // Delay necessary to indicate to user that something's going on...
  ack_count = 0;
  fail_count = 0;
  ackaddr = 0;
  quikeval_I2C_connect();
  for (address = 0; address < 128; address++)
  {
    i2c_start();
    ack = i2c_write(address << 1);
    i2c_stop();
    if (!ack)
    {
      ack_count++;
      ackaddr = address;
      Serial.print("Acknowledge received from address : 0x");
      Serial.print(address, HEX);
      Serial.print(" (7 bit) 0x:");
      Serial.print(address<<1, HEX);
      Serial.println(" (8-bit)");
    }
  }
  if (ackaddr != 0x50)
  {
    Serial.println("Didn't find an EEPROM at address 0x50, FAIL");
    ++fail_count;
  }
  quikeval_SPI_connect();

// First, test SPI port (digital levels only)
  for(i=0; i<=2; i++)
  {
    output_low(stimulus[i]); //! 1) Pull Low
    delay(1);
    if(digitalRead(readback[i]) == HIGH)
    {
      Serial.print("Fail test low: ");
      ++fail_count;
    }
    else
    {
      Serial.print("Pass test low: ");
    }

    Serial.println(descr[i]);
    output_high(stimulus[i]); //! 1) Pull High
    delay(1);
    if(digitalRead(readback[i]) == LOW)
    {
      Serial.print("Fail test high: ");
      ++fail_count;
    }
    else
    {
      Serial.print("Pass test high: ");
      
    }
    Serial.println(descr[i]);
    

  }
  
// Next, test GPIO / Analog connectivity
  for(i=3; i<=6; i++)
  {
    pinMode(stimulus[i], INPUT);
    delay(1);
    voltage = analogRead(readback[i]);
    Serial.print("Voltage: ");
    Serial.println(voltage);
    if(voltage < 512)
    {
      Serial.print("Fail test high: ");
      ++fail_count;
    }
    else
    {
      Serial.print("Pass test high: ");
      
    }
    Serial.println(descr[i]);
    
    pinMode(stimulus[i], OUTPUT);
    output_low(stimulus[i]); //! 1) Pull Low
    delay(1);
    voltage = analogRead(readback[i]);
    Serial.print("Voltage: ");
    Serial.println(voltage);
    if(voltage > 512)
    {
      Serial.print("Fail test low: ");
      ++fail_count;
    }
    else
    {
      Serial.print("Pass test low: ");
    }
    Serial.println(descr[i]);
  }
  
  if(fail_count == 0)
  {
    Serial.println("All tests PASS!!");
    Serial.println("All tests PASS!!");
    Serial.println("All tests PASS!!");
  }
  else
  {
    Serial.println("Board FAILS!!");
    Serial.println("Board FAILS!!");
    Serial.print("Error Count: ");
    Serial.println(fail_count);
  }
}


