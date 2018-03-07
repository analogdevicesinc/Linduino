/*!
LTM2893-1: 100MHz ADC SPI Isolator

@verbatim

Example Code:

Read ADC input.

    LTM2893_read(LTM2893_CS, &adc_code);   // Throws out last reading
    LTM2893_read(LTM2893_CS, &adc_code);   // Obtains the current reading and stores to adc_code variable

    // Convert adc_code to voltage
    adc_voltage = LTM2893_code_to_voltage(adc_code, vref);

@endverbatim

http://www.linear.com/product/LTM2893

http://www.linear.com/product/LTM2893#demoboards


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
    @ingroup LTM2893
    Header for LTM2893: Interface LTC2328 16/18-bit 1Msps ADC
*/

#ifndef LTM2893_H
#define LTM2893_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTM2893_CS
#define LTM2893_CS QUIKEVAL_CS
#endif

#define LTM2893_CSC_PIN QUIKEVAL_GPIO

//! @name LTM2893 Channel Address
//! @{
// Channel Address
//#define LTM2893_ADDRESS             0x00
//!@}

// UserConfig 0 Register
#define LTM2893_UC0_SA_DIR_ISO2LOGIC (1 << 0)
#define LTM2893_UC0_SB_DIR_ISO2LOGIC (1 << 1)
#define LTM2893_UC0_SC_DIR_ISO2LOGIC (1 << 2)
#define LTM2893_UC0_CRC              (1 << 3)
//- OSCDIV
#define LTM2893_UC0_OSCDIV_100       (0 << 4)
#define LTM2893_UC0_OSCDIV_66        (1 << 4)
#define LTM2893_UC0_OSCDIV_50        (2 << 4)
#define LTM2893_UC0_OSCDIV_40        (3 << 4)
#define LTM2893_UC0_OSCDIV_33        (4 << 4)
#define LTM2893_UC0_OSCDIV_25        (5 << 4)
#define LTM2893_UC0_OSCDIV_12        (6 << 4)
#define LTM2893_UC0_OSCDIV_6         (7 << 4)

// UserConfig 1 Register
//- OSCDIV
#define LTM2893_UC1_WORDL_8          ((0 << 0) | (1 << 7))
#define LTM2893_UC1_WORDL_12         ((1 << 0) | (1 << 7))
#define LTM2893_UC1_WORDL_14         ((2 << 0) | (1 << 7))
#define LTM2893_UC1_WORDL_16         ((3 << 0) | (1 << 7))
#define LTM2893_UC1_WORDL_18         ((4 << 0) | (1 << 7))
#define LTM2893_UC1_WORDL_20         ((5 << 0) | (1 << 7))
#define LTM2893_UC1_WORDL_24         ((6 << 0) | (1 << 7))
#define LTM2893_UC1_WORDL_32         ((7 << 0) | (1 << 7))
//- DEVCNT
#define LTM2893_UC1_DEVCNT_1         ((0 << 3) | (1 << 7))
#define LTM2893_UC1_DEVCNT_2         ((1 << 3) | (1 << 7))
#define LTM2893_UC1_DEVCNT_3         ((2 << 3) | (1 << 7))
#define LTM2893_UC1_DEVCNT_4         ((3 << 3) | (1 << 7))

//! Reads the LTM2893 and returns 32-bit data in 2's complement format
//! @return void
void LTM2893_read(uint8_t cs,           //!< Chip Select Pin
                  int32_t *ptr_adc_code    //!< Returns code read from ADC (from previous conversion)
                 );


//! Calculates the LTM2893 input voltage given the binary data and lsb weight.
//! @return Floating point voltage
float LTM2893_code_to_voltage(int32_t adc_code,                   //!< Raw ADC code
                              float vref              //!< Reference voltage
                             );

void LTM2893_config(uint8_t user_config);

#endif  //  LTM2893_H


