/*!
LTC2380-24: Low Noise, High Speed, 24-Bit SAR ADC With Digital Filter

@verbatim



@endverbatim

http://www.linear.com/product/LTC2380-24


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
    @ingroup LTC2380-24
    Header for LTC2380-24: Low Noise, High Speed, 24-Bit SAR ADC With Digital Filter
*/

#ifndef LTC2380_H
#define LTC2380_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC2380_CS
#define LTC2380_CS QUIKEVAL_CS
#endif

//! @name LTC2380 Channel Address
//! @{
// Channel Address
#define LTC2380_ADDRESS             0x00
//!@}


//! Reads the LTC2380 and returns 32-bit data in 2's complement format
//! @return void
void LTC2380_read(int32_t *ptr_adc_code,    //!< Returns code read from ADC (from previous conversion)
                  int16_t *ptr_cycles
                 );


//! Calculates the LTC2380 input voltage given the binary data and lsb weight.
//! @return Floating point voltage
float LTC2380_code_to_voltage(int32_t adc_code,                   //!< Raw ADC code
                              uint8_t gain_compression,
                              float vref              //!< Reference voltage
                             );

#endif  //  LTC2380_H


