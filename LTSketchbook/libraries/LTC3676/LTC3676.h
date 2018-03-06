/*!
LTC3676: Power management solution for application processors

@verbatim

The LTC3676 is a complete power management solution for advanced portable
application processor-based systems. The device contains four synchronous
step-down DC/DC converters for core, memory, I/O, and system on-chip (SoC)
rails and three 300mA LDO regulators for low noise analog supplies. The
LTC3676-1 has a 1.5A buck regulator configured to support DDR termination
plus a VTTR reference output. An I2C serial port is used to control regulator
enables, power-down sequencing, output voltage levels, dynamic voltage
scaling, operating modes and status reporting.
Regulator start-up is sequenced by connecting outputs to enable pins in the
desired order or via the I2C port. System power-on, power-off and reset
functions are controlled by pushbutton interface, pin inputs, or I2C.
The LTC3676 supports i.MX, PXA and OMAP processors with eight independent
rails at appropriate power levels. Other features include interface signals
such as the VSTB pin that toggles between programmed run and standby output
voltages on up to four rails simultaneously. The device is available in a
40-lead 6mm x 6mm QFN package.

I2C DATA FORMAT (MSB FIRST);

@endverbatim

http://www.linear.com/product/LTC3676

http://www.linear.com/product/LTC3676#demoboards


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
    @ingroup LTC3676
    Library Header File for LTC3676: Power management solution for application processors
*/

#ifndef LTC3676_H
#define LTC3676_H
#endif

/*! @name I2C_ADDRESSES
    I2C address of the LTC3676 and LTC3676-1.
@{ */
/* LTC3676 I2C Addresses */
#define LTC3676_I2C_ADDRESS         0x3C
#define LTC3676_1_I2C_ADDRESS       0x3D
/*! @} */

/*! @name FEEDBACK RESISTORS
    Feedback resistor values for the DC1976A-A in kOhm.
@{ */
/* LTC3676 Registers */
#define LTC3676_RES_BUCK1_RTOP      1000.
#define LTC3676_RES_BUCK1_RBOT      280.
#define LTC3676_RES_BUCK2_RTOP      1000.
#define LTC3676_RES_BUCK2_RBOT      1100.
#define LTC3676_RES_BUCK3_RTOP      1000.
#define LTC3676_RES_BUCK3_RBOT      1100.
#define LTC3676_RES_BUCK4_RTOP      1000.
#define LTC3676_RES_BUCK4_RBOT      931.
/*! @} */

/*! @name REGISTERS
@{ */
/* LTC3676 Registers */
#define LTC3676_REG_BUCK1       0x01
#define LTC3676_REG_BUCK2       0x02
#define LTC3676_REG_BUCK3       0x03
#define LTC3676_REG_BUCK4       0x04
#define LTC3676_REG_LDOA        0x05
#define LTC3676_REG_LDOB        0x06
#define LTC3676_REG_SQD1        0x07
#define LTC3676_REG_SQD2        0x08
#define LTC3676_REG_CNTRL       0x09
#define LTC3676_REG_DVB1A       0x0A
#define LTC3676_REG_DVB1B       0x0B
#define LTC3676_REG_DVB2A       0x0C
#define LTC3676_REG_DVB2B       0x0D
#define LTC3676_REG_DVB3A       0x0E
#define LTC3676_REG_DVB3B       0x0F
#define LTC3676_REG_DVB4A       0x10
#define LTC3676_REG_DVB4B       0x11
#define LTC3676_REG_MSKIRQ      0x12
#define LTC3676_REG_MSKPG       0x13
#define LTC3676_REG_USER        0x14
#define LTC3676_REG_IRQSTAT     0x15
#define LTC3676_REG_PGSTATL     0x16
#define LTC3676_REG_PGSTATRT    0x17
#define LTC3676_REG_HRST        0x1E
#define LTC3676_REG_CLIRQ       0x1F
/*! @} */

/*! @name LTC3676_REG_BUCKx SETTINGS
    Bit position of single bit settings in the LTC3676_REG_BUCKx registers.
@{ */
#define LTC3676_BUCK_ENABLE          7
#define LTC3676_BUCK_STARTUP         4
#define LTC3676_BUCK_PHASE_SEL       3
#define LTC3676_BUCK_CLOCK_RATE      2
#define LTC3676_BUCK_KEEP_ALIVE      1
#define LTC3676_BUCK_SLEW            0
/*! @} */

/*! @name LTC3676_REG_LDOA SETTINGS
    Bit position of single bit settings in the LTC3676_REG_LDOA register.
@{ */
#define LTC3676_LDO3_ENABLE          5
#define LTC3676_LDO3_STARTUP         4
#define LTC3676_LDO3_KEEP_ALIVE      3
#define LTC3676_LDO2_ENABLE          2
#define LTC3676_LDO2_STARTUP         1
#define LTC3676_LDO2_KEEP_ALIVE      0
/*! @} */

/*! @name LTC3676_REG_LDOB SETTINGS
    Bit position of single bit settings in the LTC3676_REG_LDOB register.
@{ */
#define LTC3676_LDO4_ENABLE          2
#define LTC3676_LDO4_STARTUP         1
#define LTC3676_LDO4_KEEP_ALIVE      0
/*! @} */

/*! @name LTC3676_REG_CNTRL SETTINGS
    Bit position of single bit settings in the LTC3676_REG_CNTRL register.
@{ */
#define LTC3676_PWR_ON               7
#define LTC3676_PB_RESET_TMR         6
#define LTC3676_SOFTWARE_CNTRL       5
/*! @} */

/*! @name LTC3676_REG_DVBxA SETTINGS
    Bit position of single bit settings in the LTC3676_REG_DVBxA registers.
@{ */
#define LTC3676_BUCK_REF_SELECT      5
/*! @} */

/*! @name LTC3676_REG_DVBxB SETTINGS
    Bit position of single bit settings in the LTC3676_REG_DVBxB register.
@{ */
#define LTC3676_BUCK_PG_MASK         5
/*! @} */

/*! @name LTC3676_REG_MSKIRQ SETTINGS
    Bit position of single bit settings in the LTC3676_REG_MSKIRQ register.
@{ */
#define LTC3676_MASK_OT_SHUTDOWN     6
#define LTC3676_MASK_OT_WARNING      5
#define LTC3676_MASK_UV_SHUTDOWN     4
#define LTC3676_MASK_UV_WARNING      3
#define LTC3676_MASK_PG_TIMEOUT      2
#define LTC3676_MASK_PB_STATUS       0
/*! @} */

/*! @name LTC3676_REG_MSKPG SETTINGS
    Bit position of single bit settings in the LTC3676_REG_MSKPG register.
@{ */
#define LTC3676_ENABLE_PG_LDO4       7
#define LTC3676_ENABLE_PG_LDO3       6
#define LTC3676_ENABLE_PG_LDO2       5
#define LTC3676_ENABLE_PG_BUCK4      3
#define LTC3676_ENABLE_PG_BUCK3      2
#define LTC3676_ENABLE_PG_BUCK2      1
#define LTC3676_ENABLE_PG_BUCK1      0
/*! @} */

/*! @name LTC3676_REG_USER SETTINGS
    Bit position of single bit settings in the LTC3676_REG_USER register.
@{ */
#define LTC3676_USER_7               7
#define LTC3676_USER_6               6
#define LTC3676_USER_5               5
#define LTC3676_USER_4               4
#define LTC3676_USER_3               3
#define LTC3676_USER_2               2
#define LTC3676_USER_1               1
#define LTC3676_USER_0               0
/*! @} */

/*! @name LTC3676_REG_IRQSTAT SETTINGS
    Bit position of status bits in the LTC3676_REG_IRQSTAT register.
@{ */
#define LTC3676_IRQ_OT_SHUTDOWN      6
#define LTC3676_IRQ_OT_WARNING       5
#define LTC3676_IRQ_UV_SHUTDOWN      4
#define LTC3676_IRQ_UV_WARNING       3
#define LTC3676_IRQ_PG_TIMEOUT       2
#define LTC3676_IRQ_HARD_RESET       1
#define LTC3676_IRQ_PB_STATUS        0
/*! @} */

/*! @name LTC3676_REG_PGSTATL SETTINGS
    Bit position of status bits in the LTC3676_REG_PGSTATL register.
@{ */
#define LTC3676_PGL_LDO4             7
#define LTC3676_PGL_LDO3             6
#define LTC3676_PGL_LDO2             5
#define LTC3676_PGL_LDO1             4
#define LTC3676_PGL_BUCK4            3
#define LTC3676_PGL_BUCK3            2
#define LTC3676_PGL_BUCK2            1
#define LTC3676_PGL_BUCK1            0
/*! @} */

/*! @name LTC3676_REG_PGSTATRT SETTINGS
    Bit position of status bits in the LTC3676_REG_PGSTATRT register.
@{ */
#define LTC3676_PGRT_LDO4            7
#define LTC3676_PGRT_LDO3            6
#define LTC3676_PGRT_LDO2            5
#define LTC3676_PGRT_LDO1            4
#define LTC3676_PGRT_BUCK4           3
#define LTC3676_PGRT_BUCK3           2
#define LTC3676_PGRT_BUCK2           1
#define LTC3676_PGRT_BUCK1           0
/*! @} */

/*! @name LTC3676 MASK SETTINGS
    Bitwise AND with register data to determine present setting.
@{ */
#define LTC3676_BUCK_SEQ_MASK(num)  (0x03<<((num)*2)-2)
#define LTC3676_LDO_SEQ_MASK(num)   (0x03<<((num-2)*2))
#define LTC3676_BUCK_MODE_MASK       0x60
#define LTC3676_BUCK4_SEQ_MASK       0xC0
#define LTC3676_BUCK3_SEQ_MASK       0x30
#define LTC3676_BUCK2_SEQ_MASK       0x0C
#define LTC3676_BUCK1_SEQ_MASK       0x03
#define LTC3676_LDO4_SEQ_MASK        0x30
#define LTC3676_LDO3_SEQ_MASK        0x0C
#define LTC3676_LDO2_SEQ_MASK        0x03
#define LTC3676_UV_WARN_THRESH_MASK  0x1C
#define LTC3676_OT_WARN_LEVEL_MASK   0x03
#define LTC3676_FB_REF_MASK          0x1F
/*! @} */

/*! @name LTC3676-1 ONLY MASK SETTINGS
    Bitwise AND with register data to determine present setting.
@{ */
#define LTC3676_1_LDO4_VOLTAGE_MASK    0x18
/*! @} */

//! Reads an 8-bit register from the LTC3676 using the standard repeated start format.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_register_read(uint8_t i2c_address,       //!< I2C address of the LTC3676.
                             uint8_t register_address,  //!< Address of the LTC3676 register to be read. This is also known as the "command byte".
                             uint8_t *register_data     //!< returns 8-bit value read from the LTC3676 register.
                            );

//! Writes to an 8-bit register inside the LTC3676 using the standard I2C repeated start format.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_register_write(uint8_t i2c_address,      //!< I2C address of the LTC3676.
                              uint8_t register_address, //!< Address of the LTC3676 register to be overwritten.  This is also known as the "command byte".
                              uint8_t register_data     //!< Value that will be written to the register.
                             );


//! Sets any bit inside the LTC3676 using the standard I2C repeated start format.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_bit_set(uint8_t i2c_address,      //!< I2C address of the LTC3676.
                       uint8_t register_address, //!< Address of the LTC3676 register to be overwritten.  This is also known as the "command byte".
                       uint8_t bit_number        //!< Bit location (0-7) to be written.
                      );

//! Clears any bit inside the LTC3676 using the standard I2C repeated start format.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_bit_clear(uint8_t i2c_address,      //!< I2C address of the LTC3676.
                         uint8_t register_address, //!< Address of the LTC3676 register to be overwritten.  This is also known as the "command byte".
                         uint8_t bit_number       //!< Address of the bit to set within the register.
                        );

//! Writes any bit inside the LTC3676 using the standard I2C repeated start format.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_bit_write(uint8_t i2c_address,       //!< I2C address of the LTC3676.
                         uint8_t register_address,  //!< Address of the LTC3676 register to be written.
                         uint8_t bit_number,        //!< Bit location (0-7) to be written.
                         uint8_t bit_data           //!< Bit data (0 or 1) to be written.
                        );

//! Reads the value of any bit in any register or the LTC3676.
//! @return Returns the bit value at the passed register subaddress and bit location.
uint8_t LTC3676_bit_is_set(uint8_t i2c_address,        //!< I2C address of the LTC3676.
                           uint8_t register_address,   //!< Address of the LTC3676 register to be read. This is also known as the "command byte".
                           uint8_t bit_number          //!< Bit location (0-7) to be read.
                          );

//! Sets the output voltage of any buck.
//! @return Returns the new buck output voltage, the closest allowable voltage to the user's chosen voltage.
float LTC3676_set_buck_output_voltage(uint8_t i2c_address, //!< I2C address of the LTC3676.
                                      uint8_t register_address, //!< Address of the LTC3676 Buck FB reference register to be written.
                                      float output_voltage //!< New output voltage to set, in mV. Assumes a valid voltage for the feedback resistors.
                                     );

//! Writes the Feedback Reference Voltage of any buck.
//! @return Returns the new Feedback Reference Input Voltage, the closest allowable voltage to the user's chosen voltage.
float LTC3676_set_buck_fb_ref(uint8_t i2c_address, //!< I2C address of the LTC3676.
                              uint8_t register_address, //!< Address of the LTC3676 Buck FB reference register to be written.
                              float fb_ref_voltage //!< New Feedback Reference voltage to set in mV.  Values between 412.5 and 800 are allowed.
                             );

//! Calculates the maximum output voltage of any buck in mV based on the feedback resistors.
//! @return Returns the maximum possible output voltage for the selected buck.
float LTC3676_buck_vout_max(uint8_t buck_number //!< Number (1-4) of buck.
                           );

//! Calculates the minimum output voltage of any buck in mV based on the feedback resistors.
//! @return Returns the minimum possible output voltage for the selected buck.
float LTC3676_buck_vout_min(uint8_t buck_number //!< Number (1-4) of buck.
                           );

//! Selects the reference for the specified buck regulator(s).
//! @return Returns the state of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_select_buck_reference(uint8_t i2c_address, //!< I2C address of the LTC3676.
                                     uint8_t buck_number, //!< Number (1-4) of the buck whose reference will be selected. Enter 0xFF for all bucks.
                                     int8_t ref_char //!< Reference to select (A or B).
                                    );

//! Sets the switching mode for the specified Buck regulator.
//! @return Returns the state of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_set_buck_mode(uint8_t i2c_address, //!< I2C address of the LTC3676.
                             uint8_t buck_number, //!< Number (1-4) of the buck to set. Enter 0xFF for all bucks.
                             uint8_t mode //!< Switching mode to be set (0=pulse skipping, 1=burst, 2=forced continuous)
                            );

//! Sets the start-up mode for all bucks.
//! @return Returns the state of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_set_startup_mode(uint8_t i2c_address, //!< I2C address of the LTC3676.
                                uint8_t buck_number, //!< Number (1-4) of the buck to set. Enter 0xFF for all bucks.
                                uint8_t startup_bit //!< Data to write to start-up bit (0=enable at any output, 1=enable only if output <300mV)
                               );

//! Sets the PGOOD mask bit in the DVBxB register for all bucks.
//! @return Returns the state of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_set_buck_pgood_mask(uint8_t i2c_address, //!< I2C address of the LTC3676.
                                   uint8_t buck_number, //!< Number (1-4) of the buck to set.
                                   uint8_t pgood_bit //!< Data to write to PGOOD bit (0=PGOOD low when slewing, 1=PGOOD not forced low when slewing)
                                  );

//! Writes a new UV warning threshold voltage in the CTRL register.
//! @return Returns the state of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_set_uv_warning_threshold(uint8_t i2c_address, //!< I2C address of the LTC3676.
                                        float uv_warning_threshold //!< New UV warning threshold to set, in volts.  Values between 2.7 and 3.4 are allowed.
                                       );

//! Writes the UV warning threshold of any buck.
//! @return Returns the state of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_set_overtemp_warning_level(uint8_t i2c_address, //!< I2C address of the LTC3676.
    uint8_t ot_warning_level //!< New overtemp warning level to set, degrees C below Overtemperature fault level.  Values 10, 20, 30, and 40 are allowed.
                                         );

//! Sets LDO4 output voltage on the LTC3676-1.
//! @return Returns the state of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_1_set_ldo4_voltage(uint8_t i2c_address, //!< I2C address of the LTC3676-1.
                                  uint8_t ldo4_output_voltage_code //!< Voltage code for new LDO4 output voltage: 0=1.2V, 1=2.5V, 2=2.8V, 3=3.0V
                                 );

//! Sets the Sequence Down bits for any buck in the SQD1 register.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_set_buck_sequence_down(uint8_t i2c_address, //!< I2C address of the LTC3676.
                                      uint8_t buck_number, //!< Number (1-4) of the buck to sequence.
                                      uint8_t sequence_phase //!< Sequence down phase: 0=with WAKE, 1=WAKE+100ms, 2=WAKE+200ms, 3=WAKE+300ms.
                                     );

//! Sets the Sequence Down bits for any buck in the SQD1 register.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_set_ldo_sequence_down(uint8_t i2c_address, //!< I2C address of the LTC3676.
                                     uint8_t ldo_number, //!< Number (2-4) of the LDO to sequence.
                                     uint8_t sequence_phase //!< Sequence down phase: 0=with WAKE, 1=WAKE+100ms, 2=WAKE+200ms, 3=WAKE+300ms.
                                    );
