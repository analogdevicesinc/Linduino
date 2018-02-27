/*!
LTC4282: High Current Hot Swap Controller with I2C Compatible Monitoring and EEPROM

@verbatim

The LTC4282 Hot Swap controller allows a board to be safely inserted and removed from a live backplane.
Using one or more external N-channel pass transistors, board supply voltage and inrush current is ramped up at an adjustable rate.
An I2C interface and onboard ADC allows for monitoring of board current, voltage, power, energy and fault status.

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

Read Voltage and Current

    static uint8_t CTRLREG_LSB = LTC4282_VIN_MODE_12_V;
    static uint8_t ILIM_ADJUST = (LTC4282_ILIM_ADJUST_25_V_0|LTC4282_FOLDBACK_MODE_12_V_0|LTC4282_ADC_VSOURCE|LTC4282_ADC_GPIO2_MODE)& ~LTC4282_ADC_16_BIT;

    resistor = .1; // Resistor Value On Demo Board

    ack |= LTC4282_read_16_bits(LTC4282_I2C_ADDRESS, LTC4282_VSOURCE_MSB_REG, &voltage_code);         //!< Read Voltage Code From VSOURCE Register
    vsource = LTC4282_code_to_voltage(voltage_code, voltage_fullscale);                               //!< Convert Voltage Code to Current Source Voltage

    ack |= LTC4282_read_16_bits(LTC4282_I2C_ADDRESS, LTC4282_CURRENT_MSB_REG, t_code); // Reads the voltage code across sense resistor
    current = LTC4282_code_to_current(current_code, resistor); // Calculates current from current code, resistor value.

    ack |= LTC4282_read_16_bits(LTC4282_I2C_ADDRESS, LTC4282_VOLTAGE_MSB_REG, &voltage_code);   // Reads voltage voltage code
    VIN = LTC4282_VIN_code_to_voltage(voltage_code);  // Calculates VIN voltage from VIN code and lsb


@endverbatim

http://www.linear.com/product/LTC4282

http://www.linear.com/product/LTC4282#demoboards


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
    @ingroup LTC4282
    Header for LTC4282: High Current Hot Swap Controller with I2C Compatible Monitoring and EEPROM
*/

#ifndef LTC4282_H
#define LTC4282_H
#include <stdint.h>
//! Use table to select address
/*!
| LTC4282 I2C Address Assignment    | Value |   AD2    |   AD1    |   AD0    |
| :-------------------------------- | :---: | :------: | :------: | :------: |
| LTC4282_I2C_ADDRESS               | 0x40  |   Low    |   NC     |   Low    |
| LTC4282_I2C_ADDRESS               | 0x41  |   Low    |   High   |   NC     |
| LTC4282_I2C_ADDRESS               | 0x42  |   Low    |   NC     |   NC     |
| LTC4282_I2C_ADDRESS               | 0x43  |   Low    |   NC     |   High   |
| LTC4282_I2C_ADDRESS               | 0x44  |   Low    |   Low    |   Low    |
| LTC4282_I2C_ADDRESS               | 0x45  |   Low    |   High   |   High   |
| LTC4282_I2C_ADDRESS               | 0x46  |   Low    |   Low    |   NC     |
| LTC4282_I2C_ADDRESS               | 0x47  |   Low    |   Low    |   High   |
| LTC4282_I2C_ADDRESS               | 0x48  |   NC     |   NC     |   Low    |
| LTC4282_I2C_ADDRESS               | 0x49  |   NC     |   High   |   NC     |
| LTC4282_I2C_ADDRESS               | 0x4A  |   NC     |   NC     |   NC     |
| LTC4282_I2C_ADDRESS               | 0x4B  |   NC     |   NC     |   High   |
| LTC4282_I2C_ADDRESS               | 0x4C  |   NC     |   Low    |   Low    |
| LTC4282_I2C_ADDRESS               | 0x4D  |   NC     |   High   |   High   |
| LTC4282_I2C_ADDRESS               | 0x4E  |   NC     |   Low    |   NC     |
| LTC4282_I2C_ADDRESS               | 0x4F  |   NC     |   Low    |   High   |
| LTC4282_I2C_ADDRESS               | 0x50  |   High   |   NC     |   Low    |
| LTC4282_I2C_ADDRESS               | 0x51  |   High   |   High   |   NC     |
| LTC4282_I2C_ADDRESS               | 0x52  |   High   |   NC     |   NC     |
| LTC4282_I2C_ADDRESS               | 0x53  |   High   |   NC     |   High   |
| LTC4282_I2C_ADDRESS               | 0x54  |   High   |   Low    |   Low    |
| LTC4282_I2C_ADDRESS               | 0x55  |   High   |   High   |   High   |
| LTC4282_I2C_ADDRESS               | 0x56  |   High   |   Low    |   NC     |
| LTC4282_I2C_ADDRESS               | 0x57  |   High   |   Low    |   High   |
| LTC4282_I2C_ADDRESS               | 0x58  |   Low    |   High   |   Low    |
| LTC4282_I2C_ADDRESS               | 0x59  |   NC     |   High   |   Low    |
| LTC4282_I2C_ADDRESS               | 0x5A  |   High   |   High   |   Low    |
| LTC4282_I2C_MASS_WRITE            | 0x5F  | X        | X        | X        |
| LTC4282_I2C_ALERT_RESPONSE        | 0x19  | X        | X        | X        |
*/


/*! @name LTC4282 I2C Address
@{ */
//#define  LTC4282_I2C_ADDRESS               0x40   //   Low    |   NC     |   Low    |
//#define  LTC4282_I2C_ADDRESS               0x41   //   Low    |   High   |   NC     |
//#define  LTC4282_I2C_ADDRESS               0x42   //   Low    |   NC     |   NC     |
//#define  LTC4282_I2C_ADDRESS               0x43   //   Low    |   NC     |   High   |
//#define  LTC4282_I2C_ADDRESS               0x44   //   Low    |   Low    |   Low    |
//#define  LTC4282_I2C_ADDRESS               0x45   //   Low    |   High   |   High   |
//#define  LTC4282_I2C_ADDRESS               0x46   //   Low    |   Low    |   NC     |
//#define  LTC4282_I2C_ADDRESS               0x47   //   Low    |   Low    |   High   |
//#define  LTC4282_I2C_ADDRESS               0x48   //   NC     |   NC     |   Low    |
//#define  LTC4282_I2C_ADDRESS               0x49   //   NC     |   High   |   NC     |
#define  LTC4282_I2C_ADDRESS               0x4A   //   NC     |   NC     |   NC     |
//#define  LTC4282_I2C_ADDRESS               0x4B   //   NC     |   NC     |   High   |
//#define  LTC4282_I2C_ADDRESS               0x4C   //   NC     |   Low    |   Low    |
//#define  LTC4282_I2C_ADDRESS               0x4D   //   NC     |   High   |   High   |
//#define  LTC4282_I2C_ADDRESS               0x4E   //   NC     |   Low    |   NC     |
//#define  LTC4282_I2C_ADDRESS               0x4F   //   NC     |   Low    |   High   |
//#define  LTC4282_I2C_ADDRESS               0x50   //   High   |   NC     |   Low    |
//#define  LTC4282_I2C_ADDRESS               0x51   //   High   |   High   |   NC     |
//#define  LTC4282_I2C_ADDRESS               0x52   //   High   |   NC     |   NC     |
//#define  LTC4282_I2C_ADDRESS               0x53   //   High   |   NC     |   High   |
//#define  LTC4282_I2C_ADDRESS               0x54   //   High   |   Low    |   Low    |
//#define  LTC4282_I2C_ADDRESS               0x55   //   High   |   High   |   High   |
//#define  LTC4282_I2C_ADDRESS               0x56   //   High   |   Low    |   NC     |
//#define  LTC4282_I2C_ADDRESS               0x57   //   High   |   Low    |   High   |
//#define  LTC4282_I2C_ADDRESS               0x58   //   Low    |   High   |   Low    |
//#define  LTC4282_I2C_ADDRESS               0x59   //   NC     |   High   |   Low    |
//#define  LTC4282_I2C_ADDRESS               0x5A   //   High   |   High   |   Low    |
#define  LTC4282_I2C_MASS_WRITE              0x5F   // X        | X        | X        |
#define  LTC4282_I2C_ALERT_RESPONSE          0x19   // X        | X        | X        |
//! @}

/*!
| Name                                              | Value |
| :------------------------------------------------ | :---: |
|  LTC4282_CONTROL_MSB_REG                          | 0x00  |
|  LTC4282_CONTROL_LSB_REG                          | 0x01  |
|  LTC4282_ALERT_MSB_REG                            | 0x02  |
|  LTC4282_ALERT_LSB_REG                            | 0x03  |
|  LTC4282_FAULT_LOG_REG                            | 0x04  |
|  LTC4282_ADC_ALERT_LOG_REG                        | 0x05  |
|  LTC4282_FET_BAD_FAULT_TIME_REG                   | 0x06  |
|  LTC4282_GPIO_CONFIG_REG                          | 0x07  |
|  LTC4282_VSOURCE_ALARM_MIN_REG                    | 0x08  |
|  LTC4282_VSOURCE_ALARM_MAX_REG                    | 0x09  |
|  LTC4282_VGPIO_ALARM_MIN_REG                      | 0x0A  |
|  LTC4282_VGPIO_ALARM_MAX_REG                      | 0x0B  |
|  LTC4282_VSENSE_ALARM_MIN_REG                     | 0x0C  |
|  LTC4282_VSENSE_ALARM_MAX_REG                     | 0x0D  |
|  LTC4282_POWER_ALARM_MIN_REG                      | 0x0E  |
|  LTC4282_POWER_ALARM_MAX_REG                      | 0x0F  |
|  LTC4282_CLK_DEC_REG                              | 0x10  |
|  LTC4282_ILIM_ADJUST_REG                          | 0x11  |
|  LTC4282_METER_MSB5_REG                           | 0x12  |
|  LTC4282_METER_MSB4_REG                           | 0x13  |
|  LTC4282_METER_MSB3_REG                           | 0x14  |
|  LTC4282_METER_MSB2_REG                           | 0x15  |
|  LTC4282_METER_MSB1_REG                           | 0x16  |
|  LTC4282_METER_LSB_REG                            | 0x17  |
|  LTC4282_TICK_COUNTER_MSB3_REG                    | 0x18  |
|  LTC4282_TICK_COUNTER_MSB2_REG                    | 0x19  |
|  LTC4282_TICK_COUNTER_MSB1_REG                    | 0x1A  |
|  LTC4282_TICK_COUNTER_LSB_REG                     | 0x1B  |
|  LTC4282_ALERT_CONTROL_REG                        | 0x1C  |
|  LTC4282_ADC_CONTROL_REG                          | 0x1D  |
|  LTC4282_STATUS_MSB_REG                           | 0x1E  |
|  LTC4282_STATUS_LSB_REG                           | 0x1F  |
|  LTC4282_EE_CONTROL_MSB_REG                       | 0x20  |
|  LTC4282_EE_CONTROL_LSB_REG                       | 0x21  |
|  LTC4282_EE_ALERT_MSB_REG                         | 0x22  |
|  LTC4282_EE_ALERT_LSB_REG                         | 0x23  |
|  LTC4282_EE_FAULT_REG                             | 0x24  |
|  LTC4282_EE_ADC_ALERT_LOG_REG                     | 0x25  |
|  LTC4282_EE_FET_BAD_FAULT_TIME_REG                | 0x26  |
|  LTC4282_EE_GPIO_CONFIG_REG                       | 0x27  |
|  LTC4282_EE_VSOURCE_ALARM_MIN_REG                 | 0x28  |
|  LTC4282_EE_VSOURCE_ALARM_MAX_REG                 | 0x29  |
|  LTC4282_EE_VGPIO_ALARM_MIN_REG                   | 0x2A  |
|  LTC4282_EE_VGPIO_ALARM_MAX_REG                   | 0x2B  |
|  LTC4282_EE_VSENSE_ALARM_MIN_REG                  | 0x2C  |
|  LTC4282_EE_VSENSE_ALARM_MAX_REG                  | 0x2D  |
|  LTC4282_EE_POWER_ALARM_MIN_REG                   | 0x2E  |
|  LTC4282_EE_POWER_ALARM_MAX_REG                   | 0x2F  |
|  LTC4282_EE_CLK_DEC_REG                           | 0x30  |
|  LTC4282_EE_ILIM_ADJUST_REG                       | 0x31  |
|  LTC4282_VGPIO_MSB_REG                            | 0x34  |
|  LTC4282_VGPIO_LSB_REG                            | 0x35  |
|  LTC4282_VGPIO_MIN_MSB_REG                        | 0x36  |
|  LTC4282_VGPIO_MIN_LSB_REG                        | 0x37  |
|  LTC4282_VGPIO_MAX_MSB_REG                        | 0x38  |
|  LTC4282_VGPIO_MAX_LSB_REG                        | 0x39  |
|  LTC4282_VSOURCE_MSB_REG                          | 0x3A  |
|  LTC4282_VSOURCE_LSB_REG                          | 0x3B  |
|  LTC4282_VSOURCE_MIN_MSB_REG                      | 0x3C  |
|  LTC4282_VSOURCE_MIN_LSB_REG                      | 0x3D  |
|  LTC4282_VSOURCE_MAX_MSB_REG                      | 0x3E  |
|  LTC4282_VSOURCE_MAX_LSB_REG                      | 0x3F  |
|  LTC4282_VSENSE_MSB_REG                           | 0x40  |
|  LTC4282_VSENSE_LSB_REG                           | 0x41  |
|  LTC4282_VSENSE_MIN_MSB_REG                       | 0x42  |
|  LTC4282_VSENSE_MIN_LSB_REG                       | 0x43  |
|  LTC4282_VSENSE_MAX_MSB_REG                       | 0x44  |
|  LTC4282_VSENSE_MAX_LSB_REG                       | 0x45  |
|  LTC4282_POWER_MSB_REG                            | 0x46  |
|  LTC4282_POWER_LSB_REG                            | 0x47  |
|  LTC4282_POWER_MIN_MSB_REG                        | 0x48  |
|  LTC4282_POWER_MIN_LSB_REG                        | 0x49  |
|  LTC4282_POWER_MAX_MSB_REG                        | 0x4A  |
|  LTC4282_POWER_MAX_LSB_REG                        | 0x4B  |
|                                                   |       |
|  LTC4282_EE_SPARE_MSB3_REG                        | 0x4C  |
|  LTC4282_EE_SPARE_MSB2_REG                        | 0x4D  |
|  LTC4282_EE_SPARE_MSB1_REG                        | 0x4E  |
|  LTC4282_EE_SPARE_LSB_REG                         | 0x4F  |
*/
/*! @name Registers
@{ */
// Registers
#define LTC4282_CONTROL_MSB_REG                     0x00
#define LTC4282_CONTROL_LSB_REG                     0x01
#define LTC4282_ALERT_MSB_REG                       0x02
#define LTC4282_ALERT_LSB_REG                       0x03
#define LTC4282_FAULT_LOG_REG                       0x04
#define LTC4282_ADC_ALERT_LOG_REG                   0x05
#define LTC4282_FET_BAD_FAULT_TIME_REG              0x06
#define LTC4282_GPIO_CONFIG_REG                     0x07
#define LTC4282_VSOURCE_ALARM_MIN_REG               0x08
#define LTC4282_VSOURCE_ALARM_MAX_REG               0x09
#define LTC4282_VGPIO_ALARM_MIN_REG                 0x0A
#define LTC4282_VGPIO_ALARM_MAX_REG                 0x0B
#define LTC4282_VSENSE_ALARM_MIN_REG                0x0C
#define LTC4282_VSENSE_ALARM_MAX_REG                0x0D
#define LTC4282_POWER_ALARM_MIN_REG                 0x0E
#define LTC4282_POWER_ALARM_MAX_REG                 0x0F
#define LTC4282_CLK_DEC_REG                         0x10
#define LTC4282_ILIM_ADJUST_REG                     0x11
#define LTC4282_METER_MSB5_REG                      0x12
#define LTC4282_METER_MSB4_REG                      0x13
#define LTC4282_METER_MSB3_REG                      0x14
#define LTC4282_METER_MSB2_REG                      0x15
#define LTC4282_METER_MSB1_REG                      0x16
#define LTC4282_METER_LSB_REG                       0x17
#define LTC4282_TICK_COUNTER_MSB3_REG               0x18
#define LTC4282_TICK_COUNTER_MSB2_REG               0x19
#define LTC4282_TICK_COUNTER_MSB1_REG               0x1A
#define LTC4282_TICK_COUNTER_LSB_REG        0x1B
#define LTC4282_ALERT_CONTROL_REG                   0x1C
#define LTC4282_ADC_CONTROL_REG                     0x1D
#define LTC4282_STATUS_MSB_REG            0x1E
#define LTC4282_STATUS_LSB_REG                      0x1F
#define LTC4282_EE_CONTROL_MSB_REG                  0x20
#define LTC4282_EE_CONTROL_LSB_REG                  0x21
#define LTC4282_EE_ALERT_MSB_REG                    0x22
#define LTC4282_EE_ALERT_LSB_REG                    0x23
#define LTC4282_EE_FAULT_REG                        0x24
#define LTC4282_EE_ADC_ALERT_LOG_REG                0x25
#define LTC4282_EE_FET_BAD_FAULT_TIME_REG           0x26
#define LTC4282_EE_GPIO_CONFIG_REG                  0x27
#define LTC4282_EE_VSOURCE_ALARM_MIN_REG            0x28
#define LTC4282_EE_VSOURCE_ALARM_MAX_REG            0x29
#define LTC4282_EE_VGPIO_ALARM_MIN_REG              0x2A
#define LTC4282_EE_VGPIO_ALARM_MAX_REG              0x2B
#define LTC4282_EE_VSENSE_ALARM_MIN_REG             0x2C
#define LTC4282_EE_VSENSE_ALARM_MAX_REG             0x2D
#define LTC4282_EE_POWER_ALARM_MIN_REG              0x2E
#define LTC4282_EE_POWER_ALARM_MAX_REG              0x2F
#define LTC4282_EE_CLK_DEC_REG                      0x30
#define LTC4282_EE_ILIM_ADJUST_REG                  0x31
#define LTC4282_VGPIO_MSB_REG                       0x34
#define LTC4282_VGPIO_LSB_REG                       0x35
#define LTC4282_VGPIO_MIN_MSB_REG                   0x36
#define LTC4282_VGPIO_MIN_LSB_REG                   0x37
#define LTC4282_VGPIO_MAX_MSB_REG                   0x38
#define LTC4282_VGPIO_MAX_LSB_REG                   0x39
#define LTC4282_VSOURCE_MSB_REG                     0x3A
#define LTC4282_VSOURCE_LSB_REG                     0x3B
#define LTC4282_VSOURCE_MIN_MSB_REG                 0x3C
#define LTC4282_VSOURCE_MIN_LSB_REG                 0x3D
#define LTC4282_VSOURCE_MAX_MSB_REG                 0x3E
#define LTC4282_VSOURCE_MAX_LSB_REG                 0x3F
#define LTC4282_VSENSE_MSB_REG                      0x40
#define LTC4282_VSENSE_LSB_REG                      0x41
#define LTC4282_VSENSE_MIN_MSB_REG                  0x42
#define LTC4282_VSENSE_MIN_LSB_REG                  0x43
#define LTC4282_VSENSE_MAX_MSB_REG                  0x44
#define LTC4282_VSENSE_MAX_LSB_REG                  0x45
#define LTC4282_POWER_MSB_REG                       0x46
#define LTC4282_POWER_LSB_REG                       0x47
#define LTC4282_POWER_MIN_MSB_REG                   0x48
#define LTC4282_POWER_MIN_LSB_REG                   0x49
#define LTC4282_POWER_MAX_MSB_REG                   0x4A
#define LTC4282_POWER_MAX_LSB_REG                   0x4B

#define LTC4282_EE_SPARE_MSB3_REG         0x4C
#define LTC4282_EE_SPARE_MSB2_REG         0x4D
#define LTC4282_EE_SPARE_MSB1_REG         0x4E
#define LTC4282_EE_SPARE_LSB_REG          0x4F
//! @}
/*!
| Command Codes                                 | Value     |
| :-------------------------------------------- | :-------: |
|  LTC4282_ON_FAULT_MASK                        |    0x80   |
|  LTC4282_ENABLE_ON_DELAY                      |    0x40   |
|  LTC4282_ON_ENB                               |    0x20   |
|  LTC4282_ENABLE_MASS_WRITE                    |    0x10   |
|  LTC4282_ENABLE_FET_ON                        |    0x08   |
|  LTC4282_ENABLE_OC_AUTORETRY                  |    0x04   |
|  LTC4282_ENABLE_UV_AUTORETRY                  |    0x02   |
|  LTC4282_ENABLE_OV_AUTORETRY                  |    0x01   |
|                                               |           |
|  LTC4282_DISABLE_ON_DELAY                     |    0xBF   |
|  LTC4282_DISABLE_ON_ENB                       |    0xDF   |
|  LTC4282_DISABLE_MASS_WRITE                   |    0xEF   |
|  LTC4282_DISABLE_FET_ON                       |    0xF7   |
|  LTC4282_DISABLE_OC_AUTORETRY                 |    0xFB   |
|  LTC4282_DISABLE_UV_AUTORETRY                 |    0xFD   |
|  LTC4282_DISABLE_OV_AUTORETRY                 |    0xFE   |
|                                               |           |
|  LTC4282_FB_MODE_EXTERNAL                     |    0x00   |
|  LTC4282_FB_MODE_5                            |    0x40   |
|  LTC4282_FB_MODE_10                           |    0x80   |
|  LTC4282_FB_MODE_15                           |    0xC0   |
|  LTC4282_UV_MODE_EXTERNAL                     |    0x00   |
|  LTC4282_UV_MODE_5                            |    0x10   |
|  LTC4282_UV_MODE_10                           |    0x20   |
|  LTC4282_UV_MODE_15                           |    0x30   |
|  LTC4282_OV_MODE_EXTERNAL                     |    0x00   |
|  LTC4282_OV_MODE_5                            |    0x04   |
|  LTC4282_OV_MODE_10                           |    0x08   |
|  LTC4282_OV_MODE_15                           |    0x0C   |
|  LTC4282_VIN_MODE_3_V_3                       |    0x00   |
|  LTC4282_VIN_MODE_5_V                         |    0x01   |
|  LTC4282_VIN_MODE_12_V                        |    0x02   |
|  LTC4282_VIN_MODE_24_V                        |    0x03   |
|                                               |           |
|  LTC4282_EEPROM_DONE_ALERT                    |    0x80   |
|  LTC4282_FET_BAD_FAULT_ALERT                  |    0x40   |
|  LTC4282_FET_SHORT_ALERT                      |    0x20   |
|  LTC4282_ON_ALERT                             |    0x10   |
|  LTC4282_PB_ALERT                             |    0x08   |
|  LTC4282_OC_ALERT                             |    0x04   |
|  LTC4282_UV_ALERT                             |    0x02   |
|  LTC4282_OV_ALERT                             |    0x01   |
|                                               |           |
|  LTC4282_POWER_ALARM_HIGH                     |    0x80   |
|  LTC4282_POWER_ALARM_LOW                      |    0x40   |
|  LTC4282_VSENSE_ALARM_HIGH                    |    0x20   |
|  LTC4282_VSENSE_ALARM_LOW                     |    0x10   |
|  LTC4282_VSOURCE_ALARM_HIGH                   |    0x08   |
|  LTC4282_VSOURCE_ALARM_LOW                    |    0x04   |
|  LTC4282_VGPIO_ALARM_HIGH                     |    0x02   |
|  LTC4282_VGPIO_ALARM_LOW                      |    0x01   |
|                                               |           |
|  LTC4282_ENABLE_GPIO3_PD                      |    0x80   |
|  LTC4282_ENABLE_GPIO2_PD                      |    0x40   |
|  LTC4282_GPIO1_CONFIG_POWER_GOOD              |    0x00   |
|  LTC4282_GPIO1_CONFIG_POWER_BAD               |    0x10   |
|  LTC4282_GPIO1_CONFIG_OUTPUT                  |    0x20   |
|  LTC4282_GPIO1_CONFIG_INPUT                   |    0x30   |
|  LTC4282_GPIO1_OUTPUT                         |    0x08   |
|  LTC4282_ENABLE_ADC_CONV_ALERT                |    0x04   |
|  LTC4282_ENABLE_STRESS_TO_GPIO2               |    0x02   |
|  LTC4282_ENABLE_METER_OF_ALERT                |    0x01   |
|                                               |           |
|  LTC4282_DISABLE_GPIO3_PD                     |    0x7F   |
|  LTC4282_DISABLE_GPIO2_PD                     |    0xBF   |
|  LTC4282_DISABLE_GPIO1_OUTPUT                 |    0xF7   |
|  LTC4282_DISABLE_ADC_CONV_ALERT               |    0xFB   |
|  LTC4282_DISABLE_STRESS_TO_GPIO2              |    0xFD   |
|  LTC4282_DISABLE_METER_OF_ALERT               |    0xFE   |
|                                               |           |
|  LTC4282_ENABLE_COULOMB_METER                 |    0x80   |
|  LTC4282_ENABLE_TICK_OUT                      |    0x40   |
|  LTC4282_ENABLE_INT_CLK_OUT                   |    0x20   |
|                                               |           |
|  LTC4282_DISABLE_COULOMB_METER                |    0x7F   |
|  LTC4282_DISABLE_TICK_OUT                     |    0xBF   |
|  LTC4282_DISABLE_INT_CLK_OUT                  |    0xDF   |
|                                               |           |
|  LTC4282_ILIM_ADJUST_12_V_5                   |    0x00   |
|  LTC4282_ILIM_ADJUST_15_V_6                   |    0x20   |
|  LTC4282_ILIM_ADJUST_18_V_7                   |    0x40   |
|  LTC4282_ILIM_ADJUST_21_V_8                   |    0x60   |
|  LTC4282_ILIM_ADJUST_25_V_0                   |    0x80   |
|  LTC4282_ILIM_ADJUST_28_V_1                   |    0xA0   |
|  LTC4282_ILIM_ADJUST_31_V_2                   |    0xC0   |
|  LTC4282_ILIM_ADJUST_34_V_3                   |    0xE0   |
|                                               |           |
|  LTC4282_FOLDBACK_MODE_3_V_3                  |    0x00   |
|  LTC4282_FOLDBACK_MODE_5_V_0                  |    0x08   |
|  LTC4282_FOLDBACK_MODE_12_V_0                 |    0x10   |
|  LTC4282_FOLDBACK_MODE_24_V_0                 |    0x18   |
|                                               |           |
|  LTC4282_ENABLE_ADC_VSOURCE                   |    0x04   |
|  LTC4282_ENABLE_ADC_GPIO_MODE                 |    0x02   |
|  LTC4282_ENABLE_ADC_16_BIT                    |    0x01   |
|                                               |           |
|  LTC4282_DISABLE_ADC_VSOURCE                  |    0xFB   |
|  LTC4282_DISABLE_ADC_GPIO_MODE                |    0xFD   |
|  LTC4282_DISABLE_ADC_16_BIT                   |    0xFE   |
|                                               |           |
|  LTC4282_ENABLE_ALERT_GENERATED               |    0x80   |
|  LTC4282_ENABLE_ALERT_PD                      |    0x40   |
|  LTC4282_DISABLE_ALERT_GENERATED              |    0x7F   |
|  LTC4282_DISABLE_ALERT_PD                     |    0xBF   |
|                                               |           |
|  LTC4282_REBOOT                               |    0x80   |
|  LTC4282_ENABLE_METER_RESET                   |    0x40   |
|  LTC4282_ENABLE_METER_HALT                    |    0x20   |
|  LTC4282_ENABLE_FAULT_LOG                     |    0x04   |
|  LTC4282_GATEUP                               |    0x02   |
|  LTC4282_ADC_HALT                             |    0x01   |
|                                               |           |
|  LTC4282_DISABLE_METER_RESET                  |    0xBF   |
|  LTC4282_DISABLE_METER_HALT                   |    0xDF   |
|  LTC4282_DISABLE_FAULT_LOG                    |    0xFB   |
|                                               |           |
|*/
/*! @name Command Codes
@{ */
// Command Codes
#define LTC4282_ON_FAULT_MASK                   0x80
//#define LTC4282_ON_FAULT_MASK                   0x80
#define LTC4282_ON_DELAY                        0x40
#define LTC4282_ON_ENB                          0x20
#define LTC4282_MASS_WRITE                      0x10
#define LTC4282_FET_ON                          0x08
#define LTC4282_OC_AUTORETRY                    0x04
#define LTC4282_UV_AUTORETRY                    0x02
#define LTC4282_OV_AUTORETRY                    0x01
#define LTC4282_FB_MODE_EXTERNAL                0x00
#define LTC4282_FB_MODE_5                       0x40
#define LTC4282_FB_MODE_10                      0x80
#define LTC4282_FB_MODE_15                      0xC0
#define LTC4282_UV_MODE_EXTERNAL                0x00
#define LTC4282_UV_MODE_5                       0x10
#define LTC4282_UV_MODE_10                      0x20
#define LTC4282_UV_MODE_15                      0x30
#define LTC4282_OV_MODE_EXTERNAL                0x00
#define LTC4282_OV_MODE_5                       0x04
#define LTC4282_OV_MODE_10                      0x08
#define LTC4282_OV_MODE_15                      0x0C
#define LTC4282_VIN_MODE_3_V_3                  0x00
#define LTC4282_VIN_MODE_5_V                    0x01
#define LTC4282_VIN_MODE_12_V                   0x02
#define LTC4282_VIN_MODE_24_V                   0x03
#define LTC4282_EEPROM_DONE_ALERT               0x80
#define LTC4282_FET_BAD_FAULT_ALERT             0x40
#define LTC4282_FET_SHORT_ALERT                 0x20
#define LTC4282_ON_ALERT                        0x10
#define LTC4282_PB_ALERT                        0x08
#define LTC4282_OC_ALERT                        0x04
#define LTC4282_UV_ALERT                        0x02
#define LTC4282_OV_ALERT                        0x01
#define LTC4282_POWER_ALARM_HIGH                0x80
#define LTC4282_POWER_ALARM_LOW                 0x40
#define LTC4282_VSENSE_ALARM_HIGH               0x20
#define LTC4282_VSENSE_ALARM_LOW                0x10
#define LTC4282_VSOURCE_ALARM_HIGH              0x08
#define LTC4282_VSOURCE_ALARM_LOW               0x04
#define LTC4282_VGPIO_ALARM_HIGH                0x02
#define LTC4282_VGPIO_ALARM_LOW                 0x01
#define LTC4282_GPIO3_PD                        0x80
#define LTC4282_GPIO2_PD                        0x40
#define LTC4282_GPIO1_CONFIG_POWER_GOOD         0x00
#define LTC4282_GPIO1_CONFIG_POWER_BAD          0x20
#define LTC4282_GPIO1_CONFIG_OUTPUT             0x10
#define LTC4282_GPIO1_CONFIG_INPUT              0x30
#define LTC4282_GPIO1_OUTPUT                    0x08
#define LTC4282_ADC_CONV_ALERT                  0x04
#define LTC4282_STRESS_TO_GPIO2                 0x02
#define LTC4282_METER_OF_ALERT                  0x01
#define LTC4282_COULOMB_METER                   0x80
#define LTC4282_TICK_OUT                        0x40
#define LTC4282_INT_CLK_OUT                     0x20
#define LTC4282_ILIM_ADJUST_12_V_5              0x00
#define LTC4282_ILIM_ADJUST_15_V_6              0x20
#define LTC4282_ILIM_ADJUST_18_V_7              0x40
#define LTC4282_ILIM_ADJUST_21_V_8              0x60
#define LTC4282_ILIM_ADJUST_25_V_0              0x80
#define LTC4282_ILIM_ADJUST_28_V_1              0xA0
#define LTC4282_ILIM_ADJUST_31_V_2              0xC0
#define LTC4282_ILIM_ADJUST_34_V_3              0xE0
#define LTC4282_FOLDBACK_MODE_3_V_3             0x00
#define LTC4282_FOLDBACK_MODE_5_V_0             0x08
#define LTC4282_FOLDBACK_MODE_12_V_0            0x10
#define LTC4282_FOLDBACK_MODE_24_V_0            0x18
#define LTC4282_ADC_VSOURCE                     0x04
#define LTC4282_ADC_GPIO2_MODE                  0x02
#define LTC4282_ADC_16_BIT                      0x01
#define LTC4282_ENABLE_ALERT_GENERATED          0x80
#define LTC4282_ENABLE_ALERT_PD                 0x40
#define LTC4282_DISABLE_ALERT_GENERATED         0x7F
#define LTC4282_DISABLE_ALERT_PD                0xBF
#define LTC4282_REBOOT                          0x80
#define LTC4282_ENABLE_METER_RESET              0x40
#define LTC4282_ENABLE_METER_HALT               0x20
#define LTC4282_ENABLE_FAULT_LOG                0x04
#define LTC4282_GATEUP                          0x02
#define LTC4282_ADC_HALT                        0x01
#define LTC4282_DISABLE_METER_RESET             0xBF
#define LTC4282_DISABLE_METER_HALT              0xDF
#define LTC4282_DISABLE_FAULT_LOG               0xFB
//! @}

/*! @name Register MASKS
@{ */
//! LTC4282 Register MASKS
#define LTC4282_FB_MODE_MASK                    0xC0
#define LTC4282_UV_MODE_MASK                    0x30
#define LTC4282_OV_MODE_MASK                    0x0C
#define LTC4282_VIN_MODE_MASK                   0x03
#define LTC4282_ILIM_ADJUST_MASK                0xE0
#define LTC4282_FOLDBACK_MASK                   0x18
#define LTC4282_GPIO1_CONFIG_MASK               0x30
//! @}

/*! @name LTC4282 LSB's
@{ */
//! LTC4282 LSB's
const float LTC4282_ADC_FS = 40E-3;
const float LTC4282_VSOURCE_24V_FS = 33.28;
const float LTC4282_VSOURCE_12V_FS = 16.64;
const float LTC4282_VSOURCE_5V_FS = 8.32;
const float LTC4282_VSOURCE_3V3_FS = 5.547;
const float LTC4282_GPIO_FS = 1.28;
//! @}

union LT_union_int64_8bytes
{
  int64_t LT_int64;    //!< 32-bit signed integer to be converted to four bytes
  uint64_t LT_uint64;  //!< 32-bit unsigned integer to be converted to four bytes
  uint8_t LT_byte[8];  //!< 4 bytes (unsigned 8-bit integers) to be converted to a 32-bit signed or unsigned integer
};

//! Write an 8-bit code to the LTC4282
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
int8_t LTC4282_write(uint8_t i2c_address,        //!< Register address for the LTC4282
                     uint8_t adc_command,            //!< The "command byte" for the LTC4282
                     uint8_t code                    //!< Value that will be written to the register
                    );

//! Write an 16-bit code to the LTC4282
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
int8_t LTC4282_write_16_bits(uint8_t i2c_address, //!< Register address for the LTC4282
                             uint8_t adc_command,              //!< The "command byte" for the LTC4282
                             uint16_t code                      //!< Value that will be written to the register
                            );

//! Write an 32-bit code to the LTC4282
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
int8_t LTC4282_write_32_bits(uint8_t i2c_address,   //!< Register address for the LTC4282
                             uint8_t adc_command,                //!< The "command byte" for the LTC4282
                             uint32_t code                       //!< Value that will be written to the register
                            );

//! Write an 48-bit code to the LTC4282
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
int8_t LTC4282_write_48_bits(uint8_t i2c_address, //!< Register address for the LTC4282
                             uint8_t adc_command,              //!< The "command byte" for the LTC4282
                             uint64_t code                      //!< Value that will be written to the register
                            );

//! Read an 8-bit code from the LTC4282
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
int8_t LTC4282_read(uint8_t i2c_address,        //!< Register address for the LTC4282
                    uint8_t adc_command,            //!< The "command byte" for the LTC4282
                    uint8_t *adc_code                //!< Value that will be read from the register
                   );

//! Read a 16-bit code from the LTC4282
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
int8_t LTC4282_read_16_bits(uint8_t i2c_address,   //!< Register address for the LTC4282
                            uint8_t adc_command,               //!< The "command byte" for the LTC4282
                            uint16_t *adc_code                 //!< Value that will be read from the
                           );

//! Read a 32-bit code from the LTC4282
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
int8_t LTC4282_read_32_bits(uint8_t i2c_address, //!< Register address for the LTC4282
                            uint8_t adc_command,            //!< The "command byte" for the LTC4282
                            uint32_t *adc_code              //!< Value that will be read from the
                           );

//! Read a 48-bit code from the LTC4282
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
int8_t LTC4282_read_48_bits(uint8_t i2c_address, //!< Register address for the LTC4282
                            uint8_t adc_command,             //!< The "command byte" for the LTC4282
                            uint64_t *adc_code              //!< Value that will be read from the
                           );

//! Convert ADC code to VGPIO
//! @return Returns floating point value of GPIO voltage
float LTC4282_code_to_VGPIO(uint16_t code       //!< Code to be converted to voltage
                           );


//! Convert ADC code to voltage
//! @return Returns floating point value of voltage
float LTC4282_code_to_voltage(uint16_t code,    //!< Code to be converted to voltage
                              float fullscaleVoltage          //!< Fullscale VSOURCE Voltage
                             );

//! Convert ADC code to current
//! @return Returns floating point value of current
float LTC4282_code_to_current(uint16_t code,    //!< Code to be converted to current
                              float resistor                  //!< Sense Resistor Value
                             );

//! Convert ADC code to power
//! @return Returns floating point value of power
float LTC4282_code_to_power(uint16_t code,      //!< Code to be converted to power
                            float fullscaleVoltage,         //!< Fullscale VSOURCE Voltage
                            float resistor                  //!< Sense Resistor Value
                           );

//! Convert ADC code to energy
//! @return Returns floating point value of energy
float LTC4282_code_to_energy(uint64_t code,     //!< Code to be converted to energy
                             float fullscaleVoltage,         //!< Fullscale VSOURCE Voltage
                             float resistor,                 //!< Sense Resistor Value
                             float tConv                     //!< Conversion Time
                            );

//! Convert ADC code to coulombs
//! @return Returns floating point value of coulombs
float LTC4282_code_to_coulombs(uint64_t code,     //!< Code to be converted to Coulombs
                               float resistor,                 //!< Sense Resistor Value
                               float tConv                     //!< Conversion Time
                              );

//! Convert ADC code to average power
//! @return Returns floating point value of average power
float LTC4282_code_to_avg_power(uint64_t code,  //!< Code to be converted to average power
                                float energy,                   //!< Energy value
                                float tConv                     //!< Conversion Time
                               );

//! Convert ADC code to average current
//! @return Returns floating point value of average current
float LTC4282_code_to_avg_current(uint64_t code,  //!< Code to be converted to average power
                                  float coulombs,                   //!< Energy value
                                  float tConv                     //!< Conversion Time
                                 );


//! Convert ADC code to GPIO alarm voltage
//! @return Returns floating point value of GPIO alarm voltage
float LTC4282_code_to_GPIO_alarm(uint8_t code   //!< Code to be converted to voltage
                                );

//! Convert ADC code to alarm voltage
//! @return Returns floating point value of alarm voltage
float LTC4282_code_to_volt_alarm(uint8_t code,  //!< Code to be converted to voltage
                                 float fullscaleVoltage          //!< Fullscale VSOURCE voltage
                                );

//! Convert ADC code to alarm current
//! @return Returns floating point value of alarm current
float LTC4282_code_to_current_alarm(uint8_t code,    //!< Code to be converted to current
                                    float resistor                       //!< Sense Resistor Value
                                   );

//! Convert ADC code to alarm power
//! @return Returns floating point value of alarm power
float LTC4282_code_to_power_alarm(uint8_t code,      //!< Code to be converted to power
                                  float fullscaleVoltage,              //!< Fullscale VSOURCE voltage
                                  float resistor                       //!< Sense Resistor Value
                                 );

//! Convert GPIO voltage to alarm code
//! @return Returns the ADC code of the floating point value parameter
uint8_t LTC4282_VGPIO_to_code_alarm(float vgpio     //!< VGPIO voltage to be converted to alarm code
                                   );

//! Convert voltage to alarm code
//! @return Returns the ADC code of the floating point value parameter
uint8_t LTC4282_volt_to_code_alarm(float volt,      //!< Voltage to be converted to alarm code
                                   float fullscaleVoltage              //!< Fullscale voltage value to convert voltage into alarm code
                                  );

//! Convert current to alarm code
//! @return Returns the ADC code of the floating point value parameter
uint8_t LTC4282_current_to_code_alarm(float current, //!< Current value to be converted to alarm code
                                      float resistor                       //!< Sense Resistor Value
                                     );

//! Convert power to alarm code
//! @return Returns the ADC code of the floating point value
uint8_t LTC4282_power_to_code_alarm(float power,    //!< Power value to be converted to alarm code
                                    float resistor,                     //!< Sense Resistor Value
                                    float fullscaleVoltage              //!< Fullsvale voltage value to convert voltage into alarm code
                                   );

#endif
