/*!
LTC5599: 140 to 1000MHz IQ modulator

@verbatim

The LTC5599 is an IQ modulator with
serial I/O.

@endverbatim

http://www.linear.com/product/LTC5599

http://www.linear.com/product/LTC5599#demoboards

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

//! @ingroup RF_Timing
//! @{
//! @defgroup LTC5599 LTC5599: Direct Conversion I/Q Modulator
//! @}

/*! @file
    @ingroup LTC5599
    Library for LTC5599: Direct Conversion I/Q Modulator
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC5599.h"
#include <SPI.h>
#include <math.h>


// Reads and returns 8-bit data
void LTC5599_write_read(uint8_t cs, uint8_t address, uint8_t rw, uint8_t tx, uint8_t *rx)
{
  address = address << 1;
  union
  {
    uint8_t b[2];
    uint16_t w;
  } command, data;

  if (rw == 0) //write
  {
    command.b[1] = address;
    command.b[0] = tx;  //write this command byte

    spi_transfer_word(cs,command.w,&data.w);  //write an address byte and a command byte to the device
  }
  else
  {
    address = address + 0x01;
    command.b[1] = address;
    command.b[0] = 0x00;  //read into this data byte

    spi_transfer_word(cs,command.w,&data.w);  //write an address byte to the device and read back a byte
    *rx = data.b[0];
  }
}
