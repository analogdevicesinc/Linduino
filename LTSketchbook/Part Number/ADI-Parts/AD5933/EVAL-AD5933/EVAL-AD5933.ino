/***************************************************************************//**
 *   @file   EVAL-AD5933.ino
 *   @brief  Exerciser program for ad5933 no-OS driver
 *   @author Mthoren (mark.thoren@analog.com)
********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#include <Arduino.h>
#include <stdint.h>
#include <Linduino.h>
#include <UserInterface.h>
//#include <EEPROM.h>
#include <platform_drivers.h>
extern "C" {
#include "ad5933.h"
};

i2c_init_param i2c_params =
{
  GENERIC_I2C,    // i2c type
  0,              // i2c id
  100000,         // i2c max speed (hz)
  0x0D,           // i2c slave address
};

ad5933_init_param init_params =
{
  i2c_params,     // i2c parameters
  AD5933_INTERNAL_SYS_CLK, //current_sys_clk
  AD5933_CONTROL_EXT_SYSCLK, //current_clock_source
  AD5933_GAIN_X1,//current_gain
  AD5933_RANGE_400mVpp, //current_range
};

ad5933_dev *device;

int32_t connected = SUCCESS;

void setup()
{
  float tempdata;
  Serial.begin(115200);

  // Give serial port a chance to initialize
  delay(100);

  // Initialize
  connected = ad5933_init(&device, init_params);
  ad5933_reset(device);
  

  if (connected != SUCCESS)
  {
    Serial.println("Connection to device failed :(");
  }
  else
  {
    Serial.println("Connection to device succeeded!");
	Serial.println("Reading out temperature registers:");
	tempdata = ad5933_get_temperature(device);
	Serial.println(tempdata);
    print_title();
  }
}

void loop()
{
  // If there is no device, don't continue
  if (connected != SUCCESS)
  {
    return;
  }

  print_prompt();

  uint8_t user_command = read_int();
  Serial.println(user_command);
  Serial.flush();

  switch (user_command)
  {
    case 1:
      menu_1_read_temperature();
      break;

    case 2:
      menu_2_asdf();
      break;

    case 3:
      menu_3_asdf();
      break;

    case 4:
      menu_4_asdf();
      break;

    case 5:
      //menu_5_critical();
      break;

    case 9:
      //EEPROM_WRITE_TEST();
      break;

    default:
      Serial.println(F("Invalid option"));
      break;
  }
}

void print_title()
{
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* EVAL-AD5933 Demonstration Program                            *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates communication with the AD5933       *"));
  Serial.println(F("* 1MSPS, 12-bit Impedance Converter, Network Analyzer           *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 select the newline terminator.    *"));
  Serial.println(F("*****************************************************************"));
}

void print_prompt()
{
  Serial.println(F("\nCommand Summary:"));

  Serial.println(F("  1- Read temperature"));
  Serial.println(F("  2- menu item 2"));
  Serial.println(F("  3- menu item 3"));
  Serial.println(F("  4- menu item 4"));
  Serial.println(F("  X- Test & clear Linduino EEPROM"));
  Serial.println();

  Serial.print(F("Enter a command: "));
}

uint8_t menu_1_read_temperature()
{
  float tempdata;
  Serial.println("\nReading out temperature:");
  tempdata = ad5933_get_temperature(device);
  Serial.print(tempdata);
  Serial.println(" deg. C\n");
  return 0;
}

uint8_t menu_2_asdf()
{

  return 0;
}

uint8_t menu_3_asdf()
{


  return 0;
}

uint8_t menu_4_asdf()
{
  

  return 0;
}


