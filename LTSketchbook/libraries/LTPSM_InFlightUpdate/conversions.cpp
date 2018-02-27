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
    Library File
*/

#include "conversions.h"

// Routines from the internet that are mixed in.
// Author Mike Jones


uint16_t httoi(char *value)
{
  struct CHexMap
  {
    char chr;
    int value;
  };

  struct CHexMap HexMap[22] =
  {
    {'0', 0}, {'1', 1},
    {'2', 2}, {'3', 3},
    {'4', 4}, {'5', 5},
    {'6', 6}, {'7', 7},
    {'8', 8}, {'9', 9},
    {'A', 10}, {'B', 11},
    {'C', 12}, {'D', 13},
    {'E', 14}, {'F', 15},
    {'a', 10}, {'b', 11},
    {'c', 12}, {'d', 13},
    {'e', 14}, {'f', 15}
  };
  char *s = value;
  uint16_t result = 0;
  boolean firsttime = true;
  boolean found;
  int i;
  if (*s == '0' && *(s + 1) == 'x') s += 2;
  while (*s != '\0')
  {
    found = false;
    for (i = 0; i < 22; i++)
    {
      if (*s == HexMap[i].chr)
      {
        if (!firsttime) result <<= 4;
        //printf("add: %d\n", HexMap[i].value);
        result |= HexMap[i].value;
        found = true;
        break;
      }
    }
    if (!found) break;
    s++;
    firsttime = false;
  }
  //printf("finish: 0x%02x\n", result);
  return result;
}

// http://forum.arduino.cc/index.php?topic=44262.0
char *ftoa(char *a, double f, int precision)
{
  long p[] = {0,10,100,1000,10000,100000,1000000,10000000,100000000};

  char *ret = a;
  long heiltal = (long)f;
  itoa(heiltal, a, 10);
  while (*a != '\0') a++;
  *a++ = '.';
  long desimal = abs((long)((f - heiltal) * p[precision]));
  itoa(desimal, a, 10);
  return ret;
}
