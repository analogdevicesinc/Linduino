/*!
 LTC2946: 12-Bit Wide Range Power, Charge and Energy Monitor

@verbatim

The LTC®2946 is a rail-to-rail system monitor that measures
current, voltage, power, charge and energy. It features an
operating range of 2.7V to 100V and includes a shunt regulator
for supplies above 100V. The current measurement common mode
range of 0V to 100V is independent of the input supply.
A 12-bit ADC measures load current, input voltage and an
auxiliary external voltage. Load current and internally
calculated power are integrated over an external clock or
crystal or internal oscillator time base for charge and energy.
An accurate time base allows the LTC2946 to provide measurement
accuracy of better than ±0.6% for charge and ±1% for power and
energy. Minimum and maximum values are stored and an overrange
alert with programmable thresholds minimizes the need for software
polling. Data is reported via a standard I2C interface.
Shutdown mode reduces power consumption to 15uA.


I2C DATA FORMAT (MSB FIRST):

Data Out:
Byte #1                                    Byte #2                     Byte #3

START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK  X  X C5 C4 C3 C2 C1 C0 SACK D7 D6 D5 D4 D3 D2 D1 D0 SACK  STOP

Data In:
Byte #1                                    Byte #2                                    Byte #3

START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK  X  X  C5 C4 C3 C2 C1 C0 SACK  Repeat Start SA6 SA5 SA4 SA3 SA2 SA1 SA0 R SACK

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

Read power, current, voltage, charge and energy.

    CTRLA = LTC2946_CHANNEL_CONFIG_V_C_3|LTC2946_SENSE_PLUS|LTC2946_OFFSET_CAL_EVERY|LTC2946_ADIN_GND;  //! Set Control A register to default value in continuous mode
  ack |= LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_CTRLA_REG, CTRLA);   //! Sets the LTC2946 to continuous mode

    resistor = .02; // Resistor Value On Demo Board

    ack |= LTC2946_read_24_bits(LTC2946_I2C_ADDRESS, LTC2946_POWER_MSB2_REG, &power_code);  // Reads the ADC registers that contains V^2
    power = LTC2946_code_to_power(power_code, resistor, LTC2946_Power_lsb); // Calculates power from power code, resistor value and power lsb

    ack |= LTC2946_read_12_bits(LTC2946_I2C_ADDRESS, LTC2946_DELTA_SENSE_MSB_REG, &current_code); // Reads the voltage code across sense resistor
    current = LTC2946_code_to_current(current_code, resistor, LTC2946_DELTA_SENSE_lsb); // Calculates current from current code, resistor value and current lsb

    ack |= LTC2946_read_12_bits(LTC2946_I2C_ADDRESS, LTC2946_VIN_MSB_REG, &VIN_code);   // Reads VIN voltage code
    VIN = LTC2946_VIN_code_to_voltage(VIN_code, LTC2946_VIN_lsb);  // Calculates VIN voltage from VIN code and lsb

    ack |= LTC2946_read_32_bits(LTC2946_I2C_ADDRESS, LTC2946_ENERGY_MSB3_REG, &energy_code);  // Reads energy code
  energy = LTC2946_code_to_energy(energy_code,resistor,LTC2946_Power_lsb, LTC2946_INTERNAL_TIME_lsb); //Calculates Energy in Joules from energy_code, resistor, power lsb and time lsb

  ack |= LTC2946_read_32_bits(LTC2946_I2C_ADDRESS, LTC2946_CHARGE_MSB3_REG, &charge_code);  // Reads charge code
    charge = LTC2946_code_to_coulombs(charge_code,resistor,LTC2946_DELTA_SENSE_lsb, LTC2946_INTERNAL_TIME_lsb); //Calculates charge in coulombs from charge_code, resistor, current lsb and time lsb



@endverbatim

http://www.linear.com/product/LTC2946

http://www.linear.com/product/ltc2946#demoboards


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
    @ingroup LTC2946
    Header for LTC2946: 12-Bit Wide Range Power, Charge and Energy Monitor
*/

#ifndef LTC2946_H
#define LTC2946_H

#include <Wire.h>

//! Use table to select address
/*!
| LTC2946 I2C Address Assignment    | Value |   AD1    |   AD2    |
| :-------------------------------- | :---: | :------: | :------: |
| LTC2946_I2C_ADDRESS               | 0x67  |   High   |   Low    |
| LTC2946_I2C_ADDRESS               | 0x68  |   Float  |   High   |
| LTC2946_I2C_ADDRESS               | 0x69  |   High   |   High   |
| LTC2946_I2C_ADDRESS               | 0x6A  |   Float  |   Float  |
| LTC2946_I2C_ADDRESS               | 0x6B  |   Float  |   Low    |
| LTC2946_I2C_ADDRESS               | 0x6C  |   Low    |   High   |
| LTC2946_I2C_ADDRESS               | 0x6D  |   High   |   Float  |
| LTC2946_I2C_ADDRESS               | 0x6E  |   Low    |   Float  |
| LTC2946_I2C_ADDRESS               | 0x6F  |   Low    |   Low    |
|                                   |       |          |          |
| LTC2946_I2C_MASS_WRITE            | 0xCC  | X        | X        |
| LTC2946_I2C_ALERT_RESPONSE        | 0x19  | X        | X        |
*/
/*! @name LTC2946 I2C Address Assignments
@{ */

// Address Choices:
// To choose an address, comment out all options except the
// configuration on the demo board.

//  Address assignment
// LTC2946 I2C Address                 //  AD1       AD0
// #define LTC2946_I2C_ADDRESS 0x67    //  High      Low
// #define LTC2946_I2C_ADDRESS 0x68    //  Float     High
// #define LTC2946_I2C_ADDRESS 0x69    //  High      High
// #define LTC2946_I2C_ADDRESS 0x6A    //  Float     Float
// #define LTC2946_I2C_ADDRESS 0x6B    //  Float     Low
// #define LTC2946_I2C_ADDRESS 0x6C    //  Low       High
// #define LTC2946_I2C_ADDRESS 0x6D    //  High      Float
// #define LTC2946_I2C_ADDRESS 0x6E    //  Low       Float
#define LTC2946_I2C_ADDRESS 0x6F       //  Low       Low

#define LTC2946_I2C_MASS_WRITE      0xCC
#define LTC2946_I2C_ALERT_RESPONSE  0x19
//! @}


/*!
| Name                                              | Value |
| :------------------------------------------------ | :---: |
| LTC2946_CTRLA_REG                                 | 0x00  |
| LTC2946_CTRLB_REG                                 | 0x01  |
| LTC2946_ALERT1_REG                                | 0x02  |
| LTC2946_STATUS1_REG                               | 0x03  |
| LTC2946_FAULT1_REG                                | 0x04  |
| LTC2946_POWER_MSB2_REG                            | 0x05  |
| LTC2946_POWER_MSB1_REG                            | 0x06  |
| LTC2946_POWER_LSB_REG                             | 0x07  |
| LTC2946_MAX_POWER_MSB2_REG                        | 0x08  |
| LTC2946_MAX_POWER_MSB1_REG                        | 0x09  |
| LTC2946_MAX_POWER_LSB_REG                         | 0x0A  |
| LTC2946_MIN_POWER_MSB2_REG                        | 0x0B  |
| LTC2946_MIN_POWER_MSB1_REG                        | 0x0C  |
| LTC2946_MIN_POWER_LSB_REG                         | 0x0D  |
| LTC2946_MAX_POWER_THRESHOLD_MSB2_REG              | 0x0E  |
| LTC2946_MAX_POWER_THRESHOLD_MSB1_REG              | 0x0F  |
| LTC2946_MAX_POWER_THRESHOLD_LSB_REG               | 0x10  |
| LTC2946_MIN_POWER_THRESHOLD_MSB2_REG              | 0x11  |
| LTC2946_MIN_POWER_THRESHOLD_MSB1_REG              | 0x12  |
| LTC2946_MIN_POWER_THRESHOLD_LSB_REG               | 0x13  |
| LTC2946_DELTA_SENSE_MSB_REG                       | 0x14  |
| LTC2946_DELTA_SENSE_LSB_REG                       | 0x15  |
| LTC2946_MAX_DELTA_SENSE_MSB_REG                   | 0x16  |
| LTC2946_MAX_DELTA_SENSE_LSB_REG                   | 0x17  |
| LTC2946_MIN_DELTA_SENSE_MSB_REG                   | 0x18  |
| LTC2946_MIN_DELTA_SENSE_LSB_REG                   | 0x19  |
| LTC2946_MAX_DELTA_SENSE_THRESHOLD_MSB_REG         | 0x1A  |
| LTC2946_MAX_DELTA_SENSE_THRESHOLD_LSB_REG         | 0x1B  |
| LTC2946_MIN_DELTA_SENSE_THRESHOLD_MSB_REG         | 0x1C  |
| LTC2946_MIN_DELTA_SENSE_THRESHOLD_LSB_REG         | 0x1D  |
| LTC2946_VIN_MSB_REG                               | 0x1E  |
| LTC2946_VIN_LSB_REG                               | 0x1F  |
| LTC2946_MAX_VIN_MSB_REG                           | 0x20  |
| LTC2946_MAX_VIN_LSB_REG                           | 0x21  |
| LTC2946_MIN_VIN_MSB_REG                           | 0x22  |
| LTC2946_MIN_VIN_LSB_REG                           | 0x23  |
| LTC2946_MAX_VIN_THRESHOLD_MSB_REG                 | 0x24  |
| LTC2946_MAX_VIN_THRESHOLD_LSB_REG                 | 0x25  |
| LTC2946_MIN_VIN_THRESHOLD_MSB_REG                 | 0x26  |
| LTC2946_MIN_VIN_THRESHOLD_LSB_REG                 | 0x27  |
| LTC2946_ADIN_MSB_REG                              | 0x28  |
| LTC2946_ADIN_LSB_REG_REG                          | 0x29  |
| LTC2946_MAX_ADIN_MSB_REG                          | 0x2A  |
| LTC2946_MAX_ADIN_LSB_REG                          | 0x2B  |
| LTC2946_MIN_ADIN_MSB_REG                          | 0x2C  |
| LTC2946_MIN_ADIN_LSB_REG                          | 0x2D  |
| LTC2946_MAX_ADIN_THRESHOLD_MSB_REG                | 0x2E  |
| LTC2946_MAX_ADIN_THRESHOLD_LSB_REG                | 0x2F  |
| LTC2946_MIN_ADIN_THRESHOLD_MSB_REG                | 0x30  |
| LTC2946_MIN_ADIN_THRESHOLD_LSB_REG                | 0x31  |
| LTC2946_ALERT2_REG                                | 0x32  |
| LTC2946_GPIO_CFG_REG                              | 0x33  |
| LTC2946_TIME_COUNTER_MSB3_REG                     | 0x34  |
| LTC2946_TIME_COUNTER_MSB2_REG                     | 0x35  |
| LTC2946_TIME_COUNTER_MSB1_REG                     | 0x36  |
| LTC2946_TIME_COUNTER_LSB_REG                      | 0x37  |
| LTC2946_CHARGE_MSB3_REG                           | 0x38  |
| LTC2946_CHARGE_MSB2_REG                           | 0x39  |
| LTC2946_CHARGE_MSB1_REG                           | 0x3A  |
| LTC2946_CHARGE_LSB_REG                            | 0x3B  |
| LTC2946_ENERGY_MSB3_REG                           | 0x3C  |
| LTC2946_ENERGY_MSB2_REG                           | 0x3D  |
| LTC2946_ENERGY_MSB1_REG                           | 0x3E  |
| LTC2946_ENERGY_LSB_REG                            | 0x3F  |
| LTC2946_STATUS2_REG                               | 0x40  |
| LTC2946_FAULT2_REG                                | 0x41  |
| LTC2946_GPIO3_CTRL_REG                            | 0x42  |
| LTC2946_CLK_DIV_REG                               | 0x43  |
*/



/*! @name Registers
@{ */
// Registers

#define LTC2946_CTRLA_REG                           0x00
#define LTC2946_CTRLB_REG                           0x01
#define LTC2946_ALERT1_REG                          0x02
#define LTC2946_STATUS1_REG                         0x03
#define LTC2946_FAULT1_REG                          0x04

#define LTC2946_POWER_MSB2_REG                      0x05
#define LTC2946_POWER_MSB1_REG                      0x06
#define LTC2946_POWER_LSB_REG                       0x07
#define LTC2946_MAX_POWER_MSB2_REG                  0x08
#define LTC2946_MAX_POWER_MSB1_REG                  0x09
#define LTC2946_MAX_POWER_LSB_REG                   0x0A
#define LTC2946_MIN_POWER_MSB2_REG                  0x0B
#define LTC2946_MIN_POWER_MSB1_REG                  0x0C
#define LTC2946_MIN_POWER_LSB_REG                   0x0D
#define LTC2946_MAX_POWER_THRESHOLD_MSB2_REG        0x0E
#define LTC2946_MAX_POWER_THRESHOLD_MSB1_REG        0x0F
#define LTC2946_MAX_POWER_THRESHOLD_LSB_REG         0x10
#define LTC2946_MIN_POWER_THRESHOLD_MSB2_REG        0x11
#define LTC2946_MIN_POWER_THRESHOLD_MSB1_REG        0x12
#define LTC2946_MIN_POWER_THRESHOLD_LSB_REG         0x13

#define LTC2946_DELTA_SENSE_MSB_REG                 0x14
#define LTC2946_DELTA_SENSE_LSB_REG                 0x15
#define LTC2946_MAX_DELTA_SENSE_MSB_REG             0x16
#define LTC2946_MAX_DELTA_SENSE_LSB_REG             0x17
#define LTC2946_MIN_DELTA_SENSE_MSB_REG             0x18
#define LTC2946_MIN_DELTA_SENSE_LSB_REG             0x19
#define LTC2946_MAX_DELTA_SENSE_THRESHOLD_MSB_REG   0x1A
#define LTC2946_MAX_DELTA_SENSE_THRESHOLD_LSB_REG   0x1B
#define LTC2946_MIN_DELTA_SENSE_THRESHOLD_MSB_REG   0x1C
#define LTC2946_MIN_DELTA_SENSE_THRESHOLD_LSB_REG   0x1D

#define LTC2946_VIN_MSB_REG                         0x1E
#define LTC2946_VIN_LSB_REG                         0x1F
#define LTC2946_MAX_VIN_MSB_REG                     0x20
#define LTC2946_MAX_VIN_LSB_REG                     0x21
#define LTC2946_MIN_VIN_MSB_REG                     0x22
#define LTC2946_MIN_VIN_LSB_REG                     0x23
#define LTC2946_MAX_VIN_THRESHOLD_MSB_REG           0x24
#define LTC2946_MAX_VIN_THRESHOLD_LSB_REG           0x25
#define LTC2946_MIN_VIN_THRESHOLD_MSB_REG           0x26
#define LTC2946_MIN_VIN_THRESHOLD_LSB_REG           0x27

#define LTC2946_ADIN_MSB_REG                        0x28
#define LTC2946_ADIN_LSB_REG_REG                    0x29
#define LTC2946_MAX_ADIN_MSB_REG                    0x2A
#define LTC2946_MAX_ADIN_LSB_REG                    0x2B
#define LTC2946_MIN_ADIN_MSB_REG                    0x2C
#define LTC2946_MIN_ADIN_LSB_REG                    0x2D
#define LTC2946_MAX_ADIN_THRESHOLD_MSB_REG          0x2E
#define LTC2946_MAX_ADIN_THRESHOLD_LSB_REG          0x2F
#define LTC2946_MIN_ADIN_THRESHOLD_MSB_REG          0x30
#define LTC2946_MIN_ADIN_THRESHOLD_LSB_REG          0x31

#define LTC2946_ALERT2_REG                          0x32
#define LTC2946_GPIO_CFG_REG                        0x33

#define LTC2946_TIME_COUNTER_MSB3_REG               0x34
#define LTC2946_TIME_COUNTER_MSB2_REG               0x35
#define LTC2946_TIME_COUNTER_MSB1_REG               0x36
#define LTC2946_TIME_COUNTER_LSB_REG                0x37

#define LTC2946_CHARGE_MSB3_REG                     0x38
#define LTC2946_CHARGE_MSB2_REG                     0x39
#define LTC2946_CHARGE_MSB1_REG                     0x3A
#define LTC2946_CHARGE_LSB_REG                      0x3B

#define LTC2946_ENERGY_MSB3_REG                     0x3C
#define LTC2946_ENERGY_MSB2_REG                     0x3D
#define LTC2946_ENERGY_MSB1_REG                     0x3E
#define LTC2946_ENERGY_LSB_REG                      0x3F

#define LTC2946_STATUS2_REG                         0x40
#define LTC2946_FAULT2_REG                          0x41
#define LTC2946_GPIO3_CTRL_REG                      0x42
#define LTC2946_CLK_DIV_REG                         0x43

//! @}

/*!
| Voltage Selection Command            | Value |
| :------------------------------------| :---: |
| LTC2946_DELTA_SENSE                  | 0x00  |
| LTC2946_VDD                          | 0x08  |
| LTC2946_ADIN                         | 0x10  |
| LTC2946_SENSE_PLUS                   | 0x18  |
*/

/*! @name Voltage Selection Command
@{ */
// Voltage Selection Command
#define LTC2946_DELTA_SENSE            0x00
#define LTC2946_VDD                    0x08
#define LTC2946_ADIN                   0x10
#define LTC2946_SENSE_PLUS             0x18
//! @}

/*!
| Command Codes                                 | Value     |
| :-------------------------------------------- | :-------: |
| LTC2946_ADIN_INTVCC                         | 0x80  |
| LTC2946_ADIN_GND                            | 0x00  |
| LTC2946_OFFSET_CAL_LAST                 |   0x60  |
| LTC2946_OFFSET_CAL_128                      |   0x40  |
| LTC2946_OFFSET_CAL_16                       | 0x20  |
| LTC2946_OFFSET_CAL_EVERY                    | 0x00  |
| LTC2946_CHANNEL_CONFIG_SNAPSHOT             | 0x07  |
| LTC2946_CHANNEL_CONFIG_V_C                  | 0x06  |
| LTC2946_CHANNEL_CONFIG_A_V_C_1              | 0x05  |
| LTC2946_CHANNEL_CONFIG_A_V_C_2              | 0x04    |
| LTC2946_CHANNEL_CONFIG_A_V_C_3              | 0x03    |
| LTC2946_CHANNEL_CONFIG_V_C_1              | 0x02  |
| LTC2946_CHANNEL_CONFIG_V_C_2              | 0x01  |
| LTC2946_CHANNEL_CONFIG_V_C_3              | 0x00  |
| LTC2946_ENABLE_ALERT_CLEAR                | 0x80  |
| LTC2946_ENABLE_SHUTDOWN                   | 0x40    |
| LTC2946_ENABLE_CLEARED_ON_READ            | 0x20    |
| LTC2946_ENABLE_STUCK_BUS_RECOVER          | 0x10    |
| LTC2946_DISABLE_ALERT_CLEAR               | 0x7F    |
| LTC2946_DISABLE_SHUTDOWN                  | 0xBF    |
| LTC2946_DISABLE_CLEARED_ON_READ           | 0xDF    |
| LTC2946_DISABLE_STUCK_BUS_RECOVER         | 0xEF    |
| LTC2946_ACC_PIN_CONTROL                   | 0x08    |
| LTC2946_DISABLE_ACC                       | 0x04    |
| LTC2946_ENABLE_ACC                        | 0x00    |
| LTC2946_RESET_ALL                         | 0x03    |
| LTC2946_RESET_ACC                         | 0x02    |
| LTC2946_ENABLE_AUTO_RESET                 | 0x01    |
| LTC2946_DISABLE_AUTO_RESET                | 0x00    |
| LTC2946_MAX_POWER_MSB2_RESET              | 0x00    |
| LTC2946_MIN_POWER_MSB2_RESET              | 0xFF    |
| LTC2946_MAX_DELTA_SENSE_MSB_RESET         | 0x00    |
| LTC2946_MIN_DELTA_SENSE_MSB_RESET         | 0xFF    |
| LTC2946_MAX_VIN_MSB_RESET                 | 0x00    |
| LTC2946_MIN_VIN_MSB_RESET                 | 0xFF    |
| LTC2946_MAX_ADIN_MSB_RESET                | 0x00    |
| LTC2946_MIN_ADIN_MSB_RESET                | 0xFF    |
| LTC2946_ENABLE_MAX_POWER_ALERT            | 0x80    |
| LTC2946_ENABLE_MIN_POWER_ALERT            | 0x40    |
| LTC2946_DISABLE_MAX_POWER_ALERT           | 0x7F    |
| LTC2946_DISABLE_MIN_POWER_ALERT           | 0xBF    |
| LTC2946_ENABLE_MAX_I_SENSE_ALERT          | 0x20    |
| LTC2946_ENABLE_MIN_I_SENSE_ALERT          | 0x10    |
| LTC2946_DISABLE_MAX_I_SENSE_ALERT         | 0xDF    |
| LTC2946_DISABLE_MIN_I_SENSE_ALERT         | 0xEF    |
| LTC2946_ENABLE_MAX_VIN_ALERT              | 0x08    |
| LTC2946_ENABLE_MIN_VIN_ALERT              | 0x04    |
| LTC2946_DISABLE_MAX_VIN_ALERT             | 0xF7    |
| LTC2946_DISABLE_MIN_VIN_ALERT             | 0xFB    |
| LTC2946_ENABLE_MAX_ADIN_ALERT             | 0x02    |
| LTC2946_ENABLE_MIN_ADIN_ALERT             | 0x01    |
| LTC2946_DISABLE_MAX_ADIN_ALERT            | 0xFD    |
| LTC2946_DISABLE_MIN_ADIN_ALERT            | 0xFE    |
| LTC2946_ENABLE_ADC_DONE_ALERT             | 0x80    |
| LTC2946_DISABLE_ADC_DONE_ALERT            | 0x7F    |
| LTC2946_ENABLE_GPIO_1_ALERT               | 0x40    |
| LTC2946_DISABLE_GPIO_1_ALERT              | 0xBF    |
| LTC2946_ENABLE_GPIO_2_ALERT               | 0x20    |
| LTC2946_DISABLE_GPIO_2_ALERT              | 0xDF    |
| LTC2946_ENABLE_STUCK_BUS_WAKE_ALERT       | 0x08    |
| LTC2946_DISABLE_STUCK_BUS_WAKE_ALERT      | 0xF7    |
| LTC2946_ENABLE_ENERGY_OVERFLOW_ALERT      | 0x04    |
| LTC2946_DISABLE_ENERGY_OVERFLOW_ALERT     | 0xFB    |
| LTC2946_ENABLE_CHARGE_OVERFLOW_ALERT      | 0x02    |
| LTC2946_DISABLE_CHARGE_OVERFLOW_ALERT     | 0xFD    |
| LTC2946_ENABLE_COUNTER_OVERFLOW_ALERT     | 0x01    |
| LTC2946_DISABLE_COUNTER_OVERFLOW_ALERT    | 0xFE    |
| LTC2946_GPIO1_IN_ACTIVE_HIGH              | 0xC0    |
| LTC2946_GPIO1_IN_ACTIVE_LOW               | 0x80    |
| LTC2946_GPIO1_OUT_HIGH_Z                  | 0x40    |
| LTC2946_GPIO1_OUT_LOW                     | 0x00    |
| LTC2946_GPIO2_IN_ACTIVE_HIGH              | 0x30    |
| LTC2946_GPIO2_IN_ACTIVE_LOW               | 0x20    |
| LTC2946_GPIO2_OUT_HIGH_Z                  | 0x10    |
| LTC2946_GPIO2_OUT_LOW                     | 0x12    |
| LTC2946_GPIO2_IN_ACC                      | 0x00    |
| LTC2946_GPIO3_IN_ACTIVE_HIGH              | 0x18    |
| LTC2946_GPIO3_IN_ACTIVE_LOW               | 0x10    |
| LTC2946_GPIO3_OUT_REG_42                  | 0x04    |
| LTC2946_GPIO3_OUT_ALERT                   | 0x00    |
| LTC2946_GPIO3_OUT_LOW                     | 0x40    |
| LTC2946_GPIO3_OUT_HIGH_Z                  | 0x00    |
| LTC2946_GPIO_ALERT_CLEAR                  | 0x00    |
*/
/*! @name Command Codes
@{ */
// Command Codes

#define LTC2946_ADIN_INTVCC                     0x80
#define LTC2946_ADIN_GND                        0x00

#define LTC2946_OFFSET_CAL_LAST                 0x60
#define LTC2946_OFFSET_CAL_128                  0x40
#define LTC2946_OFFSET_CAL_16                   0x20
#define LTC2946_OFFSET_CAL_EVERY                0x00

#define LTC2946_CHANNEL_CONFIG_SNAPSHOT         0x07
#define LTC2946_CHANNEL_CONFIG_V_C              0x06
#define LTC2946_CHANNEL_CONFIG_A_V_C_1          0x05
#define LTC2946_CHANNEL_CONFIG_A_V_C_2          0x04
#define LTC2946_CHANNEL_CONFIG_A_V_C_3          0x03
#define LTC2946_CHANNEL_CONFIG_V_C_1            0x02
#define LTC2946_CHANNEL_CONFIG_V_C_2            0x01
#define LTC2946_CHANNEL_CONFIG_V_C_3            0x00


#define LTC2946_ENABLE_ALERT_CLEAR              0x80
#define LTC2946_ENABLE_SHUTDOWN                 0x40
#define LTC2946_ENABLE_CLEARED_ON_READ          0x20
#define LTC2946_ENABLE_STUCK_BUS_RECOVER        0x10

#define LTC2946_DISABLE_ALERT_CLEAR             0x7F
#define LTC2946_DISABLE_SHUTDOWN                0xBF
#define LTC2946_DISABLE_CLEARED_ON_READ         0xDF
#define LTC2946_DISABLE_STUCK_BUS_RECOVER       0xEF

#define LTC2946_ACC_PIN_CONTROL                 0x08
#define LTC2946_DISABLE_ACC                     0x04
#define LTC2946_ENABLE_ACC                      0x00

#define LTC2946_RESET_ALL                       0x03
#define LTC2946_RESET_ACC                       0x02
#define LTC2946_ENABLE_AUTO_RESET               0x01
#define LTC2946_DISABLE_AUTO_RESET              0x00


#define LTC2946_MAX_POWER_MSB2_RESET            0x00
#define LTC2946_MIN_POWER_MSB2_RESET            0xFF
#define LTC2946_MAX_DELTA_SENSE_MSB_RESET       0x00
#define LTC2946_MIN_DELTA_SENSE_MSB_RESET       0xFF
#define LTC2946_MAX_VIN_MSB_RESET               0x00
#define LTC2946_MIN_VIN_MSB_RESET               0xFF
#define LTC2946_MAX_ADIN_MSB_RESET              0x00
#define LTC2946_MIN_ADIN_MSB_RESET              0xFF

#define LTC2946_ENABLE_MAX_POWER_ALERT          0x80
#define LTC2946_ENABLE_MIN_POWER_ALERT          0x40
#define LTC2946_DISABLE_MAX_POWER_ALERT         0x7F
#define LTC2946_DISABLE_MIN_POWER_ALERT         0xBF

#define LTC2946_ENABLE_MAX_I_SENSE_ALERT        0x20
#define LTC2946_ENABLE_MIN_I_SENSE_ALERT        0x10
#define LTC2946_DISABLE_MAX_I_SENSE_ALERT       0xDF
#define LTC2946_DISABLE_MIN_I_SENSE_ALERT       0xEF

#define LTC2946_ENABLE_MAX_VIN_ALERT            0x08
#define LTC2946_ENABLE_MIN_VIN_ALERT            0x04
#define LTC2946_DISABLE_MAX_VIN_ALERT           0xF7
#define LTC2946_DISABLE_MIN_VIN_ALERT           0xFB

#define LTC2946_ENABLE_MAX_ADIN_ALERT           0x02
#define LTC2946_ENABLE_MIN_ADIN_ALERT           0x01
#define LTC2946_DISABLE_MAX_ADIN_ALERT          0xFD
#define LTC2946_DISABLE_MIN_ADIN_ALERT          0xFE

#define LTC2946_ENABLE_ADC_DONE_ALERT           0x80
#define LTC2946_DISABLE_ADC_DONE_ALERT          0x7F

#define LTC2946_ENABLE_GPIO_1_ALERT             0x40
#define LTC2946_DISABLE_GPIO_1_ALERT            0xBF

#define LTC2946_ENABLE_GPIO_2_ALERT             0x20
#define LTC2946_DISABLE_GPIO_2_ALERT            0xDF

#define LTC2946_ENABLE_STUCK_BUS_WAKE_ALERT     0x08
#define LTC2946_DISABLE_STUCK_BUS_WAKE_ALERT    0xF7

#define LTC2946_ENABLE_ENERGY_OVERFLOW_ALERT    0x04
#define LTC2946_DISABLE_ENERGY_OVERFLOW_ALERT   0xFB

#define LTC2946_ENABLE_CHARGE_OVERFLOW_ALERT    0x02
#define LTC2946_DISABLE_CHARGE_OVERFLOW_ALERT   0xFD

#define LTC2946_ENABLE_COUNTER_OVERFLOW_ALERT   0x01
#define LTC2946_DISABLE_COUNTER_OVERFLOW_ALERT  0xFE

#define LTC2946_GPIO1_IN_ACTIVE_HIGH            0xC0
#define LTC2946_GPIO1_IN_ACTIVE_LOW             0x80
#define LTC2946_GPIO1_OUT_HIGH_Z                0x40
#define LTC2946_GPIO1_OUT_LOW                   0x00

#define LTC2946_GPIO2_IN_ACTIVE_HIGH            0x30
#define LTC2946_GPIO2_IN_ACTIVE_LOW             0x20
#define LTC2946_GPIO2_OUT_HIGH_Z                0x10
#define LTC2946_GPIO2_OUT_LOW                   0x12
#define LTC2946_GPIO2_IN_ACC                    0x00


#define LTC2946_GPIO3_IN_ACTIVE_HIGH            0x0C
#define LTC2946_GPIO3_IN_ACTIVE_LOW             0x08
#define LTC2946_GPIO3_OUT_REG_42                0x04
#define LTC2946_GPIO3_OUT_ALERT                 0x00
#define LTC2946_GPIO3_OUT_LOW                   0x40
#define LTC2946_GPIO3_OUT_HIGH_Z                0x00
#define LTC2946_GPIO_ALERT_CLEAR                0x00
//! @}


/*!
| Register Mask Command                | Value |
| :------------------------------------| :---: |
| LTC2946_CTRLA_ADIN_MASK              |  0x7F |
| LTC2946_CTRLA_OFFSET_MASK            |  0x9F |
| LTC2946_CTRLA_VOLTAGE_SEL_MASK       |  0xE7 |
| LTC2946_CTRLA_CHANNEL_CONFIG_MASK    |  0xF8 |
| LTC2946_CTRLB_ACC_MASK               |  0xF3 |
| LTC2946_CTRLB_RESET_MASK             |  0xFC |
| LTC2946_GPIOCFG_GPIO1_MASK           |  0x3F |
| LTC2946_GPIOCFG_GPIO2_MASK           |  0xCF |
| LTC2946_GPIOCFG_GPIO3_MASK           |  0xF3 |
| LTC2946_GPIOCFG_GPIO2_OUT_MASK       |  0xFD |
| LTC2946_GPIO3_CTRL_GPIO3_MASK        |  0xBF |

*/

/*! @name Register Mask Command
@{ */
// Register Mask Command
#define LTC2946_CTRLA_ADIN_MASK                0x7F
#define LTC2946_CTRLA_OFFSET_MASK              0x9F
#define LTC2946_CTRLA_VOLTAGE_SEL_MASK         0xE7
#define LTC2946_CTRLA_CHANNEL_CONFIG_MASK      0xF8
#define LTC2946_CTRLB_ACC_MASK                 0xF3
#define LTC2946_CTRLB_RESET_MASK               0xFC
#define LTC2946_GPIOCFG_GPIO1_MASK             0x3F
#define LTC2946_GPIOCFG_GPIO2_MASK             0xCF
#define LTC2946_GPIOCFG_GPIO3_MASK             0xF3
#define LTC2946_GPIOCFG_GPIO2_OUT_MASK         0xFD
#define LTC2946_GPIO3_CTRL_GPIO3_MASK          0xBF
//! @}


//! Write an 8-bit code to the LTC2946.
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2946_write(uint8_t i2c_address, //!< Register address for the LTC2946
                     uint8_t adc_command, //!< The "command byte" for the LTC2946
                     uint8_t code         //!< Value that will be written to the register.
                    );
//! Write a 16-bit code to the LTC2946.
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2946_write_16_bits(uint8_t i2c_address, //!< Register address for the LTC2946
                             uint8_t adc_command, //!< The "command byte" for the LTC2946
                             uint16_t code        //!< Value that will be written to the register.
                            );

//! Write a 24-bit code to the LTC2946.
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2946_write_24_bits(uint8_t i2c_address, //!< Register address for the LTC2946
                             uint8_t adc_command, //!< The "command byte" for the LTC2946
                             uint32_t code         //!< Value that will be written to the register.
                            );
//! Write a 32-bit code to the LTC2946.
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2946_write_32_bits(uint8_t i2c_address, //!< Register address for the LTC2946
                             uint8_t adc_command, //!< The "command byte" for the LTC2946
                             uint32_t code         //!< Value that will be written to the register.
                            );

//! Reads an 8-bit adc_code from LTC2946
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2946_read(uint8_t i2c_address, //!< Register address for the LTC2946
                    uint8_t adc_command, //!< The "command byte" for the LTC2946
                    uint8_t *adc_code    //!< Value that will be read from the register.
                   );
//! Reads a 12-bit adc_code from LTC2946
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2946_read_12_bits(uint8_t i2c_address, //!< Register address for the LTC2946
                            uint8_t adc_command, //!< The "command byte" for the LTC2946
                            uint16_t *adc_code   //!< Value that will be read from the register.
                           );
//! Reads a 16-bit adc_code from LTC2946
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2946_read_16_bits(uint8_t i2c_address, //!< Register address for the LTC2946
                            uint8_t adc_command, //!< The "command byte" for the LTC2946
                            uint16_t *adc_code   //!< Value that will be read from the register.
                           );
//! Reads a 24-bit adc_code from LTC2946
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2946_read_24_bits(uint8_t i2c_address, //!< Register address for the LTC2946
                            uint8_t adc_command, //!< The "command byte" for the LTC2946
                            uint32_t *adc_code    //!< Value that will be read from the register.
                           );
//! Reads a 32-bit adc_code from LTC2946
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2946_read_32_bits(uint8_t i2c_address, //!< Register address for the LTC2946
                            uint8_t adc_command, //!< The "command byte" for the LTC2946
                            uint32_t *adc_code    //!< Value that will be read from the register.
                           );

//! Calculate the LTC2946 VIN voltage
//! @return Returns the VIN Voltage in Volts
float LTC2946_VIN_code_to_voltage(uint16_t adc_code,        //!< The ADC value
                                  float LTC2946_VIN_lsb     //!< VIN lsb weight
                                 );
//! Calculate the LTC2946 ADIN voltage
//! @return Returns the ADIN Voltage in Volts
float LTC2946_ADIN_code_to_voltage(uint16_t adc_code,       //!< The ADC value
                                   float LTC2946_ADIN_lsb   //!< ADIN lsb weight
                                  );
//! Calculate the LTC2946 current with a sense resistor
//! @return The LTC2946 current in Amps
float LTC2946_code_to_current(uint16_t adc_code,                //!< The ADC value
                              float resistor,                   //!< The resistor value
                              float LTC2946_DELTA_SENSE_lsb     //!< Delta sense lsb weight
                             );
//! Calculate the LTC2946 power
//! @return The LTC2946 power in Watts
float LTC2946_code_to_power(int32_t adc_code,           //!< The ADC value
                            float resistor,       //!< The resistor value
                            float LTC2946_Power_lsb);     //!< Power lsb weight

//! Calculate the LTC2946 energy
//! @return The LTC2946 energy in Joules
float LTC2946_code_to_energy(int32_t adc_code,                      //!< The ADC value
                             float resistor,          //!< The resistor value
                             float LTC2946_Power_lsb,         //!< Power lsb weight
                             float LTC2946_TIME_lsb             //!< Time lsb weight
                            );

//! Calculate the LTC2946 coulombs
//! @return The LTC2946 charge in coulombs
float LTC2946_code_to_coulombs(int32_t adc_code,                      //!< The ADC value
                               float resistor,                      //!< The resistor value
                               float LTC2946_DELTA_SENSE_lsb,       //!< Delta sense lsb weight
                               float LTC2946_Time_lsb              //!< Time lsb weight
                              );
//! Calculate the LTC2946 internal time base
//! @return The internal time base in seconds.
float LTC2946_code_to_time(float time_code,             //!< Time adc code
                           float LTC2946_Time_lsb         //!< Time lsb weight
                          );


#endif  // LTC2946_H
