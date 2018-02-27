/*!
LTC6904: 1kHz to 68MHz Serial Port Programmable Oscillator

@verbatim

The LTC6903/LTC6904 are low power self contained digital
frequency sources providing a precision frequency from
1kHz to 68MHz, set through a serial port. The LTC6903/
LTC6904 require no external components other than a
power supply bypass capacitor, and they operate over a
single wide supply range of 2.7V to 5.5V.

@endverbatim

http://www.linear.com/product/LTC6904

http://www.linear.com/product/LTC6904#demoboards


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
    @ingroup LTC6904
    Library Header File for LTC6904: 1kHz to 68MHz Serial Port Programmable Oscillator
*/

#ifndef LTC6904_H
#define LTC6904_H

// LT6904 address options
// Uncomment the desired address
#define LTC6904_ADDRESS 0x17     //!< ADR 0
//#define LTC6904_ADDRESS 0x16     //!< ADR 1

/*!
@{
|   OCT Options     |   f>=  (MHz)  |   f< (MHz)    |
|   :-----------    |   :--------:  |   :-------:   |
|   LTC6904_OCT_15  |   34.05       |   68.03       |
|   LTC6904_OCT_14  |   17.02       |   34.01       |
|   LTC6904_OCT_13  |   8.5111      |   17.01       |
|   LTC6904_OCT_12  |   4.256       |   8.503       |
|   LTC6904_OCT_11  |   2.128       |   4.252       |
|   LTC6904_OCT_10  |   1.064       |   2.126       |
|   LTC6904_OCT_9   |   0.532       |   1.063       |
|   LTC6904_OCT_8   |   0.266       |   0.5314      |
|   LTC6904_OCT_7   |   0.133       |   0.2657      |
|   LTC6904_OCT_6   |   0.0665      |   0.1329      |
|   LTC6904_OCT_5   |   0.03325     |   0.06643     |
|   LTC6904_OCT_4   |   0.01662     |   0.03322     |
|   LTC6904_OCT_3   |   0.008312    |   0.01661     |
|   LTC6904_OCT_2   |   0.004156    |   0.008304    |
|   LTC6904_OCT_1   |   0.002078    |   0.004152    |
|   LTC6904_OCT_0   |   0.001039    |   0.002076    |
*/
// OCT Options
// f>=  (MHz)       | f< (MHz)
#define LTC6904_OCT_15  0x0F    // 34.05            | 68.03
#define LTC6904_OCT_14  0x0E    // 17.02            | 34.01
#define LTC6904_OCT_13  0x0D    // 8.5111           | 17.01
#define LTC6904_OCT_12  0x0C    // 4.256            | 8.503
#define LTC6904_OCT_11  0x0B    // 2.128            | 4.252
#define LTC6904_OCT_10  0x0A    // 1.064            | 2.126
#define LTC6904_OCT_9   0x09    // 0.532            | 1.063
#define LTC6904_OCT_8   0x08    // 0.266            | 0.5314
#define LTC6904_OCT_7   0x07    // 0.133            | 0.2657
#define LTC6904_OCT_6   0x06    // 0.0665           | 0.1329
#define LTC6904_OCT_5   0x05    // 0.03325          | 0.06643
#define LTC6904_OCT_4   0x04    // 0.01662          | 0.03322
#define LTC6904_OCT_3   0x03    // 0.008312         | 0.01661
#define LTC6904_OCT_2   0x02    // 0.004156         | 0.008304
#define LTC6904_OCT_1   0x01    // 0.002078         | 0.004152
#define LTC6904_OCT_0   0x00    // 0.001039         | 0.002076
//! @}

// Output Configuration
#define LTC6904_CLK_ON_CLK_INV_ON   0x00    //!< Clock on, inverted clock on
#define LTC6904_CLK_OFF_CLK_INV_ON  0x01    //!< Clock off, inverted clock on
#define LTC6904_CLK_ON_CLK_INV_OFF  0x02    //!< Clock on, inverted clock off
#define LTC6904_POWER_DOWN          0x03    //!< Powers down clocks

//! Writes 2 bytes
//! @return 1 if successful, 0 if failure
uint8_t LTC6904_write(uint8_t address,     //!< I2C address
                      uint16_t code        //!< Code to be written
                     );

//! Calculates the code necessary to create the clock frequency
//! @return the code for the LTC6904
uint16_t LTC6904_frequency_to_code(float frequency, //!< Clock frequency in MHz
                                   uint8_t clk       //!< Output configuration
                                  );

#endif

