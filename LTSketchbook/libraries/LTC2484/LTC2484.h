/*!
LTC2484: 24-Bit Delta Sigma ADC with Easy Drive Input Current Cancellation

@verbatim

The LTC2484 combines a 24-bit no latency delta-sigma analog-to-digital converter
with patented Easy Drive technology. The patented sampling scheme eliminates
dynamic input current errors and the shortcomings of on-chip buffering through
automatic cancellation of differential input current. This allows large external
source impedances and input signals with rail-to-rail input range to be directly
digitized while maintaining exceptional DC accuracy.

The LTC2484 includes an on-chip oscillator. The LTC2484 can be configured to
reject line frequencies. 50Hz, 60Hz or simultaneous 50Hz/60Hz line frequency
rejection can be selected as well as a 2x speed-up mode.

The LTC2484 allows a wide common mode input range (0V to VCC) independent of the
reference voltage. The reference can be as low as 100mV or can be tied directly
to VCC. The LTC2484 includes an on-chip trimmed oscillator, eliminating the need
for external crystals or oscillators. Absolute accuracy and low drift are
automatically maintained through continuous, transparent, offset and full-scale
calibration.

 SPI DATA FORMAT (MSB First):

             Byte #1                            Byte #2                           Byte #3                    Byte #4
 Data Out :  !EOC DMY SIG D23 D22 D21 D20 D19   D18 D17 D16 D15 D14 D13 D12 D11   D10 D9 D8 D7 D6 D5 D4 D3   D2 D1 D0 X X X X X
 Data In  :  EN   X   X   X   IM  FOA FOB SPD   X   X   X   X   X   X   X   X     X   X  X  X  X  X  X  X    X  X  X  X X X X X

 !EOC : End of Conversion Bit (Active Low)
 DMY  : Dummy Bit (Always 0)
 SIG  : Sign Bit (1-data positive, 0-data negative)
 Dx   : Data Bits
 EN   : Enable Bit (0-keep previous mode, 1-change mode)
 IM   : Internal Mode Bit (0-ADC input, 1-temperature sensor input)
 FoA  : Line Frequency Rejection Select Bit A
 FoB  : Line Frequency Rejection Select Bit B
 SPD  : Double Output Rate Select Bit (0-normal rate, auto-calibration on, 2x rate, auto_calibration off)
 X    : Don't Care

 Command Byte
 EN  IM   FoA  FoB  SPD   Comments
 0    X    X    X    X    Keep Previous Mode
 1    0    0    0    0    External Input, 50Hz and 60Hz Rejection, Autocalibration
 1    0    0    1    0    External Input, 50Hz Rejection, Autocalibration
 1    0    1    0    0    External Input, 60Hz Rejection, Autocalibration
 1    0    0    0    1    External Input, 50Hz and 60Hz Rejection, 2x Speed
 1    0    0    1    1    External Input, 50Hz Rejection, 2x Speed
 1    0    1    0    1    External Input, 60Hz Rejection, 2x Speed
 1    1    0    0    X    Temperature Input, 50Hz and 60Hz Rejection, Autocalibration
 1    1    0    1    X    Temperature Input, 50Hz Rejection, Autocalibration
 1    1    1    0    X    Temperature Input, 60Hz Rejection, Autocalibration
 1    X    1    1    X    Reserved, Do Not Use


Example Code:

Read the ADC voltage with 60Hz rejection

    uint16_t miso_timeout = 1000;
    // Build ADC command to read the ADC voltage
    adc_command = LTC2484_ENABLE;
    adc_command |= LTC2484_EXTERNAL_INPUT;
    adc_command |= LTC2484_AUTO_CALIBRATION;
    adc_command |= LTC2484_REJECTION_60HZ;


    if(LTC2484_EOC_timeout(LTC2484_CS, miso_timeout))    // Check for EOC
        return(1);
    LTC2484_read(LTC2484_CS, adc_command, &adc_code);   // Throw away last reading

    if(LTC2484_EOC_timeout(LTC2484_CS, miso_timeout))    // Check for EOC
        return(1);
    LTC2484_read(LTC2484_CS, adc_command, &adc_code);   // Obtains the current reading and stores to adc_code variable

    // Convert adc_code to voltage
    adc_voltage = LTC2484_code_to_voltage(adc_code, LTC2484_lsb, LTC2484_offset_code);

Read temperature

    uint16_t miso_timeout = 1000;

    // Build ADC command to read temperature
    adc_command = LTC2484_ENABLE;
    adc_command |= LTC2484_TEMPERATURE_INPUT;
    adc_command |= LTC2484_AUTO_CALIBRATION;

    if(LTC2484_EOC_timeout(LTC2484_CS, miso_timeout))    // Check for EOC
        return(1);
    LTC2484_read(LTC2484_CS, adc_command, &adc_code);   // Throw away last reading

    if(LTC2484_EOC_timeout(LTC2484_CS, miso_timeout))    // Check for EOC
        return(1);
    LTC2484_read(LTC2484_CS, adc_command, &adc_code);   // Obtains the current reading and stores to adc_code variable

    // Convert adc_code to temperature
    adc_temperature = LTC2484_temperature(adc_code, LTC2484_t0, LTC2484_r0);

@endverbatim

http://www.linear.com/product/LTC2484

http://www.linear.com/product/LTC2484#demoboards


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
    @ingroup LTC2484
    Header for 2484 24-Bit Delta Sigma ADC with Easy Drive Input Current Cancellation
*/


#ifndef LTC2484_H
#define LTC2484_H

//! define the SPI CS pin
#ifndef LTC2484_CS
#define LTC2484_CS QUIKEVAL_CS
#endif

//! @name LTC2484 Command constants
//!@{
//! Command constants. OR together to form the adc_command byte.
#define LTC2484_ENABLE              0x80
#define LTC2484_DISABLE             0x00

#define LTC2484_EXTERNAL_INPUT      0x00
#define LTC2484_TEMPERATURE_INPUT   0x08

#define LTC2484_REJECTION_50HZ_60HZ 0x00
#define LTC2484_REJECTION_50HZ      0x02
#define LTC2484_REJECTION_60HZ      0x04

#define LTC2484_AUTO_CALIBRATION    0x00
#define LTC2484_SPEED_2X            0x01
//! @}

// Commands
// Construct enable with any other command to form an int command. You can also enable 2Xmode,
// which will increase sample rate by a factor of 2.

//! Checks for EOC with a specified timeout
//! @return Returns 0=successful, 1=unsuccessful (exceeded timeout)
int8_t LTC2484_EOC_timeout(uint8_t cs,          //!< Chip Select pin
                           uint16_t miso_timeout //!< Timeout (in millisends)
                          );

//! Read LTC2484 result, program configuration for next conversion
// Example - read channel external input with 60Hz rejection and 2X enabled.
// adc_command = (LTC2484_EXTERNAL_INPUT | LTC2484_REJECTION_60HZ) | LTC2484_SPEED_2X;
//! @return void
void LTC2484_read(uint8_t cs,           //!< Chip Select pin
                  uint8_t adc_command,  //!< Command byte
                  int32_t *adc_code     //!< Returns raw 32-bit code read from ADC
                 );

//! Calculates the LTC2484 input bipolar voltage
//! @return Calculated voltage
float LTC2484_code_to_voltage(int32_t adc_code,           //!< Raw ADC code
                              float LTC2484_lsb,          //!< LSB value (volts)
                              int32_t LTC2484_offset_code //!< Offset (Code)
                             );

//! Calculate the LTC2484 temperature.
//! @return Calculated Temperature
float LTC2484_temperature(int32_t adc_code,               //!< ADC code
                          float LTC2484_t0,               //!< Temperature calibration value
                          float LTC2484_r0                //!< Voltage for temperature calibration value
                         );

//! Calibrate the lsb
//! @return Void
void LTC2484_cal_voltage(int32_t zero_code,               //!< Offset (Code)
                         int32_t fs_code,                 //!< Code measured with full-scale input applied
                         float zero_voltage,              //!< Measured zero voltage
                         float fs_voltage,                //!< Actual voltage applied during full-scale measurement
                         float *LTC2484_lsb,              //!< Returns LSB value (volts)
                         int32_t *LTC2484_offset_code     //!< Returns Offset (Code)
                        );

//! Calibrate temperature
//! @return Void
void LTC2484_cal_temperature(int32_t adc_code,            //!< ADC code
                             float temperature,           //!< Actual temperature
                             float *LTC2484_t0,           //!< Temperature calibration value
                             float *LTC2484_r0            //!< Voltage for temperature calibration value
                            );

#endif  // LTC2484_H
