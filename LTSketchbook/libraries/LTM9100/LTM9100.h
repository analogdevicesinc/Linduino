/*!
LTM9100: Anyside™ High Voltage Isolated Switch Controller with I²C Command and Telemetry

@verbatim

The LTM9100 μModule controller is a complete, galvanically isolated switch controller
with I2C interface, for use as a load switch or hot swap controller. The load is soft
started and controlled by an external N-channel MOSFET switch.

@endverbatim

http://www.linear.com/product/LTM9100

http://www.linear.com/product/LTC9100#demoboards

REVISION HISTORY
$Revision: 6835 $
$Date: 2017-03-30 11:37:36 -0700 (Thu, 30 Mar 2017) $

Copyright (c) 2013, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/

/*! @file
    @ingroup LTM9100
    Header for LTM9100 Anyside™ High Voltage Isolated Switch Controller with I²C Command and Telemetry.
*/

#ifndef LTM9100_H
#define LTM9100_H

#define LTM_9100_STATUS_A_REG 0x00
#define LTM_9100_FAULT_B_REG  0x01
#define LTM_9100_ALERT_C_REG  0x02
#define LTM_9100_CTRL_D_REG   0x03
#define LTM_9100_SENSE_E_REG  0x04
#define LTM_9100_SENSE_F_REG  0x05
#define LTM_9100_ADIN2_G_REG  0x06
#define LTM_9100_ADIN2_H_REG  0x07
#define LTM_9100_ADIN_I_REG   0x08
#define LTM_9100_ADIN_J_REG   0x09

#define LTM_9100_STATUS_GATE  0x07
#define LTM_9100_STATUS_PGI   0x06
#define LTM_9100_STATUS_FET   0x05
#define LTM_9100_STATUS_OC    0x02
#define LTM_9100_STATUS_UV    0x01
#define LTM_9100_STATUS_OV    0x00

#define LTM_9100_FAULT_PGI    0x06
#define LTM_9100_FAULT_FET    0x05
#define LTM_9100_FAULT_OC     0x02
#define LTM_9100_FAULT_UV     0x01
#define LTM_9100_FAULT_OV     0x00

#define LTM_9100_ALERT_PGO    LTM_9100_FAULT_PGI
#define LTM_9100_ALERT_FET    LTM_9100_FAULT_FET
#define LTM_9100_ALERT_OC     LTM_9100_FAULT_OC
#define LTM_9100_ALERT_UV     LTM_9100_FAULT_UV
#define LTM_9100_ALERT_OV     LTM_9100_FAULT_OV

#define LTM_9100_CTRL_PGIO_CFG    0x06
#define LTM_9100_CTRL_PGIO_CFG_MASK 0xC0
#define LTM_9100_CTRL_ADC_WRITE   0x05
#define LTM_9100_CTRL_GATE_CTRL   0x03
#define LTM_9100_CTRL_OC      0x02
#define LTM_9100_CTRL_UV        0x01
#define LTM_9100_CTRL_OV        0x00

#define LTM_9100_SENSE_mV_PER_TICK  0.0625
#define LTM_9100_ADIN_V_PER_TICK  0.0025
#define LTM_9100_ADIN2_V_PER_TICK 0.0025

extern uint8_t i2c_address;       //7-bit version of LTM9100 I2C device addr determined by ADR0 & ADR1 (divide datasheet address by 2). ADR0/1 low: 0x20 -> 0x10
extern uint8_t reg_read_list[10];
extern uint8_t reg_write_list[9];

extern float sense_resistor;
extern float adin_gain;
extern float adin2_gain;


//! Reads an 8-bit register from the LTM9100 using the standard repeated start format.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTM9100_register_read(uint8_t i2c_address, uint8_t register_address, uint8_t *register_data);

//! Read the specified ADC value (SENSE, ADIN, ADIN2) and output in human readable format to the serial console.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTM9100_adc_read(uint8_t i2c_address, uint8_t base_address, float *register_data);

//! Writes to an 8-bit register inside the LTM9100 using the standard I2C repeated start format.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTM9100_register_write(uint8_t i2c_address, uint8_t register_address, uint8_t register_data);

//! Sets any bit inside the LTM9100 using the standard I2C repeated start format.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTM9100_bit_set(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number);

//! Clears any bit inside the LTM9100 using the standard I2C repeated start format.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTM9100_bit_clear(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number);

//! Read the bit specified by bit_number from the LTM9100.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTM9100_bit_read(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number, uint8_t *register_data);

//! Attempts to read a byte from the I2C bus using the alert address (0xC) to ascertain pending alerts on the bus.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTM9100_alert_read(uint8_t *register_data);

//! Read all LTM9100 registers and output to the serial console.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTM9100_print_all_registers(uint8_t i2c_address);

//! Read all LTM9100 registers and output to the serial console every second until a key press is detected.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTM9100_continuous_read_all_registers(uint8_t i2c_address);

//! Check if user_register is a valid register for the LTM9100.
//! @return Returns the validity of user_register against the the supplied register set.
boolean valid_register(uint8_t user_register, uint8_t register_array[], uint8_t array_length);

#endif
