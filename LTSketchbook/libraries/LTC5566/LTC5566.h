/*!
Copyright 2018(c) Analog Devices, Inc.

LTC5566: Dual Programmable Gain Downconverting Mixer

@verbatim

The LTC5566 dual programmable gain downconverting mixer
is ideal for diversity and MIMO receivers that require precise
gain setting. Each channel incorporates an active mixer and a
digital IF VGA with 15.5dB gain control range. The IF gain of
each channel is programmed in 0.5dB steps through the SPI.

SPI DATA FORMAT (MSB First):

            Byte #1                                                  Byte #2
Data In  :  RP2 RT2[1] RT2[0] IF2[4] IF2[3] IF2[2] IF2[1] IF2[0]     RP1 RT1[1] RT1[0] IF1[4] IF1[3] IF1[2] IF1[1] IF1[0]
            |---- ATT, TUNE, POWER CONTROL WORD FOR MIXER 2 ---|     |---- ATT, TUNE, POWER CONTROL WORD FOR MIXER 1 ---|

RPx : Reduced Power Mode Bit
RTx : RF Input Tune Control Bits
IFx : IF Attenuation Control Bits

@endverbatim

http://www.linear.com/product/LTC5566

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
    @ingroup LTC5566
    Header for LTC5566: Dual Programmable Gain Downconverting Mixer
*/

#ifndef LTC5566_H
#define LTC5566_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC5566_CS
#define LTC5566_CS QUIKEVAL_CS
#endif

//! @name LTC5566 reduced power mode configuration bit.
//! @{
// reduced power mode configuration bit.
#define LTC5566_FULL_POWER      0x00
#define LTC5566_REDUCED_POWER   0x80
//! @}

//! @name LTC5566 shutdown/powerup and GPO configuration bits.
//! @{
// shutdown/powerup and GPO configuration bits.
#define LTC5566_RF_TUNE_00      0x00
#define LTC5566_RF_TUNE_01      0x20
#define LTC5566_RF_TUNE_10      0x40
#define LTC5566_RF_TUNE_11      0x60
//! @}

//! Writes to the LTC5566 twice and reads back the last two bytes to make sure
//! the LTC5566 was loaded properly
//! @return void
void LTC5566_write(uint8_t cs,                  //! Chip Select pin
                   uint16_t tx,                 //! Word to be transmitted
                   uint16_t *rx                 //! Output pointer
                  );

//! Function to duplicate settings for both LTC5566 channels
//! @return Readback after writing duplicate settings to both channels
uint8_t LTC5566_dupl_settings();

//! Function to apply unique settings for each LTC5566 channel
//! @return Readback after writing different settings for each channel
uint16_t LTC5566_diff_settings();

//! Function to get data from user for power mode
//! @return uint8_t
uint8_t LTC5566_get_power_mode(
    char *prompt   //! Prompt to ask for the desired power mode
    );

//! Function to get data from user for RF input tune mode
//! @return uint8_t
uint8_t LTC5566_get_tune_mode(
    char *prompt    //! Prompt to ask for the desired RF tune mode
    );

//! Function to get data from user for IF attenuation
//! @return uint8_t
uint8_t LTC5566_get_att(
    char *prompt      //! Prompt to ask for the desired attenuation
    );

//! Decode the register value read from the LTC5555
//! @return void
void LTC5566_decode_output(
    uint8_t output   //! Raw output from the LTC5566 to decode
    );

#endif  // LTC5566_H
