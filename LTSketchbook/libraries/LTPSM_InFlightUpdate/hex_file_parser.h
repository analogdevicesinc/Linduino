/*!
REVISION HISTORY
$Revision: 3659 $
$Date: 2015-07-01 10:19:20 -0700 (Wed, 01 Jul 2015) $

Copyright (c) 2013, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/
/*! @file
    @ingroup LTPSM_InFlightUpdate
    Library Header File
*/

#ifndef HEX_FILE_PARSER_H_
#define HEX_FILE_PARSER_H_

#include <stdlib.h>
#include <stdint.h>
#include "conversions.h"

#include "record_type_definitions.h"

extern uint8_t filter_terminations(uint8_t (*get_data)(void));
extern uint8_t detect_colons(uint8_t (*get_data)(void));
extern void reset_parse_hex(void);
extern uint16_t parse_hex_block(char *in_data, uint16_t in_length, uint8_t *out_data);
extern uint8_t parse_hex(uint8_t (*get_data)(void));
extern uint16_t parse_records(uint8_t *in_data, uint16_t in_length, tRecordHeaderLengthAndType **out_records);
extern pRecordHeaderLengthAndType parse_record(uint8_t (*get_data)(void));
extern pRecordHeaderLengthAndType print_record(pRecordHeaderLengthAndType (*get_data)(void));

#endif
