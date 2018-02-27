/*!
LTC2642: 16-/14-/12-Bit VOUT DAC with SPI Interface
LTC2641: 16-/14-/12-Bit VOUT DAC with SPI Interface

@verbatim

The LTC2641/LTC2642 are families of 16-,14 and 12-bit unbuffered voltage output
DACs. These DACs operate from 2.7V to 5.5V supply and are monotonic over
temperature.

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
    @ingroup LTC2642
    Header for LTC2642: 16-/14-/12-Bit VOUT DAC with SPI Interface
*/

#ifndef LTC2642_H
#define LTC2642_H

//! Define the SPI CS pin
#ifndef LTC2642_CS
#define LTC2642_CS QUIKEVAL_CS
#endif

#define UNIPOLAR  1
#define BIPOLAR   2

//! Writes the 16-bit data into the DAC
//! @return void
void LTC2642_write(uint8_t cs,            //!< Chip Select Pin
                   uint16_t data          //!< 16-bit digital input data
                  );
//! Calculates the output voltage from the given digital code and reference voltage
//! @return Floating point voltage
float LTC2642_code_to_voltage(uint16_t data,    //!< 16-bit digital input data
                              float reference_voltage,      //!< Reference voltage
                              uint8_t range  //!< Unipolar/Bipolar
                             );
//! Calculates the 16 bit data code from voltage
//! @return 16-bit code
uint16_t LTC2642_voltage_to_code(float voltage,   //!< Analog voltage to be converted into code
                                 float reference_voltage,     //!< Reference Voltage
                                 uint8_t range  //!< Unipolar/Bipolar
                                );
#endif      //LTC2642_H
