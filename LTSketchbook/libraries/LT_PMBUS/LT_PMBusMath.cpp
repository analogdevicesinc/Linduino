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

//! @ingroup PMBus_SMBus
//! @{
//! @defgroup LT_PMBusMath LT_PMBusMath: Value conversion routines
//! @}

/*! @file
    @ingroup LT_PMBusMath
    Library Header File for LT_PMBusMath
*/
#include "LT_PMBusMath.h"

// +---------------------------------------------------------------------------+
// |               Define the Bit Widths and Exponent Properties               |
// +---------------------------------------------------------------------------+

// IEEE 754 Float32 Single Precision Floating Point Constants (Binary32)
#define fl32_width            32        // Bit width of the Float32
#define fl32_mant_width       23        // Bit width of the encoded mantissa
#define fl32_exp_bias        127        // IEEE 754 Float32 exponent bias
#define fl32_NaN_exp           1        // Exponent encodings used for NaN

// PMBus Linear11 Floating Point Constants
#define lin11_width           16        // Bit width of the Linear11
#define lin11_mant_width      11        // Bit width of the Linear11 mantissa

// PMBus Linear16 Format Constants
#define lin16_width           16        // Bit width of the Linear16
#define lin16_exp_width        5        // Bit width of the Linear16 exponent

// +---------------------------------------------------------------------------+
// |      Calculate the Float32 Exponent Ranges and Bit Masks                  |
// +---------------------------------------------------------------------------+

// Float32 exponent constants
#define fl32_exp_width      (fl32_width - fl32_mant_width - 1)
#define fl32_max_exp        ((1UL << fl32_exp_width) - 1)
#define fl32_max_p_exp      (fl32_max_exp - fl32_exp_bias - fl32_NaN_exp)

// Define the IEEE 754 Float32 Single Precision Bit Masks
#define fl32_sign_mask      (1UL << (fl32_width - 1))
#define fl32_exp_mask       (((1UL << fl32_exp_width) - 1) << fl32_mant_width)
#define fl32_mant_mask      ((1UL << fl32_mant_width) - 1)

// +---------------------------------------------------------------------------+
// |                        PMBus Linear11 Definitions                         |
// +---------------------------------------------------------------------------+

// Define the PMBus Linear11 Bit Masks
#define lin11_mant_sign_mask    (1UL << (lin11_mant_width - 1))
#define lin11_mant_mask         ((1UL << lin11_mant_width) - 1)
#define lin11_exp_width         (lin11_width - lin11_mant_width)
#define lin11_exp_sign_mask     (1UL << (lin11_exp_width - 1))
#define lin11_exp_mask          ((1UL << lin11_exp_width) - 1)
#define lin11_ieee_mant_width   (lin11_mant_width-2)
#define lin11_ieee_mant_mask    ((1UL << lin11_ieee_mant_width) - 1)
#define lin11_max_p_exp         ((1UL << (lin11_exp_width - 1)) - 1)
#define lin11_max_n_exp         (1UL << (lin11_exp_width - 1))
#define lin11_bit_mask          ((1UL << lin11_width) - 1)

// Define the PMBus Linear11 normalized and denormal -1
#define lin11_den_n1   ((1UL<<(lin11_mant_width-1))|(1UL<<(lin11_mant_width-2)))
#define lin11_norm_n1  ( 1UL<<(lin11_mant_width-1))

// +---------------------------------------------------------------------------+
// |                        PMBus Linear16 Definitions                         |
// +---------------------------------------------------------------------------+

// Define the PMBus Linear16 Bit Masks
#define lin16_exp_sign_mask     (1UL << (lin16_exp_width - 1))
#define lin16_exp_mask          ((1UL << lin16_exp_width) - 1)
#define lin16_ieee_mant_width   (lin16_width-1)
#define lin16_ieee_mant_mask    ((1UL << lin16_ieee_mant_width) - 1)
#define lin16_max_p_exp         ((1UL << (lin16_exp_width - 1)) - 1)
#define lin16_max_n_exp         (1UL << (lin16_exp_width - 1))
#define lin16_max_value         ((1UL << lin16_width) - 1)

// Make sure the mantissa of the Float32 format is as wide as or wider than
// the mantissa of the Linear16 format
#if !(fl32_mant_width >= lin16_ieee_mant_width)
#error The Float32 mantissa does not have enough bits for L16!
#endif

// +---------------------------------------------------------------------------+
// |                   Linear11 --> Float32 Conversion Function                |
// +---------------------------------------------------------------------------+

// The conversion function implementation assumes (checked at compile time)
// that all Linear11 numbers map to normalized Float32 floating point numbers

LT_PMBusMath::fl32_t LT_PMBusMath::lin11_to_fl32 (LT_PMBusMath::lin11_t xin)
{

  // Repackage the Linear11 in an IEEE754 format (sign, exponent, mantissa)
  LT_PMBusMath::lin11_t      lin11_sign, lin11_exp, lin11_exp_sign, lin11_mant, lin11_nmant;
  LT_PMBusMath::fl32_t       xout;

  // Extract the sign, exponent and mantissa
  lin11_sign = (xin & ((lin11_t) lin11_mant_sign_mask)) << lin11_exp_width;
  lin11_exp  = (xin >> lin11_mant_width) & ((lin11_t) lin11_exp_mask);
  lin11_exp_sign = lin11_exp & ((lin11_t) lin11_exp_sign_mask);
  lin11_mant = (xin & ((lin11_t) lin11_mant_mask));

  // Add the exponent bias of the target IEEE 754 format to the L11 exponent
  if (lin11_exp_sign != 0)
  {
    lin11_exp = (lin11_exp ^ ((lin11_t) lin11_exp_mask)) + 1;
    lin11_exp = fl32_exp_bias - lin11_exp + lin11_mant_width - 2;
  }
  else
  {
    lin11_exp = fl32_exp_bias + lin11_exp + lin11_mant_width - 2;
  }

  // Invert the mantissa sign for the two's complement representation
  if (lin11_sign != 0)
  {
    lin11_nmant = (lin11_mant ^ ((lin11_t) lin11_mant_mask)) + 1;

    // The most negative mantissa is a corner case and has to be shifted
    if  (lin11_nmant == lin11_mant)
    {
      lin11_mant = lin11_nmant >> 1;
      lin11_exp++;
    }
    else
    {
      lin11_mant = lin11_nmant;
    }
  }

  // Normalize the Linear11 mantissa to fit the IEEE 754 format structure
  if (lin11_mant != 0 )
  {
    while ((lin11_mant &  (~((lin11_t) lin11_ieee_mant_mask))) == 0 )
    {
      lin11_mant = lin11_mant << 1;
      lin11_exp--;
    }
  }
  else
  {
    lin11_exp = 0;      // Special case of Zero
  }
  lin11_mant = lin11_mant & ((lin11_t) lin11_ieee_mant_mask);

  // Shift the L11 sign, exponent and mantissa to the target IEEE format
  xout  = ((fl32_t) lin11_sign) << (fl32_width - lin11_width);
  xout |= ((fl32_t) lin11_exp)  <<  fl32_mant_width;
  xout |= ((fl32_t) lin11_mant) << (fl32_mant_width - lin11_ieee_mant_width);

  return xout;
}

// +---------------------------------------------------------------------------+
// |                    Linear16 --> Float32 Conversion Function               |
// +---------------------------------------------------------------------------+

// The conversion function implementation assumes (checked at compile time)
// that all Linear16 numbers map to normalized Float32 floating point numbers

LT_PMBusMath::fl32_t LT_PMBusMath::lin16_to_fl32 (LT_PMBusMath::lin16_t lin16_mant, LT_PMBusMath::lin16m_t vout_mode)
{

  // Repackage the Linear16 in an IEEE 754 format (sign, exponent, mantissa)
  LT_PMBusMath::lin16m_t     lin16_exp_sign;
  LT_PMBusMath::fl32_t       lin16_exp,xout;

  // Extract the exponent and mantissa (sign = 0)
  lin16_exp = (fl32_t) (vout_mode & ((lin16m_t) lin16_exp_mask));
  lin16_exp_sign = vout_mode & ((lin16m_t) lin16_exp_sign_mask);

  // Add the exponent bias of the target IEEE 754 format to the L16 exponent
  if (lin16_exp_sign != (lin16m_t) 0)
  {
    lin16_exp = (lin16_exp ^ ((fl32_t) lin16_exp_mask)) + 1;
    lin16_exp = (fl32_t)(fl32_exp_bias + lin16_ieee_mant_width) - lin16_exp;
  }
  else
  {
    lin16_exp = (fl32_t)(fl32_exp_bias + lin16_ieee_mant_width) + lin16_exp;
  }

  // Normalize the Linear16 mantissa to fit the IEEE 754 format structure
  if (lin16_mant != 0 )
  {
    while ((lin16_mant &  (~((lin16_t) lin16_ieee_mant_mask))) == 0 )
    {
      lin16_mant = lin16_mant << 1;
      lin16_exp--;
    }
  }
  else
  {
    lin16_exp = 0;      // Special case of Zero
  }
  lin16_mant = lin16_mant & ((lin16_t) lin16_ieee_mant_mask);

  // Shift the L16 mantissa and exponent to the target IEEE format (sign=0)
  xout  = lin16_exp << fl32_mant_width;
  xout |= ((fl32_t) lin16_mant) << (fl32_mant_width - lin16_ieee_mant_width);

  return xout;
}

// +---------------------------------------------------------------------------+
// |                   Float32 --> Linear16 Conversion Function                |
// +---------------------------------------------------------------------------+

// The conversion function implementation assumes (checked at compile time)
// that all Linear16 numbers map to normalized Float32 floating point numbers.
// The Linear16 exponent (VOUT_MODE) is assumed a constant of the chip.

LT_PMBusMath::lin16_t LT_PMBusMath::fl32_to_lin16 (LT_PMBusMath::fl32_t xin, LT_PMBusMath::lin16m_t vout_mode)
{

  // Variables for the exponent, mantissa and mantissa shift
  LT_PMBusMath::fl32_t          fl32_exp, fl32_mant, rnd_msb, tie_mask;
  LT_PMBusMath::lin16m_t        lin16_exp_sign, lin16_exp;
  LT_PMBusMath::lin16_t         xout;
  int                     shift;
  unsigned int            ushift;

  // Extract the sign, exponent and mantissa of the Float32 number
  if ( ( xin >> (fl32_width - 1)) != 0 )
  {
    xout = 0;              // All negative Float32 values get mapped to zero
  }
  else
  {
    // Extract the exponent and mantissa of the Float32 number
    fl32_exp = (((fl32_t) fl32_exp_mask)  & xin) >> fl32_mant_width;
    fl32_mant = ((fl32_t) fl32_mant_mask) & xin;

    // Add the implicit 1.mmm to the mantissa (L16 is not normalized)
    fl32_mant |= ((fl32_t) (((fl32_t) (1)) << fl32_mant_width));

    // Calculate the required shift for the mantissa
    lin16_exp = vout_mode & ((lin16m_t) lin16_exp_mask);
    lin16_exp_sign = lin16_exp & ((lin16m_t) lin16_exp_sign_mask);
    shift = (int) (fl32_exp_bias + fl32_mant_width) - (int) fl32_exp;
    if (lin16_exp_sign != (lin16m_t) 0)
    {
      shift -= (int)(lin16_exp ^ ((lin16m_t) lin16_exp_mask)) + 1;
    }
    else
    {
      shift += (int) lin16_exp;
    }

    // Right shift the mantissa and round
    if (shift < (fl32_mant_width + 1 - lin16_width))
    {
      xout = (lin16_t) lin16_max_value;             // Saturate to 0xFFFF
    }
    else if (shift > (fl32_mant_width + 1))
    {
      xout = 0;                                     // Underflow to zero
    }
    else                                              // Shift and round
    {
      ushift = (unsigned) shift;
      xout = (lin16_t) (fl32_mant >>  ushift);
      rnd_msb  =  (((fl32_t) (1)) << (ushift - 1));
      tie_mask = ((((fl32_t) (1)) << (ushift + 1)) - 1);

      // When rounding, mantissa overflow is not possible (right shift>=1)
      if (((rnd_msb & fl32_mant)!=0)&&((tie_mask & fl32_mant)!= rnd_msb))
      {
        xout++;
      }
    }
  }
  return xout;
}

// +---------------------------------------------------------------------------+
// |                   Float32 --> Linear11 Conversion Function                |
// +---------------------------------------------------------------------------+

// The conversion function implementation assumes (checked at compile time)
// that all Linear11 numbers map to normalized Float32 floating point numbers.

LT_PMBusMath::lin11_t LT_PMBusMath::fl32_to_lin11 (LT_PMBusMath::fl32_t xin)
{

  // Variables for the floating point sign, exponent and mantissa bit fields
  LT_PMBusMath::fl32_t          fl32_exp, fl32_mant, rnd_msb, tie_mask;
  LT_PMBusMath::lin11_t         lin11_sign, lin11_mant;
  LT_PMBusMath::slin11_t        lin11_exp;
  uchar_t             mant_shift;
  const LT_PMBusMath::lin11_t   normal_n1 = (lin11_t) lin11_norm_n1;
  const LT_PMBusMath::lin11_t   denormal_n1 = (lin11_t) lin11_den_n1;

  // Extract the sign, exponent and mantissa of the Float32 number
  fl32_exp   = (((fl32_t) fl32_exp_mask)  & xin) >> fl32_mant_width;
  fl32_mant  = ((fl32_t)  fl32_mant_mask) & xin;
  fl32_mant |= (((fl32_t) (1)) << fl32_mant_width);    // Add the 1.mmm

  // Convert the extracted sign and signed exponent to Linear11
  lin11_sign  = ((lin11_t)(xin >> (fl32_width - lin11_width)));
  lin11_sign &= (((lin11_t) (1)) << (lin11_width-1));
  lin11_exp   = (slin11_t)(fl32_exp) - fl32_exp_bias - lin11_ieee_mant_width;

  // Determine if the corresponding Linear11 number is normalized or denormal
  mant_shift = (uchar_t) (fl32_mant_width - lin11_ieee_mant_width);
  if  (lin11_exp < (-(slin11_t) lin11_max_n_exp))      // Denormal Linear11
  {
    mant_shift -= (uchar_t) (lin11_exp + ((slin11_t) lin11_max_n_exp));
    lin11_exp  = -((slin11_t) lin11_max_n_exp);
  }

  // Calculate the Linear11 mantissa and check if rounding up is needed
  // If the mantissa overflowed after the rounding, increment the exponent
  if  (mant_shift > (uchar_t)(fl32_mant_width + 1))
  {
    lin11_mant = 0;
  }
  else
  {
    lin11_mant = (lin11_t) (fl32_mant >> mant_shift);
    rnd_msb  =  (((fl32_t) (1)) << (fl32_t) (mant_shift - (uchar_t) 1));
    tie_mask = ((((fl32_t) (1)) << (fl32_t) (mant_shift + (uchar_t) 1))-1);

    if (((rnd_msb & fl32_mant) !=0) && ((tie_mask & fl32_mant) != rnd_msb))
    {
      // Increment the mantissa and check for mantissa overflow
      lin11_mant++;
      if ((lin11_mant & ((lin11_t) lin11_ieee_mant_mask)) == 0)
      {
        lin11_exp++;
        lin11_mant = lin11_mant >> 1;
      }
    }
  }

  // Convert the mantissa to two's complement
  if (lin11_sign != 0)
  {
    lin11_mant = ((~lin11_mant + 1) & (lin11_t) lin11_mant_mask);
  }

  // Normalize the special case of -1 x 2^N
  if ((lin11_mant == denormal_n1) && (lin11_exp > -(slin11_t)lin11_max_n_exp))
  {
    lin11_mant = normal_n1;
    lin11_exp--;
  }

  // Saturate to maximum if the exponent is too large
  if (lin11_exp > (slin11_t) lin11_max_p_exp)
  {
    lin11_exp = (slin11_t) lin11_max_p_exp;
    if (lin11_sign != 0)
    {
      lin11_mant = normal_n1;
    }
    else
    {
      lin11_mant = (((lin11_t) (1)) << (lin11_mant_width - 1)) - 1;
    }
  }

  // Select a unique representation for zero - if the mantissa is zero,
  // return 0x0000 (zero exponent)
  if (lin11_mant == 0) lin11_exp = 0;

  lin11_exp  = (slin11_t)((lin11_t) lin11_exp << (lin11_t) lin11_mant_width);
  lin11_exp &= ((lin11_t) lin11_bit_mask);
  return ((lin11_t)(lin11_exp | lin11_mant));
}

// +---------------------------------------------------------------------------+
// |      Versions of the fl32_t Conversion Functions Interpreted as Float     |
// +---------------------------------------------------------------------------+
// |                                                                           |
// |  NOTE: The functions below are for convenience only and are not as fast   |
// |  as calling the binary functions directly with the appropriate pointer    |
// |  assignments.                                                             |
// |                                                                           |
// +---------------------------------------------------------------------------+

// PMBus Linear11 to Single Precision Float
float LT_PMBusMath::lin11_to_float (LT_PMBusMath::lin11_t xin)
{
  LT_PMBusMath::fl32_t  xout;
  xout = lin11_to_fl32(xin);
  return *((float *) &xout);
}

// PMBus Linear16 to Single Precision Float
float LT_PMBusMath::lin16_to_float (LT_PMBusMath::lin16_t lin16_mant, LT_PMBusMath::lin16m_t vout_mode)
{
  LT_PMBusMath::fl32_t  xout;
  xout = lin16_to_fl32(lin16_mant, vout_mode);
  return *((float *) &xout);
}

// Single Precision Float to PMBus Linear11
LT_PMBusMath::lin11_t LT_PMBusMath::float_to_lin11 (float xin)
{
  LT_PMBusMath::fl32_t  xin_fl32;
  xin_fl32 = *((fl32_t *) &xin);
  return fl32_to_lin11(xin_fl32);
}

// Single Precision Float to PMBus Linear16
LT_PMBusMath::lin16_t LT_PMBusMath::float_to_lin16 (float xin, LT_PMBusMath::lin16m_t vout_mode)
{
  LT_PMBusMath::fl32_t  xin_fl32;
  xin_fl32 = *((fl32_t *) &xin);
  return fl32_to_lin16 (xin_fl32, vout_mode);
}



LT_PMBusMath math_ = LT_PMBusMath();
