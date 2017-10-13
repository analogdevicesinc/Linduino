/*!
LT3965 - Octal Matrix LED Bypass Switch

@verbatim

The LT3965 is an LED bypass switching device for dimming indiviadual LEDs in a
string using a common current source. It features eight individually controlled
floating source N-channel MOSFET switches rated for 17V/330 mΩ. The eight
switches can be connected in parallel and/or in series to bypass current around
one or more LEDs in a string. The LT3965 as a slave uses the I2C serial interface
to communicate with the master. Each of eight channels can be independently
programmed to bypass the LED string in constant on, constant off, dimming without
fade transition or dimming with fade transition mode.

@endverbatim

http://www.linear.com/product/LT3965

http://www.linear.com/product/LT3965#demoboards

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

//! @ingroup Switching_Regulators
//! @{
//! @defgroup LT3965 LT3965: Octal Matrix LED Bypass Switch
//! @}

/*! @file
   @ingroup LT3965
   Library for LT3965 Octal Matrix LED Bypass Switch
*/

#include <stdio.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LT3965.h"


// ACMODE Write Command to write a "value" byte to device at "address"
int8_t i2c_acwrite(uint8_t address, uint8_t value)
{
  int8_t ret = 0 ;
  address = address | AC_ADDR_0;
  ret |= i2c_write_byte(address, value);
  return ret;
}


// ACMODE Read Command to read 3 data bytes from device at "address"
int8_t i2c_acread(uint8_t address, uint8_t *values)
{
  int8_t ret = 0 ;
  address = address | AC_ADDR_0;
  ret |= i2c_read_block_data(address, 3, values);
  return ret;
}


// SCMODE Write Command Short Format to write a "value" byte to device at "address"
int8_t i2c_scwriteshort(uint8_t address, uint8_t channel, uint8_t data_SCMREG)
{
  int8_t ret = 0 ;
  uint8_t data = 0;

  address = address | SC_ADDR_0;
  data = (channel << 4) | data_SCMREG;      //Write value

  ret |= i2c_write_byte(address, data);
  return ret;
}


// SCMODE Write Command Long Format to write 2 "value" bytes to device at "address"
int8_t i2c_scwritelong(uint8_t address, uint8_t channel, uint8_t data_SCMREG, uint8_t dimming_value)
{
  int8_t ret = 0;
  uint8_t command = 0x80;

  address = address | SC_ADDR_0;
  command |= ((channel << 4) | data_SCMREG);

  ret |= i2c_write_byte_data(address, command, dimming_value);
  return ret;
}


// SCMODE Write Short + SCMODE Read Command
int8_t i2c_scwriteshort_scread(uint8_t address, uint8_t channel, uint8_t data_SCMREG, uint8_t *values)
{
  int8_t ret = 0;
  uint8_t command = 0;

  address = address | SC_ADDR_0;
  command |= ((channel << 4) | data_SCMREG);

  ret |= i2c_read_block_data(address, command, 2, values);
  return ret;
}


// BCMODE Read Command
int8_t i2c_bcread(uint8_t *value)
{
  int8_t ret = 0 ;
  uint8_t address = BC_ADDR;
  ret |= i2c_read_byte(address, value);
  return ret;
}
