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
    Library for LTC3676: Power management solution for application processors
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LTC3676.h"

// Reads an 8-bit register from the LTC3676 using the standard repeated start format.
int8_t LTC3676_register_read(uint8_t i2c_address, uint8_t register_address, uint8_t *register_data)
{
  int8_t ack = 0;
  ack = i2c_read_byte_data(i2c_address, register_address, register_data);
  return(ack);
}

// Writes to an 8-bit register inside the LTC3676 using the standard I2C repeated start format.
int8_t LTC3676_register_write(uint8_t i2c_address, uint8_t register_address, uint8_t register_data)
{
  int8_t ack = 0;
  ack = i2c_write_byte_data(i2c_address, register_address, register_data);
  return(ack);
}


// Sets a bit within any register inside the LTC3676 using the standard I2C repeated start format.
int8_t LTC3676_bit_set(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number)
{
  uint8_t register_data;
  uint8_t bit_mask;
  int8_t ack = 0;
  bit_mask = 0x01 << bit_number;
  ack |= LTC3676_register_read(i2c_address, register_address, &register_data);  //Read register
  register_data = register_data | bit_mask;  //Set the bit at bit_address
  ack |= LTC3676_register_write(i2c_address, register_address, register_data);  //Write new data to register
  return(ack);
}

// Sets a bit within any register inside the LTC3676 using the standard I2C repeated start format.
int8_t LTC3676_bit_clear(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number)
{
  uint8_t register_data;
  uint8_t bit_mask;
  int8_t ack = 0;
  bit_mask = 0x01 << bit_number;
  ack |= LTC3676_register_read(i2c_address, register_address, &register_data);  //Read register
  register_data = register_data & (~bit_mask);  //Clears the bit at bit_address
  ack |= LTC3676_register_write(i2c_address, register_address, register_data);  //Write new data to register
  return(ack);
}

//! Writes any bit inside the LTC3676 using the standard I2C repeated start format.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_bit_write(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number, uint8_t bit_data)
{
  uint8_t register_data;
  uint8_t bit_mask;
  int8_t ack = 0;
  bit_mask = 0x01 << bit_number;
  ack |= LTC3676_register_read(i2c_address, register_address, &register_data);  //Read register
  register_data = register_data & (~bit_mask);  //Clears the bit at bit_address
  register_data = register_data | ((bit_data & 0x01) << bit_number);  //Writes the new bit
  ack |= LTC3676_register_write(i2c_address, register_address, register_data);  //Write new data to register
  return(ack);
}

//! Reads the value of any bit in any register or the LTC3676.
//! @return Returns the bit value at the passed register subaddress and bit location.
uint8_t LTC3676_bit_is_set(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number)
{
  uint8_t register_data;
  int8_t ack = 0;
  ack |= LTC3676_register_read(i2c_address, register_address, &register_data);
  register_data = register_data >> bit_number;  //Clears everything but the bit of interest
  return (register_data & 0x01);
}

//! Sets the output voltage of any buck.
float LTC3676_set_buck_output_voltage(uint8_t i2c_address, uint8_t register_address, float output_voltage)
{
  uint8_t register_data;
  float fb_ref = 0;
  int8_t nack = 0;
  if (register_address == 0x0A | register_address == 0x0B)
  {
    fb_ref = output_voltage / (1 + LTC3676_RES_BUCK1_RTOP/LTC3676_RES_BUCK1_RBOT);  //Generates desired FB reference input for given output voltage.
    return (1 + LTC3676_RES_BUCK1_RTOP/LTC3676_RES_BUCK1_RBOT) * LTC3676_set_buck_fb_ref(i2c_address, register_address, fb_ref);  //Writes new buck output fb refernce.
  }
  if (register_address == 0x0C | register_address == 0x0D)
  {
    fb_ref = output_voltage / (1 + LTC3676_RES_BUCK2_RTOP/LTC3676_RES_BUCK2_RBOT);  //Generates desired FB reference input for given output voltage.
    return (1 + LTC3676_RES_BUCK2_RTOP/LTC3676_RES_BUCK2_RBOT) * LTC3676_set_buck_fb_ref(i2c_address, register_address, fb_ref);  //Writes new buck output fb refernce.
  }
  if (register_address == 0x0E | register_address == 0x0F)
  {
    fb_ref = output_voltage / (1 + LTC3676_RES_BUCK3_RTOP/LTC3676_RES_BUCK3_RBOT);  //Generates desired FB reference input for given output voltage.
    return (1 + LTC3676_RES_BUCK3_RTOP/LTC3676_RES_BUCK3_RBOT) * LTC3676_set_buck_fb_ref(i2c_address, register_address, fb_ref);  //Writes new buck output fb refernce.
  }
  if (register_address == 0x10 | register_address == 0x11)
  {
    fb_ref = output_voltage / (1 + LTC3676_RES_BUCK4_RTOP/LTC3676_RES_BUCK4_RBOT);  //Generates desired FB reference input for given output voltage.
    return (1 + LTC3676_RES_BUCK4_RTOP/LTC3676_RES_BUCK4_RBOT) * LTC3676_set_buck_fb_ref(i2c_address, register_address, fb_ref);  //Writes new buck output fb refernce.
  }
}

//! Writes the Feedback Reference Voltage of any buck.
//! @return Returns the new Feedback Reference Input Voltage, the closest allowable voltage to the user's chosen voltage.
float LTC3676_set_buck_fb_ref(uint8_t i2c_address, uint8_t register_address, float fb_ref_voltage)
{
  uint8_t register_data;
  uint8_t fb_ref_data = 0;
  int8_t nack = 0;
  fb_ref_data = (uint8_t) ((fb_ref_voltage + 6.25 - 412.5) / 12.5);  //Generates 5-bit code for closest allowable value to user's chosen reference voltage.
  nack |= LTC3676_register_read(i2c_address, register_address, &register_data); //Read current register value
  register_data = register_data & (~LTC3676_FB_REF_MASK); //Clears existing feedback reference bits
  register_data = register_data | fb_ref_data;
  nack |= LTC3676_register_write(i2c_address, register_address, register_data);  //Writes new register value
  return (412.5 + (fb_ref_data * 12.5));
}

//! Calculates the maximum output voltage of any buck in mV based on the feedback resistors.
//! @return Returns the maximum possible output voltage for the selected buck.
float LTC3676_buck_vout_max(uint8_t buck_number)
{
  switch (buck_number)
  {
    case 1:
      return ((1 + (LTC3676_RES_BUCK1_RTOP/LTC3676_RES_BUCK1_RBOT)) * 800);
      break;
    case 2:
      return ((1 + (LTC3676_RES_BUCK2_RTOP/LTC3676_RES_BUCK2_RBOT)) * 800);
      break;
    case 3:
      return ((1 + (LTC3676_RES_BUCK3_RTOP/LTC3676_RES_BUCK3_RBOT)) * 800);
      break;
    case 4:
      return ((1 + (LTC3676_RES_BUCK4_RTOP/LTC3676_RES_BUCK4_RBOT)) * 800);
      break;
  }
}

//! Calculates the minimum output voltage of any buck in mV based on the feedback resistors.
//! @return Returns the minimum possible output voltage for the selected buck.
float LTC3676_buck_vout_min(uint8_t buck_number)
{
  switch (buck_number)
  {
    case 1:
      return ((1 + LTC3676_RES_BUCK1_RTOP/LTC3676_RES_BUCK1_RBOT) * 412.5);
      break;
    case 2:
      return ((1 + LTC3676_RES_BUCK2_RTOP/LTC3676_RES_BUCK2_RBOT) * 412.5);
      break;
    case 3:
      return ((1 + LTC3676_RES_BUCK3_RTOP/LTC3676_RES_BUCK3_RBOT) * 412.5);
      break;
    case 4:
      return ((1 + LTC3676_RES_BUCK4_RTOP/LTC3676_RES_BUCK4_RBOT) * 412.5);
      break;
  }
}

//! Selects the reference for the specified buck regulator(s).
//! @return Returns the state of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_select_buck_reference(uint8_t i2c_address, uint8_t buck_number, int8_t ref_char)
{
  uint8_t user_register;
  int8_t ack = 0;
  if (buck_number == 0xFF)
  {
    ack |= LTC3676_select_buck_reference(i2c_address, 1, ref_char);
    ack |= LTC3676_select_buck_reference(i2c_address, 2, ref_char);
    ack |= LTC3676_select_buck_reference(i2c_address, 3, ref_char);
    ack |= LTC3676_select_buck_reference(i2c_address, 4, ref_char);
    return(ack);
  }
  user_register = (buck_number*2) + 8;  //Converts selection to appropriate register address.
  if (ref_char == 'A' || ref_char == 'a')
    ack |= LTC3676_bit_clear(i2c_address, user_register, 5);
  else if (ref_char == 'B' || ref_char == 'b')
    ack |= LTC3676_bit_set(i2c_address, user_register, 5);
  return(ack);
}

//! Sets the switching mode for the specified Buck regulator.
//! @return Returns the state of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_set_buck_mode(uint8_t i2c_address, uint8_t buck_number, uint8_t mode)
{
  uint8_t register_data;
  int8_t ack = 0;
  if (buck_number == 0xFF)
  {
    ack |= LTC3676_set_buck_mode(i2c_address, 0x01, mode);
    ack |= LTC3676_set_buck_mode(i2c_address, 0x02, mode);
    ack |= LTC3676_set_buck_mode(i2c_address, 0x03, mode);
    ack |= LTC3676_set_buck_mode(i2c_address, 0x04, mode);
    return(ack);
  }
  ack |= LTC3676_register_read(i2c_address, buck_number, &register_data);  //Read register
  register_data = register_data & (~LTC3676_BUCK_MODE_MASK);  //Clears the Mode bits in the Buck register
  register_data = register_data | (mode << 5);
  ack |= LTC3676_register_write(i2c_address, buck_number, register_data);  //Writes new register value
  return(ack);
}

//! Sets the start-up mode for all bucks.
//! @return Returns the state of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_set_startup_mode(uint8_t i2c_address, uint8_t buck_number, uint8_t startup_bit)
{
  int8_t ack = 0;
  if (buck_number == 0xFF)
  {
    ack |= LTC3676_bit_write(LTC3676_I2C_ADDRESS, LTC3676_REG_BUCK1, LTC3676_BUCK_STARTUP, startup_bit);
    ack |= LTC3676_bit_write(LTC3676_I2C_ADDRESS, LTC3676_REG_BUCK2, LTC3676_BUCK_STARTUP, startup_bit);
    ack |= LTC3676_bit_write(LTC3676_I2C_ADDRESS, LTC3676_REG_BUCK3, LTC3676_BUCK_STARTUP, startup_bit);
    ack |= LTC3676_bit_write(LTC3676_I2C_ADDRESS, LTC3676_REG_BUCK4, LTC3676_BUCK_STARTUP, startup_bit);
    return(ack);
  }
  ack |= LTC3676_bit_write(LTC3676_I2C_ADDRESS, buck_number, LTC3676_BUCK_STARTUP, startup_bit);
  return(ack);
}

//! Sets the PGOOD mask bit in the DVBxB register for all bucks.
//! @return Returns the state of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_set_buck_pgood_mask(uint8_t i2c_address, uint8_t buck_number, uint8_t pgood_bit)
{
  int8_t ack = 0;
  uint8_t dvbxb_register = (2*buck_number)+9;
  if (buck_number == 0xFF)
  {
    ack |= LTC3676_bit_write(LTC3676_I2C_ADDRESS, LTC3676_REG_DVB1B, LTC3676_BUCK_PG_MASK, pgood_bit);
    ack |= LTC3676_bit_write(LTC3676_I2C_ADDRESS, LTC3676_REG_DVB2B, LTC3676_BUCK_PG_MASK, pgood_bit);
    ack |= LTC3676_bit_write(LTC3676_I2C_ADDRESS, LTC3676_REG_DVB3B, LTC3676_BUCK_PG_MASK, pgood_bit);
    ack |= LTC3676_bit_write(LTC3676_I2C_ADDRESS, LTC3676_REG_DVB4B, LTC3676_BUCK_PG_MASK, pgood_bit);
    return(ack);
  }
  ack |= LTC3676_bit_write(LTC3676_I2C_ADDRESS, dvbxb_register, LTC3676_BUCK_PG_MASK, pgood_bit);
  return(ack);
}

//! Writes a new UV warning threshold voltage in the CTRL register.
//! @return Returns the state of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_set_uv_warning_threshold(uint8_t i2c_address, float uv_warning_threshold)
{
  int8_t ack = 0;
  uint8_t uv_warning_data = 0;
  uint8_t register_data;
  ack |= LTC3676_register_read(i2c_address, LTC3676_REG_CNTRL, &register_data);  //Read CNTRL register
  register_data = register_data & (~LTC3676_UV_WARN_THRESH_MASK);  //Clears the UV Warning Threshold bits in the CNTRL register
  uv_warning_data = (uint8_t) ((uv_warning_threshold + 0.05 - 2.7) / 0.1);  //Generates 3-bit code for closest allowable value to user's chosen threshold voltage.
  register_data = register_data | (uv_warning_data << 2);
  ack |= LTC3676_register_write(i2c_address, LTC3676_REG_CNTRL, register_data);  //Writes new register value
  return(ack);
}

//! Writes the UV warning threshold of any buck.
//! @return Returns the state of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_set_overtemp_warning_level(uint8_t i2c_address, uint8_t ot_warning_level)
{
  int8_t ack = 0;
  uint8_t ot_warning_data = 0;
  uint8_t register_data;
  ack |= LTC3676_register_read(i2c_address, LTC3676_REG_CNTRL, &register_data);  //Read CNTRL register
  register_data = register_data & (~LTC3676_OT_WARN_LEVEL_MASK);  //Clears the over temperature warning level bits in the CNTRL register
  ot_warning_data = (uint8_t) ((ot_warning_level + 5 - 10) / 10);  //Generates 2-bit code for closest allowable value to user's chosen threshold voltage.
  register_data = register_data | ot_warning_data;
  ack |= LTC3676_register_write(i2c_address, LTC3676_REG_CNTRL, register_data);  //Writes new register value
  return(ack);
}

//! Sets LDO4 output voltage on the LTC3676-1.
//! @return Returns the state of the acknowledge bit after the I2C addresss write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_1_set_ldo4_voltage(uint8_t i2c_address, uint8_t ldo4_output_voltage_code)
{
  int8_t ack = 0;
  uint8_t register_data;
  ack |= LTC3676_register_read(i2c_address, LTC3676_REG_LDOB, &register_data);  //Read register
  register_data = register_data & (~LTC3676_1_LDO4_VOLTAGE_MASK);  //Clears the LDO4 Output Voltage bits in the LDOB register
  register_data = register_data | (ldo4_output_voltage_code << 3);
  ack |= LTC3676_register_write(i2c_address, LTC3676_REG_LDOB, register_data);  //Writes new register value
  return(ack);
}

//! Sets the Sequence Down bits for any buck in the SQD1 register.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_set_buck_sequence_down(uint8_t i2c_address, uint8_t buck_number, uint8_t sequence_phase)
{
  int8_t ack = 0;
  uint8_t register_data;
  sequence_phase = sequence_phase & (0x03); //Mask out everything but the first two bits of sequence_phase
  ack |= LTC3676_register_read(i2c_address, LTC3676_REG_SQD1, &register_data);  //Read SQD1 register.
  register_data &= (~LTC3676_BUCK_SEQ_MASK(buck_number)); //Clear data in sequence down bits to be written.
  register_data |= (sequence_phase << ((buck_number*2)- 2)); //Bit shift sequence bits to the correct position.
  ack |= LTC3676_register_write(i2c_address, LTC3676_REG_SQD1, register_data); //Write new sequence data.
  return(ack);
}

//! Sets the Sequence Down bits for any buck in the SQD1 register.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC3676_set_ldo_sequence_down(uint8_t i2c_address, uint8_t ldo_number, uint8_t sequence_phase)
{
  int8_t nack = 0;
  uint8_t register_data;
  sequence_phase = sequence_phase & (0x03); //Mask out everything but the first two bits of sequence_phase
  nack |= LTC3676_register_read(i2c_address, LTC3676_REG_SQD2, &register_data);  //Read SQD2 register.
  register_data &= (~LTC3676_LDO_SEQ_MASK(ldo_number)); //Bit shift sequence bits to the correct position.
  register_data |= (sequence_phase << ((ldo_number-2)* 2)); //Bit shift sequence bits to the correct position.
  nack |= LTC3676_register_write(i2c_address, LTC3676_REG_SQD2, register_data); //Write new sequence data.
  return(nack);
}
