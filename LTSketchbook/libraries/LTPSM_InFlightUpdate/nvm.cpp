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

//! @ingroup PMBus_SMBus
//! @{
//! @defgroup LTPSM_InFlightUpdate LTPSM_InFlightUpdate
//! @}

/*! @file
    @ingroup LTPSM_InFlightUpdate
    Library File
*/

#include "nvm.h"

#define DEBUG_FLASH 0

const unsigned char  *icpFile;
uint32_t flashLocation = 0;

LT_SMBusNoPec *smbusNoPec__;
LT_SMBusPec *smbusPec__;

uint8_t get_hex_data(void)
{
  uint8_t c = '\0';
#if (DEBUG_FLASH)
  Serial.print("Loc ");
  Serial.println(flashLocation, HEX);
#endif
  c = pgm_read_byte_near(icpFile + flashLocation++);
#if (DEBUG_FLASH)
  Serial.print("Raw ");
  Serial.println(c, HEX);
#endif
  if ('\0' != c)
    return c;
  else
    return 0;
}

uint8_t get_filtered_hex_data(void)
{
  return filter_terminations(get_hex_data);
}

uint8_t get_record_data(void)
{
  return parse_hex(get_filtered_hex_data);
}

pRecordHeaderLengthAndType get_record(void)
{
  return parse_record(get_record_data);
}

NVM::NVM(LT_SMBusNoPec *smbusNoPec, LT_SMBusPec *smbusPec)
{
  smbusNoPec__ = smbusNoPec;
  smbusPec__ = smbusPec;
}

bool NVM::programWithData(const unsigned char *data)
{
  icpFile = data;
  flashLocation = 0;

  reset_parse_hex();
  if (processRecordsOnDemand(get_record) == 0)
  {
    reset_parse_hex();
    return 0;
  }
  reset_parse_hex();
  return 1;
}

bool NVM::verifyWithData(const unsigned char *data)
{
  icpFile = data;
  flashLocation = 0;

  reset_parse_hex();
  if (verifyRecordsOnDemand(get_record) == 0)
  {
    reset_parse_hex();
    return 0;
  }
  reset_parse_hex();
  return 1;
}
