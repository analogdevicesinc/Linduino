/*!
LTC PMBus Support: Math conversion routines

@verbatim

This API is shared with Linduino and RTOS code.

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
