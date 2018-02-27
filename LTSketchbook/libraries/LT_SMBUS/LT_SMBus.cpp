/*!
LTC SMBus Support: API for a shared SMBus layer

@verbatim

This API is shared with Linduino and RTOS code. End users should code to this
API to enable use of the PMBus code without modifications.

@endverbatim


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
//! @defgroup LT_SMBus LT_SMBus: Base class of all SMBus with utility functions
//! @}

/*! @file
    @ingroup LT_SMBus
    Library Header File for LT_SMBus
*/

#include "LT_SMBus.h"
const uint8_t table_[256] PROGMEM  = { 0, 7, 14, 9, 28, 27, 18, 21,
                                       56, 63, 54, 49, 36, 35, 42, 45,
                                       112, 119, 126, 121, 108, 107, 98, 101,
                                       72, 79, 70, 65, 84, 83, 90, 93,
                                       224, 231, 238, 233, 252, 251, 242, 245,
                                       216, 223, 214, 209, 196, 195, 202, 205,
                                       144, 151, 158, 153, 140, 139, 130, 133,
                                       168, 175, 166, 161, 180, 179, 186, 189,
                                       199, 192, 201, 206, 219, 220, 213, 210,
                                       255, 248, 241, 246, 227, 228, 237, 234,
                                       183, 176, 185, 190, 171, 172, 165, 162,
                                       143, 136, 129, 134, 147, 148, 157, 154,
                                       39, 32, 41, 46, 59, 60, 53, 50,
                                       31, 24, 17, 22, 3, 4, 13, 10,
                                       87, 80, 89, 94, 75, 76, 69, 66,
                                       111, 104, 97, 102, 115, 116, 125, 122,
                                       137, 142, 135, 128, 149, 146, 155, 156,
                                       177, 182, 191, 184, 173, 170, 163, 164,
                                       249, 254, 247, 240, 229, 226, 235, 236,
                                       193, 198, 207, 200, 221, 218, 211, 212,
                                       105, 110, 103, 96, 117, 114, 123, 124,
                                       81, 86, 95, 88, 77, 74, 67, 68,
                                       25, 30, 23, 16, 5, 2, 11, 12,
                                       33, 38, 47, 40, 61, 58, 51, 52,
                                       78, 73, 64, 71, 82, 85, 92, 91,
                                       118, 113, 120, 127, 106, 109, 100, 99,
                                       62, 57, 48, 55, 34, 37, 44, 43,
                                       6, 1, 8, 15, 26, 29, 20, 19,
                                       174, 169, 160, 167, 178, 181, 188, 187,
                                       150, 145, 152, 159, 138, 141, 132, 131,
                                       222, 217, 208, 215, 194, 197, 204, 203,
                                       230, 225, 232, 239, 250, 253, 244, 243
                                     };

LT_SMBus::LT_SMBus()
{
  poly_ = 0x07;
  crc_polynomial_ = 0x0107;
  pec_enabled_ = false;

  // Set up the PEC table.
  constructTable(crc_polynomial_);
}


/*
 * Construct a table for PEC calculation
 */
void LT_SMBus::constructTable(uint16_t user_polynomial)
{
//  uint8_t temp;
//  uint8_t a;
//  uint8_t poly = (uint8_t)(user_polynomial & 0x0FF);

//  if ((user_polynomial & 0xFFFFFF01) != 0x00000101)
//      throw new ApplicationException("Invalid userPolynomial.");

  crc_polynomial_ = user_polynomial;
  // for (i = 0; i < 256; i++)
  // {
  // temp = 0;
  // a = (uint8_t)i;
  // for (j = 0; j < 8; j++)
  // {
  // if (((temp ^ a) & 0x80) != 0)
  // {
  // temp = (uint8_t)((temp << 1) ^ poly);
  // }
  // else
  // {
  // temp <<= 1;
  // }
  // a <<= 1;
  // }
  // table_[i] = temp;
  // }
}

/*
 * Helper function for calculating PEC
 */
uint8_t LT_SMBus::doCalculate(uint8_t data, uint8_t begining_value)
{
  return table_[(0xFF & (begining_value ^ data))];
}

/*
 * Helper function for calculating PEC
 */
uint8_t LT_SMBus::calculate(uint8_t *data, uint8_t begining_value, uint8_t start_index, uint8_t length)
{
  uint8_t i;
  uint8_t ret_value = begining_value;

  for (i = start_index; i < (start_index + length); i++)
  {
    ret_value = doCalculate(data[i], ret_value);
  }

  return ret_value;
}

/*
 * Clear the PEC value and get ready to calculate.
 */
void LT_SMBus::pecClear(void)
{
  running_pec_ = 0;
}

/*
 * Add a value to the PEC
 */
void LT_SMBus::pecAdd(uint8_t byte_value)
{
  uint8_t i;
  running_pec_ = running_pec_ ^ byte_value;

  for (i=0; i<8; i++)
  {
    if ((running_pec_ & 0x80) > 0x00)
    {
      running_pec_ = (running_pec_ << 1) ^ poly_;
    }
    else
    {
      running_pec_ = running_pec_ << 1;
    }

  }
}

/*
 * Get the final PEC value
 */
uint8_t LT_SMBus::pecGet(void)
{
  return running_pec_;
}

/*
 * Check CRC of block data organized as 31 data bytes plus CRC
*/
bool LT_SMBus::checkCRC (uint8_t *data)
{
  uint8_t pos;
  bool nok;

  pecClear();

  for (pos = 0; pos < 31; pos++)
    pecAdd(data[pos]);

  nok = pecGet() != data[pos];

  pecClear();

  return nok;
}

/*
 * Get CRC of block data organized as 31 data bytes plus CRC
*/
uint8_t LT_SMBus::getCRC (uint8_t *data)
{
  uint8_t pos;
  bool nok;
  uint8_t pec;

  pecClear();

  for (pos = 0; pos < 31; pos++)
    pecAdd(data[pos]);

  pec = pecGet();

  pecClear();

  return pec;
}