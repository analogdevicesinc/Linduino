/*!
LTC PMBus Support: Math conversion routines

@verbatim

This API is shared with Linduino and RTOS code.

@endverbatim

REVISION HISTORY
$Revision: 3659 $
$Date: 2015-07-01 10:19:20 -0700 (Wed, 01 Jul 2015) $

Copyright (c) 2014, Linear Technology Corp.(LTC)
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
    @ingroup LT_PMBUS_MATH
    Library Header File for LT_PMBusMath
*/

#ifndef LT_PMBusMath_H_
#define LT_PMBusMath_H_

class LT_PMBusMath
{

  public:

    typedef unsigned long   fl32_t;    // Type for the bit representation of "float"
    typedef unsigned int    lin11_t;   // Type for PMBus Linear11 mantissa
    typedef unsigned int    lin16_t;   // Type for PMBus Linear16 mantissa
    typedef unsigned int    lin16m_t;  // Type for PMBus Linear16 VOUT_MODE
    typedef int             slin11_t;  // A signed type for PMBus Linear11 exponent
    typedef unsigned char   uchar_t;   // Type for bit shifts

    fl32_t lin11_to_fl32 (lin11_t xin);
    fl32_t lin16_to_fl32 (lin16_t lin16_mant, lin16_t lin16_exp);
    lin16_t fl32_to_lin16 (fl32_t xin, lin16_t lin16_exp);
    lin11_t fl32_to_lin11 (fl32_t xin);

    float lin11_to_float (lin11_t xin);
    float lin16_to_float (lin16_t lin16_mant, lin16m_t vout_mode);
    lin11_t float_to_lin11 (float xin);
    lin16_t float_to_lin16 (float xin, lin16m_t vout_mode);

};

extern LT_PMBusMath math_;

#endif /* LT_PMBusMath_H_ */
