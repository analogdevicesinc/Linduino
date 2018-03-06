/*!
LTC3589: 8-Output Regulator with Sequencing and I2C

@verbatim

The LTC3589 is a complete power management solution for ARM and ARM-based
processors and advanced portable microprocessor systems. The device contains
three step-down DC/DC converters for core, memory and SoC rails, a
buck-boost regulator for I/O at 1.8V to 5V and three 250mA LDO regulators
for low noise analog supplies. An I2C serial port is used to control enables,
output voltage levels, dynamic voltage scaling, operating modes and status
reporting.
Regulator start-up is sequenced by connecting outputs to enable pins in the
desired order or programmed via the I2C port. System power-on, power-off,
and reset functions are controlled by pushbutton interface, pin inputs, or
I2C interface.
The LTC3589 supports i.MX53/51, PXA and OMAP processors with eight
independent rails at appropriate power levels. Other features include
interface signals such as the VSTB pin that simultaneously toggle up to
four rails between programmed run and standby output voltages.  The device
is available in a low profile 40-pin 6mm x 6mm exposed pad QFN package.

I2C DATA FORMAT (MSB FIRST);

@endverbatim

http://www.linear.com/product/LTC3589

http://www.linear.com/product/LTC3589#demoboards


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
    @ingroup LTC3589
    Library Header File for LTC3589: 8-Output Regulator with Sequencing and I2C
*/

#ifndef LTC3589_H
#define LTC3589_H
#endif

/*! @name I2C Address
    I2C address of the LTC3589/LTC3589-1/LTC3589-2.
@{ */
/* LTC3589 I2C Addresses */
#define LTC3589_I2C_ADDRESS         0x34
/*! @} */

/*! @name Feedback Resistors
    Factory feedback resistor values for the LTC3589 demo boards in kOhm.
@{ */
#define LTC3589_RES_BUCK1_RTOP      604
#define LTC3589_RES_BUCK1_RBOT      768
#define LTC3589_RES_BUCK2_RTOP      715
#define LTC3589_RES_BUCK2_RBOT      422
#define LTC3589_RES_BUCK3_RTOP      681
#define LTC3589_RES_BUCK3_RBOT      787
#define LTC3589_RES_LDO2_RTOP       604
#define LTC3589_RES_LDO2_RBOT       768
/*! @} */

/*! @name Registers
@{ */
/* LTC3589 Registers */
#define LTC3589_REG_SCR1             0x07
#define LTC3589_REG_OVEN             0x10
#define LTC3589_REG_SCR2             0x12
#define LTC3589_REG_VCCR             0x20
#define LTC3589_REG_CLIRQ            0x21
#define LTC3589_REG_B1DTV1           0x23
#define LTC3589_REG_B1DTV2           0x24
#define LTC3589_REG_VRRCR            0x25
#define LTC3589_REG_B2DTV1           0x26
#define LTC3589_REG_B2DTV2           0x27
#define LTC3589_REG_B3DTV1           0x29
#define LTC3589_REG_B3DTV2           0x2A
#define LTC3589_REG_L2DTV1           0x32
#define LTC3589_REG_L2DTV2           0x33
#define LTC3589_REG_IRQSTAT          0x02
#define LTC3589_REG_PGSTAT           0x13
/*! @} */

/*! @name LTC3589_REG_SCR1 Register Settings
    Bit position of single bit settings in the LTC3589_REG_SCR1 registers.
@{ */
#define LTC3589_BUCKBOOST_MODE       6
/*! @} */

/*! @name LTC3589_REG_OVEN Register Settings
    Bit position of single bit settings in the LTC3589_REG_OVEN register.
@{ */
#define LTC3589_SOFTWARE_CNTRL       7
#define LTC3589_EN_LDO4              6
#define LTC3589_EN_LDO3              5
#define LTC3589_EN_LDO2              4
#define LTC3589_EN4                  3
#define LTC3589_EN3                  2
#define LTC3589_EN2                  1
#define LTC3589_EN1                  0
/*! @} */

/*! @name LTC3589_REG_SCR2 Register Settings
    Bit position of single bit settings in the LTC3589_REG_SCR2 register.
@{ */
#define LTC3589_MASK_PG_SHUTDOWN     7
#define LTC3589_LDO4_STARTUP         6
#define LTC3589_LDO3_STARTUP         5
#define LTC3589_LDO2_STARTUP         4
#define LTC3589_BB_STARTUP           3
#define LTC3589_BUCK3_STARTUP        2
#define LTC3589_BUCK2_STARTUP        1
#define LTC3589_BUCK1_STARTUP        0
/*! @} */

/*! @name LTC3589_REG_VCCR Register Settings
    Bit position of single bit settings in the LTC3589_REG_VCCR register.
@{ */
#define LTC3589_LDO2_REF_SELECT      7
#define LTC3589_LDO2_GO              6
#define LTC3589_BUCK3_REF_SELECT     5
#define LTC3589_BUCK3_GO             4
#define LTC3589_BUCK2_REF_SELECT     3
#define LTC3589_BUCK2_GO             2
#define LTC3589_BUCK1_REF_SELECT     1
#define LTC3589_BUCK1_GO             0
/*! @} */

/*! @name LTC3589_REG_BxDTV1 Register Settings
    Bit position of single bit settings in the LTC3589_REG_BxDTV1 registers.
@{ */
#define LTC3589_BUCK_PG_MASK         5
/*! @} */

/*! @name LTC3589_REG_BxDTV2 Register Settings
    Bit position of single bit settings in the LTC3589_REG_BxDTV2 register.
@{ */
#define LTC3589_BUCK_KEEP_ALIVE      7
#define LTC3589_BUCK_PHASE_SEL       6
#define LTC3589_BUCK_CLOCK_RATE      5
/*! @} */

/*! @name LTC3589_REG_L2DTV1 Register Settings
    Bit position of single bit settings in the LTC3589_REG_L2DTV1 register.
@{ */
#define LTC3589_LDO2_KEEP_ALIVE      7
#define LTC3589_LDO2_PG_MASK         5
/*! @} */

/*! @name LTC3589_REG_L2DTV2 Register Settings
    Bit position of single bit settings in the LTC3589_REG_L2DTV2 register.
@{ */
#define LTC3589_LDO2_CNTRL_MODE      7
/*! @} */

/*! @name LTC3589_REG_IRQSTAT Register Settings
    Bit position of status bits in the LTC3589_REG_IRQSTAT register.
@{ */
#define LTC3589_IRQ_OT_SHUTDOWN      7
#define LTC3589_IRQ_OT_WARNING       6
#define LTC3589_IRQ_UV_SHUTDOWN      5
#define LTC3589_IRQ_UV_WARNING       4
#define LTC3589_IRQ_PG_SHUTDOWN      3
/*! @} */

/*! @name LTC3589_REG_PGSTAT Register Settings
    Bit position of status bits in the LTC3589_REG_PGSTAT register.
@{ */
#define LTC3589_PG_LDO4              7
#define LTC3589_PG_LDO3              6
#define LTC3589_PG_LDO2              5
#define LTC3589_PG_BB                4
#define LTC3589_PG_BUCK3             3
#define LTC3589_PG_BUCK2             2
#define LTC3589_PG_BUCK1             1
#define LTC3589_PG_LDO1              0
/*! @} */

/*! @name LTC3589 Mask Settings
    Bitwise AND with register data to determine present setting.
@{ */
#define LTC3589_BUCK_MODE_MASK(num) (0x03<<((num)*2)-2)
#define LTC3589_BUCK3_MODE_MASK      0x30
#define LTC3589_BUCK2_MODE_MASK      0x0C
#define LTC3589_BUCK1_MODE_MASK      0x03
#define LTC3589_BUCK_DVDT_MASK       0xC0
#define LTC3589_FB_REF_MASK          0x1F
#define LTC3589_LDO2_SLEW_MASK       0xC0
#define LTC3589_BUCK3_SLEW_MASK      0x30
#define LTC3589_BUCK2_SLEW_MASK      0x0C
#define LTC3589_BUCK1_SLEW_MASK      0x03
#define LTC3589_LDO4_VOLTAGE_MASK    0x60
/*! @} */


//! Reads an 8-bit register from the LTC3589 using the standard repeated start format.
//! @return State of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3589_register_read(uint8_t i2c_address,       //!< I2C address of the LTC3589.
                             uint8_t register_address,  //!< Address of the LTC3589 register to be read. This is also known as the "command byte".
                             uint8_t *register_data     //!< returns 8-bit value read from the LTC3589 register.
                            );

//! Writes to an 8-bit register inside the LTC3589 using the standard I2C repeated start format.
//! @return State of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3589_register_write(uint8_t i2c_address,      //!< I2C address of the LTC3589.
                              uint8_t register_address, //!< Address of the LTC3589 register to be overwritten.  This is also known as the "command byte".
                              uint8_t register_data     //!< Value that will be written to the register.
                             );

//! Sets any bit inside the LTC3589 using the standard I2C repeated start format.
//! @return State of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3589_bit_set(uint8_t i2c_address,      //!< I2C address of the LTC3589.
                       uint8_t register_address, //!< Address of the LTC3589 register to be overwritten.  This is also known as the "command byte".
                       uint8_t bit_number        //!< Bit location (0-7) to be written.
                      );

//! Clears any bit inside the LTC3589 using the standard I2C repeated start format.
//! @return State of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3589_bit_clear(uint8_t i2c_address,      //!< I2C address of the LTC3589.
                         uint8_t register_address, //!< Address of the LTC3589 register to be overwritten.  This is also known as the "command byte".
                         uint8_t bit_number       //!< Address of the bit to set within the register.
                        );

//! Writes any bit inside the LTC3589 using the standard I2C repeated start format.
//! @return State of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3589_bit_write(uint8_t i2c_address,       //!< I2C address of the LTC3589.
                         uint8_t register_address,  //!< Address of the LTC3589 register to be written.
                         uint8_t bit_number,        //!< Bit location (0-7) to be written.
                         uint8_t bit_data           //!< Bit data (0 or 1) to be written.
                        );

//! Reads the value of any bit in any register or the LTC3589.
//! @return Bit value at the passed register subaddress and bit location.
uint8_t LTC3589_bit_is_set(uint8_t i2c_address,        //!< I2C address of the LTC3589.
                           uint8_t register_address,   //!< Address of the LTC3589 register to be read. This is also known as the "command byte".
                           uint8_t bit_number          //!< Bit location (0-7) to be read.
                          );

//! Sets the output voltage of LDO2.
//! @return New LDO2 output voltage, the closest allowable voltage to the user's chosen voltage.
float LTC3589_set_ldo2_output_voltage(uint8_t i2c_address, //!< I2C address of the LTC3589.
                                      uint8_t register_address, //!< Address of the LTC3589 LDO2 FB reference register to be written.
                                      float output_voltage //!< New output voltage to set, in mV. Assumes a valid voltage for the feedback resistors.
                                     );

//! Sets the output voltage of any buck.
//! @return New buck output voltage, the closest allowable voltage to the user's chosen voltage.
float LTC3589_set_buck_output_voltage(uint8_t i2c_address, //!< I2C address of the LTC3589.
                                      uint8_t register_address, //!< Address of the LTC3589 Buck FB reference register to be written.
                                      float output_voltage //!< New output voltage to set, in mV. Assumes a valid voltage for the feedback resistors.
                                     );

//! Writes the Feedback Reference Voltage of LDO2.
//! @return New Feedback Reference Input Voltage, the closest allowable voltage to the user's chosen voltage.
float LTC3589_set_ldo2_fb_ref(uint8_t i2c_address, //!< I2C address of the LTC3589.
                              uint8_t register_address, //!< Address of the LTC3589 LDO2 FB reference register to be written.
                              float fb_ref_voltage //!< New Feedback Reference voltage to set in mV.  Values between 362.5 and 750 are allowed.
                             );

//! Writes the Feedback Reference Voltage of any buck.
//! @return New Feedback Reference Input Voltage, the closest allowable voltage to the user's chosen voltage.
float LTC3589_set_buck_fb_ref(uint8_t i2c_address, //!< I2C address of the LTC3589.
                              uint8_t register_address, //!< Address of the LTC3589 Buck FB reference register to be written.
                              float fb_ref_voltage //!< New Feedback Reference voltage to set in mV.  Values between 362.5 and 750 are allowed.
                             );

//! Calculates the maximum output voltage of LDO2 in mV based on the feedback resistors.
//! @return Maximum possible output voltage for LDO2.
float LTC3589_ldo2_vout_max();

//! Calculates the minimum output voltage of LDO2 mV based on the feedback resistors.
//! @return Minimum possible output voltage for LDO2.
float LTC3589_ldo2_vout_min();

//! Calculates the maximum output voltage of any buck in mV based on the feedback resistors.
//! @return Maximum possible output voltage for the selected buck.
float LTC3589_buck_vout_max(uint8_t buck_number //!< Number (1-3) of buck.
                           );

//! Calculates the minimum output voltage of any buck in mV based on the feedback resistors.
//! @return Minimum possible output voltage for the selected buck.
float LTC3589_buck_vout_min(uint8_t buck_number //!< Number (1-3) of buck.
                           );

//! Selects the reference for LDO2.
//! @return State of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3589_select_ldo2_reference(uint8_t i2c_address, //!< I2C address of the LTC3589.
                                     int8_t ref_char //!< Reference to select (A or B).
                                    );

//! Selects the reference for the specified buck regulator(s).
//! @return State of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3589_select_buck_reference(uint8_t i2c_address, //!< I2C address of the LTC3589.
                                     uint8_t buck_number, //!< Number (1-3) of the buck whose reference will be selected. Enter 0xFF for all bucks.
                                     int8_t ref_char //!< Reference to select (A or B).
                                    );

//! Sets the switching mode for the specified buck regulator(s).
//! @return State of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3589_set_buck_mode(uint8_t i2c_address, //!< I2C address of the LTC3589.
                             uint8_t buck_number, //!< Number (1-3) of the buck to set. Enter 0xFF for all bucks.
                             uint8_t mode //!< Switching mode to be set (0=pulse skipping, 1=burst, 2=forced continuous)
                            );

//! Sets the switching mode for the buck-boost regulator.
//! @return State of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3589_set_buckboost_mode(uint8_t i2c_address, //!< I2C address of the LTC3589.
                                  uint8_t mode //!< Switching mode to be set (0=pulse skipping, 1=burst, 2=forced continuous)
                                 );

//! Sets the start-up mode for all regulators.
//! @return State of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3589_set_startup_mode(uint8_t i2c_address, //!< I2C address of the LTC3589.
                                uint8_t startup_bit //!< Data to write to start-up bit (0=enable at any output, 1=enable only if output <300mV)
                               );

//! Sets the PGOOD mask bit for all bucks and LDO2.
//! @return State of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3589_set_pgood_mask(uint8_t i2c_address, //!< I2C address of the LTC3589.
                              uint8_t pgood_bit //!< Data to write to PGOOD bit (0=PGOOD low when slewing, 1=PGOOD not forced low when slewing)
                             );

//! Sets LDO4 output voltage on the LTC3589.
//! @return State of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3589_set_ldo4_voltage(uint8_t i2c_address, //!< I2C address of the LTC3589.
                                uint8_t ldo4_output_voltage_code //!< Voltage code for new LDO4 output voltage (varies by part option).
                               );

//! Sets the dynamnic reference slew rate for the regulator(s).
//! @return State of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3589_set_regulator_slew(uint8_t i2c_address, //!< I2C address of the LTC3589.
                                  uint8_t regulator_mask, //!< Mask of the regulator to set. Enter 0xFF for all regulators.
                                  uint8_t slew_rate //!< Slew rate code to set.
                                 );

//! Sets the switch DV/DT control for the buck regulators.
//! @return State of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3589_set_switch_dvdt_control(uint8_t i2c_address, //!< I2C address of the LTC3589.
                                       uint8_t dvdt_control_bits //!< Two-bit DV/DT control code to set.
                                      );

//! Sets all of the GO bits in the VCCR register.
int8_t LTC3589_set_go_bits(uint8_t i2c_address //!< I2C address of the LTC3589.
                          );
