/*!
LTC2460 Ultra-Tiny, 16-bit delta sigma ADCs with 10ppm/degree C Max Precision Reference

@verbatim

The LTC2460/LTC2462 are ultra tiny, 16-Bit analog-to- digital converters
with an integrated precision reference. They use a single 2.7V to 5.5V supply
and communicate through an SPI Interface. The LTC2460 is single-ended with
a 0V to VREF input range and the LTC2462 is differential with a ±VREF input
range. Both ADC’s include a 1.25V integrated reference with 2ppm/°C drift
performance and 0.1% initial accuracy. The converters are available in a
12-pin DFN 3mm × 3mm package or an MSOP-12 package. They include an integrated
oscillator and perform conversions with no latency for multiplexed applications.
The LTC2460/LTC2462 include a proprietary input sampling scheme that reduces
the average input current several orders of magnitude when compared to conventional
delta sigma converters.


SPI DATA FORMAT (MSB First):

            Byte #1                            Byte #2

Data Out :  D15 D14 D13 D12 D11 D10 D9 D8   D7 D6 D5 D4 D3 D2 D1 D10
Data In  :  EN1 EN2 SPD SLP  X   X  X  X    X  X  X  X  X  X  X  X


Dx   : Data Bits
EN1/EN2   : Enable Bits (00-keep previous mode, 10-change mode)
SPD  : Double Output Rate Select Bit (1-Normal rate 30hz, auto-calibration on, 0- 2x rate 60hz, auto_calibration off)
SLP : 1 - powers down chip reference after the next conversion is complete
Command Byte #1
EN1  EN2  SPD  SLP  Comments
0    X    X    X    Keep Previous Mode
1    0    1    X    60 Hz conversion
1    0    0    X    30 Hz auto-calibrated conversion

Example Code:

Read in 2X mode without turning off reference

    uint16_t miso_timeout = 1000;
    adc_command =LTC2460_SPEED_2X | LTC2460_REF_ON;   // Build ADC command for channel 0

    LTC2460_read(LTC2460_CS, adc_command, &adc_code);    // Throws out last reading
    delay(50);
    LTC2460_read(LTC2460_CS, adc_command, &adc_code);    // Obtains the current reading and stores to adc_code variable
    // Convert adc_code to voltage
    adc_voltage = LTC2460_code_to_voltage(adc_code, LTC2460_lsb);

@endverbatim

http://www.linear.com/product/LTC2460

http://www.linear.com/product/LTC2460#demoboards


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
    @ingroup LTC2460
    Header for LTC2460: Ultra-Tiny, 16-bit delta sigma ADCs with 10ppm/degree C Max Precision Reference
*/

#ifndef LTC2460_H
#define LTC2460_H

//! Define the SPI CS pin
#ifndef LTC2460_CS
#define LTC2460_CS QUIKEVAL_CS
#endif

/*! @name Mode Configuration
 @{
*/
#define LTC2460_KEEP_PREVIOUS_MODE              0x00
#define LTC2460_SPEED_1X                        0xA0
#define LTC2460_SPEED_2X                        0x80
#define LTC2460_REF_ON                          0x00
#define LTC2460_REF_OFF                         0x10
/*!
 @}
*/


/*Commands
Construct a control word by bitwise ORing bitfields defined above.

Example - read with 1X mode enabled and reference on.
adc_command = (LTC2460_SPEED_1X | LTC2460_REF_ON);
*/

//! Reads from LTC2460.
void LTC2460_read(uint8_t cs,           //!< Chip select
                  uint8_t adc_command,  //!< 1 byte command written to LTC2460
                  int32_t *adc_code     //!< 4 byte conversion code read from LTC2460
                 );

//! Calculates the voltage corresponding to an adc code, given the reference (in volts)
//! @return Returns voltage calculated from ADC code.
float LTC2460_code_to_voltage(int32_t adc_code,     //!< Code read from adc
                              float vref            //!< VRef (in volts)
                             );

#endif  // LTC2460_H

