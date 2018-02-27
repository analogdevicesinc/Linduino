/*!
LTC4245: Hot Swap Controller with I2C Compatible Monitoring

@verbatim

The LTC4245 Hot Swap controller allows a board to be safely inserted and removed
from a live backplane. Using an external N-channel pass transistor, board supply
voltage and inrush current are ramped up at an adjustable rate. An I2C interface
and onboard ADC allow for monitoring of load current, voltage and fault status.

@endverbatim

http://www.linear.com/product/LTC4245

http://www.linear.com/product/LTC4245#demoboards


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
//! @defgroup LTC4245 LTC4245: Hot Swap Controller with I2C Compatible Monitoring
//! @}

/*! @file
    @ingroup LTC4245
    Library for LTC4245 Hot Swap Controller with I2C Compatible Monitoring
*/
#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include <Wire.h>
#include <SPI.h>
#include "LTC4245.h"

// Calculates voltage from register code data
float LTC4245_code_to_voltage(uint8_t register_code, float full_scale_voltage)
{
  float voltage;
  voltage = (float)register_code * full_scale_voltage / 255;   //! 1) Calculate voltage from code and full scale voltage
  return voltage;
}


// SMBus Alert ResponseProtocol: Sends an alert response command and releases /ALERT pin. LTC4245 responds with its address
int8_t LTC4245_ARA(uint8_t alert_response_address, uint8_t *i2c_address)
{
  int8_t ack;
  ack = i2c_read_byte(alert_response_address, i2c_address);
  return ack;
}

// Write an 8-bit code to the LTC4245
int8_t LTC4245_write(uint8_t i2c_address, uint8_t command, uint8_t code)
{
  int8_t ack;
  ack = i2c_write_byte_data(i2c_address,command,code);
  return ack;
}

// Reads an 8-bit adc_code from LTC4245
int8_t LTC4245_read(uint8_t i2c_address, uint8_t command, uint8_t *code)
{
  int8_t ack;
  ack = i2c_read_byte_data(i2c_address,command,code);
  return ack;
}
