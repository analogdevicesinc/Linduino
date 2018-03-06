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

I2C DATA FORMAT (MSB FIRST)

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

//! @ingroup Switching_Regulators
//! @{
//! @defgroup LTC3589 LTC3589: 8-Output Regulator with Sequencing and I2C
//! @}

/*! @file
    @ingroup LTC3589
    Library for LTC3589: 8-Output Regulator with Sequencing and I2C
*/

#include <Arduino.h>
#include <stdint.h>
#include <math.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LTC3589.h"

// Reads an 8-bit register from the LTC3589 using the standard repeated start format.
int8_t LTC3589_register_read(uint8_t i2c_address, uint8_t register_address, uint8_t *register_data)
{
  int8_t ack = 0;
  ack = i2c_read_byte_data(i2c_address, register_address, register_data);
  return(ack);
}

// Writes to an 8-bit register inside the LTC3589 using the standard I2C repeated start format.
int8_t LTC3589_register_write(uint8_t i2c_address, uint8_t register_address, uint8_t register_data)
{
  int8_t ack = 0;
  ack = i2c_write_byte_data(i2c_address, register_address, register_data);
  return(ack);
}

// Sets any bit inside the LTC3589 using the standard I2C repeated start format.
int8_t LTC3589_bit_set(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number)
{
  uint8_t register_data;
  uint8_t bit_mask;
  int8_t ack = 0;
  bit_mask = 0x01 << bit_number;
  ack |= LTC3589_register_read(i2c_address, register_address, &register_data);  //Read register
  register_data = register_data | bit_mask;  //Set the bit at bit_address
  ack |= LTC3589_register_write(i2c_address, register_address, register_data);  //Write new data to register
  return(ack);
}

// Clears any bit inside the LTC3589 using the standard I2C repeated start format.
int8_t LTC3589_bit_clear(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number)
{
  uint8_t register_data;
  uint8_t bit_mask;
  int8_t ack = 0;
  bit_mask = 0x01 << bit_number;
  ack |= LTC3589_register_read(i2c_address, register_address, &register_data);  //Read register
  register_data = register_data & (~bit_mask);  //Clears the bit at bit_address
  ack |= LTC3589_register_write(i2c_address, register_address, register_data);  //Write new data to register
  return(ack);
}

// Writes any bit inside the LTC3589 using the standard I2C repeated start format.
int8_t LTC3589_bit_write(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number, uint8_t bit_data)
{
  uint8_t register_data;
  uint8_t bit_mask;
  int8_t ack = 0;
  bit_mask = 0x01 << bit_number;
  ack |= LTC3589_register_read(i2c_address, register_address, &register_data);  //Read register
  register_data = register_data & (~bit_mask);  //Clears the bit at bit_address
  register_data = register_data | ((bit_data & 0x01) << bit_number);  //Writes the new bit
  ack |= LTC3589_register_write(i2c_address, register_address, register_data);  //Write new data to register
  return(ack);
}

// Reads the value of any bit in any register or the LTC3589.
uint8_t LTC3589_bit_is_set(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number)
{
  uint8_t register_data;
  int8_t ack = 0;
  ack |= LTC3589_register_read(i2c_address, register_address, &register_data);
  register_data = register_data >> bit_number;  //Clears everything but the bit of interest
  return (register_data & 0x01);
}


// Sets the output voltage of LDO2.
float LTC3589_set_ldo2_output_voltage(uint8_t i2c_address, uint8_t register_address, float output_voltage)
{
  uint8_t register_data;
  float fb_ref = 0;
  int8_t nack = 0;
  if (register_address == 0x32 | register_address == 0x33)
  {
    fb_ref = output_voltage / (1 + LTC3589_RES_LDO2_RTOP/LTC3589_RES_LDO2_RBOT);  //Generates desired FB reference input for given output voltage.
    return (1 + LTC3589_RES_LDO2_RTOP/LTC3589_RES_LDO2_RBOT) * LTC3589_set_ldo2_fb_ref(i2c_address, register_address, fb_ref);  //Writes new buck output fb refernce.
  }
}

// Sets the output voltage of any buck.
float LTC3589_set_buck_output_voltage(uint8_t i2c_address, uint8_t register_address, float output_voltage)
{
  uint8_t register_data;
  float fb_ref = 0;
  int8_t nack = 0;
  if (register_address == 0x23 | register_address == 0x24)
  {
    fb_ref = output_voltage / (1 + LTC3589_RES_BUCK1_RTOP/LTC3589_RES_BUCK1_RBOT);  //Generates desired FB reference input for given output voltage.
    return (1 + LTC3589_RES_BUCK1_RTOP/LTC3589_RES_BUCK1_RBOT) * LTC3589_set_buck_fb_ref(i2c_address, register_address, fb_ref);  //Writes new buck output fb refernce.
  }
  if (register_address == 0x26 | register_address == 0x27)
  {
    fb_ref = output_voltage / (1 + LTC3589_RES_BUCK2_RTOP/LTC3589_RES_BUCK2_RBOT);  //Generates desired FB reference input for given output voltage.
    return (1 + LTC3589_RES_BUCK2_RTOP/LTC3589_RES_BUCK2_RBOT) * LTC3589_set_buck_fb_ref(i2c_address, register_address, fb_ref);  //Writes new buck output fb refernce.
  }
  if (register_address == 0x29 | register_address == 0x2A)
  {
    fb_ref = output_voltage / (1 + LTC3589_RES_BUCK3_RTOP/LTC3589_RES_BUCK3_RBOT);  //Generates desired FB reference input for given output voltage.
    return (1 + LTC3589_RES_BUCK3_RTOP/LTC3589_RES_BUCK3_RBOT) * LTC3589_set_buck_fb_ref(i2c_address, register_address, fb_ref);  //Writes new buck output fb refernce.
  }
}

// Writes the Feedback Reference Voltage of LDO2.
float LTC3589_set_ldo2_fb_ref(uint8_t i2c_address, uint8_t register_address, float fb_ref_voltage)
{
  uint8_t register_data;
  uint8_t fb_ref_data = 0;
  int8_t nack = 0;
  fb_ref_data = (uint8_t) ((fb_ref_voltage + 6.25 - 362.5) / 12.5);  //Generates 5-bit code for closest allowable value to user's chosen reference voltage.
  nack |= LTC3589_register_read(i2c_address, register_address, &register_data); //Read current register value
  register_data = register_data & (~LTC3589_FB_REF_MASK); //Clears existing feedback reference bits
  register_data = register_data | fb_ref_data;
  nack |= LTC3589_register_write(i2c_address, register_address, register_data);  //Writes new register value
  nack |= LTC3589_set_go_bits(i2c_address);
  return (362.5 + (fb_ref_data * 12.5));
}

// Writes the Feedback Reference Voltage of any buck.
float LTC3589_set_buck_fb_ref(uint8_t i2c_address, uint8_t register_address, float fb_ref_voltage)
{
  uint8_t register_data;
  uint8_t fb_ref_data = 0;
  int8_t nack = 0;
  fb_ref_data = (uint8_t) ((fb_ref_voltage + 6.25 - 362.5) / 12.5);  //Generates 5-bit code for closest allowable value to user's chosen reference voltage.
  nack |= LTC3589_register_read(i2c_address, register_address, &register_data); //Read current register value
  register_data = register_data & (~LTC3589_FB_REF_MASK); //Clears existing feedback reference bits
  register_data = register_data | fb_ref_data;
  nack |= LTC3589_register_write(i2c_address, register_address, register_data);  //Writes new register value
  nack |= LTC3589_set_go_bits(i2c_address);
  return (362.5 + (fb_ref_data * 12.5));
}

// Calculates the maximum output voltage of LDO2 in mV based on the feedback resistors.
float LTC3589_ldo2_vout_max()
{
  return ((1 + (LTC3589_RES_LDO2_RTOP/LTC3589_RES_LDO2_RBOT)) * 750);
}

// Calculates the minimum output voltage of LDO2 mV based on the feedback resistors.
float LTC3589_ldo2_vout_min()
{
  return ((1 + (LTC3589_RES_LDO2_RTOP/LTC3589_RES_LDO2_RBOT)) * 362.5);
}

// Calculates the maximum output voltage of any buck in mV based on the feedback resistors.
float LTC3589_buck_vout_max(uint8_t buck_number)
{
  switch (buck_number)
  {
    case 1:
      return ((1 + (LTC3589_RES_BUCK1_RTOP/LTC3589_RES_BUCK1_RBOT)) * 750);
      break;
    case 2:
      return ((1 + (LTC3589_RES_BUCK2_RTOP/LTC3589_RES_BUCK2_RBOT)) * 750);
      break;
    case 3:
      return ((1 + (LTC3589_RES_BUCK3_RTOP/LTC3589_RES_BUCK3_RBOT)) * 750);
      break;
  }
}

// Calculates the minimum output voltage of any buck in mV based on the feedback resistors.
float LTC3589_buck_vout_min(uint8_t buck_number)
{
  switch (buck_number)
  {
    case 1:
      return ((1 + LTC3589_RES_BUCK1_RTOP/LTC3589_RES_BUCK1_RBOT) * 362.5);
      break;
    case 2:
      return ((1 + LTC3589_RES_BUCK2_RTOP/LTC3589_RES_BUCK2_RBOT) * 362.5);
      break;
    case 3:
      return ((1 + LTC3589_RES_BUCK3_RTOP/LTC3589_RES_BUCK3_RBOT) * 362.5);
      break;
  }
}

// Selects the reference for LDO2.
int8_t LTC3589_select_ldo2_reference(uint8_t i2c_address, int8_t ref_char)
{
  int8_t ack = 0;
  if (ref_char == '1')
    ack |= LTC3589_bit_clear(i2c_address, LTC3589_REG_VCCR, LTC3589_LDO2_REF_SELECT);
  else if (ref_char == '2')
    ack |= LTC3589_bit_set(i2c_address, LTC3589_REG_VCCR, LTC3589_LDO2_REF_SELECT);
  ack |= LTC3589_set_go_bits(i2c_address);
  return(ack);
}

// Selects the reference for the specified buck regulator(s).
int8_t LTC3589_select_buck_reference(uint8_t i2c_address, uint8_t buck_number, int8_t ref_char)
{
  int8_t ack = 0;
  if (buck_number == 0xFF)
  {
    ack |= LTC3589_select_buck_reference(i2c_address, 1, ref_char);
    ack |= LTC3589_select_buck_reference(i2c_address, 2, ref_char);
    ack |= LTC3589_select_buck_reference(i2c_address, 3, ref_char);
    return(ack);
  }
  if (ref_char == '1')
    ack |= LTC3589_bit_clear(i2c_address, LTC3589_REG_VCCR, (buck_number*2)-1);
  else if (ref_char == '2')
    ack |= LTC3589_bit_set(i2c_address, LTC3589_REG_VCCR, (buck_number*2)-1);
  ack |= LTC3589_set_go_bits(i2c_address);
  return(ack);
}

// Sets the switching mode for the specified buck regulator(s).
int8_t LTC3589_set_buck_mode(uint8_t i2c_address, uint8_t buck_number, uint8_t mode)
{
  uint8_t register_data;
  int8_t ack = 0;
  if (buck_number == 0xFF)
  {
    ack |= LTC3589_set_buck_mode(i2c_address, 0x01, mode);
    ack |= LTC3589_set_buck_mode(i2c_address, 0x02, mode);
    ack |= LTC3589_set_buck_mode(i2c_address, 0x03, mode);
    return(ack);
  }
  ack |= LTC3589_register_read(i2c_address, LTC3589_REG_SCR1, &register_data);  //Read register
  register_data = register_data & (~LTC3589_BUCK_MODE_MASK(buck_number));  //Clears the Mode bits in the Buck register
  register_data = register_data | (mode << ((buck_number*2)-2));
  ack |= LTC3589_register_write(i2c_address, LTC3589_REG_SCR1, register_data);  //Writes new register value
  return(ack);
}

// Sets the switching mode for the buck-boost regulator.
int8_t LTC3589_set_buckboost_mode(uint8_t i2c_address, uint8_t mode)
{
  uint8_t register_data;
  int8_t ack = 0;
  ack |= LTC3589_bit_write(i2c_address, LTC3589_REG_SCR1, LTC3589_BUCKBOOST_MODE, mode);  //Writes new mode-bit value
  return(ack);
}

// Sets the start-up mode for all regulators.
int8_t LTC3589_set_startup_mode(uint8_t i2c_address, uint8_t startup_bit)
{
  int8_t ack = 0;
  ack |= LTC3589_bit_write(LTC3589_I2C_ADDRESS, LTC3589_REG_SCR2, LTC3589_BUCK1_STARTUP, startup_bit);
  ack |= LTC3589_bit_write(LTC3589_I2C_ADDRESS, LTC3589_REG_SCR2, LTC3589_BUCK2_STARTUP, startup_bit);
  ack |= LTC3589_bit_write(LTC3589_I2C_ADDRESS, LTC3589_REG_SCR2, LTC3589_BUCK3_STARTUP, startup_bit);
  ack |= LTC3589_bit_write(LTC3589_I2C_ADDRESS, LTC3589_REG_SCR2, LTC3589_BB_STARTUP, startup_bit);
  ack |= LTC3589_bit_write(LTC3589_I2C_ADDRESS, LTC3589_REG_SCR2, LTC3589_LDO2_STARTUP, startup_bit);
  ack |= LTC3589_bit_write(LTC3589_I2C_ADDRESS, LTC3589_REG_SCR2, LTC3589_LDO3_STARTUP, startup_bit);
  ack |= LTC3589_bit_write(LTC3589_I2C_ADDRESS, LTC3589_REG_SCR2, LTC3589_LDO4_STARTUP, startup_bit);
  return(ack);
}

// Sets the PGOOD mask bit for all bucks and LDO2.
int8_t LTC3589_set_pgood_mask(uint8_t i2c_address, uint8_t pgood_bit)
{
  int8_t ack = 0;
  ack |= LTC3589_bit_write(LTC3589_I2C_ADDRESS, LTC3589_REG_B1DTV1, LTC3589_BUCK_PG_MASK, pgood_bit);
  ack |= LTC3589_bit_write(LTC3589_I2C_ADDRESS, LTC3589_REG_B2DTV1, LTC3589_BUCK_PG_MASK, pgood_bit);
  ack |= LTC3589_bit_write(LTC3589_I2C_ADDRESS, LTC3589_REG_B3DTV1, LTC3589_BUCK_PG_MASK, pgood_bit);
  ack |= LTC3589_bit_write(LTC3589_I2C_ADDRESS, LTC3589_REG_L2DTV1, LTC3589_LDO2_PG_MASK, pgood_bit);
  return(ack);
}

// Sets LDO4 output voltage on the LTC3589.
int8_t LTC3589_set_ldo4_voltage(uint8_t i2c_address, uint8_t ldo4_output_voltage_code)
{
  int8_t ack = 0;
  uint8_t register_data;
  ack |= LTC3589_register_read(i2c_address, LTC3589_REG_L2DTV2, &register_data);  //Read register
  register_data = register_data & (~LTC3589_LDO4_VOLTAGE_MASK);  //Clears the LDO4 Output Voltage bits in the L2DTV2 register
  register_data = register_data | (ldo4_output_voltage_code << 5);
  ack |= LTC3589_register_write(i2c_address, LTC3589_REG_L2DTV2, register_data);  //Writes new register value
  return(ack);
}

// Sets the dynamic reference slew rate for the regulator(s).
int8_t LTC3589_set_regulator_slew(uint8_t i2c_address, uint8_t regulator_mask, uint8_t slew_rate)
{
  uint8_t register_data;
  uint8_t bit_shift = log(regulator_mask/3) / log(2);
  int8_t ack = 0;
  if (regulator_mask == 0xFF)
  {
    ack |= LTC3589_set_regulator_slew(i2c_address, LTC3589_BUCK1_SLEW_MASK, slew_rate);
    ack |= LTC3589_set_regulator_slew(i2c_address, LTC3589_BUCK2_SLEW_MASK, slew_rate);
    ack |= LTC3589_set_regulator_slew(i2c_address, LTC3589_BUCK3_SLEW_MASK, slew_rate);
    ack |= LTC3589_set_regulator_slew(i2c_address, LTC3589_LDO2_SLEW_MASK, slew_rate);
    return(ack);
  }
  ack |= LTC3589_register_read(i2c_address, LTC3589_REG_VRRCR, &register_data);  //Read register
  register_data = register_data & (~regulator_mask);  //Clears the Mode bits in the Buck register
  register_data = register_data | (slew_rate << bit_shift);
  ack |= LTC3589_register_write(i2c_address, LTC3589_REG_VRRCR, register_data);  //Writes new register value
  return(ack);
}

// Sets the switch DV/DT control for the buck regulators.
int8_t LTC3589_set_switch_dvdt_control(uint8_t i2c_address, uint8_t dvdt_control_bits)
{
  uint8_t register_data;
  int8_t ack = 0;
  ack |= LTC3589_register_read(i2c_address, LTC3589_REG_B1DTV1, &register_data);
  register_data = register_data & (~LTC3589_BUCK_DVDT_MASK);
  register_data = register_data | (dvdt_control_bits << 6);
  ack |= LTC3589_register_write(i2c_address, LTC3589_REG_B1DTV1, register_data);
  return(ack);
}

// Sets all four GO bits in the VCCR register.
int8_t LTC3589_set_go_bits(uint8_t i2c_address)
{
  int8_t ack = 0;
  ack |= LTC3589_bit_set(i2c_address, LTC3589_REG_VCCR, LTC3589_BUCK1_GO);
  ack |= LTC3589_bit_set(i2c_address, LTC3589_REG_VCCR, LTC3589_BUCK2_GO);
  ack |= LTC3589_bit_set(i2c_address, LTC3589_REG_VCCR, LTC3589_BUCK3_GO);
  ack |= LTC3589_bit_set(i2c_address, LTC3589_REG_VCCR, LTC3589_LDO2_GO);
  return(ack);
}
