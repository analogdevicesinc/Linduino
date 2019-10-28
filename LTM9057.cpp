/*!
LTM9057: Anyside™ I2C Power, Charge and Energy μModule Isolator

@verbatim

The LTM®9057 is a complete μModule® (micromodule) isolator that measures current, voltage,
power, charge and energy with I2C interface for systems where the measurement sense element
is at a potential far removed from system ground. Communication is uninterrupted for common
mode transients greater than 50kV/μs.

@endverbatim

https://www.analog.com/en/products/ltm9057.html

https://www.analog.com/en/products/ltm9057.html#product-evaluationkit

REVISION HISTORY
$Revision: 6835 $
$Date: 2019-09-25 11:37:36 -0700 (Wed, 25 Sept 2019) $

Copyright (c) 2019, Analog Devices Inc. (ADI)
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided 
that the following conditions are met:
  - Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright notice, this list of  conditions and
    the following disclaimer in the documentation and/or other materials provided with the distribution.  
  - Modified versions of the software must be conspicuously marked as such.
  - This software is licensed solely and exclusively for use with processors/products manufactured by or
    for Analog Devices, Inc.
  - This software may not be combined or merged with other code in any manner that would cause the 
    software to become subject to terms and conditions which differ from those listed here.
  - Neither the name of Analog Devices, Inc. nor the names of its contributors may be used to endorse
    or promote products derived from this software without specific prior written permission.
  - The use of this software may or may not infringe the patent rights of one or more patent holders.  
    This license does not release you from the requirement that you obtain separate licenses from these 
    patent holders to use this software.
    
    THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-
    INFRINGEMENT, TITLE, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
    DISCLAIMED. IN NO EVENT SHALL ANALOG DEVICES, INC. OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, PUNITIVE OR CONSEQUENTIAL 
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, DAMAGES ARISING OUT OF CLAIMS OF
    INTELLECTUAL PROPERTY RIGHTS INFRINGEMENT; PROCUREMENT OF SUBSTITUTE GOODS 
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN  ANY WAY OUT OF THE USE OF 
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
    
    20180927-7CBSD SLA
*/


//! @ingroup Power_Monitors_and_Coulomb_Counters
//! @{
//! @defgroup LTM9057: Anyside™ High Voltage Isolated I²C Power, Charge and Energy uModule
//! @}

/*! @file
    @ingroup LTM9057
    Library for LTM9057 Anyside™ HHigh Voltage Isolated I²C Power, Charge and Energy uModule.
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LTM9057.h"

uint8_t reg_read_list[68] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,
0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,
0x2D,0x2E,0x2F,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,0x40,0x41,0x42,0x43};

uint8_t reg_write_list[66] = {0x00,0x01,0x02,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,
0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,
0x2D,0x2E,0x2F,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,0x41,0x42,0x43};

// Reads an 8-bit register from the LTM9057 using the standard repeated start format.
int8_t LTM9057_register_read(uint8_t i2c_address, uint8_t register_address, uint8_t *register_data)
{
  int8_t ack = 0;
  ack = i2c_read_byte_data(i2c_address, register_address, register_data);
  return(ack);
}

// Read the specified ADC value (SENSE, VIN or ADIN) and output in human readable format to the serial console.
int8_t LTM9057_adc_read(uint8_t i2c_address, uint8_t base_address, float *register_data)
{
  uint8_t data_LSBs, data_MSBs;
  uint16_t data_twelve_bit;
  int8_t ack = 0;
  ack |= LTM9057_register_read(i2c_address, base_address, &data_MSBs);
  ack |= LTM9057_register_read(i2c_address, base_address+1, &data_LSBs);
  data_twelve_bit = ((uint16_t)data_MSBs<<4) | ((uint16_t)data_LSBs>>4);

  switch (base_address)
  {
    case LTM_9057_dSENSE_MSB_REG:
//      *register_data = ((float)data_twelve_bit * LTM_9057_dSENSE_mV_PER_TICK/sense_resistor);
      *register_data = ((float)data_twelve_bit * LTM_9057_dSENSE_mV_PER_TICK) / sense_resistor;
      Serial.print(F("ADC Current Sense(mA): "));
      break;
    case LTM_9057_VIN_MSB_REG:
      *register_data = ((float)data_twelve_bit * LTM_9057_VIN_V_PER_TICK);
      Serial.print(F("VIN Voltage(V): "));
      break;
    case LTM_9057_ADIN_MSB_REG:
      *register_data = ((float)data_twelve_bit * LTM_9057_ADIN_V_PER_TICK) * adin_gain;
      Serial.print(F("ADIN Voltage(V): "));
      break;
    default:
      return ack;
  }

  Serial.println(*register_data, 4);
  return ack;
}

// Writes to an 8-bit register inside the LTM9057 using the standard I2C repeated start format.
int8_t LTM9057_register_write(uint8_t i2c_address, uint8_t register_address, uint8_t register_data)
{
  int8_t ack = 0;
  ack = i2c_write_byte_data(i2c_address, register_address, register_data);
  return(ack);
}

// Sets any bit inside the LTM9057 using the standard I2C repeated start format.
int8_t LTM9057_bit_set(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number)
{
  uint8_t register_data;
  uint8_t bit_mask;
  int8_t ack = 0;
  bit_mask = 0x01 << bit_number;
  ack |= LTM9057_register_read(i2c_address, register_address, &register_data);  //Read register
  register_data = register_data | bit_mask;  //Set the bit at bit_address
  ack |= LTM9057_register_write(i2c_address, register_address, register_data);  //Write new data to register
  return(ack);
}

// Clears any bit inside the LTM9057 using the standard I2C repeated start format.
int8_t LTM9057_bit_clear(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number)
{
  uint8_t register_data;
  uint8_t bit_mask;
  int8_t ack = 0;
  bit_mask = 0x01 << bit_number;
  ack |= LTM9057_register_read(i2c_address, register_address, &register_data);  //Read register
  register_data = register_data & (~bit_mask);  //Clears the bit at bit_address
  ack |= LTM9057_register_write(i2c_address, register_address, register_data);  //Write new data to register
  return(ack);
}

// Read the bit specified by bit_number from the LTM9057.
int8_t LTM9057_bit_read(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number, uint8_t *register_data)
{
  int8_t ack = 0;
  ack = LTM9057_register_read(i2c_address, register_address, register_data);
  *register_data = (*register_data >> bit_number) & 0x1;
  return ack;
}

// Attempts to read a byte from the I2C bus using the alert1 address to ascertain pending alerts on the bus.
int8_t LTM9057_alert1_read(uint8_t *register_data)
{
  int8_t ack = 0;
  ack = i2c_read_byte(LTM_9057_ALERT1_REG, register_data);
  return(ack);
}

// Attempts to read a byte from the I2C bus using the alert1 address to ascertain pending alerts on the bus.
int8_t LTM9057_alert2_read(uint8_t *register_data)
{
  int8_t ack = 0;
  ack = i2c_read_byte(LTM_9057_ALERT2_REG, register_data);
  return(ack);
}

// Read all LTM9057 registers and output to the serial console.
int8_t LTM9057_print_all_registers(uint8_t i2c_address)
{
  int8_t ack = 0;
  uint8_t data;
  uint8_t i;
  for (i=0; i<sizeof(reg_read_list); i++)
  {
    ack |= LTM9057_register_read(i2c_address, reg_read_list[i], &data);   //! Read register
    Serial.print("Register 0x");
    Serial.print(reg_read_list[i], HEX);
    Serial.print(":\t");
    if (!ack)
    {
      Serial.print("0x");
      Serial.println(data, HEX);
    }
    else
    {
      Serial.println("No Acknowledge");
    }
  }
  return ack;
}

// Read all LTM9057 registers and output to the serial console every second until a key press is detected.
int8_t LTM9057_continuous_read_all_registers(uint8_t i2c_address)
{
  int8_t ack = 0;
  uint8_t count = 0;
  float data = 0;
  char anykey[1];

  //Note: Arduino serial monitor doesn't support all VT100 commands,
  //thus the continuous_read can't scrollback and rewrite the latest register values.
  while (count < 1)
  {
    ack |= LTM9057_print_all_registers(i2c_address);
    ack |= LTM9057_adc_read(i2c_address, LTM_9057_dSENSE_MSB_REG, &data);
    ack |= LTM9057_adc_read(i2c_address, LTM_9057_VIN_MSB_REG, &data);
    ack |= LTM9057_adc_read(i2c_address, LTM_9057_ADIN_MSB_REG, &data);
    Serial.println("");

    count = Serial.readBytes(anykey, 1); //default timeout is 1000 milliseconds. Change via the setTimeout method of Serial.
  }

  return ack;
}

// Check if user_register is a valid register for the LTM9057.
boolean valid_register(uint8_t user_register, uint8_t register_array[], uint8_t array_length)
{
  uint8_t i=0;
  for (i=0; i<array_length; i++)
  {
    if (register_array[i] == user_register)
    {
      return true;
    }
  }
  return false;
}
