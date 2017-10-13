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

REVISION HISTORY
$Revision: 6237 $
$Date: 2016-12-20 15:09:16 -0800 (Tue, 20 Dec 2016) $

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
