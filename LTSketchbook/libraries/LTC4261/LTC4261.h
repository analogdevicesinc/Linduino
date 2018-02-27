/*!
LTC4261: Negative Voltage Hot Swap Controller with ADC and I2C Monitoring

@verbatim

The LTC4261/LTC4261-2 negative voltage Hot SwapTM controller allows a board
to be safely inserted and removed from a live backplane. Using an external
N-channel pass transistor, the board supply voltage can be ramped at an
adjustable rate. The devices feature independently adjustable inrush current
and overcurrent limits to minimize stresses on the pass transistor during
start-up, input step and output short conditions. The LTC4261 defaults
to latch-off while the LTC4261-2 defaults to auto-retry on overcurrent faults.

I2C DATA FORMAT (MSB FIRST):

Data Out:
Byte #1                                    Byte #2                       Byte #3

START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK  C7  C6 C5 C4 C3 C2 C1 C0 SACK D7 D6 D5 D4 D3 D2 D1 D0 SACK  STOP

Data In:
Byte #1                                    Byte #2                                      Byte #3

START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK  C7  C6  C5 C4 C3 C2 C1 C0 SACK  Repeat Start SA6 SA5 SA4 SA3 SA2 SA1 SA0 R SACK

Byte #4                                   Byte #5
MSB                                       LSB
D15 D14  D13  D12  D11  D10  D9 D8 MACK   D7 D6 D5 D4 D3  D2  D1  D0  MNACK  STOP

START       : I2C Start
Repeat Start: I2C Repeat Start
STOP        : I2C Stop
SAx         : I2C Address
SACK        : I2C Slave Generated Acknowledge (Active Low)
MACK        : I2C Master Generated Acknowledge (Active Low)
MNACK       : I2C Master Generated Not Acknowledge
W           : I2C Write (0)
R           : I2C Read  (1)
Cx          : Command Code
Dx          : Data Bits
X           : Don't care

Example Code:

Read ADIN and Current Values

    ack |= LTC4261_read_10_bits(LTC4261_I2C_Address, LTC4261_SENSE_MSB_REG, &current_code);   // reads current sense ADC
    ack |= LTC4261_read_10_bits(LTC4261_I2C_Address, LTC4261_ADIN_MSB_REG, &adin_code);       // reads ADIN ADC
    ack |= LTC4261_read_10_bits(LTC4261_I2C_Address, LTC4261_ADIN2_MSB_REG, &adin2_code);     // reads ADIN2 ADC

    //convert codes to voltage and current values
    current = LTC4261_code_to_current(current_code, resitor, current_lsb);
    adin_voltage = LTC4261_ADIN_code_to_voltage(adin_code, adin_lsb, resisive_ratio);
    adin2_voltage = LTC4261_ADIN_code_to_voltage(adin2_code, adin_lsb, resisive_ratio);

@endverbatim

http://www.linear.com/product/LTC4261

http://www.linear.com/product/LTC4261#demoboards


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
    @ingroup LTC4261
    Header for LTC4261: Negative Voltage Hot Swap Controllers with ADC and I2C Monitoring
*/

#ifndef LTC4261_H
#define LTC4261_H

#include <Wire.h>

//! @name LTC4261 Address Assignment
//! @{
#define LTC4261_I2C_Address                     0x10
// #define LTC4261_I2C_Address                     0x11
// #define LTC4261_I2C_Address                     0x12
// #define LTC4261_I2C_Address                     0x13
// #define LTC4261_I2C_Address                     0x14
// #define LTC4261_I2C_Address                     0x15
// #define LTC4261_I2C_Address                     0x16
// #define LTC4261_I2C_Address                     0x17
#define LTC4261_I2C_ALERT_RESPONSE               0x0C
#define LTC4261_I2C_MASS_WRITE                   0x3E
//!@}

/*! @name LTC4261 Register Addresses
@{ */
// Register Addresses
#define LTC4261_STATUS_REG                       0x00
#define LTC4261_FAULT_REG                        0x01
#define LTC4261_ALERT_REG                        0x02
#define LTC4261_CONTROL_REG                      0x03
#define LTC4261_SENSE_MSB_REG                    0x04
#define LTC4261_SENSE_LSB_REG                    0x05
#define LTC4261_ADIN2_MSB_REG                    0x06
#define LTC4261_ADIN2_LSB_REG                    0x07
#define LTC4261_ADIN_MSB_REG                     0x08
#define LTC4261_ADIN_LSB_REG                     0x09
//!@}

/*! @name Command Codes
@{ */
#define LTC4261_PGIO_POWER_GOODX                 0x00
#define LTC4261_PGIO_POWER_GOOD                  0x80
#define LTC4261_PGIO_GENERAL_PURPOSE_OUTPUT      0x40
#define LTC4261_PGIO_GENERAL_PURPOSE_INPUT       0xC0

#define LTC4261_TEST_MODE_ENABLE                 0x20
#define LTC4261_TEST_MODE_DISABLE                0xDF

#define LTC4261_POWER_BAD_AUTO_RETRY_ENABLE      0x10
#define LTC4261_POWER_BAD_AUTO_RETRY_DISABLE     0xEF

#define LTC4261_FET_ON                           0x08
#define LTC4261_FET_OFF                          0xF7

#define LTC4261_OVERCURRENT_AUTO_RETRY_ENABLE    0x04
#define LTC4261_OVERCURRENT_AUTO_RETRY_DISABLE   0xFB

#define LTC4261_UNDERVOLTAGE_AUTO_RETRY_ENABLE   0x02
#define LTC4261_UNDERVOLTAGE_AUTO_RETRY_DISABLE  0xFD

#define LTC4261_OVERVOLTAGE_AUTO_RETRY_ENABLE    0x01
#define LTC4261_OVERVOLTAGE_AUTO_RETRY_DISABLE   0xFE
//!@}

/*! @name Fault Codes
@{ */
#define LTC4261_EXTERNAL_FAULT                  0x80
#define LTC4261_PGIO_INPUT_HIGH                 0x40
#define LTC4261_FET_SHORT_FAULT                 0x20
#define LTC4261_EN_STATE_CHANGE                 0x10
#define LTC4261_POWER_BAD_FAULT                 0x08
#define LTC4261_OVERCURRENT_FAULT               0x04
#define LTC4261_UNDERVOLTAGE_FAULT              0x02
#define LTC4261_OVERVOLTAGE_FAULT               0x01
//!@}

/*! @name Alert Codes
@{ */
#define LTC4261_EXTERNAL_FAULT_ENABLE           0x80
#define LTC4261_EXTERNAL_FAULT_DISABLE          0x7F
#define LTC4261_PGIO_OUTPUT_ENABLE              0x40
#define LTC4261_PGIO_OUTPUT_DISABLE             0xBF
#define LTC4261_FET_SHORT_ENABLE                0x20
#define LTC4261_FET_SHORT_DISABLE               0xDF
#define LTC4261_EN_STATE_ENABLE                 0x10
#define LTC4261_EN_STATE_DISABLE                0xEF
#define LTC4261_POWER_BAD_ENABLE                0x08
#define LTC4261_POWER_BAD_DISABLE               0xF7
#define LTC4261_OVERCURRENT_ENABLE              0x04
#define LTC4261_OVERCURRENT_DISABLE             0xFB
#define LTC4261_UNDERVOLTAGE_ENABLE             0x02
#define LTC4261_UNDERVOLTAGE_DISABLE            0xFD
#define LTC4261_OVERVOLTAGE_ENABLE              0x01
#define LTC4261_OVERVOLTAGE_DISABLE             0xFE
//!@}

//! Write an 8-bit code to the LTC4261.
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC4261_write(uint8_t i2c_address,   //!< I2C Address for the LTC4261
                     uint8_t command,     //!< The "command byte" for the LTC4261 that corresponds to register address
                     uint8_t code           //!< Value that will be written to the register
                    );

//! Reads an 8-bit adc_code from LTC4261
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC4261_read(uint8_t i2c_address,  //!< I2C Address for the LTC4261
                    uint8_t command,    //!< The "command byte" for the LTC4261 that corresponds to register address
                    uint8_t *code       //!< Value that will be read from the register
                   );

//! Reads a 10-bit adc_code from LTC4261
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC4261_read_10_bits(uint8_t i2c_address, //!< I2C Address for the LTC4261
                            uint8_t adc_command, //!< The "command byte" for the LTC4261 that corresponds to register address
                            uint16_t *adc_code   //!< Value that will be read from the register.
                           );

//! SMBus Alert Response Protocol: Sends an alert response command and releases /ALERT pin. LTC4261 responds with its address
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC4261_ARA(uint8_t alert_response_address,  //!< the Alert Response Address on the I2C bus
                   uint8_t *i2c_address             //!< the address of the alert source
                  );

//! Calculate the LTC4261 ADIN voltage
//! @return Returns the ADIN Voltage in Volts
float LTC4261_ADIN_code_to_voltage(uint16_t adc_code,         //!< The ADC value
                                   float LTC4261_ADIN_lsb,    //!< ADIN lsb weight
                                   float resistor_divider_ratio //!< the ratio that the voltages have been divided down
                                  );
//! Calculate the LTC4261 current with a sense resistor
//! @return The LTC4261 sense current
float LTC4261_code_to_current(uint16_t adc_code,                //!< The ADC value
                              float resistor,                   //!< The resistor value
                              float LTC4261_DELTA_SENSE_lsb     //!< Delta sense lsb weight
                             );
#endif // LTC4261_H