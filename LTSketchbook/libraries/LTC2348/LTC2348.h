/*!
LTC2348-18: Octal, 18-Bit, 200ksps Differential ±10.24V Input SoftSpan ADC with Wide Input Common Mode Range.
LTC2348-16: Octal, 16-Bit, 200ksps Differential ±10.24V Input SoftSpan ADC with Wide Input Common Mode Range.
LTC2344-18: Quad, 18-Bit, 400ksps/ch Differential SoftSpan ADC with Wide Input Common Mode Range.
LTC2344-16: Quad, 16-Bit, 400ksps/ch Differential SoftSpan ADC with Wide Input Common Mode Range.
LTC2333-18: Buffered 8-Channel, 18-Bit, 800ksps Differential ±10.24V ADC with 30VP-P Common Mode Range.
LTC2333-16: Buffered 8-Channel, 16-Bit, 800ksps Differential ±10.24V ADC with 30VP-P Common Mode Range.
LTC2357-18: Buffered Quad, 18-Bit, 350ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.
LTC2357-16: Buffered Quad, 16-Bit, 350ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.
LTC2358-18: Buffered Octal, 18-Bit, 200ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.
LTC2358-16: Buffered Octal, 16-Bit, 200ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.
LTC2353-18: Buffered Dual, 18-Bit, 550ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.
LTC2353-16: Buffered Dual, 16-Bit, 550ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.


@verbatim

The LTC2348 is an 18-bit,low noise 8-channel simultaneous sampling successive
approximation register(SAR) ADC with differential,wide common mode range
inputs. Operating from a 5V low voltage supply, flexible high voltage supplies,
and using the internal reference and buffer, each channel of this SoftSpan ADC
can be independently configured on a conversion-by-conversion basis to accept
±10.24V, 0V to 10.24V, ±5.12V,or 0V to 5.12V signals. Individual channels may
also be disabled to increase throughput on the remaining channels

SPI DATA FORMAT :
            SoftSpan Configuration Word for Conversion N+1
Data In  :  S23 S22 S21 S20 S19 S18 S17 S16 S15 S14 S13 S12 S11 S10 S9  S8  S7  S6  S5  S4  S3  S2  S1  S0

            Conversion N   Channel 0
SDO0   :  D17 D16 D15 D14 D13 D12 D11 D10 D9  D8  D7  D6  D5  D4  D3  D2  D1  D0  C2  C1  C0  SS2 SS1 SS0

Sx   : SoftSpan Configuration Bits
Dx   : Data Bits
Cx   : Channel ID Bits
SSx  : SoftSpan Configuration Bits of corresponding channel

@endverbatim

http://www.linear.com/product/LTC2348-18
http://www.linear.com/product/LTC2348-16
http://www.linear.com/product/LTC2344-18
http://www.linear.com/product/LTC2344-16
http://www.linear.com/product/LTC2333-18
http://www.linear.com/product/LTC2333-16
http://www.linear.com/product/LTC2357-18
http://www.linear.com/product/LTC2357-16
http://www.linear.com/product/LTC2358-18
http://www.linear.com/product/LTC2358-16
http://www.linear.com/product/LTC2353-18
http://www.linear.com/product/LTC2353-16

http://www.linear.com/product/LTC2348-18#demoboards
http://www.linear.com/product/LTC2348-16#demoboards
http://www.linear.com/product/LTC2344-18#demoboards
http://www.linear.com/product/LTC2344-16#demoboards
http://www.linear.com/product/LTC2333-18#demoboards
http://www.linear.com/product/LTC2333-16#demoboards
http://www.linear.com/product/LTC2357-18#demoboards
http://www.linear.com/product/LTC2357-16#demoboards
http://www.linear.com/product/LTC2358-18#demoboards
http://www.linear.com/product/LTC2358-16#demoboards
http://www.linear.com/product/LTC2353-18#demoboards
http://www.linear.com/product/LTC2353-16#demoboards


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
    @ingroup LTC23XX
    Header for LTC23XX: 16/18-Bit Differential Input SoftSpan ADC with Wide Input Common Mode Range.
*/

#ifndef LTC2348_H
#define LTC2348_H

#define VREF 4.096
#define POW2_18 262144
#define POW2_17 131072

#define CHANNEL_NUMBER 0x38
#define SOFTSPAN 0x07

#define SNEAKER_PORT_I2C_ADDRESS 0x20

//! Creates 24-bit configuration word for the 8 channels.
void LTC23XX_create_config_word(uint8_t channel,      //!< Channel number
                                uint8_t config_number,    //!< Configuration number for the channel
                                uint32_t *config_word   //!< 24 bit config word created
                               );

//! Transmits 24 bits (3 bytes) of configuration information and
//! reads back 24 bytes of data (3 bytes/ 24 bits for each channel)
//! 24 bits: 18 bit data + 3 bit config + 3 bit channel number
void LTC23XX_read(uint8_t cs_pin,         //!< Chip select
                  uint32_t config_word,     //!< 3 bytes of configutaion data for 8 channels
                  uint8_t data_array[24]    //!< Data array to read in 24 bytes of data from 8 channels
                 );

//! Calculates the voltage from ADC output data depending on the channel configuration
float LTC23XX_voltage_calculator(uint32_t data,             //!< 24 bits of ADC output data for a single channel
                                 uint8_t channel_configuration      //!< 3 bits of channel configuration data
                                );

#endif