/*!

Copyright 2020(c) Analog Devices, Inc.

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
    @ingroup LTPSM_InFlightUpdate
    Library Header File
*/

#ifndef NVM_CREATE_H_
#define NVM_CREATE_H_

#include <stdint.h>

extern uint8_t *create_event(uint16_t id);
extern uint8_t *create_meta_data(uint16_t data);
extern uint8_t *create_delay_ms(uint16_t delay);
extern uint8_t *create_send_byte(uint16_t address, uint8_t command, bool pec = false);
extern uint8_t *create_write_byte(uint16_t address, uint8_t command, uint8_t data, bool pec = false);
extern uint8_t *create_extended_write_byte(uint16_t address, uint16_t command, uint8_t data, bool pec = false);
extern uint8_t *create_write_word(uint16_t address, uint8_t command, uint16_t data, bool pec = false);
extern uint8_t *create_extended_write_word(uint16_t address, uint16_t command, uint16_t data, bool pec = false);
extern uint8_t *create_write_block(uint16_t address, uint8_t command, uint8_t *data, int size, bool pec = false);
extern uint8_t *create_read_byte_expect(uint16_t address, uint8_t command, uint8_t data, bool pec = false);
extern uint8_t *create_extended_read_byte_expect(uint16_t address, uint16_t command, uint8_t data, bool pec = false);
extern uint8_t *create_read_word_expect(uint16_t address, uint8_t command, uint16_t data, bool pec = false);
extern uint8_t *create_extended_read_word_expect(uint16_t address, uint16_t command, uint16_t data, bool pec = false);
extern uint8_t *create_termination();
extern uint8_t *pack_records(uint8_t **records);

#endif /* NVM_CREATE_H_ */