/*!
LTC2942: Battery Gas Gauge with Temperature, Voltage Measurement.
LTC2941: Battery Gas Gauge with I2C Interface

@verbatim



@endverbatim

http://www.linear.com/product/LTC2942
http://www.linear.com/product/LTC2941

http://www.linear.com/product/LTC2942#demoboards
http://www.linear.com/product/LTC2941#demoboards

REVISION HISTORY
$Revision: 6595 $
$Date: 2017-02-27 15:04:18 -0800 (Mon, 27 Feb 2017) $

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

//! @ingroup Power_Monitors
//! @{
//! @defgroup LTC2942 LTC2942:  Battery Gas Gauge with Temperature, Voltage Measurement
//! @}

/*! @file
   @ingroup LTC2942
   Library for LTC2942  Battery Gas Gauge with Temperature, Voltage Measurement
*/


#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LTC2942.h"
#include <Wire.h>

// Write an 8-bit code to the LTC2942.
int8_t LTC2942_write(uint8_t i2c_address, uint8_t adc_command, uint8_t code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int32_t ack;

  ack = i2c_write_byte_data(i2c_address, adc_command, code);
  return(ack);
}


// Write a 16-bit code to the LTC2942.
int8_t LTC2942_write_16_bits(uint8_t i2c_address, uint8_t adc_command, uint16_t code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack;

  ack = i2c_write_word_data(i2c_address, adc_command, code);
  return(ack);
}

// Reads an 8-bit adc_code from LTC2942
int8_t LTC2942_read(uint8_t i2c_address, uint8_t adc_command, uint8_t *adc_code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int32_t ack;

  ack = i2c_read_byte_data(i2c_address, adc_command, adc_code);

  return(ack);
}

// Reads a 16-bit adc_code from LTC2942
int8_t LTC2942_read_16_bits(uint8_t i2c_address, uint8_t adc_command, uint16_t *adc_code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int32_t ack;

  ack = i2c_read_word_data(i2c_address, adc_command, adc_code);

  return(ack);
}


float LTC2942_code_to_coulombs(uint16_t adc_code, float resistor, uint16_t prescalar)
// The function converts the 16-bit RAW adc_code to Coulombs
{
  float coulomb_charge;
  coulomb_charge =  1000*(float)(adc_code*LTC2942_CHARGE_lsb*prescalar*50E-3)/(resistor*128);
  coulomb_charge = coulomb_charge*3.6f;
  return(coulomb_charge);
}

float LTC2942_code_to_mAh(uint16_t adc_code, float resistor, uint16_t prescalar )
// The function converts the 16-bit RAW adc_code to mAh
{
  float mAh_charge;
  mAh_charge = 1000*(float)(adc_code*LTC2942_CHARGE_lsb*prescalar*50E-3)/(resistor*128);
  return(mAh_charge);
}

float LTC2942_code_to_voltage(uint16_t adc_code)
// The function converts the 16-bit RAW adc_code to Volts
{
  float voltage;
  voltage = ((float)adc_code/(65535))*LTC2942_FULLSCALE_VOLTAGE;
  return(voltage);
}


float LTC2942_code_to_kelvin_temperature(uint16_t adc_code)
// The function converts the 16-bit RAW adc_code to Kelvin
{
  float temperature;
  temperature = adc_code*((float)(LTC2942_FULLSCALE_TEMPERATURE)/65535);
  return(temperature);
}

float LTC2942_code_to_celcius_temperature(uint16_t adc_code)
// The function converts the 16-bit RAW adc_code to Celcius
{
  float temperature;
  temperature = adc_code*((float)(LTC2942_FULLSCALE_TEMPERATURE)/65535) - 273.15;
  return(temperature);
}

// Used to set and clear bits in a control register.  bits_to_set will be bitwise OR'd with the register.
// bits_to_clear will be inverted and bitwise AND'd with the register so that every location with a 1 will result in a 0 in the register.
int8_t LTC2942_register_set_clear_bits(uint8_t i2c_address, uint8_t register_address, uint8_t bits_to_set, uint8_t bits_to_clear)
{
  uint8_t register_data;
  int8_t ack = 0;

  ack |= LTC2942_read(i2c_address, register_address, &register_data);
  register_data = register_data & (~bits_to_clear);
  register_data = register_data | bits_to_set;
  ack |= LTC2942_write(i2c_address, register_address, register_data);
  return(ack);
}



