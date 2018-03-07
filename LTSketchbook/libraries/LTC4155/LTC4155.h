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


/*! @file
 *  @ingroup LTC4155
 *  @brief LTC4155 communication library core header file defining
 *  prototypes, data structures and constants used by LTC4155.c
 *
 *  Functions  matching  the  prototypes  of  @ref  smbus_write_register and @ref
 *  smbus_read_register  must  be  provided  to this API. They will implement the
 *  SMBus  read  and write transactions on your hardware. If the register size of
 *  the  LTC4155  is 8 bits, functions should be provided that implement SMBus
 *  read  byte and write byte. If the register size of the LTC4155 is 16 bits,
 *  functions  should  be provided that implement SMBus read word and write word.
 *  smbus_read_register  needs  to  store the value read from the LTC4155 into
 *  the  variable  data.  Both  functions  should return 0 on success and a non-0
 *  error  code  on  failure.  The  API functions will return your error codes on
 *  failure and a 0 on success.
 */

#ifndef LTC4155_H_
#define LTC4155_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "LTC4155_reg_defs.h"
#include "LTC4155_formats.h"
#include <stdint.h>
#include <stddef.h>

  // Type declarations
  /*! Hardware port information. Modify as needed for local hardware
  requirements. Available to user supplied read and write functions. */
  typedef struct
  {
    int file_descriptor; //!< Linux SMBus file handle
  } port_configuration_t;
  /*! Prototype of user supplied SMBus write_byte or write_word function. Should return 0 on success and a non-0 error code on failure. */
  typedef int (*smbus_write_register)(uint8_t addr, //!< Target IC's SMBus address
                                      uint8_t command_code, //!< Command code to be written to
                                      uint8_t data, //!< Data to be written
                                      port_configuration_t *port_configuration //!< Pointer to a user supplied port_configuration struct
                                     );
  /*! Prototype of user supplied SMBus read_byte or read_word function. Should return 0 on success and a non-0 error code on failure. */
  typedef int (*smbus_read_register)(uint8_t addr, //!< Target IC's SMBus address
                                     uint8_t command_code, //!< command code to be read from
                                     uint8_t *data, //!< Pointer to data destination
                                     port_configuration_t *port_configuration //!< Pointer to a user supplied port_configuration struct
                                    );
  typedef void *LTC4155;
  /*! Information required to access hardware SMBus port */
  typedef struct
  {
    uint8_t addr; //!< Target IC's SMBus address
    smbus_read_register read_register; //!< Pointer to a user supplied smbus_read_register function
    smbus_write_register write_register; //!< Pointer to a user supplied smbus_write_register function
    port_configuration_t *port_configuration; //!< Pointer to a user supplied port_configuration struct
  } LTC4155_chip_cfg_t;

  // function declarations
  /*! Returns a pointer to a LTC4155 structure used by LTC4155_write_register and LTC4155_read_register */
  LTC4155 LTC4155_init(LTC4155_chip_cfg_t *cfg //!< Information required to access hardware SMBus port
                      );
  /*! Function to modify a bit field within a register while preserving the unaddressed bit fields */
  int LTC4155_write_register(LTC4155 chip_handle, //!< Struct returned by LTC4155_init
                             uint16_t registerinfo, //!< Bit field name from LTC4155_regdefs.h
                             uint8_t data //!< Data to be written
                            );
  /*! Retrieves a bit field data into *data. Right shifts the addressed portion down to the 0 position */
  int LTC4155_read_register(LTC4155 chip_handle, //!< Struct returned by LTC4155_init
                            uint16_t registerinfo, //!< Register name from LTC4155_regdefs.h
                            uint8_t *data //!< Pointer to the data destination
                           );
  /*! Multiple LTC4155 use.
    Multiple LTC4155s can be used with this API. Each one must be initialized.
    The LTC4155_init requires some memory for each LTC4155. This memory can
    be  statically  allocated  by  defining  MAX_NUM_LTC4155_INSTANCES  to the
    number of LTC4155s required. Alternatively it can be dynamically allocated
    by  defining  LTC4155_USE_MALLOC.  The  default  is  to not use malloc and
    statically allocate one LTC4155.
  */
#ifndef MAX_NUM_LTC4155_INSTANCES
#define MAX_NUM_LTC4155_INSTANCES 1
#endif
#ifdef __cplusplus
}
#endif
#endif /* LTC4155_H_ */
