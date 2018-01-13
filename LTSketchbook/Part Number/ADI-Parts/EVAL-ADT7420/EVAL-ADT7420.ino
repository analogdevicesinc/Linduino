/***************************************************************************//**
 *   @file   EVAL-ADT7420.ino
 *   @brief  Exerciser program for adt7420 no-OS driver
 *   @author Gbrisebois (gregory.brisebois@analog.com)
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
#include <EEPROM.h>
#include <platform_drivers.h>
extern "C" {
#include "adt7420.h"
};

i2c_init_param i2c_params =
{
  GENERIC_I2C,    // i2c type
  0,              // i2c id
  100000,         // i2c max speed (hz)
  0x48,           // i2c slave address
};

adt7420_init_param init_params =
{
  i2c_params,     // i2c parameters
  0,              // resolution setting
};

adt7420_dev *device;

int32_t connected = SUCCESS;

void setup()
{
  Serial.begin(115200);

  // Give serial port a chance to initialize
  delay(100);

  // Initialize
  connected = adt7420_init(&device, init_params);

  if (connected != SUCCESS)
  {
    Serial.println("Connection to device failed :(");
  }
  else
  {
    Serial.println("Connection to device succeeded!");
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
      menu_2_set_resolution();
      break;

    case 3:
      menu_3_set_op_mode();
      break;

    case 4:
      menu_4_bunchoftemps();
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
  Serial.println(F("* EVAL-7420SDZ Demonstration Program                            *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates communication with the ADT7420      *"));
  Serial.println(F("* high accuracy digital temperature sensor                      *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 select the newline terminator.    *"));
  Serial.println(F("*****************************************************************"));
}

void print_prompt()
{
  Serial.println(F("\nCommand Summary:"));

  Serial.println(F("  1- Read temperature"));
  Serial.println(F("  2- Set resolution"));
  Serial.println(F("  3- Set operation mode"));
  Serial.println(F("  4- Poll for a bunch of temperatures"));
  Serial.println(F("  X- Get critical temperature setting"));
  Serial.println(F("  X- Test & clear Linduino EEPROM"));
  Serial.println();

  Serial.print(F("Enter a command: "));
}

uint8_t menu_1_read_temperature()
{
  float temp = adt7420_get_temperature(device);

  Serial.print("Current temperature: ");
  Serial.print(temp);
  Serial.println(F(" C"));
}

uint8_t menu_2_set_resolution()
{
  Serial.println(F("  Available resolutions:"));
  Serial.println(F("    1- 13-bit"));
  Serial.println(F("    2- 16-bit"));
  Serial.print(F("  Select an option: "));

  uint8_t new_res = read_int();
  Serial.println(new_res);

  new_res = (new_res == 1) ? 0 : 1;

  adt7420_set_resolution(device, new_res);

  Serial.print(F("Set resolution to "));
  Serial.print((13 + 3 * new_res));
  Serial.println(F("-bit"));

  //EEPROM.write(64, 1);
  //EEPROM.write(65, new_res);

  return 0;
}

uint8_t menu_3_set_op_mode()
{
  Serial.println(F("  Available operation modes:"));
  Serial.println(F("    1- Continuous conversion mode (default)"));
  Serial.println(F("    2- One-shot mode"));
  Serial.println(F("    3- 1 SPS mode"));
  Serial.println(F("    4- Shutdown"));
  Serial.print(F("  Select a mode: "));

  uint8_t new_mode = read_int();
  Serial.println(new_mode);

  switch (new_mode)
  {
    case 1:
      adt7420_set_operation_mode(device, ADT7420_OP_MODE_CONT_CONV);
      break;

    case 2:
      adt7420_set_operation_mode(device, ADT7420_OP_MODE_ONE_SHOT);
      break;

    case 3:
      adt7420_set_operation_mode(device, ADT7420_OP_MODE_1_SPS);
      break;

    case 4:
      adt7420_set_operation_mode(device, ADT7420_OP_MODE_SHUTDOWN);
      break;

    default:
      Serial.println(F("Invalid option"));
      break;
  }

  return 0;
}

uint8_t menu_4_bunchoftemps()
{
  Serial.print(F("  Enter number of desired samples: "));
  uint16_t num_samples = read_int();
  Serial.println(num_samples);

  Serial.print(F("  Enter a desired frequency in samples/sec (max 10): "));
  uint16_t sample_freq = read_int();
  sample_freq = constrain(sample_freq, 1, 10);
  Serial.println(sample_freq);

  uint16_t delay_sec = 1000 / sample_freq;

  Serial.print(F("  Gathering "));
  Serial.print(num_samples / sample_freq);
  Serial.println(F(" seconds of samples, press enter to continue"));

  uint8_t temp = read_int();

  for (int i = 0; i < num_samples; i++)
  {
    Serial.print(F("  #"));
    Serial.print(i + 1);
    Serial.print(F(":\t"));

    float temp = adt7420_get_temperature(device);
    Serial.println(temp);

    delay(delay_sec);
  }

  return 0;
}

/** Convert a temperature to the code the sensor understands */
void temp_to_code(float temp, uint8_t *msb, uint8_t *lsb, uint8_t resolution) // res of 0 = 13 bit, 1 = 16bit
{
  uint16_t code = 0;

  if (resolution)
  {
    if (temp < 0)
    {
      code = (uint16_t)((temp * 128) + 65536);
    }
    else
    {
      code = (uint16_t)(temp * 128);
    }
  }
  else
  {
    if (temp < 0)
    {
      code = (uint16_t) (temp * 16) + 8192;
    }
    else
    {
      code = (uint16_t) (temp * 16) + 8192;
    }

    code <<= 3;
  }

  *msb = (uint8_t)(code >> 8);
  *lsb = (uint8_t)(code & 255);
}
