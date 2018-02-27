/*!
LTC5599: 140 to 1000MHz IQ modulator

@verbatim

The LTC5599 is an IQ modulator with
serial I/O.

@endverbatim

http://www.linear.com/product/LTC5599

http://www.linear.com/product/LTC5599#demoboards


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
