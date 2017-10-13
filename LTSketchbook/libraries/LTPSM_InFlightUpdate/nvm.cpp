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
