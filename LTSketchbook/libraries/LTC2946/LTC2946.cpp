/*!
LTC2946: 12-Bit Wide Range Power, Charge and Energy Monitor

@verbatim

The LTC®2946 is a rail-to-rail system monitor that measures
current, voltage, power, charge and energy. It features an
operating range of 2.7V to 100V and includes a shunt regulator
for supplies above 100V. The current measurement common mode
range of 0V to 100V is independent of the input supply.
A 12-bit ADC measures load current, input voltage and an
auxiliary external voltage. Load current and internally
calculated power are integrated over an external clock or
crystal or internal oscillator time base for charge and energy.
An accurate time base allows the LTC2946 to provide measurement
accuracy of better than ±0.6% for charge and ±1% for power and
energy. Minimum and maximum values are stored and an overrange
alert with programmable thresholds minimizes the need for software
polling. Data is reported via a standard I2C interface.
Shutdown mode reduces power consumption to 15uA.

@endverbatim

http://www.linear.com/product/LTC2946

http://www.linear.com/product/ltc2946#demoboards


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

//! @ingroup Power_Monitors
//! @{
//! @defgroup LTC2946 LTC2946: 12-Bit Wide Range Power, Charge and Energy Monitor
//! @}

/*! @file
   @ingroup LTC2946
   Library for LTC2946 12-Bit Wide Range Power, Charge and Energy Monitor
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LTC2946.h"
#include <Wire.h>

// Write an 8-bit code to the LTC2946.
int8_t LTC2946_write(uint8_t i2c_address, uint8_t adc_command, uint8_t code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int32_t ack;

  ack = i2c_write_byte_data(i2c_address, adc_command, code);

  return ack;

}

// Write a 16-bit code to the LTC2946.
int8_t LTC2946_write_16_bits(uint8_t i2c_address, uint8_t adc_command, uint16_t code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack;

  ack = i2c_write_word_data(i2c_address, adc_command, code);
  return(ack);
}

// Write a 24-bit code to the LTC2946.
int8_t LTC2946_write_24_bits(uint8_t i2c_address, uint8_t adc_command, uint32_t code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack;

  LT_union_int32_4bytes data;
  data.LT_int32 = code;

  ack = i2c_write_block_data(i2c_address, adc_command, (uint8_t) 3, data.LT_byte);

  return(ack);
}

int8_t LTC2946_write_32_bits(uint8_t i2c_address, uint8_t adc_command, uint32_t code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack;

  LT_union_int32_4bytes data;
  data.LT_int32 = code;

  ack = i2c_write_block_data(i2c_address, adc_command, (uint8_t) 4, data.LT_byte);

  return(ack);
}

// Reads an 8-bit adc_code from LTC2946
int8_t LTC2946_read(uint8_t i2c_address, uint8_t adc_command, uint8_t *adc_code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int32_t ack;

  ack = i2c_read_byte_data(i2c_address, adc_command, adc_code);

  return ack;
}

// Reads a 12-bit adc_code from LTC2946
int8_t LTC2946_read_12_bits(uint8_t i2c_address, uint8_t adc_command, uint16_t *adc_code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  // Use union type defined in Linduino.h to combine two uint8_t's (8-bit unsigned integers) into one uint16_t (unsigned 16-bit integer)
  // Then, shift by 4 bits and return in *adc_code
  int32_t ack;

  ack = i2c_read_word_data(i2c_address, adc_command, adc_code);

  *adc_code >>= 4;
  return ack;
}

// Reads a 16-bit adc_code from LTC2946
int8_t LTC2946_read_16_bits(uint8_t i2c_address, uint8_t adc_command, uint16_t *adc_code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int32_t ack;

  ack = i2c_read_word_data(i2c_address, adc_command, adc_code);

  return ack;
}

// Reads a 24-bit adc_code from LTC2946
int8_t LTC2946_read_24_bits(uint8_t i2c_address, uint8_t adc_command, uint32_t *adc_code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack;

  LT_union_int32_4bytes data;

  ack = i2c_read_block_data(i2c_address, adc_command, (uint8_t)3, data.LT_byte);

  *adc_code = 0x0FFFFFF & data.LT_int32;
  return(ack);
}

// Reads a 32-bit adc_code from LTC2946
int8_t LTC2946_read_32_bits(uint8_t i2c_address, uint8_t adc_command, uint32_t *adc_code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack;

  LT_union_int32_4bytes data;

  ack = i2c_read_block_data(i2c_address, adc_command, (uint8_t) 4, data.LT_byte);

  *adc_code = 0xFFFFFFFF & data.LT_int32;
  return(ack);
}

// Calculate the LTC2946 VIN voltage
float LTC2946_VIN_code_to_voltage(uint16_t adc_code, float LTC2946_VIN_lsb)
// Returns the VIN Voltage in Volts
{
  float voltage;
  voltage = (float)adc_code*LTC2946_VIN_lsb;    //! 1) Calculate voltage from code and lsb
  return(voltage);
}

// Calculate the LTC2946 ADIN voltage
float LTC2946_ADIN_code_to_voltage(uint16_t adc_code, float LTC2946_ADIN_lsb)
// Returns the ADIN Voltage in Volts
{
  float adc_voltage;
  adc_voltage = (float)adc_code*LTC2946_ADIN_lsb;   //! 1) Calculate voltage from code and ADIN lsb
  return(adc_voltage);
}

// Calculate the LTC2946 current with a sense resistor
float LTC2946_code_to_current(uint16_t adc_code, float resistor, float LTC2946_DELTA_SENSE_lsb)
// Returns the LTC2946 current in Amps
{
  float voltage, current;
  voltage = (float)adc_code*LTC2946_DELTA_SENSE_lsb;    //! 1) Calculate voltage from ADC code and delta sense lsb
  current = voltage/resistor;                           //! 2) Calculate current, I = V/R
  return(current);
}

// Calculate the LTC2946 power
float LTC2946_code_to_power(int32_t adc_code, float resistor, float LTC2946_Power_lsb)
// Returns The LTC2946 power in Watts
{
  float power;
  power = (float)adc_code*LTC2946_Power_lsb/resistor;  //! 1) Calculate Power using Power lsb and resistor

  return(power);
}


// Calculate the LTC2946 energy
float LTC2946_code_to_energy(int32_t adc_code,float resistor, float LTC2946_Power_lsb, float LTC2946_TIME_lsb)
// Returns the LTC2946 energy in Joules
{
  float energy_lsb, energy;
  energy_lsb=(float)(LTC2946_Power_lsb/resistor)*65536*LTC2946_TIME_lsb;   //! 1) Calculate Energy lsb from Power lsb and Time lsb
  energy = adc_code*energy_lsb;                               //! 2) Calculate Energy using Energy lsb and adc code
  return(energy);
}

// Calculate the LTC2946 Coulombs
float LTC2946_code_to_coulombs(int32_t adc_code, float resistor, float LTC2946_DELTA_SENSE_lsb, float LTC2946_TIME_lsb)
// Returns the LTC2946 Coulombs
{
  float coulomb_lsb, coulombs;
  coulomb_lsb=(float)(LTC2946_DELTA_SENSE_lsb/resistor)*16*LTC2946_TIME_lsb;   //! 1) Calculate Coulomb lsb Current lsb and Time lsb
  coulombs = adc_code*coulomb_lsb;                                             //! 2) Calculate Coulombs using Coulomb lsb and adc code
  return(coulombs);
}

//Calculate the LTC2946 Time in Seconds
float LTC2946_code_to_time(float time_code, float LTC2946_TIME_lsb)
{
  float seconds;
  seconds = LTC2946_TIME_lsb * time_code;
  return seconds;
}

