/*!
LTC4260: Hot Swap Controller with I2C Compatible Monitoring

@verbatim

The LTC4260 Hot Swap controller allows a board to be safely inserted and removed
from a live backplane. Using an external N-channel pass transistor, board supply
voltage and inrush current are ramped up at an adjustable rate. An I2C interface
and onboard ADC allow for monitoring of load current, voltage and fault status.

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

  float adin_lsb = (2.56/255);                     // ADIN Voltage Data is 8-Bit Data with 4.82mV LSB and 1.23V Full Scale
  float adin_resisive_ratio = ((392 + 10)/10);     // Resistor divider circuit at ADIN pin.
  ack |= LTC4260_read(LTC4260_I2C_ADDRESS, LTC4260_ADIN_REG, &adin_code);           // Read ADIN Register
  adin_voltage = LTC4260_code_to_voltage(adin_code, adin_lsb, adin_resisive_ratio); // Calculate ADIN Voltage from ADIN Register data

@endverbatim

http://www.linear.com/product/LTC4260

http://www.linear.com/product/LTC4260#demoboards


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
    @ingroup LTC4260
    Header for LTC4260: Hot Swap Controller with I2C Compatible Monitoring
*/

#ifndef LTC4260_H
#define LTC4260_H

//! @name LTC4261 Address Assignment
//! @{
#define LTC4260_I2C_ADDRESS                      0x44 //ADDR2 = L, ADDR1 = L, ADDR0 = L
#define LTC4260_I2C_ALERT_RESPONSE               0x0C
#define LTC4260_I2C_MASS_WRITE                   0x5F
//!@}

//! @name LTC4260 Register addresses
//! @{
// Register Addresses
#define LTC4260_CONTROL_REG                      0x00
#define LTC4260_ALERT_REG                        0x01
#define LTC4260_STATUS_REG                       0x02
#define LTC4260_FAULT_REG                        0x03
#define LTC4260_SENSE_REG                        0x04
#define LTC4260_SOURCE_REG                       0x05
#define LTC4260_ADIN_REG                         0x06
//!@}

//! @name LTC4260 Fault Register Bits
//! @{
// Fault Register Bit Positions
#define LTC4260_FET_SHORT_FAULT                  0x20
#define LTC4260_EN_STATE_CHANGE                  0x10
#define LTC4260_POWER_BAD_FAULT                  0x08
#define LTC4260_OVERCURRENT_FAULT                0x04
#define LTC4260_UNDERVOLTAGE_FAULT               0x02
#define LTC4260_OVERVOLTAGE_FAULT                0x01
//!@}

//! @name LTC4260 Alert Register Bits
//! @{
// Alert Register Bits
#define LTC4260_GPIO_OUTPUT_ENABLE               0x40
#define LTC4260_GPIO_OUTPUT_DISABLE              0xBF
#define LTC4260_FET_SHORT_ENABLE                 0x20
#define LTC4260_FET_SHORT_DISABLE                0xDF
#define LTC4260_EN_STATE_ENABLE                  0x10
#define LTC4260_EN_STATE_DISABLE                 0xEF
#define LTC4260_POWER_BAD_ENABLE                 0x08
#define LTC4260_POWER_BAD_DISABLE                0xF7
#define LTC4260_OVERCURRENT_ENABLE               0x04
#define LTC4260_OVERCURRENT_DISABLE              0xFB
#define LTC4260_UNDERVOLTAGE_ENABLE              0x02
#define LTC4260_UNDERVOLTAGE_DISABLE             0xFD
#define LTC4260_OVERVOLTAGE_ENABLE               0x01
#define LTC4260_OVERVOLTAGE_DISABLE              0xFE
//!@}

//! @name LTC4260 Control Register Bits
//! @{
// Control Register Bit Positions
#define LTC4260_PGIO_POWER_GOODX                 0x00
#define LTC4260_PGIO_POWER_GOOD                  0x80
#define LTC4260_PGIO_GENERAL_PURPOSE_OUTPUT      0x40
#define LTC4260_PGIO_GENERAL_PURPOSE_INPUT       0xC0

#define LTC4260_TEST_MODE_ENABLE                 0x20
#define LTC4260_TEST_MODE_DISABLE                0xDF

#define LTC4260_MASS_WRITE_ENABLE                0x10
#define LTC4260_MASS_WRITE_DISABLE               0xEF

#define LTC4260_FET_ON                           0x08
#define LTC4260_FET_OFF                          0xF7

#define LTC4260_OVERCURRENT_AUTO_RETRY_ENABLE    0x04
#define LTC4260_OVERCURRENT_AUTO_RETRY_DISABLE   0xFB

#define LTC4260_UNDERVOLTAGE_AUTO_RETRY_ENABLE   0x02
#define LTC4260_UNDERVOLTAGE_AUTO_RETRY_DISABLE  0xFD

#define LTC4260_OVERVOLTAGE_AUTO_RETRY_ENABLE    0x01
#define LTC4260_OVERVOLTAGE_AUTO_RETRY_DISABLE   0xFE
//!@}

//! Calculates voltage from register code data
//! @return The function returns valtage calculated from register data
float LTC4260_code_to_voltage(uint8_t register_code,    //! Data from registers
                              float LSB,          //! Smallest incremental voltage
                              float resistor_divider_ratio  //! the ratio that the voltages have been divided down. Set to 1 for no resistir divider circuit
                             );

//! SMBus Alert ResponseProtocol: Sends an alert response command and releases /ALERT pin. LTC4260 responds with its address
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge
int8_t LTC4260_ARA(uint8_t alert_response_address,      //!< the Alert Response Address on the I2C bus
                   uint8_t *i2c_address           //!< the address of the alert source
                  );

//! Write an 8-bit code to the LTC4260
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC4260_write(uint8_t i2c_address,  //! LTC4260 I2C ADDRESS
                     uint8_t command,      //! Register address to be written into
                     uint8_t code      //! Data to be written into the register
                    );


//! Reads an 8-bit adc_code from LTC4260
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC4260_read(uint8_t i2c_address, //! LTC4260 I2C ADDRESS
                    uint8_t command,   //! Register address to read from
                    uint8_t *code    //! Contents of the requested register
                   );

#endif