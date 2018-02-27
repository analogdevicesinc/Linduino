/*!
LTC2942: Battery Gas Gauge with Temperature, Voltage Measurement.
LTC2941: Battery Gas Gauge with I2C Interface.

@verbatim

The LTC®2942 measures battery charge state, battery
voltage and chip temperature in handheld PC and portable
product applications. Its operating range is perfectly suited
for single-cell Li-Ion batteries. A precision coulomb counter
integrates current through a sense resistor between the
battery’s positive terminal and the load or charger. Battery
voltage and on-chip temperature are measured with an
internal 14-bit No Latency ∆∑™ ADC. The three measured
quantities (charge, voltage and temperature) are stored in
internal registers accessible via the onboard SMBus/I2C
interface.

I2C DATA FORMAT (MSB FIRST):

Data Out:
Byte #1                                    Byte #2                     Byte #3

START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK  C7  C6 C5 C4 C3 C2 C1 C0 SACK D7 D6 D5 D4 D3 D2 D1 D0 SACK  STOP

Data In:
Byte #1                                    Byte #2                                    Byte #3

START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK  C7  C6  C5 C4 C3 C2 C1 C0 SACK  Repeat Start SA6 SA5 SA4 SA3 SA2 SA1 SA0 R SACK

Byte #4                                   Byte #5
MSB                                       LSB
D15 D14  D13  D12  D11  D10  D9 D8 MACK   D7 D6 D5 D4 D3  D2  D1  D0  MNACK  STOP

START       : I2C Start
Repeat Start: I2c Repeat Start
STOP        : I2C Stop
SAx         : I2C Address
SACK        : I2C Slave Generated Acknowledge (Active Low)
MACK        : I2C Master Generated Acknowledge (Active Low)
MNACK       : I2c Master Generated Not Acknowledge
W           : I2C Write (0)
R           : I2C Read  (1)
Cx          : Command Code
Dx          : Data Bits
X           : Don't care



Example Code:

Read charge, current, and voltage.

    adc_command = LTC2942_SENSE_MONITOR | LTC2942_AUTOMATIC_MODE; // Builds commands to set LTC2942 to automatic mode
    ack |= LTC2942_write(LTC2942_I2C_ADDRESS, LTC2942_CONTROL_REG, adc_command);   // Sets the LTC2942 to automatic mode

    resistor = .1; // Resistor Value On Demo Board

    ack |= LTC2942_read_16_bits(LTC2942_I2C_ADDRESS, LTC2942_CHARGE_MSB_REG, &charge_code);  // Reads the ADC registers that contains charge value
    charge = LTC2942_code_to_coulombs(charge_code, resistor, prescalarValue); // Calculates charge from charge code, resistor and prescalar

    ack |= LTC2942_read_16_bits(LTC2942_I2C_ADDRESS, LTC2942_CURRENT_MSB_REG, &current_code); // Reads the voltage code across sense resistor
    current = LTC2942_code_to_current(current_code, resistor); // Calculates current from current code, resistor value.

    ack |= LTC2942_read_16_bits(LTC2942_I2C_ADDRESS, LTC2942_VOLTAGE_MSB_REG, &voltage_code);   // Reads voltage voltage code
    VIN = LTC2942_VIN_code_to_voltage(voltage_code);  // Calculates VIN voltage from VIN code and lsb


@endverbatim

http://www.linear.com/product/LTC2942
http://www.linear.com/product/LTC2941

http://www.linear.com/product/LTC2942#demoboards
http://www.linear.com/product/LTC2941#demoboards


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
    @ingroup LTC2942
    Header for LTC2942: Battery Gas Gauge with Temperature, Voltage Measurement
*/


#ifndef LTC2942_H
#define LTC2942_H

#include <Wire.h>


/*!
| LTC2942 I2C Address Assignment    | Value |
| :-------------------------------- | :---: |
| LTC2942_I2C_ADDRESS               | 0xC8  |
*/

/*! @name LTC2942 I2C Address
@{ */

#define LTC2942_I2C_ADDRESS 0x64
#define LTC2942_I2C_ALERT_RESPONSE  0x0C
//! @}


/*!
| Name                                              | Value |
| :------------------------------------------------ | :---: |
| LTC2942_STATUS_REG                                | 0x00  |
| LTC2942_CONTROL_REG                               | 0x01  |
| LTC2942_ACCUM_CHARGE_MSB_REG                      | 0x02  |
| LTC2942_ACCUM_CHARGE_LSB_REG                      | 0x03  |
| LTC2942_CHARGE_THRESH_HIGH_MSB_REG                | 0x04  |
| LTC2942_CHARGE_THRESH_HIGH_LSB_REG                | 0x05  |
| LTC2942_CHARGE_THRESH_LOW_MSB_REG                 | 0x06  |
| LTC2942_CHARGE_THRESH_LOW_LSB_REG                 | 0x07  |
| LTC2942_VOLTAGE_MSB_REG                           | 0x08  |
| LTC2942_VOLTAGE_LSB_REG                           | 0x09  |
| LTC2942_VOLTAGE_THRESH_HIGH_REG                   | 0x0A  |
| LTC2942_VOLTAGE_THRESH_LOW_REG                    | 0x0B  |
| LTC2942_TEMPERATURE_MSB_REG                       | 0x0C  |
| LTC2942_TEMPERATURE_LSB_REG                       | 0x0D  |
| LTC2942_TEMPERATURE_THRESH_HIGH_REG               | 0x0E  |
| LTC2942_TEMPERATURE_THRESH_LOW_REG                | 0x0F  |

*/

/*! @name Registers
@{ */
// Registers
#define LTC2942_STATUS_REG                          0x00
#define LTC2942_CONTROL_REG                         0x01
#define LTC2942_ACCUM_CHARGE_MSB_REG                0x02
#define LTC2942_ACCUM_CHARGE_LSB_REG                0x03
#define LTC2942_CHARGE_THRESH_HIGH_MSB_REG          0x04
#define LTC2942_CHARGE_THRESH_HIGH_LSB_REG          0x05
#define LTC2942_CHARGE_THRESH_LOW_MSB_REG           0x06
#define LTC2942_CHARGE_THRESH_LOW_LSB_REG           0x07
#define LTC2942_VOLTAGE_MSB_REG                     0x08
#define LTC2942_VOLTAGE_LSB_REG                     0x09
#define LTC2942_VOLTAGE_THRESH_HIGH_REG             0x0A
#define LTC2942_VOLTAGE_THRESH_LOW_REG              0x0B
#define LTC2942_TEMPERATURE_MSB_REG                 0x0C
#define LTC2942_TEMPERATURE_LSB_REG                 0x0D
#define LTC2942_TEMPERATURE_THRESH_HIGH_REG         0x0E
#define LTC2942_TEMPERATURE_THRESH_LOW_REG          0x0F
//! @}

/*!
| Command Codes                                 | Value     |
| :-------------------------------------------- | :-------: |
| LTC2942_AUTOMATIC_MODE                        | 0xC0      |
| LTC2942_MANUAL_VOLTAGE                        | 0x80      |
| LTC2942_MANUAL_TEMPERATURE                    | 0x40      |
| LTC2942_SLEEP_MODE                            | 0x00      |
| LTC2942_PRESCALAR_M_1                         | 0x00      |
| LTC2942_PRESCALAR_M_2                         | 0x08      |
| LTC2942_PRESCALAR_M_4                         | 0x10      |
| LTC2942_PRESCALAR_M_8                         | 0x18      |
| LTC2942_PRESCALAR_M_16                        | 0x20      |
| LTC2942_PRESCALAR_M_32                        | 0x28      |
| LTC2942_PRESCALAR_M_64                        | 0x30      |
| LTC2942_PRESCALAR_M_128                       | 0x31      |
| LTC2942_ALERT_MODE                            | 0x04      |
| LTC2942_CHARGE_COMPLETE_MODE                  | 0x02      |
| LTC2942_DISABLE_ALCC_PIN                      | 0x00      |
| LTC2942_SHUTDOWN_MODE                         | 0x01      |
*/

/*! @name Command Codes
@{ */
// Command Codes
#define LTC2942_AUTOMATIC_MODE                  0xC0
#define LTC2942_MANUAL_VOLTAGE                  0x80
#define LTC2942_MANUAL_TEMPERATURE              0x40
#define LTC2942_SLEEP_MODE                      0x00

#define LTC2942_PRESCALAR_M_1                   0x00
#define LTC2942_PRESCALAR_M_2                   0x08
#define LTC2942_PRESCALAR_M_4                   0x10
#define LTC2942_PRESCALAR_M_8                   0x18
#define LTC2942_PRESCALAR_M_16                  0x20
#define LTC2942_PRESCALAR_M_32                  0x28
#define LTC2942_PRESCALAR_M_64                  0x30
#define LTC2942_PRESCALAR_M_128                 0x38

#define LTC2942_ALERT_MODE                      0x04
#define LTC2942_CHARGE_COMPLETE_MODE            0x02
#define LTC2942_DISABLE_ALCC_PIN                0x00

#define LTC2942_SHUTDOWN_MODE                   0x01

//! @}

/*!
| Conversion Constants                              |  Value   |
| :------------------------------------------------ | :------: |
| LTC2942_CHARGE_lsb                                | 0.085 mAh|
| LTC2942_VOLTAGE_lsb                               | 366.2  uV|
| LTC2942_TEMPERATURE_lsb                           | 0.586   C|
| LTC2942_FULLSCALE_VOLTAGE                         |  6      V|
| LTC2942_FULLSCALE_TEMPERATURE                     | 600     K|

*/
/*! @name Conversion Constants
@{ */
const float LTC2942_CHARGE_lsb = 0.085E-3;
const float LTC2942_VOLTAGE_lsb = .3662E-3;
const float LTC2942_TEMPERATURE_lsb = 0.25;
const float LTC2942_FULLSCALE_VOLTAGE = 6;
const float LTC2942_FULLSCALE_TEMPERATURE = 600;
//! @}

//! @}


//! Write an 8-bit code to the LTC2942.
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2942_write(uint8_t i2c_address, //!< Register address for the LTC2942
                     uint8_t adc_command, //!< The "command byte" for the LTC2942
                     uint8_t code         //!< Value that will be written to the register.
                    );

//! Write a 16-bit code to the LTC2942.
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2942_write_16_bits(uint8_t i2c_address, //!< Register address for the LTC2942
                             uint8_t adc_command, //!< The "command byte" for the LTC2942
                             uint16_t code        //!< Value that will be written to the register.
                            );


//! Reads an 8-bit adc_code from LTC2942
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2942_read(uint8_t i2c_address, //!< Register address for the LTC2942
                    uint8_t adc_command, //!< The "command byte" for the LTC2942
                    uint8_t *adc_code    //!< Value that will be read from the register.
                   );

//! Reads a 16-bit adc_code from LTC2942
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2942_read_16_bits(uint8_t i2c_address, //!< Register address for the LTC2942
                            uint8_t adc_command, //!< The "command byte" for the LTC2942
                            uint16_t *adc_code   //!< Value that will be read from the register.
                           );


//! Calculate the LTC2942 charge in Coulombs
//! @return Returns the Coulombs of charge in the ACR register.
float LTC2942_code_to_coulombs(uint16_t adc_code,        //!< The RAW ADC value
                               float resistor,         //!< The sense resistor value
                               uint16_t prescalar      //!< The prescalar value
                              );

//! Calculate the LTC2942 charge in mAh
//! @return Returns the Coulombs of charge in the ACR register.
float LTC2942_code_to_mAh(uint16_t adc_code,            //!< The RAW ADC value
                          float resistor,       //!< The sense resistor value
                          uint16_t prescalar    //!< The prescalar value
                         );

//! Calculate the LTC2942 SENSE+ voltage
//! @return Returns the SENSE+ Voltage in Volts
float LTC2942_code_to_voltage(uint16_t adc_code              //!< The RAW ADC value
                             );


//! Calculate the LTC2942 temperature
//! @return Returns the temperature in Kelvin
float LTC2942_code_to_kelvin_temperature(uint16_t adc_code           //!< The RAW ADC value
                                        );

//! Calculate the LTC2942 temperature
//! @return Returns the temperature in Celcius
float LTC2942_code_to_celcius_temperature(uint16_t adc_code          //!< The RAW ADC value
                                         );

#endif  // LTC2942_H
