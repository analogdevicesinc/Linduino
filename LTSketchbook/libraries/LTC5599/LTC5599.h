/*!
LTC5599: 140 to 1000MHz IQ modulator

@verbatim


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

/*! @file
    @ingroup LTC5599
    Header for LTC5599: Direct Conversion I/Q Modulator
*/

#ifndef LTC5599_H
#define LTC5599_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC5599_CS
#define LTC5599_CS QUIKEVAL_CS
#endif

//! @name LTC5599 read/write bits
//! @{
// Write Command
#define LTC5599_WRITE       0x00
// Read Command
#define LTC5599_READ        0x01
//!@}


//! Reads the modulator register and returns 8-bit data
//! @return void
void LTC5599_write_read(uint8_t cs,           //!< Chip Select Pin
                        uint8_t address,  //!< Channel address, config bits ORed together
                        uint8_t rw,
                        uint8_t tx,
                        uint8_t *rx    //!< Returns code read from device
                       );

#endif  //  LTC5599_H