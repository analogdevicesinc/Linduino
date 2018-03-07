/*!

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
    @ingroup LTPSM_InFlightUpdate
    Library Header File
*/

#ifndef MAIN_RECORD_PROCESSOR_H_
#define MAIN_RECORD_PROCESSOR_H_

#include <stdlib.h>
#include <stdint.h>
#include "../LT_SMBUS/LT_SMBusNoPec.h"
#include "../LT_SMBUS/LT_SMBusPec.h"
#include "record_type_definitions.h"                  /* Record Type Definitions */
#include "in_circuit_programming_record_type_list_item_definition.h"  /* Linked List Structure for buffering all records before programming from Intel HEX file */
#include "nvm_data_helpers.h"


extern _InCircuitProgrammingRecordTypeListItem_p ICP_RTLI_Root;

extern LT_SMBusNoPec *smbusNoPec__;
extern LT_SMBusPec *smbusPec__;

extern uint8_t processRecordsOnDemand(pRecordHeaderLengthAndType (*getRecord)(void));
extern uint8_t verifyRecordsOnDemand(pRecordHeaderLengthAndType (*getRecord)(void));

#endif /* MAIN_RECORD_PROCESSOR_H_ */
