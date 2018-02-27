/*!
 LTC2992: Dual Wide Range Power Monitor

@verbatim

The LTC®2992 is a rail-to-rail system monitor that measures
current, voltage, power, charge and energy. It features an
operating range of 2.7V to 100V and includes a shunt regulator
for supplies above 100V. The current measurement common mode
range of 0V to 100V is independent of the input supply.
A 12-bit ADC measures load current, input voltage and an
auxiliary external voltage. Load current and internally
calculated power are integrated over an external clock or
crystal or internal oscillator time base for charge and energy.
An accurate time base allows the LTC2992 to provide measurement
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

Read power, current and voltage

    CTRLA = LTC2992_CHANNEL_CONFIG_V_C_3|LTC2992_SENSE_PLUS|LTC2992_OFFSET_CAL_EVERY|LTC2992_ADIN_GND;  //! Set Control A register to default value in continuous mode
    ack |= LTC2992_write(LTC2992_I2C_ADDRESS, LTC2992_CTRLA_REG, CTRLA);   //! Sets the LTC2992 to continuous mode

    resistor = .02; // Resistor Value On Demo Board

    ack |= LTC2992_read_24_bits(LTC2992_I2C_ADDRESS, LTC2992_POWER_MSB2_REG, &power_code);  // Reads the ADC registers that contains V^2
    power = LTC2992_code_to_power(power_code, resistor, LTC2992_Power_lsb); // Calculates power from power code, resistor value and power lsb

    ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_DELTA_SENSE_MSB_REG, &current_code); // Reads the voltage code across sense resistor
    current = LTC2992_code_to_current(current_code, resistor, LTC2992_DELTA_SENSE_lsb); // Calculates current from current code, resistor value and current lsb

    ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_SENSE_MSB_REG, &SENSE_code);   // Reads SENSE voltage code
    SENSE = LTC2992_SENSE_code_to_voltage(SENSE_code, LTC2992_SENSE_lsb);  // Calculates SENSE voltage from SENSE code and lsb

    ack |= LTC2992_read_32_bits(LTC2992_I2C_ADDRESS, LTC2992_ENERGY_MSB3_REG, &energy_code);  // Reads energy code
  energy = LTC2992_code_to_energy(energy_code,resistor,LTC2992_Power_lsb, LTC2992_INTERNAL_TIME_lsb); //Calculates Energy in Joules from energy_code, resistor, power lsb and time lsb

  ack |= LTC2992_read_32_bits(LTC2992_I2C_ADDRESS, LTC2992_CHARGE_MSB3_REG, &charge_code);  // Reads charge code
    charge = LTC2992_code_to_coulombs(charge_code,resistor,LTC2992_DELTA_SENSE_lsb, LTC2992_INTERNAL_TIME_lsb); //Calculates charge in coulombs from charge_code, resistor, current lsb and time lsb



@endverbatim

http://www.linear.com/product/LTC2992

http://www.linear.com/product/LTC2992#demoboards


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
    @ingroup LTC2992
    Header for LTC2992: Dual Wide Range Power Monitor
*/

#ifndef LTC2992_H
#define LTC2992_H

#include <Wire.h>


//! Use table to select address
/*!
| LTC2992 I2C Address Assignment    | Value |   AD1    |   AD2    |
| :-------------------------------- | :---: | :------: | :------: |
| LTC2992_I2C_ADDRESS               | 0x67  |   High   |   Low    |
| LTC2992_I2C_ADDRESS               | 0x68  |   Float  |   High   |
| LTC2992_I2C_ADDRESS               | 0x69  |   High   |   High   |
| LTC2992_I2C_ADDRESS               | 0x6A  |   Float  |   Float  |
| LTC2992_I2C_ADDRESS               | 0x6B  |   Float  |   Low    |
| LTC2992_I2C_ADDRESS               | 0x6C  |   Low    |   High   |
| LTC2992_I2C_ADDRESS               | 0x6D  |   High   |   Float  |
| LTC2992_I2C_ADDRESS               | 0x6E  |   Low    |   Float  |
| LTC2992_I2C_ADDRESS               | 0x6F  |   Low    |   Low    |
|                                   |       |          |          |
| LTC2992_I2C_MASS_WRITE            | 0x66  | X        | X        |
| LTC2992_I2C_ALERT_RESPONSE        | 0x0C  | X        | X        |
*/
/*! @name LTC2992 I2C Address Assignments
@{ */
// Address Choices:
// To choose an address, comment out all options except the
// configuration on the demo board.

//  Address assignment
// LTC2992 I2C Address                 //  AD1       AD0
// #define LTC2992_I2C_ADDRESS 0x67    //  High      Low
// #define LTC2992_I2C_ADDRESS 0x68    //  Float     High
// #define LTC2992_I2C_ADDRESS 0x69    //  High      High
#define LTC2992_I2C_ADDRESS 0x6A    //  Float     Float
// #define LTC2992_I2C_ADDRESS 0x6B    //  Float     Low
// #define LTC2992_I2C_ADDRESS 0x6C    //  Low       High
// #define LTC2992_I2C_ADDRESS 0x6D    //  High      Float
// #define LTC2992_I2C_ADDRESS 0x6E    //  Low       Float
//#define LTC2992_I2C_ADDRESS 0x6F       //  Low       Low

#define LTC2992_I2C_MASS_WRITE      0x66
#define LTC2992_I2C_ALERT_RESPONSE  0x0C
//! @}


/*!
| Name                                              | Value |
| :------------------------------------------------ | :---: |
| LTC2992_CTRLA_REG                                 | 0x00  |
| LTC2992_CTRLB_REG                                 | 0x01  |
| LTC2992_ALERT1_REG                                | 0x02  |
| LTC2992_FAULT1_REG                                | 0x03  |
| LTC2992_NADC_REG                                  | 0x04  |
| LTC2992_POWER1_MSB2_REG                           | 0x05  |
| LTC2992_POWER1_MSB1_REG                           | 0x06  |
| LTC2992_POWER1_LSB_REG                            | 0x07  |
| LTC2992_MAX_POWER1_MSB2_REG                       | 0x08  |
| LTC2992_MAX_POWER1_MSB1_REG                       | 0x09  |
| LTC2992_MAX_POWER1_LSB_REG                        | 0x0A  |
| LTC2992_MIN_POWER1_MSB2_REG                       | 0x0B  |
| LTC2992_MIN_POWER1_MSB1_REG                       | 0x0C  |
| LTC2992_MIN_POWER1_LSB_REG                        | 0x0D  |
| LTC2992_MAX_POWER1_THRESHOLD_MSB2_REG             | 0x0E  |
| LTC2992_MAX_POWER1_THRESHOLD_MSB1_REG             | 0x0F  |
| LTC2992_MAX_POWER1_THRESHOLD_LSB_REG              | 0x10  |
| LTC2992_MIN_POWER1_THRESHOLD_MSB2_REG             | 0x11  |
| LTC2992_MIN_POWER1_THRESHOLD_MSB1_REG             | 0x12  |
| LTC2992_MIN_POWER1_THRESHOLD_LSB_REG              | 0x13  |
| LTC2992_DELTA_SENSE1_MSB_REG                      | 0x14  |
| LTC2992_DELTA_SENSE1_LSB_REG                      | 0x15  |
| LTC2992_MAX_DELTA1_SENSE_MSB_REG                  | 0x16  |
| LTC2992_MAX_DELTA1_SENSE_LSB_REG                  | 0x17  |
| LTC2992_MIN_DELTA1_SENSE_MSB_REG                  | 0x18  |
| LTC2992_MIN_DELTA1_SENSE_LSB_REG                  | 0x19  |
| LTC2992_MAX_DELTA1_SENSE_THRESHOLD_MSB_REG        | 0x1A  |
| LTC2992_MAX_DELTA1_SENSE_THRESHOLD_LSB_REG        | 0x1B  |
| LTC2992_MIN_DELTA1_SENSE_THRESHOLD_MSB_REG        | 0x1C  |
| LTC2992_MIN_DELTA1_SENSE_THRESHOLD_LSB_REG        | 0x1D  |
| LTC2992_SENSE1_MSB_REG                            | 0x1E  |
| LTC2992_SENSE1_LSB_REG                            | 0x1F  |
| LTC2992_MAX_SENSE1_MSB_REG                        | 0x20  |
| LTC2992_MAX_SENSE1_LSB_REG                        | 0x21  |
| LTC2992_MIN_SENSE1_MSB_REG                        | 0x22  |
| LTC2992_MIN_SENSE1_LSB_REG                        | 0x23  |
| LTC2992_MAX_SENSE1_THRESHOLD_MSB_REG              | 0x24  |
| LTC2992_MAX_SENSE1_THRESHOLD_LSB_REG              | 0x25  |
| LTC2992_MIN_SENSE1_THRESHOLD_MSB_REG              | 0x26  |
| LTC2992_MIN_SENSE1_THRESHOLD_LSB_REG              | 0x27  |
| LTC2992_GPIO1_MSB_REG                             | 0x28  |
| LTC2992_GPIO1_LSB_REG_REG                         | 0x29  |
| LTC2992_MAX_GPIO1_MSB_REG                         | 0x2A  |
| LTC2992_MAX_GPIO1_LSB_REG                         | 0x2B  |
| LTC2992_MIN_GPIO1_MSB_REG                         | 0x2C  |
| LTC2992_MIN_GPIO1_LSB_REG                         | 0x2D  |
| LTC2992_MAX_GPIO1_THRESHOLD_MSB_REG               | 0x2E  |
| LTC2992_MAX_GPIO1_THRESHOLD_LSB_REG               | 0x2F  |
| LTC2992_MIN_GPIO1_THRESHOLD_MSB_REG               | 0x30  |
| LTC2992_MIN_GPIO1_THRESHOLD_LSB_REG               | 0x31  |
| LTC2992_ADC_STATUS_REG                            | 0x32  |
| LTC2992_ALERT2_REG                                | 0x34  |
| LTC2992_FAULT2_REG                                | 0x35  |
| LTC2992_POWER2_MSB2_REG                           | 0x37  |
| LTC2992_POWER2_MSB1_REG                           | 0x38  |
| LTC2992_POWER2_LSB_REG                            | 0x39  |
| LTC2992_MAX_POWER2_MSB2_REG                       | 0x3A  |
| LTC2992_MAX_POWER2_MSB1_REG                       | 0x3B  |
| LTC2992_MAX_POWER2_LSB_REG                        | 0x3C  |
| LTC2992_MIN_POWER2_MSB2_REG                       | 0x3D  |
| LTC2992_MIN_POWER2_MSB1_REG                       | 0x3E  |
| LTC2992_MIN_POWER2_LSB_REG                        | 0x3F  |
| LTC2992_MAX_POWER2_THRESHOLD_MSB2_REG             | 0x40  |
| LTC2992_MAX_POWER2_THRESHOLD_MSB1_REG             | 0x41  |
| LTC2992_MAX_POWER2_THRESHOLD_LSB_REG              | 0x42  |
| LTC2992_MIN_POWER2_THRESHOLD_MSB2_REG             | 0x43  |
| LTC2992_MIN_POWER2_THRESHOLD_MSB1_REG             | 0x44  |
| LTC2992_MIN_POWER2_THRESHOLD_LSB_REG              | 0x45  |
| LTC2992_DELTA_SENSE2_MSB_REG                      | 0x46  |
| LTC2992_DELTA_SENSE2_LSB_REG                      | 0x47  |
| LTC2992_MAX_DELTA2_SENSE_MSB_REG                  | 0x48  |
| LTC2992_MAX_DELTA2_SENSE_LSB_REG                  | 0x49  |
| LTC2992_MIN_DELTA2_SENSE_MSB_REG                  | 0x4A  |
| LTC2992_MIN_DELTA2_SENSE_LSB_REG                  | 0x4B  |
| LTC2992_MAX_DELTA2_SENSE_THRESHOLD_MSB_REG        | 0x4C  |
| LTC2992_MAX_DELTA2_SENSE_THRESHOLD_LSB_REG        | 0x4D  |
| LTC2992_MIN_DELTA2_SENSE_THRESHOLD_MSB_REG        | 0x4E  |
| LTC2992_MIN_DELTA2_SENSE_THRESHOLD_LSB_REG        | 0x4F  |
| LTC2992_SENSE2_MSB_REG                            | 0x50  |
| LTC2992_SENSE2_LSB_REG                            | 0x51  |
| LTC2992_MAX_SENSE2_MSB_REG                        | 0x52  |
| LTC2992_MAX_SENSE2_LSB_REG                        | 0x53  |
| LTC2992_MIN_SENSE2_MSB_REG                        | 0x54  |
| LTC2992_MIN_SENSE2_LSB_REG                        | 0x55  |
| LTC2992_MAX_SENSE2_THRESHOLD_MSB_REG              | 0x56  |
| LTC2992_MAX_SENSE2_THRESHOLD_LSB_REG              | 0x57  |
| LTC2992_MIN_SENSE2_THRESHOLD_MSB_REG              | 0x58  |
| LTC2992_MIN_SENSE2_THRESHOLD_LSB_REG              | 0x59  |
| LTC2992_GPIO2_MSB_REG                             | 0x5A  |
| LTC2992_GPIO2_LSB_REG_REG                         | 0x5B  |
| LTC2992_MAX_GPIO2_MSB_REG                         | 0x5C  |
| LTC2992_MAX_GPIO2_LSB_REG                         | 0x5D  |
| LTC2992_MIN_GPIO2_MSB_REG                         | 0x5E  |
| LTC2992_MIN_GPIO2_LSB_REG                         | 0x5F  |
| LTC2992_MAX_GPIO2_THRESHOLD_MSB_REG               | 0x60  |
| LTC2992_MAX_GPIO2_THRESHOLD_LSB_REG               | 0x61  |
| LTC2992_MIN_GPIO2_THRESHOLD_MSB_REG               | 0x62  |
| LTC2992_MIN_GPIO2_THRESHOLD_LSB_REG               | 0x63  |
| LTC2992_GPIO3_MSB_REG                             | 0x64  |
| LTC2992_GPIO3_LSB_REG_REG                         | 0x65  |
| LTC2992_MAX_GPIO3_MSB_REG                         | 0x66  |
| LTC2992_MAX_GPIO3_LSB_REG                         | 0x67  |
| LTC2992_MIN_GPIO3_MSB_REG                         | 0x68  |
| LTC2992_MIN_GPIO3_LSB_REG                         | 0x69  |
| LTC2992_MAX_GPIO3_THRESHOLD_MSB_REG               | 0x6A  |
| LTC2992_MAX_GPIO3_THRESHOLD_LSB_REG               | 0x6B  |
| LTC2992_MIN_GPIO3_THRESHOLD_MSB_REG               | 0x6C  |
| LTC2992_MIN_GPIO3_THRESHOLD_LSB_REG               | 0x6D  |
| LTC2992_GPIO4_MSB_REG                             | 0x6E  |
| LTC2992_GPIO4_LSB_REG_REG                         | 0x6F  |
| LTC2992_MAX_GPIO4_MSB_REG                         | 0x70  |
| LTC2992_MAX_GPIO4_LSB_REG                         | 0x71  |
| LTC2992_MIN_GPIO4_MSB_REG                         | 0x72  |
| LTC2992_MIN_GPIO4_LSB_REG                         | 0x73  |
| LTC2992_MAX_GPIO4_THRESHOLD_MSB_REG               | 0x74  |
| LTC2992_MAX_GPIO4_THRESHOLD_LSB_REG               | 0x75  |
| LTC2992_MIN_GPIO4_THRESHOLD_MSB_REG               | 0x76  |
| LTC2992_MIN_GPIO4_THRESHOLD_LSB_REG               | 0x77  |
| LTC2992_ISUM_MSB_REG                              | 0x78  |
| LTC2992_ISUM_LSB_REG_REG                          | 0x79  |
| LTC2992_MAX_ISUM_MSB_REG                          | 0x7A  |
| LTC2992_MAX_ISUM_LSB_REG                          | 0x7B  |
| LTC2992_MIN_ISUM_MSB_REG                          | 0x7C  |
| LTC2992_MIN_ISUM_LSB_REG                          | 0x7D  |
| LTC2992_MAX_ISUM_THRESHOLD_MSB_REG                | 0x7E  |
| LTC2992_MAX_ISUM_THRESHOLD_LSB_REG                | 0x7F  |
| LTC2992_MIN_ISUM_THRESHOLD_MSB_REG                | 0x80  |
| LTC2992_MIN_ISUM_THRESHOLD_LSB_REG                | 0x81  |
| LTC2992_PSUM_MSB1_REG                             | 0x82  |
| LTC2992_PSUM_MSB_REG                              | 0x83  |
| LTC2992_PSUM_LSB_REG_REG                          | 0x84  |
| LTC2992_MAX_PSUM_MSB1_REG                         | 0x85  |
| LTC2992_MAX_PSUM_MSB_REG                          | 0x86  |
| LTC2992_MAX_PSUM_LSB_REG                          | 0x87  |
| LTC2992_MIN_PSUM_MSB1_REG                         | 0x88  |
| LTC2992_MIN_PSUM_MSB_REG                          | 0x89  |
| LTC2992_MIN_PSUM_LSB_REG                          | 0x8A  |
| LTC2992_MAX_PSUM_THRESHOLD_MSB1_REG               | 0x8B  |
| LTC2992_MAX_PSUM_THRESHOLD_MSB_REG                | 0x8C  |
| LTC2992_MAX_PSUM_THRESHOLD_LSB_REG                | 0x8D  |
| LTC2992_MIN_PSUM_THRESHOLD_MSB1_REG               | 0x8E  |
| LTC2992_MIN_PSUM_THRESHOLD_MSB_REG                | 0x8F  |
| LTC2992_MIN_PSUM_THRESHOLD_LSB_REG                | 0x90  |
| LTC2992_ALERT3_REG                                | 0x91  |
| LTC2992_FAULT3_REG                                | 0x92  |
| LTC2992_ALERT4_REG                                | 0x93  |
| LTC2992_FAULT4_REG                                | 0x94  |
| LTC2992_GPIO_STATUS_REG                           | 0x95  |
| LTC2992_GPIO_IO_CONT_REG                          | 0x96  |
| LTC2992_GPIO4_CFG_REG                             | 0x97  |
  */




/*! @name Registers
@{ */
// Registers

#define LTC2992_CTRLA_REG                                  0x00
#define LTC2992_CTRLB_REG                                  0x01
#define LTC2992_ALERT1_REG                                 0x02
#define LTC2992_FAULT1_REG                                 0x03
#define LTC2992_NADC_REG                                   0x04
#define LTC2992_POWER1_MSB2_REG                            0x05
#define LTC2992_POWER1_MSB1_REG                            0x06
#define LTC2992_POWER1_LSB_REG                             0x07
#define LTC2992_MAX_POWER1_MSB2_REG                        0x08
#define LTC2992_MAX_POWER1_MSB1_REG                        0x09
#define LTC2992_MAX_POWER1_LSB_REG                         0x0A
#define LTC2992_MIN_POWER1_MSB2_REG                        0x0B
#define LTC2992_MIN_POWER1_MSB1_REG                        0x0C
#define LTC2992_MIN_POWER1_LSB_REG                         0x0D
#define LTC2992_MAX_POWER1_THRESHOLD_MSB2_REG              0x0E
#define LTC2992_MAX_POWER1_THRESHOLD_MSB1_REG              0x0F
#define LTC2992_MAX_POWER1_THRESHOLD_LSB_REG               0x10
#define LTC2992_MIN_POWER1_THRESHOLD_MSB2_REG              0x11
#define LTC2992_MIN_POWER1_THRESHOLD_MSB1_REG              0x12
#define LTC2992_MIN_POWER1_THRESHOLD_LSB_REG               0x13
#define LTC2992_DELTA_SENSE1_MSB_REG                       0x14
#define LTC2992_DELTA_SENSE1_LSB_REG                       0x15
#define LTC2992_MAX_DELTA1_SENSE_MSB_REG                   0x16
#define LTC2992_MAX_DELTA1_SENSE_LSB_REG                   0x17
#define LTC2992_MIN_DELTA1_SENSE_MSB_REG                   0x18
#define LTC2992_MIN_DELTA1_SENSE_LSB_REG                   0x19
#define LTC2992_MAX_DELTA1_SENSE_THRESHOLD_MSB_REG         0x1A
#define LTC2992_MAX_DELTA1_SENSE_THRESHOLD_LSB_REG         0x1B
#define LTC2992_MIN_DELTA1_SENSE_THRESHOLD_MSB_REG         0x1C
#define LTC2992_MIN_DELTA1_SENSE_THRESHOLD_LSB_REG         0x1D
#define LTC2992_SENSE1_MSB_REG                             0x1E
#define LTC2992_SENSE1_LSB_REG                             0x1F
#define LTC2992_MAX_SENSE1_MSB_REG                         0x20
#define LTC2992_MAX_SENSE1_LSB_REG                         0x21
#define LTC2992_MIN_SENSE1_MSB_REG                         0x22
#define LTC2992_MIN_SENSE1_LSB_REG                         0x23
#define LTC2992_MAX_SENSE1_THRESHOLD_MSB_REG               0x24
#define LTC2992_MAX_SENSE1_THRESHOLD_LSB_REG               0x25
#define LTC2992_MIN_SENSE1_THRESHOLD_MSB_REG               0x26
#define LTC2992_MIN_SENSE1_THRESHOLD_LSB_REG               0x27
#define LTC2992_GPIO1_MSB_REG                              0x28
#define LTC2992_GPIO1_LSB_REG_REG                          0x29
#define LTC2992_MAX_GPIO1_MSB_REG                          0x2A
#define LTC2992_MAX_GPIO1_LSB_REG                          0x2B
#define LTC2992_MIN_GPIO1_MSB_REG                          0x2C
#define LTC2992_MIN_GPIO1_LSB_REG                          0x2D
#define LTC2992_MAX_GPIO1_THRESHOLD_MSB_REG                0x2E
#define LTC2992_MAX_GPIO1_THRESHOLD_LSB_REG                0x2F
#define LTC2992_MIN_GPIO1_THRESHOLD_MSB_REG                0x30
#define LTC2992_MIN_GPIO1_THRESHOLD_LSB_REG                0x31
#define LTC2992_ADC_STATUS_REG                             0x32
#define LTC2992_ALERT2_REG                                 0x34
#define LTC2992_FAULT2_REG                                 0x35
#define LTC2992_POWER2_MSB2_REG                            0x37
#define LTC2992_POWER2_MSB1_REG                            0x38
#define LTC2992_POWER2_LSB_REG                             0x39
#define LTC2992_MAX_POWER2_MSB2_REG                        0x3A
#define LTC2992_MAX_POWER2_MSB1_REG                        0x3B
#define LTC2992_MAX_POWER2_LSB_REG                         0x3C
#define LTC2992_MIN_POWER2_MSB2_REG                        0x3D
#define LTC2992_MIN_POWER2_MSB1_REG                        0x3E
#define LTC2992_MIN_POWER2_LSB_REG                         0x3F
#define LTC2992_MAX_POWER2_THRESHOLD_MSB2_REG              0x40
#define LTC2992_MAX_POWER2_THRESHOLD_MSB1_REG              0x41
#define LTC2992_MAX_POWER2_THRESHOLD_LSB_REG               0x42
#define LTC2992_MIN_POWER2_THRESHOLD_MSB2_REG              0x43
#define LTC2992_MIN_POWER2_THRESHOLD_MSB1_REG              0x44
#define LTC2992_MIN_POWER2_THRESHOLD_LSB_REG               0x45
#define LTC2992_DELTA_SENSE2_MSB_REG                       0x46
#define LTC2992_DELTA_SENSE2_LSB_REG                       0x47
#define LTC2992_MAX_DELTA2_SENSE_MSB_REG                   0x48
#define LTC2992_MAX_DELTA2_SENSE_LSB_REG                   0x49
#define LTC2992_MIN_DELTA2_SENSE_MSB_REG                   0x4A
#define LTC2992_MIN_DELTA2_SENSE_LSB_REG                   0x4B
#define LTC2992_MAX_DELTA2_SENSE_THRESHOLD_MSB_REG         0x4C
#define LTC2992_MAX_DELTA2_SENSE_THRESHOLD_LSB_REG         0x4D
#define LTC2992_MIN_DELTA2_SENSE_THRESHOLD_MSB_REG         0x4E
#define LTC2992_MIN_DELTA2_SENSE_THRESHOLD_LSB_REG         0x4F
#define LTC2992_SENSE2_MSB_REG                             0x50
#define LTC2992_SENSE2_LSB_REG                             0x51
#define LTC2992_MAX_SENSE2_MSB_REG                         0x52
#define LTC2992_MAX_SENSE2_LSB_REG                         0x53
#define LTC2992_MIN_SENSE2_MSB_REG                         0x54
#define LTC2992_MIN_SENSE2_LSB_REG                         0x55
#define LTC2992_MAX_SENSE2_THRESHOLD_MSB_REG               0x56
#define LTC2992_MAX_SENSE2_THRESHOLD_LSB_REG               0x57
#define LTC2992_MIN_SENSE2_THRESHOLD_MSB_REG               0x58
#define LTC2992_MIN_SENSE2_THRESHOLD_LSB_REG               0x59
#define LTC2992_GPIO2_MSB_REG                              0x5A
#define LTC2992_GPIO2_LSB_REG_REG                          0x5B
#define LTC2992_MAX_GPIO2_MSB_REG                          0x5C
#define LTC2992_MAX_GPIO2_LSB_REG                          0x5D
#define LTC2992_MIN_GPIO2_MSB_REG                          0x5E
#define LTC2992_MIN_GPIO2_LSB_REG                          0x5F
#define LTC2992_MAX_GPIO2_THRESHOLD_MSB_REG                0x60
#define LTC2992_MAX_GPIO2_THRESHOLD_LSB_REG                0x61
#define LTC2992_MIN_GPIO2_THRESHOLD_MSB_REG                0x62
#define LTC2992_MIN_GPIO2_THRESHOLD_LSB_REG                0x63
#define LTC2992_GPIO3_MSB_REG                              0x64
#define LTC2992_GPIO3_LSB_REG_REG                          0x65
#define LTC2992_MAX_GPIO3_MSB_REG                          0x66
#define LTC2992_MAX_GPIO3_LSB_REG                          0x67
#define LTC2992_MIN_GPIO3_MSB_REG                          0x68
#define LTC2992_MIN_GPIO3_LSB_REG                          0x69
#define LTC2992_MAX_GPIO3_THRESHOLD_MSB_REG                0x6A
#define LTC2992_MAX_GPIO3_THRESHOLD_LSB_REG                0x6B
#define LTC2992_MIN_GPIO3_THRESHOLD_MSB_REG                0x6C
#define LTC2992_MIN_GPIO3_THRESHOLD_LSB_REG                0x6D
#define LTC2992_GPIO4_MSB_REG                              0x6E
#define LTC2992_GPIO4_LSB_REG_REG                          0x6F
#define LTC2992_MAX_GPIO4_MSB_REG                          0x70
#define LTC2992_MAX_GPIO4_LSB_REG                          0x71
#define LTC2992_MIN_GPIO4_MSB_REG                          0x72
#define LTC2992_MIN_GPIO4_LSB_REG                          0x73
#define LTC2992_MAX_GPIO4_THRESHOLD_MSB_REG                0x74
#define LTC2992_MAX_GPIO4_THRESHOLD_LSB_REG                0x75
#define LTC2992_MIN_GPIO4_THRESHOLD_MSB_REG                0x76
#define LTC2992_MIN_GPIO4_THRESHOLD_LSB_REG                0x77
#define LTC2992_ISUM_MSB_REG                               0x78
#define LTC2992_ISUM_LSB_REG_REG                           0x79
#define LTC2992_MAX_ISUM_MSB_REG                           0x7A
#define LTC2992_MAX_ISUM_LSB_REG                           0x7B
#define LTC2992_MIN_ISUM_MSB_REG                           0x7C
#define LTC2992_MIN_ISUM_LSB_REG                           0x7D
#define LTC2992_MAX_ISUM_THRESHOLD_MSB_REG                 0x7E
#define LTC2992_MAX_ISUM_THRESHOLD_LSB_REG                 0x7F
#define LTC2992_MIN_ISUM_THRESHOLD_MSB_REG                 0x80
#define LTC2992_MIN_ISUM_THRESHOLD_LSB_REG                 0x81
#define LTC2992_PSUM_MSB1_REG                              0x82
#define LTC2992_PSUM_MSB_REG                               0x83
#define LTC2992_PSUM_LSB_REG_REG                           0x84
#define LTC2992_MAX_PSUM_MSB1_REG                          0x85
#define LTC2992_MAX_PSUM_MSB_REG                           0x86
#define LTC2992_MAX_PSUM_LSB_REG                           0x87
#define LTC2992_MIN_PSUM_MSB1_REG                          0x88
#define LTC2992_MIN_PSUM_MSB_REG                           0x89
#define LTC2992_MIN_PSUM_LSB_REG                           0x8A
#define LTC2992_MAX_PSUM_THRESHOLD_MSB1_REG                0x8B
#define LTC2992_MAX_PSUM_THRESHOLD_MSB_REG                 0x8C
#define LTC2992_MAX_PSUM_THRESHOLD_LSB_REG                 0x8D
#define LTC2992_MIN_PSUM_THRESHOLD_MSB1_REG                0x8E
#define LTC2992_MIN_PSUM_THRESHOLD_MSB_REG                 0x8F
#define LTC2992_MIN_PSUM_THRESHOLD_LSB_REG                 0x90
#define LTC2992_ALERT3_REG                                 0x91
#define LTC2992_FAULT3_REG                                 0x92
#define LTC2992_ALERT4_REG                                 0x93
#define LTC2992_FAULT4_REG                                 0x94
#define LTC2992_GPIO_STATUS_REG                            0x95
#define LTC2992_GPIO_IO_CONT_REG                           0x96
#define LTC2992_GPIO4_CFG_REG                              0x97


//! @}

/*! @name Command Codes
@{ */
// Command Codes

#define LTC2992_OFFSET_CAL_DEMAND                          0x80
#define LTC2992_OFFSET_CAL_EVERY                           0x00

#define LTC2992_MODE_SHUTDOWN                              0x60
#define LTC2992_MODE_SINGLE_CYCLE                          0x40
#define LTC2992_MODE_SNAPSHOT                0x20
#define LTC2992_MODE_CONTINUOUS                            0x00

#define LTC2992_CONTINUOUS_GPIO_1_2_3_4                     0x18
#define LTC2992_CONTINUOUS_GPIO_1_2                         0x10
#define LTC2992_CONTINUOUS_SENSE_1_2            0x08
#define LTC2992_CONTINUOUS_SENSE_1_2_GPIO_1_2_3_4           0x00

#define LTC2992_SNAPSHOT_GPIO_1_2                           0x07
#define LTC2992_SNAPSHOT_SENSE_1_2                          0x06
#define LTC2992_SNAPSHOT_GPIO_4                             0x05
#define LTC2992_SNAPSHOT_GPIO_3                             0x04
#define LTC2992_SNAPSHOT_GPIO_2                             0x03
#define LTC2992_SNAPSHOT_GPIO_1                             0x02
#define LTC2992_SNAPSHOT_SENSE_2                            0x01
#define LTC2992_SNAPSHOT_SENSE_1                            0x00

#define LTC2992_ENABLE_ALERT_CLEAR              0x80
#define LTC2992_ENABLE_CLEARED_ON_READ          0x20
#define LTC2992_ENABLE_STUCK_BUS_RECOVER        0x10
#define LTC2992_ENABLE_RESET_PEAK_VALUES        0x08
#define LTC2992_ENABLE_RESET_ALL                0x01

#define LTC2992_DISABLE_ALERT_CLEAR             0x7F
#define LTC2992_DISABLE_CLEARED_ON_READ         0xDF
#define LTC2992_DISABLE_STUCK_BUS_RECOVER       0xEF
#define LTC2992 DISABLE_RESET_PEAK_VALUES       0xF7
#define LTC2992_DISABLE_RESET_ALL               0xFE

#define LTC2992_ENABLE_MAX_POWER1_ALERT          0x80
#define LTC2992_ENABLE_MIN_POWER1_ALERT          0x40
#define LTC2992_DISABLE_MAX_POWER1_ALERT         0x7F
#define LTC2992_DISABLE_MIN_POWER1_ALERT         0xBF

#define LTC2992_ENABLE_MAX_I_SENSE1_ALERT        0x20
#define LTC2992_ENABLE_MIN_I_SENSE1_ALERT        0x10
#define LTC2992_DISABLE_MAX_I_SENSE1_ALERT       0xDF
#define LTC2992_DISABLE_MIN_I_SENSE1_ALERT       0xEF

#define LTC2992_ENABLE_MAX_SENSE1_ALERT            0x08
#define LTC2992_ENABLE_MIN_SENSE1_ALERT            0x04
#define LTC2992_DISABLE_MAX_SENSE1_ALERT           0xF7
#define LTC2992_DISABLE_MIN_SENSE1_ALERT           0xFB

#define LTC2992_ENABLE_MAX_GPIO1_ALERT           0x02
#define LTC2992_ENABLE_MIN_GPIO1_ALERT           0x01
#define LTC2992_DISABLE_MAX_GPIO1_ALERT          0xFD
#define LTC2992_DISABLE_MIN_GPIO1_ALERT          0xFE


//NADC
#define LTC2992_ADC_RESOLUTION                0x80

//ALERT2
#define LTC2992_ENABLE_MAX_POWER2_ALERT          0x80
#define LTC2992_ENABLE_MIN_POWER2_ALERT          0x40
#define LTC2992_DISABLE_MAX_POWER2_ALERT         0x7F
#define LTC2992_DISABLE_MIN_POWER2_ALERT         0xBF

#define LTC2992_ENABLE_MAX_I_SENSE2_ALERT        0x20
#define LTC2992_ENABLE_MIN_I_SENSE2_ALERT        0x10
#define LTC2992_DISABLE_MAX_I_SENSE2_ALERT       0xDF
#define LTC2992_DISABLE_MIN_I_SENSE2_ALERT       0xEF

#define LTC2992_ENABLE_MAX_SENSE2_ALERT            0x08
#define LTC2992_ENABLE_MIN_SENSE2_ALERT            0x04
#define LTC2992_DISABLE_MAX_SENSE2_ALERT           0xF7
#define LTC2992_DISABLE_MIN_SENSE2_ALERT           0xFB

#define LTC2992_ENABLE_MAX_GPIO2_ALERT           0x02
#define LTC2992_ENABLE_MIN_GPIO2_ALERT           0x01
#define LTC2992_DISABLE_MAX_GPIO2_ALERT          0xFD
#define LTC2992_DISABLE_MIN_GPIO2_ALERT          0xFE

//ALERT3
#define LTC2992_ENABLE_MAX_GPIO3_ALERT          0x80
#define LTC2992_ENABLE_MIN_GPIO3_ALERT          0x40
#define LTC2992_DISABLE_MAX_GPIO3_ALERT         0x7F
#define LTC2992_DISABLE_MIN_GPIO3_ALERT         0xBF

#define LTC2992_ENABLE_MAX_GPIO4_ALERT        0x20
#define LTC2992_ENABLE_MIN_GPIO4_ALERT        0x10
#define LTC2992_DISABLE_MAX_GPIO4_ALERT       0xDF
#define LTC2992_DISABLE_MIN_GPIO4_ALERT       0xEF

#define LTC2992_ENABLE_MAX_I_SUM_ALERT            0x08
#define LTC2992_ENABLE_MIN_I_SUM_ALERT            0x04
#define LTC2992_DISABLE_MAX_I_SUM_ALERT           0xF7
#define LTC2992_DISABLE_MIN_I_SUM_ALERT           0xFB

#define LTC2992_ENABLE_MAX_P_SUM_ALERT           0x02
#define LTC2992_ENABLE_MIN_P_SUM_ALERT           0x01
#define LTC2992_DISABLE_MAX_P_SUM_ALERT          0xFD
#define LTC2992_DISABLE_MIN_P_SUM_ALERT          0xFE

//ALERT4
#define LTC2992_ENABLE_V_ADC_READY_ALERT          0x80
#define LTC2992_ENABLE_I_ADC_READY_ALERT          0x40
#define LTC2992_DISABLE_V_ADC_READY_ALERT         0x7F
#define LTC2992_DISABLE_I_ADC_READY_ALERT         0xBF

#define LTC2992_ENABLE_STUCK_BUS_TIMEOUT_ALERT        0x10
#define LTC2992_DISABLE_STUCK_BUS_TIMEOUT_ALERT       0xEF

#define LTC2992_ENABLE_GPIO1_INPUT_ALERT            0x08
#define LTC2992_ENABLE_GPIO2_INPUT_ALERT            0x04
#define LTC2992_DISABLE_GPIO1_INPUT_ALERT           0xF7
#define LTC2992_DISABLE_GPIO2_INPUT_ALERT           0xFB

#define LTC2992_ENABLE_GPIO3_INPUT_ALERT           0x02
#define LTC2992_DISABLE_GPIO3_INPUT_ALERT          0xFD


//GPIO IO Control Registers
#define LTC2992_GPIO1_OUT_HIGH_Z                  0x7F
#define LTC2992_GPIO1_OUT_LOW                     0x80
#define LTC2992_GPIO2_OUT_HIGH_Z                  0xBF
#define LTC2992_GPIO2_OUT_LOW                     0x40

#define LTC2992_GPIO3_CONFIG_LOW_DATARDY          0x30
#define LTC2992_GPIO3_CONFIG_128_LOW              0x20
#define LTC2992_GPIO3_CONFIG_16_LOW               0x10
#define LTC2992_GPIO3_CONFIG_IO                   0x00

#define LTC2992_GPIO1_IN_HIGH_POL_ALERT           0x08
#define LTC2992_GPIO1_IN_LOW_POL_ALERT            0xFC
#define LTC2992_GPIO2_IN_HIGH_POL_ALERT           0x04
#define LTC2992_GPIO2_IN_LOW_POL_ALERT            0xFB
#define LTC2992_GPIO3_IN_HIGH_POL_ALERT           0x02
#define LTC2992_GPIO3_IN_LOW_POL_ALERT            0xFD
#define LTC2992_GPIO3_OUT_LOW                     0x01
#define LTC2992_GPIO3_OUT_HIGH_Z                  0xFE

//GPIO4 Control Reg
#define LTC2992_ALERT_GENERATED_TRUE              0x80
#define LTC2992_ALERT_GENERATED_CLEAR             0x7F
#define LTC2992_GPIO4_OUT_LOW                     0x40
#define LTC2992_GPIO4_OUT_HI_Z                   0xBF
//! @}

/*! @name Register Mask Command
@{ */
// Register Mask Command
#define LTC2992_CTRLA_OFFSET_MASK                      0x7F
#define LTC2992_CTRLA_MEASUREMENT_MODE_MASK            0x9F
#define LTC2992_CTRLA_VOLTAGE_SEL_CONTINIOUS_MASK      0xE7
#define LTC2992_CTRLA_VOLTAGE_SEL_SNAPSHOT_MASK        0xF8
#define LTC2992_CTRLB_ACC_MASK                         0xF3
#define LTC2992_CTRLB_RESET_MASK                       0xFC
#define LTC2992_GPIOCFG_GPIO1_MASK                     0x3F
#define LTC2992_GPIOCFG_GPIO2_MASK                     0xCF
#define LTC2992_GPIOCFG_GPIO3_MASK                     0xCF
#define LTC2992_GPIOCFG_GPIO2_OUT_MASK                 0xFD
#define LTC2992_GPIO3_CTRL_GPIO3_MASK                  0xBF
//! @}

//! Write an 8-bit code to the LTC2992.
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2992_write(uint8_t i2c_address, //!< Register address for the LTC2992
                     uint8_t adc_command, //!< The "command byte" for the LTC2992
                     uint8_t code         //!< Value that will be written to the register.
                    );
//! Write a 16-bit code to the LTC2992.
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2992_write_16_bits(uint8_t i2c_address, //!< Register address for the LTC2992
                             uint8_t adc_command, //!< The "command byte" for the LTC2992
                             uint16_t code        //!< Value that will be written to the register.
                            );

//! Write a 24-bit code to the LTC2992.
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2992_write_24_bits(uint8_t i2c_address, //!< Register address for the LTC2992
                             uint8_t adc_command, //!< The "command byte" for the LTC2992
                             uint32_t code         //!< Value that will be written to the register.
                            );

//! Reads an 8-bit adc_code from LTC2992
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2992_read(uint8_t i2c_address, //!< Register address for the LTC2992
                    uint8_t adc_command, //!< The "command byte" for the LTC2992
                    uint8_t *adc_code    //!< Value that will be read from the register.
                   );
//! Reads a 12-bit adc_code from LTC2992
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2992_read_12_bits(uint8_t i2c_address, //!< Register address for the LTC2992
                            uint8_t adc_command, //!< The "command byte" for the LTC2992
                            uint16_t *adc_code   //!< Value that will be read from the register.
                           );
//! Reads a 16-bit adc_code from LTC2992
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2992_read_16_bits(uint8_t i2c_address, //!< Register address for the LTC2992
                            uint8_t adc_command, //!< The "command byte" for the LTC2992
                            uint16_t *adc_code   //!< Value that will be read from the register.
                           );
//! Reads a 24-bit adc_code from LTC2992
//! @return The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2992_read_24_bits(uint8_t i2c_address, //!< Register address for the LTC2992
                            uint8_t adc_command, //!< The "command byte" for the LTC2992
                            uint32_t *adc_code    //!< Value that will be read from the register.
                           );

//! Calculate the LTC2992 SENSE voltage
//! @return Returns the SENSE Voltage in Volts
float LTC2992_SENSE_code_to_voltage(uint16_t adc_code,        //!< The ADC value
                                    float LTC2992_SENSE_lsb     //!< SENSE lsb weight
                                   );
//! Calculate the LTC2992 GPIO voltage
//! @return Returns the GPIO Voltage in Volts
float LTC2992_GPIO_code_to_voltage(uint16_t adc_code,       //!< The ADC value
                                   float LTC2992_ADIN_lsb   //!< GPIO lsb weight
                                  );
//! Calculate the LTC2992 current with a sense resistor
//! @return The LTC2992 current in Amps
float LTC2992_code_to_current(uint16_t adc_code,                //!< The ADC value
                              float resistor,                   //!< The resistor value
                              float LTC2992_DELTA_SENSE_lsb     //!< Delta sense lsb weight
                             );

//! Calculate the LTC2992 current sum with a sense resistor
//! @return The LTC2992 current in Amps
float LTC2992_code_to_current_sum(uint16_t adc_code,                //!< The ADC value
                                  float resistor,                   //!< The resistor value
                                  float LTC2992_DELTA_SENSE_lsb     //!< Delta sense lsb weight
                                 );

//! Calculate the LTC2992 power
//! @return The LTC2992 power in Watts
float LTC2992_code_to_power(int32_t adc_code,           //!< The ADC value
                            float resistor,       //!< The resistor value
                            float LTC2992_Power_lsb);     //!< Power lsb weight

float LTC2992_code_to_power_sum(int32_t adc_code,             //!< The ADC value
                                float resistor,       //!< The Resistor Value
                                float LTC2992_Power_lsb);    //!< Power LSB Weight

#endif  // LTC2945_H

