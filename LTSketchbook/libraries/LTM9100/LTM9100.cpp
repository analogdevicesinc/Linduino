/*!
LTM9100: Anyside™ High Voltage Isolated Switch Controller with I²C Command and Telemetry

@verbatim

The LTM9100 μModule controller is a complete, galvanically isolated switch controller
with I2C interface, for use as a load switch or hot swap controller. The load is soft
started and controlled by an external N-channel MOSFET switch.

@endverbatim

http://www.linear.com/product/LTM9100

http://www.linear.com/product/LTC9100#demoboards


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

//! @ingroup Switching_Regulators
//! @{
//! @defgroup LTM9100 LTM9100: Anyside™ High Voltage Isolated Switch Controller with I²C Command and Telemetry.

//! @}

/*! @file
    @ingroup LTM9100
    Library for LTM9100 Anyside™ High Voltage Isolated Switch Controller with I²C Command and Telemetry.
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LTM9100.h"

uint8_t reg_read_list[10] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09};
uint8_t reg_write_list[9] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09};

// Reads an 8-bit register from the LTM9100 using the standard repeated start format.
int8_t LTM9100_register_read(uint8_t i2c_address, uint8_t register_address, uint8_t *register_data)
{
  int8_t ack = 0;
  ack = i2c_read_byte_data(i2c_address, register_address, register_data);
  return(ack);
}

// Read the specified ADC value (SENSE, ADIN, ADIN2) and output in human readable format to the serial console.
int8_t LTM9100_adc_read(uint8_t i2c_address, uint8_t base_address, float *register_data)
{
  uint8_t data_LSBs, data_MSBs;
  uint16_t data_ten_bit;
  int8_t ack = 0;
  ack |= LTM9100_register_read(i2c_address, base_address, &data_MSBs);
  ack |= LTM9100_register_read(i2c_address, base_address+1, &data_LSBs);
  data_ten_bit = ((uint16_t)data_MSBs<<2) | ((uint16_t)data_LSBs>>6);

  switch (base_address)
  {
    case LTM_9100_SENSE_E_REG:
      *register_data = ((float)data_ten_bit * LTM_9100_SENSE_mV_PER_TICK)/sense_resistor;
      Serial.print(F("ADC Current Sense Voltage(mA): "));
      break;
    case LTM_9100_ADIN_I_REG:
      *register_data = ((float)data_ten_bit * LTM_9100_ADIN_V_PER_TICK)/adin_gain;
      Serial.print(F("ADIN Voltage(V): "));
      break;
    case LTM_9100_ADIN2_G_REG:
      *register_data = ((float)data_ten_bit * LTM_9100_ADIN2_V_PER_TICK)/adin2_gain;
      Serial.print(F("ADIN2 Voltage(V or K): "));
      break;
    default:
      return ack;
  }

  Serial.println(*register_data, 4);
  return ack;
}

// Writes to an 8-bit register inside the LTM9100 using the standard I2C repeated start format.
int8_t LTM9100_register_write(uint8_t i2c_address, uint8_t register_address, uint8_t register_data)
{
  int8_t ack = 0;
  ack = i2c_write_byte_data(i2c_address, register_address, register_data);
  return(ack);
}

// Sets any bit inside the LTM9100 using the standard I2C repeated start format.
int8_t LTM9100_bit_set(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number)
{
  uint8_t register_data;
  uint8_t bit_mask;
  int8_t ack = 0;
  bit_mask = 0x01 << bit_number;
  ack |= LTM9100_register_read(i2c_address, register_address, &register_data);  //Read register
  register_data = register_data | bit_mask;  //Set the bit at bit_address
  ack |= LTM9100_register_write(i2c_address, register_address, register_data);  //Write new data to register
  return(ack);
}

// Clears any bit inside the LTM9100 using the standard I2C repeated start format.
int8_t LTM9100_bit_clear(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number)
{
  uint8_t register_data;
  uint8_t bit_mask;
  int8_t ack = 0;
  bit_mask = 0x01 << bit_number;
  ack |= LTM9100_register_read(i2c_address, register_address, &register_data);  //Read register
  register_data = register_data & (~bit_mask);  //Clears the bit at bit_address
  ack |= LTM9100_register_write(i2c_address, register_address, register_data);  //Write new data to register
  return(ack);
}

// Read the bit specified by bit_number from the LTM9100.
int8_t LTM9100_bit_read(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number, uint8_t *register_data)
{
  int8_t ack = 0;
  ack = LTM9100_register_read(i2c_address, register_address, register_data);
  *register_data = (*register_data >> bit_number) & 0x1;
  return ack;
}

// Attempts to read a byte from the I2C bus using the alert address (0xC) to ascertain pending alerts on the bus.
int8_t LTM9100_alert_read(uint8_t *register_data)
{
  int8_t ack = 0;
  ack = i2c_read_byte(0x0C, register_data);
  return(ack);
}

// Read all LTM9100 registers and output to the serial console.
int8_t LTM9100_print_all_registers(uint8_t i2c_address)
{
  int8_t ack = 0;
  uint8_t data;
  uint8_t i;
  for (i=0; i<sizeof(reg_read_list); i++)
  {
    ack |= LTM9100_register_read(i2c_address, reg_read_list[i], &data);   //! Read register
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

// Read all LTM9100 registers and output to the serial console every second until a key press is detected.
int8_t LTM9100_continuous_read_all_registers(uint8_t i2c_address)
{
  int8_t ack = 0;
  uint8_t count = 0;
  float data = 0;
  char anykey[1];

  //Note: Arduino serial monitor doesn't support all VT100 commands,
  //thus the continuous_read can't scrollback and rewrite the latest register values.
  while (count < 1)
  {
    ack |= LTM9100_print_all_registers(i2c_address);
    ack |= LTM9100_adc_read(i2c_address, LTM_9100_SENSE_E_REG, &data);
    ack |= LTM9100_adc_read(i2c_address, LTM_9100_ADIN_I_REG, &data);
    ack |= LTM9100_adc_read(i2c_address, LTM_9100_ADIN2_G_REG, &data);
    Serial.println("");

    count = Serial.readBytes(anykey, 1); //default timeout is 1000 milliseconds. Change via the setTimeout method of Serial.
  }

  return ack;
}

// Check if user_register is a valid register for the LTM9100.
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
