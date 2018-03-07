/*!
LTC2473: 16-Bit, Delta Sigma ADC with I2C interface.

@verbatim

The LTC2471/LTC2473 are small, 16-bit analog-to-digital converters with an
integrated precision reference and a selectable 208sps or 833sps output
rate. They use a single 2.7V to 5.5V supply and communicate through an I2C
Interface. The LTC2471 is single-ended with a 0V to VREF input range and
the LTC2473 is differential with a +-VREF input range. Both ADC's include
a 1.25V integrated reference with 2ppm/C drift performance and 0.1% initial
accuracy. They include an integrated oscillator and perform conversions with
no latency for multiplexed applications. The LTC2471/LTC2473 include a
proprietary input sampling scheme that reduces the average input current
several orders of magnitude when compared to conventional delta sigma
converters.

I2C DATA FORMAT (MSB FIRST);

Data Out:
Byte #1                                                   Byte #2
                                                          MSB
START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK   REPEAT START  D15 D14 D13 D12 D11 D10 D9 D8 MACK

Byte #3
LSB
D7 D6 D5 D4 D3 D2 D1 D0 MNACK STOP

Data In:
Byte #1                                     Byte #2
START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK   C3 C2 C1 C0 X X X X SACK STOP


START        : I2C Start
REPEAT Start : I2C Repeat Start
STOP         : I2C Stop
SACK         : I2C Slave Generated Acknowledge (Active Low)
MACK         : I2C Master Generated Acknowledge (Active Low)
MNACK        : I2C Master Generated Not Acknowledge
SAx  : I2C Address
W    : I2C Write (0)
R    : I2C Read  (1)
Cx   : Command Code
Dx   : Data Bits
X    : Don't care

Example Code:

Read LTC2473:
    uint8_t acknowledge;
    int32_t adc_code = 0;
    uint16_t timeout = 300; // timeout in microseconds
    float adc_voltage;
    acknowledge = LTC2473_read(i2c_address, &adc_code, timeout);

    // Convert adc_code to voltage
    adc_voltage = LTC2473_code_to_voltage(adc_code, LTC2473_lsb, LTC2473_offset_code);

@endverbatim

http://www.linear.com/product/LTC2473

http://www.linear.com/product/LTC2473#demoboards


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
    @ingroup LTC2473
    Header for LTC2473: 16-Bit, Delta Sigma ADC with I2C interface
*/

#ifndef LTC2473_H
#define LTC2473_H

/*! @name I2C_Addresses
@{ */

//! I2C address of the LTC2473.
//! Configured by tying the ADR0 pin high or low. See datasheet for details.
//! Uncomment LTC2473_I2C_ADDRESS to match demo board configuration.
//  Address assignment
// LTC2473 I2C Address                //  AD0
#define LTC2473_I2C_ADDRESS 0x14      //  Low 
//#define LTC2473_I2C_ADDRESS 0x54    //  High
/*!
@}
*/

/*! @name Mode Configuration
 @{
*/
#define LTC2473_ENABLE_PROGRAMMING  0x80
#define LTC2473_SPS_208             0x00
#define LTC2473_SPS_833             0x20
#define LTC2473_SLEEP               0x10
/*!
 @}
*/

/*Commands
Construct a control word by bitwise ORing one choice from the configuration
and the enable programming.

Example - Set the LTC2473 to 833 samples per second.
ack = LTC2473_write(i2c_address, LTC2473_ENABLE_PROGRAMMING | LTC2473_SPS_833);
*/

//! Reads from LTC2473.
//! @return  1 if no acknowledge, 0 if acknowledge
uint8_t LTC2473_read(uint8_t i2c_address,   //!< I2C address (7-bit format) for part
                     int32_t *adc_code,     //!< 2 byte conversion code read from LTC2473
                     uint16_t timeout       //!< The timeout in (microseconds)
                    );

//! Writes to the LTC2473
//! @return  1 if no acknowledge, 0 if acknowledge
uint8_t LTC2473_write(uint8_t i2c_address,  //!< I2C address (7-bit format) for part
                      uint8_t adc_command   //!< Command byte written to LTC2473
                     );

//! Calculates the voltage corresponding to an ADC code, given the reference (in volts)
//! @return Returns voltage calculated from ADC code.
float LTC2473_code_to_voltage(int32_t adc_code,     //!< Code read from adc
                              float vref            //!< VRef (in volts)
                             );

#endif  // LTC2473_H