/*!
LTC2990: 14-bit ADC Quad I2C voltage, current, and temperature monitor.

@verbatim

The LTC2990 is used to monitor system temperatures, voltages and currents.
Through the I2C serial interface, the four monitors can individually measure
supply voltages and can be paired for differential measurements of current sense
resistors or temperature sensing transistors. Additional measurements include
internal temperature and internal VCC. The internal 10ppm reference minimizes
the number of supporting components and area required. Selectable address and
configurable functionality give the LTC2990 flexibility to be incorporated in
various systems needing temperature, voltage or current data. The LTC2990 fits
well in systems needing sub-millivolt voltage resolution, 1% current measurement
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
    ack |= LTC2990_register_set_clear_bits(LTC2990_I2C_ADDRESS, LTC2990_CONTROL_REG, LTC2990_V1_V2_V3_V4, LTC2990_VOLTAGE_MODE_MASK);

    // Triggers a conversion by writing any value to the trigger register
    ack |= LTC2990_register_write(LTC2990_I2C_ADDRESS, LTC2990_TRIGGER_REG, 0x00);

    // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
    ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V1_MSB_REG, &code, &data_valid, LTC2990_TIMEOUT);

    voltage = LTC2990_code_to_single_ended_voltage(code, LTC2990_SINGLE_ENDED_lsb);

Read current from V3-V4.

    resistor = 1; // R_sense across V3-V4 in ohms

    // Enables differential mode.
    ack |= LTC2990_register_set_clear_bits(LTC2990_I2C_ADDRESS, LTC2990_CONTROL_REG, LTC2990_V1V2_V3V4, LTC2990_VOLTAGE_MODE_MASK);

    // Triggers a conversion by writing any value to the trigger register
    ack |= LTC2990_register_write(LTC2990_I2C_ADDRESS, LTC2990_TRIGGER_REG, 0x00);

    // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
    ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V2_MSB_REG, &code, &data_valid, LTC2990_TIMEOUT);

    voltage = LTC2990_code_to_differential_voltage(code, LTC2990_DIFFERENTIAL_lsb);

    current = voltage / resistor; // Calculates current

Read temperature from diode connected to V1-V2.

    // Enables temperature mode
    ack |= LTC2990_register_set_clear_bits(LTC2990_I2C_ADDRESS, LTC2990_CONTROL_REG, LTC2990_TR1_TR2, LTC2990_VOLTAGE_MODE_MASK);

    // Triggers a conversion by writing any value to the trigger register
    ack |= LTC2990_register_write(LTC2990_I2C_ADDRESS, LTC2990_TRIGGER_REG, 0x00);


    // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
    ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V1_MSB_REG, &adc_code, &data_valid, LTC2990_TIMEOUT);

    // Check which temperature mode the LTC2990 in currently in
    ack |= LTC2990_register_read(LTC2990_I2C_ADDRESS, LTC2990_CONTROL_REG, &reg_data);
    if (reg_data & LTC2990_KELVIN_ENABLE) isKelvin= true;
    else isKelvin=false;

    temperature = LTC2990_temperature(adc_code, LTC2990_TEMPERATURE_lsb, isKelvin);

@endverbatim

http://www.linear.com/product/LTC2990

http://www.linear.com/product/LTC2990#demoboards


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
    @ingroup LTC2990
    Library Header File for LTC2990: 14-bit Quad I2C Voltage, Current, and Temperature Monitor
*/

#ifndef LTC2990_H
#define LTC2990_H

#include <Wire.h>

/*! @name I2C_Addresses
@{ */

//! I2C address of the LTC2990.
//! Configured by tying the ADR0, ADR1, and ADR2 pins high or low. See Table 1 of datasheet.
//! Uncomment LTC2990_I2C_ADDRESS to match demo board configuration.
//  Address assignment
// LTC2990 I2C Address                //       AD1       AD0
#define LTC2990_I2C_ADDRESS 0x4C      //       Low       Low
// #define LTC2990_I2C_ADDRESS 0x4D    //      Low       High
// #define LTC2990_I2C_ADDRESS 0x4E    //      High      Low
// #define LTC2990_I2C_ADDRESS 0x4F    //      High      High


//! LTC2990 Global I2C Address.
#define LTC2990_I2C_GLOBAL_ADDRESS 0x77  //  Global Address

/*! @} */
/*! @name REGISTERS
@{ */

#define LTC2990_STATUS_REG                  0x00    //!< Indicates BUSY state and conversion status
#define LTC2990_CONTROL_REG                 0x01    //!< Controls Mode, Single/Repeat, Celsius/Kelvin
#define LTC2990_TRIGGER_REG                 0x02    //!< Triggers a conversion
#define LTC2990_TINT_MSB_REG                0x04    //!< Internal Temperature MSB
#define LTC2990_TINT_LSB_REG                0x05    //!< Internal Temperature LSB
#define LTC2990_V1_MSB_REG                  0x06    //!< V1, V1-V2, or T_R1 T MSB
#define LTC2990_V1_LSB_REG                  0x07    //!< V1, V1-V2, or T_R1 T LSB
#define LTC2990_V2_MSB_REG                  0x08    //!< V2, V1-V2, or T_R2 Voltage MSB
#define LTC2990_V2_LSB_REG                  0x09    //!< V2, V1-V2, or T_R2 Voltage LSB
#define LTC2990_V3_MSB_REG                  0x0A    //!< V3, V3-V4, or T_R2 T MSB
#define LTC2990_V3_LSB_REG                  0x0B    //!< V3, V3-V4, or T_R2 T LSB
#define LTC2990_V4_MSB_REG                  0x0C    //!< V4, V3-V4, or T_R2 Voltage MSB
#define LTC2990_V4_LSB_REG                  0x0D    //!< V4, V3-V4, or T_R2 Voltage LSB
#define LTC2990_VCC_MSB_REG                 0x0E    //!< Vcc MSB
#define LTC2990_VCC_LSB_REG                 0x0F    //!< Vcc LSB

/*! @} */
/*! @name LTC2990_CONTROL_REG SETTINGS
    Bitwise OR settings, and write to LTC2990_CONTROL_REG to configure settings.
    Bitwise AND with value read from LTC2990_CONTROL_REG to determine present setting.
@{ */

#define LTC2990_KELVIN_ENABLE                 0x80 //!< Enable for Kelvin. 
#define LTC2990_CELSIUS_ENABLE                0x00 //!< Enable for Celsius.
#define LTC2990_TEMP_FORMAT_MASK              0x80 //!< Use mask when changing temp formats

#define LTC2990_SINGLE_ENABLE                 0x40 //!< Enable for Single Acquisition
#define LTC2990_REPEATED_ENABLE               0x00 //!< Enable for Repeated Acquisition Mode
#define LTC2990_ACQUISITION_MASK              0x40 //!< Use mask when changing acquisition settings


/*! @} */
/*! @name LTC2990_CONTROL_REG ENABLE
    Bitwise AND 0xE7 then Bitwise OR settings, and write to LTC2990_CONTROL_REG to configure enable settings.
    Bitwise AND with value read from LTC2990_CONTROL_REG to determine present enable setting.
@{ */

#define LTC2990_ENABLE_INT_TEMPERATURE        0x00 //!< Read only Internal Temperature
#define LTC2990_ENABLE_V1                     0x08 //!< Tr1, V1 or V1-V2 per Mode are enabled
#define LTC2990_ENABLE_V2                     0x10 //!< Tr2, V3 or V3-V4 per Mode are enabled
#define LTC2990_ENABLE_ALL                    0x18 //!< All measurements per Mode are enabled 
#define LTC2990_TEMP_MEAS_MODE_MASK           0x18 //!< Use mask when changing temp meas modes

/*! @} */
/*! @name LTC2990_CONTROL_REG MODE
    Bitwise AND 0xF8 then Bitwise OR settings, and write to LTC2990_CONTROL_REG to configure enable settings.
    Bitwise AND with value read from LTC2990_CONTROL_REG to determine present enable setting.
@{ */

#define LTC2990_V1_V2_TR2                   0x00 //!< Read V1, V2 and TR2
#define LTC2990_V1V2_TR2                    0x01 //!< Read V1-V2 and TR2
#define LTC2990_V1V2_V3_V4                  0x02 //!< Read V1-V2, V3 and V4
#define LTC2990_TR1_V3_V4                   0x03 //!< Read TR1, V3 and V4
#define LTC2990_TR1_V3V4                    0x04 //!< Read TR1 and V3-V4
#define LTC2990_TR1_TR2                     0x05 //!< Read TR1 and TR2
#define LTC2990_V1V2_V3V4                   0x06 //!< Read V1-V2 and V3-V4 
#define LTC2990_V1_V2_V3_V4                 0x07 //!< Read V1, V2, V3 and V4
#define LTC2990_VOLTAGE_MODE_MASK           0x07 //!< Use mode mask when changing modes



/*!@} */

//! Reads a 14-bit adc_code from LTC2990.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2990_adc_read(uint8_t i2c_address,    //!< I2C address of the LTC2990. Configured by tying the ADR0, ADR1, and ADR2 pins high or low. See Table 1 of datasheet.
                        uint8_t msb_register_address,   /*!< Address of the LTC2990 MSB register to be read. This is also known as the "command byte".
                                                             Two sequential 8-bit registers are read, starting with the msb_register_address.*/
                        int16_t *adc_code,      //!< returns 14-bit value read from the ADC
                        int8_t *data_valid      //!< returns the status of the DATA_VALID bit. *data_valid=0 indicates stale data
                       );

//! Reads a 14-bit adc_code from the LTC2990 but enforces a maximum timeout.
//! Similar to LTC2990_adc_read except it repeats until the data_valid bit is set, it fails to receive an I2C acknowledge, or the timeout (in milliseconds)
//! expires. It keeps trying to read from the LTC2990 every millisecond until the data_valid bit is set (indicating new data since the previous
//! time this register was read) or until it fails to receive an I2C acknowledge (indicating an error on the I2C bus).
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2990_adc_read_timeout(uint8_t i2c_address,    //!< I2C address of the LTC2990. Configured by tying the ADR0, ADR1, and ADR2 pins high or low. See Table 1 of datasheet.
                                uint8_t msb_register_address,   /*!< Address of the LTC2990 MSB register to be read. This is also known as the "command byte".
                                                                     Two sequential 8-bit registers will be read, starting with the msb_register_address.*/
                                int16_t *adc_code,      //!< returns 14-bit value read from the ADC
                                int8_t *data_valid,     //!< returns the status of the DATA_VALID bit. *data_valid=0 indicates stale data
                                uint16_t timeout,        /*!< maximum timeout in milliseconds. If at any time a NACK is received the function aborts.*/
                                uint8_t status_bit                            //<!If the timeout is reached without valid data (*data_valid=1) the function exits.
                               );

//! Reads new data (even after a mode change) by flushing old data and waiting for the data_valid bit to be set.
//! This function simplifies ADC reads when modes are changing.  For example, if V1-V2 changes from temperature mode
//! to differential voltage mode, the data in the register may still correspond to the temperature reading immediately
//! after the mode change.  Flushing one reading and waiting for a new reading guarantees fresh data is received.
//! If the timeout is reached without valid data (*data_valid=1) the function exits.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2990_adc_read_new_data(uint8_t i2c_address,   //!< I2C address of the LTC2990. Configured by tying the ADR0, ADR1, and ADR2 pins high or low. See Table 1 of datasheet.
                                 uint8_t msb_register_address,  /*!< Address of the LTC2990 MSB register to be read. This is also known as the "command byte".
                                                                     Two sequential 8-bit registers will be read, starting with the msb_register_address.*/
                                 int16_t *adc_code,     //!< returns 14-bit value read from the ADC
                                 int8_t *data_valid,    //!< returns the status of the DATA_VALID bit. *data_valid=0 indicates stale data
                                 uint16_t timeout       /*!< maximum timeout in milliseconds. If at any time a NACK is received the function aborts.
                                                             If the timeout is reached without valid data (*data_valid=1) the function exits.*/
                                );

//! Reads an 8-bit register from the LTC2990 using the standard repeated start format.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2990_register_read(uint8_t i2c_address,       //!< I2C address of the LTC2990. Configured by tying the ADR0, ADR1, and ADR2 pins high or low. See Table 1 of datasheet.
                             uint8_t register_address,  //!< Address of the LTC2990 register to be read. This is also known as the "command byte".
                             uint8_t *register_data     //!< returns 8-bit value read from the LTC2990 register.
                            );

//! Write one byte to an LTC2990 register.
//! Writes to an 8-bit register inside the LTC2990 using the standard I2C repeated start format.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2990_register_write(uint8_t i2c_address,      //!< I2C address of the LTC2990. Configured by tying the ADR0, ADR1, and ADR2 pins high or low. See Table 1 of datasheet.
                              uint8_t register_address, //!< Address of the LTC2990 register to be overwritten.  This is also known as the "command byte".
                              uint8_t register_data     //!< Value that will be written to the register.
                             );


//! Used to set and clear bits in a control register.  bits_to_set will be bitwise OR'd with the register.
//! bits_to_clear will be inverted and bitwise AND'd with the register so that every location with a 1 will result in a 0 in the register.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2990_register_set_clear_bits(uint8_t i2c_address,         //!< I2C address of the LTC2990. Configured by tying the ADR0, ADR1, and ADR2 pins high or low. See Table 1 of datasheet.
                                       uint8_t register_address,    //!< Address of the LTC2990 register to be modified.
                                       uint8_t bits_to_set,         //!< bits_to_set will be bitwise OR'd with the register.
                                       uint8_t bits_to_clear        //!< bits_to_clear will be inverted and bitwise AND'd with the register
                                      );


//! Calculates the LTC2990 single-ended input voltages
//! @return the single-ended voltage in volts
float LTC2990_code_to_single_ended_voltage(int16_t adc_code,                //!< code read from the ADC (from a function such as LTC2990_adc_read)
    float LTC2990_single_ended_lsb   //!< single-ended LSB weight. If not calibrated, use LTC2990_SINGLE_ENDED_LSB
                                          );

//! Calculates the LTC2990 Vcc voltage
//! @return the Vcc voltage in volts
float LTC2990_code_to_vcc_voltage(int16_t adc_code,                 //!< code read from the ADC (from a function such as LTC2990_adc_read)
                                  float LTC2990_single_ended_lsb    //!< Vcc LSB weight. If not calibrated, use LTC2990_VCC_LSB
                                 );

//! Calculates the LTC2990 differential input voltage.
//! @return the differential voltage in volts
float LTC2990_code_to_differential_voltage(int16_t adc_code,                //!< code read from the ADC (from a function such as LTC2990_adc_read)
    float LTC2990_differential_lsb   //!< differential LSB weight. If not calibrated, use LTC2990_DIFFERENTIAL_LSB
                                          );

//! Calculates the LTC2990 temperature
//! @return the temperature in degrees Celsius or degrees Kelvin (dependent on mode setting).
float LTC2990_temperature(int16_t adc_code,                 //!< code read from the ADC (from a function such as LTC2990_adc_read).
                          float LTC2990_temperature_lsb,     //!< temperature LSB weight. If not calibrated, use LTC2990_TEMPERATURE_LSB
                          boolean unit                     //!< The temperature unit, true for Kelvin, false for Celsius
                         );

//! Calculates the LTC2990 diode voltage
//! @return the diode voltage in volts.
float LTC2990_code_to_diode_voltage(int16_t adc_code,               //!< code read from the ADC (from a function such as LTC2990_adc_read)
                                    float LTC2990_diode_voltage_lsb //!< diode voltage LSB weight. If not calibrated, use LTC2990_DIODE_VOLTAGE_LSB
                                   );

#endif  // LTC2990_H
