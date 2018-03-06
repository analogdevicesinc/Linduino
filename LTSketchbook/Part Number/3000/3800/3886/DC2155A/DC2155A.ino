/*!
Linear Technology DC2155A Demonstartion Board
LTC3886: Power Management Solution for Application Processors

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim

http://www.linear.com/product/LTC3886

http://www.linear.com/product/LTC3886#demoboards


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
    @ingroup LTC3886
*/


#include <Arduino.h>
#include <stdint.h>
#include "UserInterface.h"
#include "Linduino.h"
#include <LT_I2CBus.h>
#include <LT_PMBus.h>
#include <LT_PMBusMath.h>
#include <LT_SMBus.h>
#include <LT_SMBusBase.h>
#include <LT_SMBusGroup.h>
#include <LT_SMBusNoPec.h>
#include <LT_SMBusPec.h>
#include <LT_Wire.h>

#define LTC3886_I2C_ADDRESS 0x4F
#define MFR_PWM_COMP 0xD3          // PMBus command for setting the compensation network gm and Rth
#define PAGE 0x00                  // PMBus command for selecting output
#define CH_0 0x00
#define CH_1 0x01

#define ITH_MAX 62
#define GM_MAX 5.73


// Global variables
static uint8_t ltc3886_i2c_address;
static LT_SMBusNoPec *smbus = new LT_SMBusNoPec();

static uint8_t channel;            // current output channel
static uint8_t ith;                // current ith hex value
static uint8_t gm;                 // current gm hex value
static uint8_t comp_config;        // data byte for setting gm and Rth
static float ith_dec;              // decimal representaiton of ith
static float gm_dec;               // decimal represnetaiton of gm

// Arrays of ith and gm values make it easy to transfer between decimal and hex
static float ith_vals[] = {0, .25, .5, .75, 1, 1.25, 1.5, 1.75, 2, 2.5, 3, 3.5, 4, 4.5, 5, 5.5, 6, 7, 8, 9, 11, 13, 15, 17, 20, 24, 28, 32, 38, 46, 54, 62};
static float gm_vals[] = {1, 1.68, 2.35, 3.02, 3.69, 4.36, 5.04, 5.73};
static uint8_t ith_vals_hex[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};
static uint8_t gm_vals_hex[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};


//! Initialize Linduino
void setup()
{
  Serial.begin(115200);            //! Initialize the serial port to the PC
  print_title();
  ltc3886_i2c_address = LTC3886_I2C_ADDRESS;
  channel = CH_0;                  // Initialize to CH0
  print_comp_config();
  print_prompt();
}

//! Repeats Linduino loop
void loop()
{
  uint8_t user_select;             // The user's choice of the three menu options
  float user_value;                // data value user inputs, either for gm or Rth
  if (Serial.available())
  {
    user_select = read_int();
    switch (user_select)
    {
      case 1:                      // Change gm
        Serial.print(F("\nEnter desired gm (1-5.73), this will be rounded to nearest legal value:\n"));
        user_value = read_float();

        gm_dec = gm_nearest_legal_value(user_value);    // Snap given decimal gm value to nearest actual gm decimal value
        gm = gm_decimal2hex(gm_dec);                    // Convert decimal gm to hex

        comp_config = (gm << 5) | ith;                  // Update comp_config with new gm value. Must bitshift gm left by 5. See page 74 of 3886 datasheet for details.
        smbus->writeByte(ltc3886_i2c_address, MFR_PWM_COMP, comp_config);

        print_comp_config();
        print_prompt();
        break;
      case 2:                      // Change ith
        Serial.print(F("\nEnter desired ith (0-62), this will be rounded to nearest legal value:\n"));
        user_value = read_float();

        ith_dec = ith_nearest_legal_value(user_value);  // Snap given decimal ith value to nearest actual ith decimal value
        ith = ith_decimal2hex(ith_dec);                 // Convert decimal ith to hex

        comp_config = (gm << 5) | ith;                  // Update comp_config with new ith value. Must bitshift gm left by 5. See page 74 of 3886 datasheet for details.
        smbus->writeByte(ltc3886_i2c_address, MFR_PWM_COMP, comp_config);

        print_comp_config();
        print_prompt();
        break;
      case 3:                      // Toggle Channel
        if (channel == 0)
        {
          smbus->writeByte(ltc3886_i2c_address, PAGE, CH_1);
          channel = CH_1;
        }
        else
        {
          smbus->writeByte(ltc3886_i2c_address, PAGE, CH_0);
          channel = CH_0;
        }

        Serial.println();
        print_comp_config();
        print_prompt();
        break;
    }
  }
}


// Function Definitions

//! Prints the title block when program first starts.
//! @return void
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* LTC3886 Adjustable Compensation Program                       *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This is an example program for adjusting LTC3886 compensation.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
//! @return void
void print_prompt()
{
  Serial.print(F("\n  1-Adjust gm\n"));
  Serial.print(F("  2-Adjust ith\n"));
  Serial.print(F("  3-Switch Channel"));
}

//! Prints current compensation configuration and saves ith, gm, and configuration hex values.
//! @return void
void print_comp_config()
{
  comp_config = smbus->readByte(ltc3886_i2c_address, MFR_PWM_COMP);

  ith = 0x1F & comp_config;                 // Extract ith via bit-wise AND with 5 LSD
  gm = (0xE0 & comp_config) >> 5;           // Extract ith via bit-wise AND with 3 MSD. Bit shift right by 5.
  ith_dec = ith_hex2decimal(ith);           // Update ith hex value
  gm_dec = gm_hex2decimal(gm);              // Update gm hex value

  Serial.print(F("\nCH"));
  Serial.print(channel, DEC);
  Serial.print(F(" "));
  Serial.print(F("Configuration, (gm, ith): ("));
  Serial.print(gm_dec);
  Serial.print(F(", "));
  Serial.print(ith_dec);
  Serial.print(F(")"));
}


//! Rounds decimal input ith to nearest legal decimal value.
//! @return double
float ith_nearest_legal_value(float ith)
{
  for (int i=0; i<(sizeof(ith_vals)/4)-1; i++)
  {
    float cutoff = ith_vals[i] + (ith_vals[i+1] - ith_vals[i])/2;
    if (ith < cutoff)
    {
      return ith_vals[i];
    }
  }
  return ITH_MAX;
}

//! Rounds decimal input gm to nearest legal decimal value.
//! @return double
float gm_nearest_legal_value(float gm)
{
  for (int i=0; i<(sizeof(gm_vals)/4)-1; i++)
  {
    float cutoff = gm_vals[i] + (gm_vals[i+1] - gm_vals[i])/2;
    if (gm < cutoff)
    {
      return gm_vals[i];
    }
  }
  return GM_MAX;
}

//! Converts ith demo circuit hex value to its decimal equivalent.
//! @return double
float ith_hex2decimal(uint8_t ith)
{
  for (int i=0; i<sizeof(ith_vals_hex); i++)
  {
    if (ith_vals_hex[i]==ith)
    {
      return ith_vals[i];
    }
  }
}

//! Converts gm demo circuit hex value to its decimal equivalent.
//! @return double
float gm_hex2decimal(uint8_t gm)
{
  for (int i=0; i<sizeof(gm_vals_hex); i++)
  {
    if (gm_vals_hex[i]==gm)
    {
      return gm_vals[i];
    }
  }
}

//! Converts ith decimal value to the demo circuit hex equivalent.
//! @return uint8_t
uint8_t ith_decimal2hex(float ith)
{
  for (int i=0; i<sizeof(ith_vals); i++)
  {
    if (abs(ith_vals[i]-ith) < .1)                         // Epsilon of .1 selected to prevent equality check of floating numbers
    {
      return ith_vals_hex[i];
    }
  }
}

//! Converts gm decimal value to the demo circuit hex equivalent.
//! @return uint8_t
uint8_t gm_decimal2hex(float gm)
{
  for (int i=0; i<sizeof(gm_vals); i++)
  {
    if (abs(gm_vals[i]-gm) < .1)                           // Epsilon of .1 selected to prevent equality check of floating numbers
    {
      return gm_vals_hex[i];
    }
  }
}

