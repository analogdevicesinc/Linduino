/*!
LTC4261: Negative Voltage Hot Swap Controller with ADC and I2C Monitoring

@verbatim

The LTC4261/LTC4261-2 negative voltage Hot SwapTM controller allows a board
to be safely inserted and removed from a live backplane. Using an external
N-channel pass transistor, the board supply voltage can be ramped at an
adjustable rate. The devices feature independently adjustable inrush current
and overcurrent limits to minimize stresses on the pass transistor during
start-up, input step and output short conditions. The LTC4261 defaults
to latch-off while the LTC4261-2 defaults to auto-retry on overcurrent faults.

@endverbatim

http://www.linear.com/product/LTC4261

http://www.linear.com/product/LTC4261#demoboards


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
//! @defgroup LTC4261 LTC4261: Negative Voltage Hot Swap Controllers with ADC and I2C Monitoring
//! @}

/*! @file
  @ingroup LTC4261
  Library for LTC4261: Negative Voltage Hot Swap Controllers with ADC and I2C Monitoring
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LTC4261.h"
#include <Wire.h>

// Writes an 8-bit code to the LTC4261
int8_t LTC4261_write(uint8_t i2c_address, uint8_t command, uint8_t code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge
{
  int8_t ack;
  ack = i2c_write_byte_data(i2c_address,command,code);
  return ack;
}

// Reads an 8-bit code from the LTC4261
int8_t LTC4261_read(uint8_t i2c_address, uint8_t command, uint8_t *code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge
{
  int8_t ack;
  ack = i2c_read_byte_data(i2c_address,command,code);
  return ack;
}

// Reads a 10-bit adc_code from LTC4261
int8_t LTC4261_read_10_bits(uint8_t i2c_address, uint8_t adc_command, uint16_t *adc_code)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge
{
  int8_t ack;
  uint8_t values[2];
  ack = i2c_read_block_data(i2c_address,adc_command, 2,values);
  *adc_code = (uint16_t)(values[0]) <<2 | values[1]>>6;
  return ack;
}

// SMBus Alert Response Protocol: Sends an alert response command and releases /ALERT pin. LTC4261 responds with its address
int8_t LTC4261_ARA(uint8_t alert_response_address, uint8_t *i2c_address)
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge
{
  int8_t ack;
  ack = i2c_read_byte(alert_response_address, i2c_address);
  return ack;
}
// Calculates the LTC4261 ADIN voltage
float LTC4261_ADIN_code_to_voltage(uint16_t adc_code, float LTC4261_ADIN_lsb, float resistor_divider_ratio)
// Returns the ADIN Voltage in Volts
{
  float adc_voltage;
  adc_voltage = (float)adc_code*LTC4261_ADIN_lsb;   //! 1) Calculate voltage from code and ADIN lsb
  return(adc_voltage*resistor_divider_ratio);
}

// Calculates the LTC4261 current with a sense resistor
float LTC4261_code_to_current(uint16_t adc_code, float resistor, float LTC4261_DELTA_SENSE_lsb)
// Returns the LTC4261 sense current
{
  float voltage, current;
  voltage = (float)adc_code*LTC4261_DELTA_SENSE_lsb;    //! 1) Calculate voltage from ADC code and delta sense lsb
  current = voltage/resistor;                           //! 2) Calculate current, I = V/R
  return(current);
}