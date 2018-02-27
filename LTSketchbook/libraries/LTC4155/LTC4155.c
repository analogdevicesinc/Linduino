/*!
LTC4155: Dual-Input Power Manager / 3.5A Li-Ion Battery Charger with I²C Control and USB OTG

@verbatim
The LTC®4155 is a 15 watt I²C controlled power manager with PowerPath™ instant-
on operation, high efficiency switching battery charging and USB compatibility.
The LTC4155 seamlessly manages power distribution from two 5V sources, such as a
USB port and a wall adapter, to a single-cell rechargeable Lithium-Ion/Polymer
battery and a system load.
@endverbatim

http://www.linear.com/product/LTC4155

http://www.linear.com/product/LTC4155#demoboards


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

Generated on: 2016-01-19
*/

//! @ingroup Switching_Regulators
//! @{
//! @defgroup LTC4155 LTC4155: Dual-Input Power Manager / 3.5A Li-Ion Battery Charger with I²C Control and USB OTG
//! @}

/*! @file
 *  @ingroup LTC4155
 *  @brief LTC4155 lightweight, hardware ambiguous, embeddable C Communication
 *  Library.
 *
 * Communication  is  bit-field based as well as whole-register based. This library
 * automatically masks and right-justifies bit fields to  ease programmer workload.
 *
 * This C library provides a header file with the complete definitions of registers
 * and  bit  fields  within  those  registers, and routines to read and write those
 * registers and individual bit-fields.
 *
 * Outside  of  the  Linduino  environment,  this  library  can  be  built with any
 * standard  C  compiler  and  can  be  used with different I2C/SMBus communication
 * hardware  simply  by  swapping  out  the  pointers  to appropriate user-supplied
 * functions   *@ref   smbus_write_register   and   *@ref  smbus_read_register.  An
 * example  is  provided  using  the  i2c-tools  Linux kernel driver which has been
 * tested  on  the  BeagleBone  Black  Linux  single board computer. It can also be
 * readily  adapted  to  common  microcontrollers  with  minimal  memory  impact on
 * embedded systems.
 *
 * A higher level hardware ambiguous Python communication library is also available.
 *
 * Please   visit   http://www.linear.com/product/LTC4155#code  or  contact  the
 * factory at 408-432-1900 or www.linear.com for further information.
 */

#include "LTC4155.h"

#ifndef LTC4155_USE_MALLOC
int LTC4155_instances = 0;
LTC4155_chip_cfg_t LTC4155_chip_array[MAX_NUM_LTC4155_INSTANCES];
#endif


//private function
LTC4155 LTC4155_alloc(void)
{
  //! this function "allocates" a LTC4155_chip structure.
  //! It may or may not use malloc.
#ifdef LTC4155_USE_MALLOC
  return malloc(sizeof(LTC4155_chip_cfg_t));
#else
  if (LTC4155_instances < MAX_NUM_LTC4155_INSTANCES)
  {
    return &LTC4155_chip_array[LTC4155_instances++];
  }
  else
  {
    return 0;
  }
#endif
}

LTC4155 LTC4155_init(LTC4155_chip_cfg_t *cfg)
{
  LTC4155_chip_cfg_t *chip = LTC4155_alloc();
  if (chip == NULL) return NULL;
  chip->addr = cfg->addr;
  chip->write_register = cfg->write_register;
  chip->read_register = cfg->read_register;
  chip->port_configuration = cfg->port_configuration;
  return (LTC4155) chip;
}

static inline uint8_t get_size(uint16_t registerinfo)
{
  return ((registerinfo >> 8) & 0x0F) + 1;
}
static inline uint8_t get_subaddr(uint16_t registerinfo)
{
  return (registerinfo) & 0xFF;
}
static inline uint8_t get_offset(uint16_t registerinfo)
{
  return (registerinfo >> 12) & 0x0F;
}
static inline uint8_t get_mask(uint16_t registerinfo)
{
  uint8_t mask = 1 << get_offset(registerinfo);
  uint8_t size = get_size(registerinfo);
  uint8_t i;
  for (i=0; i<size-1; i++)
  {
    mask |= mask << 1;
  }
  return mask;
}

int LTC4155_write_register(LTC4155 chip_handle, uint16_t registerinfo, uint8_t data)
{
  LTC4155_chip_cfg_t *chip = (LTC4155_chip_cfg_t *) chip_handle;
  int failure;
  uint8_t command_code = get_subaddr(registerinfo);
  if (get_size(registerinfo) != 8)
  {
    uint8_t offset = get_offset(registerinfo);
    uint16_t mask = get_mask(registerinfo);
    uint8_t read_data;
    failure = chip->read_register(chip->addr,command_code,&read_data,chip->port_configuration);
    if (failure) return failure;
    data = (read_data & ~mask) | (data << offset);
  }
  return chip->write_register(chip->addr,command_code,data,chip->port_configuration);
}

int LTC4155_read_register(LTC4155 chip_handle, uint16_t registerinfo, uint8_t *data)
{
  LTC4155_chip_cfg_t *chip = (LTC4155_chip_cfg_t *) chip_handle;
  int result;
  uint8_t command_code = get_subaddr(registerinfo);
  uint8_t offset = get_offset(registerinfo);
  uint16_t mask = get_mask(registerinfo);
  result = chip->read_register(chip->addr,command_code,data,chip->port_configuration);
  *data &= mask;
  *data = *data >> offset;
  return result;
}
