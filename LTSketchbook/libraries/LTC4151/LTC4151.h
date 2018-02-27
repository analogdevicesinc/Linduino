/*!
LTC4151: High Voltage I2C Current and Voltage Monitor

@verbatim

The LTC4151 is a high side power monitor that operates over a wide voltage
range of 7V to 80V. In default operation mode, the onboard 12-bit ADC
continuously measures high side current, input voltage and an external voltage.
Data is reported through the I2C interface when polled by a host. The LTC4151
can also perform on-demand measurement in a snapshot mode. The LTC4151 features
a dedicated shutdown pin to reduce power consumption. The LTC4151-1/LTC4151-2
feature split I2C data pins to drive opto-isolators. The data out on the
LTC4151-1 is inverted while that on the LTC4151-2 is not.

I2C DATA FORMAT (MSB FIRST);

Data Out:

Byte #1                                    Byte #2                       Byte #3
START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK  X  X  X  X  C3 C2 C1 C0 SACK  D7 D6 D5 D4 D3 D2 D1 D0 SACK  STOP

Data In:

Byte #1                                    Byte #2                                    Byte #3                                                                                                                                 MSB                                       LSB
START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK  X  X  X  X  C3 C2 C1 C0 SACK REPEAT START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 R SACK

Byte #4                                   Byte #5
D15 D14  D13  D12  D11  D10  D0 D8 MACK   D7 D6 D5 D4 D3  D2  D1  D0  MNACK  STOP

START        : I2C Start
REPEAT START : I2C Repeat Start
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


@endverbatim

http://www.linear.com/product/LTC4151

http://www.linear.com/product/LTC4151#demoboards


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
    @ingroup LTC4151
    Library Header File for LTC4151: High Voltage I2C Current and Voltage Monitor
*/

#ifndef LTC4151_H
#define LTC4151_H

#include <Wire.h>

//! Address Choices:
//! To choose an address, comment out all options except the
//! configuration on the demo board.

/*!
| LTC4151 I2C Address | Value      |   AD1    |   AD2    |
| :------------------ | :------:   | :------: | :------: |
| LTC4151_I2C_ADDRESS | 0x67       |   High   |   Low    |
| LTC4151_I2C_ADDRESS | 0x68       |  Float   |  High    |
| LTC4151_I2C_ADDRESS | 0x69       |  High    |  High    |
| LTC4151_I2C_ADDRESS | 0x6A       |  Float   |  Float   |
| LTC4151_I2C_ADDRESS | 0x6B       |  Float   |  Low     |
| LTC4151_I2C_ADDRESS | 0x6C       |  Low     |  High    |
| LTC4151_I2C_ADDRESS | 0x6D       |  High    |  Float   |
| LTC4151_I2C_ADDRESS | 0x6E       |  Low     |  Float   |
| LTC4151_I2C_ADDRESS | 0x6F       |  Low     |  Low     |
| LTC4151_I2C_GLOBAL_ADDRESS | 0x66 |  X      |  X       |
*/

/*! @name I2C_Addresses
@{ */

// Address assignment
// LTC4151 I2C Address                 //  AD1       AD0
// #define LTC4151_I2C_ADDRESS 0x67    //  High      Low
// #define LTC4151_I2C_ADDRESS 0x68    //  Float     High
// #define LTC4151_I2C_ADDRESS 0x69    //  High      High
// #define LTC4151_I2C_ADDRESS 0x6A    //  Float     Float
// #define LTC4151_I2C_ADDRESS 0x6B    //  Float     Low
// #define LTC4151_I2C_ADDRESS 0x6C    //  Low       High
// #define LTC4151_I2C_ADDRESS 0x6D    //  High      Float
// #define LTC4151_I2C_ADDRESS 0x6E    //  Low       Float
#define LTC4151_I2C_ADDRESS 0x6F      //  Low       Low

// LTC4151 Global I2C Address.
// #define LTC4151_I2C_GLOBAL_ADDRESS 0x66  //  Global Address

//! @} */

//! Note: Register 0x07 is Reserved.
/*!
| Registers                 |   Location    | Descrition                                |
| :------------------------ | :-----------: | :---------------------------------------- |
| LTC4151_SENSE_MSB_REG     | 0x00          | ADC Current Sense Voltage Data (8 MSB)    |
| LTC4151_SENSE_LSB_REG     | 0x01          | ADC Current Sense Voltage Data (4 LSB)    |
| LTC4151_VIN_MSB_REG       | 0x02          | ADC V_IN Voltage Data (8 MSB)             |
| LTC4151_VIN_LSB_REG       | 0x03          | ADC V_IN Voltage Data (4 LSB)             |
| LTC4151_ADIN_MSB_REG      | 0x04          | ADC ADIN Voltage Data (8 MSB)             |
| LTC4151_ADIN_LSB_REG      | 0x05          | ADC ADIN Voltage Data (4 LSB)             |
| LTC4151_CONTROL_REG       | 0x06          | Controls ADC Operation Mode and Test Mode |

*/

/*! @name REGISTERS
@{ */
// Registers
#define LTC4151_SENSE_MSB_REG       0x00  // ADC Current Sense Voltage Data (8 MSB)
#define LTC4151_SENSE_LSB_REG       0x01  // ADC Current Sense Voltage Data (4 LSB)
#define LTC4151_VIN_MSB_REG         0x02  // ADC V_IN Voltage Data (8 MSB) 
#define LTC4151_VIN_LSB_REG         0x03  // ADC V_IN Voltage Data (4 LSB)
#define LTC4151_ADIN_MSB_REG        0x04  // ADC ADIN Voltage Data (8 MSB)
#define LTC4151_ADIN_LSB_REG        0x05  // ADC ADIN Voltage Data (4 LSB)
#define LTC4151_CONTROL_REG         0x06  // Controls ADC Operation Mode and Test Mode
//!@} */

//! Bitwise AND  with channel register, and write to LTC4151_CONTROL_REG.
/*!
| Commands and Channel Registers    |   Value   |   Descrition                          |
| :----------------------------     |   :---:   |   :---------                          |
| LTC4151_CONTINUOUS_MODE           |   0x00    |   Sets LTC4151 in Continuous Mode     |
| LTC4151_SNAPSHOT_MODE             |   0x80    |   Sets LTC4151 in Snapshot Mode       |
| LTC4151_SENSE_CHANNEL_REG         |   0x00    |   ADC Sense Channel For Snapshot Mode |
| LTC4151_VIN_CHANNEL_REG           |   0x20    |   DC V_IN Channel For Snapshot Mode   |
| LTC4151_ADIN_CHANNEL_REG          |   0x40    |   ADC ADIN Channel For Snapshot Mode  |
*/

/*! @name LTC4151 Commands
@{ */

#define LTC4151_SENSE_CHANNEL_REG   0x00  // ADC Sense Channel For Snapshot Mode
#define LTC4151_VIN_CHANNEL_REG     0x20  // ADC V_IN Channel For Snapshot Mode
#define LTC4151_ADIN_CHANNEL_REG    0x40  // ADC ADIN Channel For Snapshot Mode

#define LTC4151_CONTINUOUS_MODE 0x00
#define LTC4151_SNAPSHOT_MODE   0x80
//! @} */

//! Write one byte to an LTC4151 register.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC4151_write(uint8_t i2c_address,   //!< I2C address of the LTC4151. Configured by tying the ADR0 and ADR1 pins high or low. See Table 1 of datasheet.
                     uint8_t adc_command,   //!< The "command byte" for the LTC4151.
                     uint8_t code           //!< Value that will be written to the register.
                    );

//! Reads a 12-bit value from LTC4151
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC4151_read_12_bits(uint8_t i2c_address,    //!< I2C address of the LTC4151. Configured by tying the ADR0 and ADR1 pins high or low. See Table 1 of datasheet.
                            uint8_t adc_command,    //!< The "command byte" for the LTC4151.
                            uint16_t *adc_code      //!< Returns 12-bit value read from the adc
                           );

//! Calculates the LTC4151 sense current in Amps given "resistor" value in ohms and "LTC4151_sense_lsb" LSB weight in volts.
//! @return Returns the current in Amperes.
float LTC4151_code_to_sense_current(uint16_t adc_code,      //!< Code read from the adc (from a function such as LTC4151_read_12_bits)
                                    float resistor,         //!< The sense resistor value
                                    float LTC4151_sense_lsb //!< Current LSB weight.
                                   );

//! Calculates the LTC4151 V_IN voltage given "LTC_vin_lsb" LSB weight in volts
//! @return the V_IN voltage in volts.
float LTC4151_code_to_vin_voltage(uint16_t adc_code,        //!< Code read from the adc (from a function such as LTC4151_read_12_bits)
                                  float LTC4151_vin_lsb     //!< Voltage LSB weight.
                                 );

//! Calculates the LTC4151 ADIN voltage in mV given "LTC4151_adin_lsb" LSB weight in mV
//! @return the ADIN voltage in volts.
float LTC4151_code_to_ADIN_voltage(uint16_t adc_code,       //!< Code read from the adc (from a function such as LTC4151_read_12_bits)
                                   float LTC4151_adin_lsb   //!< ADIN voltage LSB weight.
                                  );

#endif  // LTC4151_H
