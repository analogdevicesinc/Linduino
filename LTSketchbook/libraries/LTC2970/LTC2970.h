/*!
LTC2970: Dual I2C Power Supply Monitor and Margining Controller

@verbatim

The LTC2970 is a dual-channel power supply monitor and margining controller.
It has monitors two voltage and two current channels using an on-board, highly-accurate 14-bit delta-sigma ADC.
It can monitor for overvoltage and over-current faults on both channels, and assert ALERT when a fault is detected.
Two independent DAC outputs can control each power regulator for accurate voltage margining.
It has an I2C/SMBus interface to read and write the on-board configuration and status registers.

@endverbatim

http://www.linear.com/product/LTC2970

http://www.linear.com/demo/#demoboards


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

//! @ingroup Power_System_Management
//! @{
//! @defgroup LTC2970 LTC2970: Dual I2C Power Supply Monitor and Margining Controller
//! @}

/*! @file
    @ingroup LTC2970
    Header for LTC2970: Dual I2C Power Supply Monitor and Margining Controller

*/

#include <Arduino.h>
#include "LT_SMBusNoPec.h"

#ifndef LTC2970_h
#define LTC2970_h

/*DEVICE I2C ADDRESSES (7-bit)*/
// GLOBAL ADDRESS
#define LTC2970_I2C_GLOBAL_ADDRESS  0x5B
#define LTC2970_I2C_ARA_ADDRESS     0x0C

/********************************************************************************/
//! @name LTC2970 command address definitions
//!  NOTE: commands prefixed with LTC2970_1_ are for the LTC2970-1 only

#define LTC2970_FAULT               0x00
//  0x01
//  ...
//  0x07
#define LTC2970_FAULT_EN            0x08
//  0x09
#define LTC2970_FAULT_LA_INDEX      0x10
#define LTC2970_FAULT_LA            0x11
//  0x12
//  ...
//  0x16
#define LTC2970_IO                  0x17
#define LTC2970_ADC_MON             0x18
//  0x19
//  ...
//  0x1E
#define LTC2970_1_SYNC              0x1F
//  0x20
//  ...
//  0x27
#define LTC2970_VDD_ADC             0x28
#define LTC2970_VDD_OV              0x29
#define LTC2970_VDD_UV              0x2A
//  0x2B
//  ...
//  0x37
#define LTC2970_V12_ADC             0x38
#define LTC2970_V12_OV              0x39
#define LTC2970_V12_UV              0x3A
//  0x3B
//  ...
//  0x3F
#define LTC2970_CH0_A_ADC           0x40
#define LTC2970_CH0_A_OV            0x41
#define LTC2970_CH0_A_UV            0x42
#define LTC2970_CH0_A_SERVO         0x43
#define LTC2970_CH0_A_IDAC          0x44
#define LTC2970_1_CH0_A_IDAC_TRACK  0x45
#define LTC2970_1_CH0_A_DELAY_TRACK 0x46
//  0x47
#define LTC2970_CH0_B_ADC           0x48
#define LTC2970_CH0_B_OV            0x49
#define LTC2970_CH0_B_UV            0x4A
//  0x4B
//  ...
//  0x4F
#define LTC2970_CH1_A_ADC           0x50
#define LTC2970_CH1_A_OV            0x51
#define LTC2970_CH1_A_UV            0x52
#define LTC2970_CH1_A_SERVO         0x53
#define LTC2970_CH1_A_IDAC          0x54
#define LTC2970_1_CH1_A_IDAC_TRACK  0x55
#define LTC2970_1_CH1_A_DELAY_TRACK 0x56
//  0x55
//  ...
//  0x57
#define LTC2970_CH1_B_ADC           0x58
#define LTC2970_CH1_B_OV            0x59
#define LTC2970_CH1_B_UV            0x5A
//  0x5B
//  ...
//  0x67
#define LTC2970_TEMP_ADC            0x68

//! @}



///////////////////////////////////// FUNCTIONS /////////////////////////////////

//! Set GPIO_n high
//! @return void
void ltc2970_gpio_up(LT_SMBus *smbus, uint8_t ltc2970_i2c_address, int gpio_number);


//! Set GPIO_n low
//! @return void
void ltc2970_gpio_down(LT_SMBus *smbus, uint8_t ltc2970_i2c_address, int gpio_number);

//! Disconnect a DAC from its channel
//! @return void
void ltc2970_dac_disconnect(LT_SMBus *smbus, uint8_t ltc2970_i2c_address, int dac_number);

//! Unceremoniously connect DAC0 to the control node
//!  no attempt to equalize voltages
//! @return void
void ltc2970_hard_connect_dac(LT_SMBus *smbus, uint8_t ltc2970_i2c_address, int dac_number, uint8_t dac_value_8);

//! soft-connect DACn to its controlled node
//! @return int
int ltc2970_soft_connect_dac(LT_SMBus *smbus, uint8_t ltc2970_i2c_address, int dac_number);

//! Servo once to a given ADC value
//! @return void
void ltc2970_servo_to_adc_val(LT_SMBus *smbus, uint8_t ltc2970_i2c_address, int channel_number, uint16_t code);

//! Prints die temperature on the LTC2970
//! @return void
void ltc2970_print_die_temp (LT_SMBus *smbus, uint8_t ltc2970_i2c_address);

//! Read FAULT, FAULT_LA, and FAULT_LA_INDEX registers
//!  print the results
//! @return void
void ltc2970_read_faults(LT_SMBus *smbus, uint8_t ltc2970_i2c_address);



#endif
