/*!
LTC2642: 16-/14-/12-Bit VOUT DAC with SPI Interface
LTC2641: 16-/14-/12-Bit VOUT DAC with SPI Interface

@verbatim

The LTC2641/LTC2642 are families of 16-,14 and 12-bit unbuffered voltage output
DACs. These DACs operate from 2.7V to 5.5V supply and are monotonic over
temperature.

Both the LTC2641 and LTC2642 feature a reference input range of 2V to VDD. VOUT
swings from 0V to VREF . For bipolar operation, the LTC2642 includes matched scaling
resistors for use with an external precision op amp (such as the LT1678), generating
a ±VREF output swing at RFB.

They communicate via SPI interface. 16 bits of data are loaded through DIN
after a high-to-low transition of CS (MSB first). After 16 data bits have
been loaded into the serial input register, a low-to-high transition on CS
transfers the data to the 16-bit DAC latch, updating the DAC output.

For the 14-bit DACs, (LTC2641-14/LTC2642-14), the MSB remains in the same
(left-justified) position in the input 16-bit data word. Therefore, two
“don’t-care” bits must be loaded after the LSB, to make up the required
16 data bits. Similarly, for the 12-bit family members(LTC2641-12/LTC2642-12)
four “don’t-care” bits must follow the LSB.

@endverbatim

http://www.linear.com/product/LTC2642
http://www.linear.com/product/LTC2641

http://www.linear.com/product/LTC2642#demoboards
http://www.linear.com/product/LTC2641#demoboards

REVISION HISTORY
$Revision: 6880 $
$Date: 2017-04-05 15:34:50 -0700 (Wed, 05 Apr 2017) $

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

//! @ingroup Digital_to_Analog_Converters
//! @{
//! @defgroup LTC2642 LTC2642: 16-/14-/12-Bit VOUT DAC with SPI Interface
//! @}

/*! @file
    @ingroup LTC2642
    Library for LTC2642 16-/14-/12-Bit VOUT DAC with SPI Interface
*/

#include <SPI.h>
#include "LT_SPI.h"
#include "Linduino.h"
#include "LTC2642.h"

// Writes 16 bit data into serial input register
void LTC2642_write(uint8_t cs, uint16_t data)
{
  uint16_t rx;
  spi_transfer_word(cs, data, &rx);
}

// Calculates the 16 bit data code from voltage
uint16_t LTC2642_voltage_to_code(float voltage, float reference_voltage, uint8_t range)
{
  uint16_t code;
  if (range == UNIPOLAR)
  {
    code = voltage / reference_voltage * 0xFFFF ;
  }
  else
  {
    code = (voltage * 0x7FFF)/reference_voltage + 0x7FFF;
  }
  return code;
}

// Calculates the output voltage from the given digital code and reference voltage
float LTC2642_code_to_voltage(uint16_t code, float reference_voltage, uint8_t range)
{
  float voltage;
  if (range == UNIPOLAR)
  {
    voltage = code * reference_voltage / 0xFFFF ;
  }
  else
  {
    code = (int16_t)code;
    voltage = (((float)code/0x7FFF) - 1) * reference_voltage;
  }
  return voltage;
}