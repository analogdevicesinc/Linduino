/*!
  LTC6804-1 Multicell Battery Monitor
@verbatim
  The LTC6804 is a 3rd generation multicell battery stack
  monitor that measures up to 12 series connected battery
  cells with a total measurement error of less than 1.2mV. The
  cell measurement range of 0V to 5V makes the LTC6804
  suitable for most battery chemistries. All 12 cell voltages
  can be captured in 290uS, and lower data acquisition rates
  can be selected for high noise reduction.

  Using the LTC6804-1, multiple devices are connected in
  a daisy-chain with one host processor connection for all
  devices.
@endverbatim
REVISION HISTORY
$Revision: 3659 $
$Date: 2013-12-13

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

Copyright 2013 Linear Technology Corp. (LTC)
***********************************************************/
//! @defgroup LTC68041 LTC6804-1: Multicell Battery Monitor

/*! @file
    @ingroup LTC68041
    Library for LTC6804-1 Multicell Battery Monitor
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "DC1304.h"
#include <SPI.h>

LTC6603::LTC6603(uint8_t device_cs)
{ 
  cs = device_cs;
  control_byte = 0x00;
  pinMode (cs, OUTPUT);  // Chip select needs to be an output
}

uint8_t LTC6603::set_control_byte(uint8_t mask, uint8_t mask_value)
{
  uint8_t rx = 0;
  control_byte = (control_byte & mask) | mask_value;
  spi_transfer_byte(cs, control_byte, &rx);
  return rx;
}

void LTC6603::set_multiplier(uint16_t m1, uint16_t m2, uint16_t m3)
{
  multiplier[0] = m1;
  multiplier[1] = m2;
  multiplier[2] = m3;
}

bool LTC6603::check_device_ready()
{
  uint8_t rx;
  spi_transfer_byte(cs, control_byte, &rx);
  return (rx == control_byte);
}

void LTC6603::print_control_byte()
{
  Serial.print ("\nControl byte 0x" + String(control_byte, HEX));
}




LTC6903::LTC6903(uint8_t device_cs)
{ 
  clock_cs = device_cs;
  pinMode (clock_cs, OUTPUT);  // Chip select needs to be an output

}

void LTC6903::set_frequency(float freq)
{
  uint16_t clock_code;
  //clock_code = LTC6903_frequency_to_code(freq/1000, LTC6903_output_config);
  clock_code = LTC6903_frequency_to_code(freq/1000, LTC6903_CLK_ON_CLK_INV_ON);
  Serial.print(F("\nClock code: 0x"));
  Serial.print(clock_code, HEX);
  LTC6903_write(LTC6903_CS, (uint16_t)clock_code);
}
