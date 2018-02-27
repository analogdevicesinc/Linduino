/*!
 LTC4282: High Current Hot Swap Controller with I2C Compatible Monitoring and EEPROM

@verbatim

The LTC4282 Hot Swap controller allows a board to be safely inserted and removed from a live backplane.
Using one or more external N-channel pass transistors, board supply voltage and inrush current is ramped up at an adjustable rate.
An I2C interface and onboard ADC allows for monitoring of board current, voltage, power, energy and fault status.

@endverbatim

http://www.linear.com/product/LTC4282

http://www.linear.com/product/LTC4282#demoboards


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

//! @ingroup Hot_Swap
//! @{
//! @defgroup LTC4282 LTC4282: High Current Hot Swap Controller with I2C Compatible Monitoring and EEPROM
//! @}

/*! @file
    @ingroup LTC4282
    Library for LTC4282: High Current Hot Swap Controller with I2C Compatible Monitoring and EEPROM
*/

#include <Arduino.h>
#include <stdint.h>
#include <Linduino.h>
#include <LT_I2C.h>
#include "LTC4282.h"


// Write an 8-bit code to the LTC4282.
int8_t LTC4282_write(uint8_t i2c_address, uint8_t adc_command, uint8_t code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int32_t ack;

  ack = i2c_write_byte_data(i2c_address, adc_command, code);

  return ack;

}

// Write a 16-bit code to the LTC4282.
int8_t LTC4282_write_16_bits(uint8_t i2c_address, uint8_t adc_command, uint16_t code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack;

  ack = i2c_write_word_data(i2c_address, adc_command, code);
  return(ack);
}

// Write a 32-bit code to the LTC4282.
int8_t LTC4282_write_32_bits(uint8_t i2c_address, uint8_t adc_command, uint32_t code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack;

  LT_union_int32_4bytes data;
  data.LT_int32 = code;

  ack = i2c_write_block_data(i2c_address, adc_command, (uint8_t) 4, data.LT_byte);

  return(ack);
}

// Write a 48-bit code to the LTC4282.
int8_t LTC4282_write_48_bits(uint8_t i2c_address, uint8_t adc_command, uint64_t code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack;

  LT_union_int64_8bytes data;
  data.LT_uint64 = code;

  ack = i2c_write_block_data(i2c_address, adc_command, (uint8_t) 6, data.LT_byte);

  return(ack);
}


// Reads an 8-bit adc_code from LTC4282
int8_t LTC4282_read(uint8_t i2c_address, uint8_t adc_command, uint8_t *adc_code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack;
  ack = i2c_read_byte_data(i2c_address, adc_command, adc_code);
  return ack;
}

// Reads a 16-bit adc_code from LTC4282
int8_t LTC4282_read_16_bits(uint8_t i2c_address, uint8_t adc_command, uint16_t *adc_code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int32_t ack;

  ack = i2c_read_word_data(i2c_address, adc_command, adc_code);

  return ack;
}

// Reads a 32-bit adc_code from LTC4282
int8_t LTC4282_read_32_bits(uint8_t i2c_address, uint8_t adc_command, uint32_t *adc_code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack;

  LT_union_int32_4bytes data;

  ack = i2c_read_block_data(i2c_address, adc_command, (uint8_t) 4, data.LT_byte);

  *adc_code = 0xFFFFFFFF & data.LT_int32;
  return(ack);
}

// Reads a 48-bit adc_code from LTC4282
int8_t LTC4282_read_48_bits(uint8_t i2c_address, uint8_t adc_command, uint64_t *adc_code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack;

  LT_union_int64_8bytes data;

  ack = i2c_read_block_data(i2c_address, adc_command, (uint8_t) 6, data.LT_byte);

  *adc_code =  0xFFFFFFFFFFFFLL &data.LT_uint64;
  return(ack);
}

// Convert ADC code to VGPIO
float LTC4282_code_to_VGPIO(uint16_t code)
// Returns floating point value of GPIO Voltage
{
  float value = (code*1.28)/(65535);
  return value;

}

// Convert ADC code to voltage
float LTC4282_code_to_voltage(uint16_t code, float fullscaleVoltage)
// Returns floating point value of voltage
{
  float value = (code*fullscaleVoltage)/((65536)-1);
  return value;
}

// Convert ADC code to current
float LTC4282_code_to_current(uint16_t code, float resistor)
// Returns floating point value of current
{
  float value = (code*.04)/(((65536)-1)*resistor);
  return value;
}

// Convert ADC code to power
float LTC4282_code_to_power(uint16_t code, float fullscaleVoltage, float resistor)
// Returns floating point value of power
{
  float value = (code*.04*fullscaleVoltage*65536.0)/(65535.0*65535.0*resistor);
  return value;
}
// Convert ADC code to energy
float LTC4282_code_to_energy(uint64_t code, float fullscaleVoltage, float resistor, float tConv)
// Returns floating point value of power
{
  float value = (code*.04*fullscaleVoltage*tConv*256.0)/(65535.0*65535.0*resistor);
  return value;
}

// Convert ADC code to coulombs
float LTC4282_code_to_coulombs(uint64_t code, float resistor, float tConv)
// Returns floating point value of coulombs
{
  float value = (code*.04*tConv)/(((65536)-1)*resistor);
  return value;
}

// Convert ADC code to average power
float LTC4282_code_to_avg_power(uint64_t code, float energy, float tConv)
// Returns floating point value of average power
{
  float value = energy/(tConv*code);
  return value;
}

// Convert ADC code to average current
float LTC4282_code_to_avg_current(uint64_t code, float coulombs, float tConv)
// Returns floating point value of average power
{
  float value = coulombs/(tConv*code);
  return value;
}

// Convert ADC code to GPIO alarm voltage
float LTC4282_code_to_GPIO_alarm(uint8_t code)
// Returns floating point value of GPIO alarm voltage
{
  float value = code*1.280/255;
  return value;
}

// Convert ADC code to alarm voltage
float LTC4282_code_to_volt_alarm(uint8_t code, float fullscaleVoltage)
// Returns floating point value of alarm voltage
{
  float value = code*fullscaleVoltage/255;
  return value;
}

// Convert ADC code to alarm current
float LTC4282_code_to_current_alarm(uint8_t code, float resistor)
// Returns floating point value of alarm current
{
  float value = (code*.04)/(255*resistor);
  return value;
}

// Convert ADC code to alarm power
float LTC4282_code_to_power_alarm(uint8_t code, float fullscaleVoltage, float resistor)
// Returns floating point value of alarm power
{
  float value = (code*fullscaleVoltage*.04*256)/(255.0*255.0*resistor);
  return value;
}

// Convert GPIO voltage to alarm code
uint8_t LTC4282_VGPIO_to_code_alarm(float vgpio)
// Returns the ADC code of the floating point value parameter
{
  uint8_t code = (int)(vgpio*255.0/1.280);
  return code;
}

// Convert voltage to alarm code
uint8_t LTC4282_volt_to_code_alarm(float volt, float fullscaleVoltage)
// Returns the ADC code of the floating point value parameter
{
  uint8_t code = (int)(volt*255.0/fullscaleVoltage);
  return code;
}

// Convert current to alarm code
uint8_t LTC4282_current_to_code_alarm(float current, float resistor)
// Returns the ADC code of the floating point value parameter
{
  uint8_t code = (current*255.0*resistor)/(.04);
  return code;
}

// Convert power to alarm code
uint8_t LTC4282_power_to_code_alarm(float power, float resistor, float fullscaleVoltage)
// Returns the ADC code of the floating point value parameter
{
  uint8_t code = (power*255.0*255.0*resistor/(256*.04*fullscaleVoltage));
  return code;
}