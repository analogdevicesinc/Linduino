/*!
LTC2440: 24-Bit, Differential Delta Sigma ADCs with Selectable Speed/Resolution.

@verbatim

The LTC2440 is a high speed 24-bit No Latency Delta Sigma TM ADC with 5ppm
INL and 5uV offset. It uses proprietary delta-sigma architecture enabling
variable speed and resolution with no latency. Ten speed/resolution
combinations (6.9Hz/200nVRMS to 3.5kHz/25uVRMS) are programmed through a
simple serial interface. Alternatively, by tying a single pin HIGH or
LOW, a fast (880Hz/2uVRMS) or ultralow noise (6.9Hz, 200nVRMS, 50/60Hz
rejection) speed/resolution combination can be easily selected. The
accuracy (offset, full-scale, linearity, drift) and power dissipation are
independent of the speed selected. Since there is no latency, a
speed/resolution change may be made between conversions with no
degradation in performance.


SPI DATA FORMAT (MSB First):

            Byte #1                            Byte #2

Data Out :  !EOC DMY SIG D28 D27 D26 D25 D24   D23 D22 D21 D20 D19 D18 D17 D16
Data In  :  OSR4 OSR3 OSR2 OSR1 OSR1 X X X     X    X  X   X   X  X  X  X

Byte #3                        Byte #4
D15 D14 D13 D12 D11 D10 D9 D8  D7 D6 D5 D4 *D3 *D2 *D1 *D0
X   X   X   X   X   X   X  X   X  X  X  X   X  X   X   X

!EOC : End of Conversion Bit (Active Low)
DMY  : Dummy Bit (Always 0)
SIG  : Sign Bit (1-data positive, 0-data negative)
Dx   : Data Bits
EN   : Enable Bit (0-keep previous mode, 1-change mode)
SGL  : Enable Single-Ended Bit (0-differential, 1-single-ended)
Sx   : Address Select Bit
0SRX : Over Sampling Rate Bits

                          |      CONVERSION RATE          |
                          |   INTERNAL   |   EXTERNAL     |
OSR4 OSR3 OSR2 OSR1 OSR1  |  9MHz CLOCK  |  10.24MHz CLOCK  |  RMS NOISE  |  ENOB  |  OSR
x    0    0    0    1        3.52kHz        4kHz               23uV          17       64
x    0    0    1    0        1.76kHz        2kHz               3.5uV         20       128
0    0    0    0    0        880Hz          1kHz               2uV           21.3     256*
x    0    0    1    1        880Hz          1kHz               2uV           21.3     256
x    0    1    0    0        440Hz          500Hz              1.4uV         21.8     512
x    0    1    0    1        220Hz          250Hz              1uV           22.4     1024
x    0    1    1    0        110Hz          125Hz              750nV         22.9     2048
x    0    1    1    1        55Hz           62.5Hz             510nV         23.4     4096
x    1    0    0    0        27.5Hz         31.25Hz            375nV         24       8192
X    1    0    0    1        13.75Hz        15.625Hz           250nV         24.4     16384
X    1    1    1    1        6.87kHz        7.8125Hz           200nV         24.6     32768**
** Address allows tying SDI HIGH  *Additional address to allow tying SDI LOW


Example Code:

Read ADC with OSR of 65536

    uint16_t miso_timeout = 1000;

    if(!LTC2440_EOC_timeout(LTC2440_CS, miso_timeout)) // Check for EOC
      LTC2440_read(LTC2440_CS, adc_command, &adc_code);     // Throws out reading
    else
    {
      return;
    }

    if(!LTC2440_EOC_timeout(LTC2440_CS, miso_timeout)) // Check for EOC
      LTC2440_read(LTC2440_CS, adc_command, &adc_code); // Take valid reading
    else
    {
      return;
    }

    // Convert adc_code to voltage
    adc_voltage = LTC2440_code_to_voltage(adc_code, LTC2440_vref);

@endverbatim

http://www.linear.com/product/LTC2440

http://www.linear.com/product/LTC2440#demoboards


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
    @ingroup LTC2440
    Header for LTC2440: 24-Bit, Differential Delta Sigma ADCs with Selectable Speed/Resolution.
*/

#ifndef LTC2440_H
#define LTC2440_H

//! Define the SPI CS pin
#ifndef LTC2440_CS
#define LTC2440_CS QUIKEVAL_CS
#endif

/*! @name Oversample Ratio (OSR) Commands
@{ */
#define LTC2440_OSR_64         0x08
#define LTC2440_OSR_128        0x10
#define LTC2440_OSR_256_       0x00 // See note above
#define LTC2440_OSR_256        0x18
#define LTC2440_OSR_512        0x20
#define LTC2440_OSR_1024       0x28
#define LTC2440_OSR_2048       0x30
#define LTC2440_OSR_4096       0x38
#define LTC2440_OSR_8192       0x40
#define LTC2440_OSR_16384      0x48
#define LTC2440_OSR_32768      0x78
/*! @}*/


//! Checks for EOC with a specified timeout.
//! @return Returns 0=successful, 1=unsuccessful (exceeded timeout)
int8_t LTC2440_EOC_timeout(uint8_t cs,           //!< Chip Select pin
                           uint16_t miso_timeout  //!< Timeout (in milliseconds)
                          );

//! Reads from LTC2440.
void LTC2440_read( uint8_t cs,               //!< Chip select
                   uint8_t adc_command,      //!< 1 byte command written to LTC2440
                   int32_t *adc_code         //!< 4 byte conversion code read from LTC2440
                 );

//! Calculates the voltage corresponding to an adc code, given the reference (in volts)
//! @return Returns voltage calculated from ADC code.
float LTC2440_code_to_voltage(int32_t adc_code,     //!< Code read from adc
                              float vref            //!< VRef (in volts)
                             );
#endif  // LTC2440_H