/*!
LTC2991: 14-bit ADC octal I2C voltage, current, and temperature monitor.

@verbatim

The LTC2991 is used to monitor system temperatures, voltages and currents.
Through the I2C serial interface, the eight monitors can individually measure
supply voltages and can be paired for differential measurements of current sense
resistors or temperature sensing transistors. Additional measurements include
internal temperature and internal VCC. The internal 10ppm reference minimizes
the number of supporting components and area required. Selectable address and
configurable functionality give the LTC2991 flexibility to be incorporated in
various systems needing temperature, voltage or current data. The LTC2991 fits
well in systems needing submillivolt voltage resolution, 1% current measurement
and 1 degree Celsius temperature accuracy or any combination of the three.

I2C DATA FORMAT (MSB FIRST);

Data Out:
Byte #1                                     Byte #2                    Byte #3

START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK   X X X X C3 C2 C1 C0 SACK   D7 D6 D5 D4 D3 D2 D1 D0 SACK STOP

Data In:
Byte #1                                     Byte #2                                  Byte #3

START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK   X X X X C3 C2 C1 C0 SACK   REPEAT START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 R SACK

Byte #4                              Byte #5
MSB                                  LSB
D15 D14 D13 D12 D11 D10 D9 D8 MACK   D7 D6 D5 D4 D3 D2 D1 D0 MNACK STOP

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

Read single-ended voltage from V1.

    // Enable Single-Ended Mode
    ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, 0x00, LTC2991_V1_V2_DIFFERENTIAL_ENABLE | LTC2991_V1_V2_TEMP_ENABLE);

    // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
    ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V1_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);

    voltage = LTC2991_code_to_single_ended_voltage(code, LTC2991_SINGLE_ENDED_lsb); // Converts code to voltage from single-ended lsb

Read current from V3-V4.

    resistor = 1; // R_sense across V3-V4 in ohms

    // Enable Differential Mode
    ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, LTC2991_V3_V4_DIFFERENTIAL_ENABLE, LTC2991_V3_V4_TEMP_ENABLE);

    // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
    ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V4_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);

    voltage = LTC2991_code_to_differential_voltage(code, LTC2991_DIFFERENTIAL_lsb); // Converts code to voltage from differential lsb
    current = voltage / resistor; // Calculates current

Read temperature from diode connected to V7-V8.

    // Enable temperature mode.
    ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, LTC2991_V7_V8_TEMP_ENABLE, 0x00);

    // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
    ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V7_MSB_REG, &adc_code, &data_valid, LTC2991_TIMEOUT);

    // Converts code to temperature from adc code and temperature lsb
    temperature = LTC2991_temperature(adc_code, LTC2991_TEMPERATURE_lsb);

@endverbatim

http://www.linear.com/product/LTC2991

http://www.linear.com/product/LTC2991#demoboards


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
    @ingroup LTC2991
    Library Header File for LTC2991: 14-bit Octal I2C Voltage, Current, and Temperature Monitor
*/

#ifndef LTC2991_H
#define LTC2991_H

#include <Wire.h>

// Calibration Variables
//! Typical single-ended LSB weight in volts
const float LTC2991_SINGLE_ENDED_lsb = 3.05176E-04;
//! Typical differential LSB weight in volts
const float LTC2991_DIFFERENTIAL_lsb = 1.90735E-05;
//! Typical VCC LSB weight in volts
const float LTC2991_VCC_lsb = 3.05176E-04;
//! Typical temperature LSB weight in degrees Celsius (and Kelvin).
//! Used for internal temperature as well as remote diode temperature measurements.
const float LTC2991_TEMPERATURE_lsb = 0.0625;
//! Typical remote diode LSB weight in volts.
//! Used to readback diode voltage when in temperature measurement mode.
const float LTC2991_DIODE_VOLTAGE_lsb = 3.815E-05;

/*! @name I2C_Addresses
@{ */

//! I2C address of the LTC2991.
//! Configured by tying the ADR0, ADR1, and ADR2 pins high or low. See Table 1 of datasheet.
//! Uncomment LTC2991_I2C_ADDRESS to match demo board configuration.
//  Address assignment
// LTC2991 I2C Address                //  AD2       AD1       AD0
#define LTC2991_I2C_ADDRESS 0x48      //  Low       Low       Low
// #define LTC2991_I2C_ADDRESS 0x49    //  Low       Low       High
// #define LTC2991_I2C_ADDRESS 0x4A    //  Low       High      Low
// #define LTC2991_I2C_ADDRESS 0x4B    //  Low       High      High
// #define LTC2991_I2C_ADDRESS 0x4C    //  High      Low       Low
// #define LTC2991_I2C_ADDRESS 0x4D    //  High      Low       High
// #define LTC2991_I2C_ADDRESS 0x4E    //  High      High      Low
// #define LTC2991_I2C_ADDRESS 0x4F    //  High      High      High

//! LTC2991 Global I2C Address.
#define LTC2991_I2C_GLOBAL_ADDRESS 0x77  //  Global Address

/*! @} */
/*! @name REGISTERS
@{ */

#define LTC2991_STATUS_LOW_REG              0x00    //!< Data_Valid Bits(V1 Through V8)
#define LTC2991_STATUS_HIGH_REG             0x01    //!< Data_valid bits
#define LTC2991_CHANNEL_ENABLE_REG          0x01    //!< Channel Enable, Vcc, T_internal Conversion Status, Trigger
#define LTC2991_CONTROL_V1234_REG           0x06    //!< V1, V2, V3, and V4 Control Register
#define LTC2991_CONTROL_V5678_REG           0x07    //!< V5, V6, V7, AND V8 Control Register
#define LTC2991_CONTROL_PWM_Tinternal_REG   0x08    //!< PWM Threshold and T_internal Control Register
#define LTC2991_PWM_THRESHOLD_MSB_REG       0x09    //!< PWM Threshold
#define LTC2991_V1_MSB_REG                  0x0A    //!< V1, or T_R1 T MSB
#define LTC2991_V1_LSB_REG                  0x0B    //!< V1, or T_R1 T LSB
#define LTC2991_V2_MSB_REG                  0x0C    //!< V2, V1-V2, or T_R2 Voltage MSB
#define LTC2991_V2_LSB_REG                  0x0D    //!< V2, V1-V2, or T_R2 Voltage LSB
#define LTC2991_V3_MSB_REG                  0x0E    //!< V3, or T_R2 T MSB
#define LTC2991_V3_LSB_REG                  0x0F    //!< V3, or T_R2 T LSB
#define LTC2991_V4_MSB_REG                  0x10    //!< V4, V3-V4, or T_R2 Voltage MSB
#define LTC2991_V4_LSB_REG                  0x11    //!< V4, V3-V4, or T_R2 Voltage LSB
#define LTC2991_V5_MSB_REG                  0x12    //!< V5, or T_R3 T MSB
#define LTC2991_V5_LSB_REG                  0x13    //!< V5, or T_R3 T LSB
#define LTC2991_V6_MSB_REG                  0x14    //!< V6, V5-V6, or T_R3 Voltage MSB
#define LTC2991_V6_LSB_REG                  0x15    //!< V6, V5-V6, or T_R3 Voltage LSB
#define LTC2991_V7_MSB_REG                  0x16    //!< V7, or T_R4 T MSB
#define LTC2991_V7_LSB_REG                  0x17    //!< V7, or T_R4 T LSB
#define LTC2991_V8_MSB_REG                  0x18    //!< V8, V7-V8, or T_R4 Voltage MSB
#define LTC2991_V8_LSB_REG                  0x19    //!< V8, V7-V8, or T_R4 Voltage LSB
#define LTC2991_T_Internal_MSB_REG          0x1A    //!< T_Internal MSB
#define LTC2991_T_Internal_LSB_REG          0x1B    //!< T_Internal LSB
#define LTC2991_Vcc_MSB_REG                 0x1C    //!< Vcc MSB
#define LTC2991_Vcc_LSB_REG                 0x1D    //!< Vcc LSB

/*! @} */
/*! @name LTC2991_CHANNEL_ENABLE_REG SETTINGS
    Bitwise OR settings, and write to LTC2991_CHANNEL_ENABLE_REG to configure settings.
    Bitwise AND with value read from LTC2991_CHANNEL_ENABLE_REG to determine present setting.
@{ */

#define LTC2991_V7_V8_TR4_ENABLE              0x80  //!< Enable V7-V8 measurements, including TR4 temperature
#define LTC2991_V5_V6_TR3_ENABLE              0x40  //!< Enable V5-V6 measurements, including TR3 temperature
#define LTC2991_V3_V4_TR2_ENABLE              0x20  //!< Enable V3-V4 measurements, including TR2 temperature
#define LTC2991_V1_V2_TR1_ENABLE              0x10  //!< Enable V1-V2 measurements, including TR1 temperature
#define LTC2991_VCC_TINTERNAL_ENABLE          0x08  //!< Enable Vcc internal voltage measurement
#define LTC2991_ENABLE_ALL_CHANNELS           0xF8  //!< Use to enable all LTC2991 channels.  Equivalent to bitwise OR'ing all channel enables.
#define LTC2991_BUSY                          0x04  //!< LTC2991 Busy Bit

/*! @} */
/*! @name LTC2991_CONTROL_V1234_REG SETTINGS
    Bitwise OR settings, and write to LTC2991_CONTROL_V1234_REG to configure settings.
    Bitwise AND with value read from LTC2991_CONTROL_V1234_REG to determine present setting.
@{ */

#define LTC2991_V3_V4_FILTER_ENABLE           0x80 //!< Enable filters on V3-V4
#define LTC2991_V3_V4_KELVIN_ENABLE           0x40 //!< Enable V3-V4 for Kelvin. Otherwise, Celsius.
#define LTC2991_V3_V4_TEMP_ENABLE             0x20 //!< Enable V3-V4 temperature mode.
#define LTC2991_V3_V4_DIFFERENTIAL_ENABLE     0x10 //!< Enable V3-V4 differential mode.  Otherwise, single-ended.
#define LTC2991_V1_V2_FILTER_ENABLE           0x08 //!< Enable filters on V1-V2
#define LTC2991_V1_V2_KELVIN_ENABLE           0x04 //!< Enable V1-V2 for Kelvin. Otherwise, Celsius.
#define LTC2991_V1_V2_TEMP_ENABLE             0x02 //!< Enable V1-V2 temperature mode.
#define LTC2991_V1_V2_DIFFERENTIAL_ENABLE     0x01 //!< Enable V1-V2 differential mode.  Otherwise, single-ended.

/*! @} */
/*! @name LTC2991_CONTROL_V5678_REG SETTINGS
    Bitwise OR settings, and write to LTC2991_CONTROL_V5678_REG to configure settings.
    Bitwise AND with value read from LTC2991_CONTROL_V5678_REG to determine present setting.
@{ */

#define LTC2991_V7_V8_FILTER_ENABLE           0x80 //!< Enable filters on V7-V8
#define LTC2991_V7_V8_KELVIN_ENABLE           0x40 //!< Enable V7-V8 for Kelvin. Otherwise, Celsius.
#define LTC2991_V7_V8_TEMP_ENABLE             0x20 //!< Enable V7-V8 temperature mode.
#define LTC2991_V7_V8_DIFFERENTIAL_ENABLE     0x10 //!< Enable V7-V8 differential mode.  Otherwise, single-ended.
#define LTC2991_V5_V6_FILTER_ENABLE           0x08 //!< Enable filters on V5-V6
#define LTC2991_V5_V6_KELVIN_ENABLE           0x04 //!< Enable V5-V6 for Kelvin. Otherwise, Celsius.
#define LTC2991_V5_V6_TEMP_ENABLE             0x02 //!< Enable V5-V6 temperature mode.
#define LTC2991_V5_V6_DIFFERENTIAL_ENABLE     0x01 //!< Enable V5-V6 differential mode.  Otherwise, single-ended.

/*! @} */
/*! @name LTC2991_CONTROL_PWM_Tinternal_REG SETTINGS
    Bitwise OR settings, and write to LTC2991_CONTROL_PWM_Tinternal_REG to configure settings.
    Bitwise AND with value read from LTC2991_CONTROL_PWM_Tinternal_REG to determine present setting.
@{ */

#define LTC2991_PWM_0                         0x80 //!< PWM threshold Least Significant Bit
#define LTC2991_PWM_INVERT                    0x40 //!< Invert PWM
#define LTC2991_PWM_ENABLE                    0x20 //!< Enable PWM
#define LTC2991_REPEAT_MODE                   0x10 //!< Enable Repeated Aquisition Mode
#define LTC2991_INT_FILTER_ENABLE             0x08 //!< Enable Internal Temperature Filter
#define LTC2991_INT_KELVIN_ENABLE             0x04 //!< Enable internal temperature for Kelvin. Otherwise, Celsius.
/*!@} */

//! Reads a 14-bit adc_code from LTC2991.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2991_adc_read(uint8_t i2c_address,    //!< I2C address of the LTC2991. Configured by tying the ADR0, ADR1, and ADR2 pins high or low. See Table 1 of datasheet.
                        uint8_t msb_register_address,        /*!< Address of the LTC2991 MSB register to be read. This is also known as the "command byte".
                                                     Two sequential 8-bit registers are read, starting with the msb_register_address.*/
                        int16_t *adc_code,      //!< returns 14-bit value read from the adc
                        int8_t *data_valid      //!< returns the status of the DATA_VALID bit. *data_valid=0 indicates stale data
                       );

//! Reads a 14-bit adc_code from the LTC2991 but enforces a maximum timeout.
//! Similar to LTC2991_adc_read except it repeats until the data_valid bit is set, it fails to receive an I2C acknowledge, or the timeout (in milliseconds)
//! expires. It keeps trying to read from the LTC2991 every millisecond until the data_valid bit is set (indicating new data since the previous
//! time this register was read) or until it fails to receive an I2C acknowledge (indicating an error on the I2C bus).
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2991_adc_read_timeout(uint8_t i2c_address,    //!< I2C address of the LTC2991. Configured by tying the ADR0, ADR1, and ADR2 pins high or low. See Table 1 of datasheet.
                                uint8_t msb_register_address,        /*!< Address of the LTC2991 MSB register to be read. This is also known as the "command byte".
                                                             Two sequential 8-bit registers will be read, starting with the msb_register_address.*/
                                int16_t *adc_code,      //!< returns 14-bit value read from the adc
                                int8_t *data_valid,     //!< returns the status of the DATA_VALID bit. *data_valid=0 indicates stale data
                                uint16_t timeout,        //!< maximum timeout in millisceonds. If at any time a NACK is received the function aborts.
                                uint8_t status_bit        //!<                     If the timeout is reached without valid data (*data_valid=1) the function exits.*/
                               );

//! Reads new data (even after a mode change) by flushing old data and waiting for the data_valid bit to be set.
//! This function simplifies adc reads when modes are changing.  For example, if V1-V2 changes from temperature mode
//! to differential voltage mode, the data in the register may still correspond to the temperature reading immediately
//! after the mode change.  Flushing one reading and waiting for a new reading guarantees fresh data is received.
//! If the timeout is reached without valid data (*data_valid=1) the function exits.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2991_adc_read_new_data(uint8_t i2c_address,   //!< I2C address of the LTC2991. Configured by tying the ADR0, ADR1, and ADR2 pins high or low. See Table 1 of datasheet.
                                 uint8_t msb_register_address,       /*!< Address of the LTC2991 MSB register to be read. This is also known as the "command byte".
                                                             Two sequential 8-bit registers will be read, starting with the msb_register_address.*/
                                 int16_t *adc_code,     //!< returns 14-bit value read from the adc
                                 int8_t *data_valid,    //!< returns the status of the DATA_VALID bit. *data_valid=0 indicates stale data
                                 uint16_t timeout       /*!< maximum timeout in millisceonds. If at any time a NACK is received the function aborts.
                                                             If the timeout is reached without valid data (*data_valid=1) the function exits.*/
                                );

//! Reads an 8-bit register from the LTC2991 using the standard repeated start format.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2991_register_read(uint8_t i2c_address,       //!< I2C address of the LTC2991. Configured by tying the ADR0, ADR1, and ADR2 pins high or low. See Table 1 of datasheet.
                             uint8_t register_address,  //!< Address of the LTC2991 register to be read. This is also known as the "command byte".
                             uint8_t *register_data     //!< returns 8-bit value read from the LTC2991 register.
                            );

//! Write one byte to an LTC2991 register.
//! Writes to an 8-bit register inside the LTC2991 using the standard I2C repeated start format.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2991_register_write(uint8_t i2c_address,      //!< I2C address of the LTC2991. Configured by tying the ADR0, ADR1, and ADR2 pins high or low. See Table 1 of datasheet.
                              uint8_t register_address, //!< Address of the LTC2991 register to be overwritten.  This is also known as the "command byte".
                              uint8_t register_data     //!< Value that will be written to the register.
                             );


//! Used to set and clear bits in a control register.  bits_to_set will be bitwise OR'd with the register.
//! bits_to_clear will be inverted and bitwise AND'd with the register so that every location with a 1 will result in a 0 in the register.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2991_register_set_clear_bits(uint8_t i2c_address,         //!< I2C address of the LTC2991. Configured by tying the ADR0, ADR1, and ADR2 pins high or low. See Table 1 of datasheet.
                                       uint8_t register_address,    //!< Address of the LTC2991 register to be modified.
                                       uint8_t bits_to_set,         //!< bits_to_set will be bitwise OR'd with the register.
                                       uint8_t bits_to_clear        //!< bits_to_clear will be inverted and bitwise AND'd with the register
                                      );

//! Calculates the LTC2991 single-ended input voltages
//! @return the single-ended voCalculatesltage in volts
float LTC2991_code_to_single_ended_voltage(int16_t adc_code,                //!< code read from the adc (from a function such as LTC2991_adc_read)
    float LTC2991_single_ended_lsb   //!< single-ended LSB weight. If not calibrated, use LTC2991_SINGLE_ENDED_LSB
                                          );

//! Calculates the LTC2991 Vcc voltage
//! @return the Vcc voltage in volts
float LTC2991_code_to_vcc_voltage(int16_t adc_code,                 //!< code read from the adc (from a function such as LTC2991_adc_read)
                                  float LTC2991_single_ended_lsb    //!< Vcc LSB weight. If not calibrated, use LTC2991_VCC_LSB
                                 );

//! Calculates the LTC2991 differential input voltage.
//! @return the differential voltage in volts
float LTC2991_code_to_differential_voltage(int16_t adc_code,                //!< code read from the adc (from a function such as LTC2991_adc_read)
    float LTC2991_differential_lsb   //!< differential LSB weight. If not calibrated, use LTC2991_DIFFERENTIAL_LSB
                                          );

//! Calculates the LTC2991 temperature
//! @return the temperature in degrees Celsius or degrees Kevlin (dependent on mode setting).
float LTC2991_temperature(int16_t adc_code,                 //!< code read from the adc (from a function such as LTC2991_adc_read).
                          float LTC2991_temperature_lsb,     //!< temperature LSB weight. If not calibrated, use LTC2991_TEMPERATURE_LSB
                          boolean unit                     //!< The temperature unit, true for Kelvin, false for Celsius
                         );

//! Calcultates the LTC2991 diode voltage
//! @return the diode voltage in volts.
float LTC2991_code_to_diode_voltage(int16_t adc_code,               //!< code read from the adc (from a function such as LTC2991_adc_read)
                                    float LTC2991_diode_voltage_lsb //!< diode voltage LSB weight. If not calibrated, use LTC2991_DIODE_VOLTAGE_LSB
                                   );

#endif  // LTC2991_H
