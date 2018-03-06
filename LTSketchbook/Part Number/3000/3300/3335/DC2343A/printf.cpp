/*
 Linear Technology DC2343A Demonstration Board.

 @verbatim
 This code implements a simple printf function over the Arduino serial port.
 @endverbatim

 REVISION HISTORY
 $Revision: 4639 $
 $Date: 2016-01-29 13:42:40 -0800 (Fri, 29 Jan 2016) $

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

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Includes
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include <string.h>
#include <stdarg.h>
#include "Linduino.h"
#include "printf.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#define MAX_WIDTH             16
#define MAX_PRECISION         9
#define DEFAULT_PRECISION     6   // default precision for %f

#define MAX_OUTPUT_SIZE       100 // Maximum characters that can be output to serial port at once.

typedef enum
{
  PARSE_INCOMPLETE,
  PARSE_STRING,
  PARSE_FLASH_STRING,
  PARSE_CHARACTER,
  PARSE_DECIMAL,
  PARSE_BINARY,
  PARSE_HEX,
  PARSE_FLOAT,
  PARSE_PERCENT,
  PARSE_NONSENSE,
  PARSE_NONE,
} PARSE_TYPE;

typedef struct
{
  PARSE_TYPE ParseType;
  int16_t MinWidth;
  int16_t Precision;
  boolean ZeroFill;
  boolean Long;
  boolean ForceSign;
  boolean Signed;
  boolean Decimal;
  boolean UpperCase;
} PARSEOPTION_TYPE;

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Global Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Local Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Table provides method of multiplying the floating point fraction by desired precision with one math instruction.
const float PrecisionTable[MAX_PRECISION + 1] = { 1.0, // 0
    10.0, // 1
    100.0, // 2
    1000.0, // 3
    10000.0, // 4
    100000.0, // 5
    1000000.0, // 6
    10000000.0, // 7
    100000000.0, // 8
    1000000000.0, // 9
                                                };

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Local Prototypes
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
static inline int8_t hex_digit(uint8_t value, boolean upperCase);
static void init_option_data(PARSEOPTION_TYPE *options);
static void parse_format(const char **format, PARSEOPTION_TYPE *options);
static uint16_t expand_data(va_list *ap, PARSEOPTION_TYPE *options, char *print_buffer, int16_t buffer_size);

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Global Functions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

/////////////////////////////////////////////////////////////////////////
// Minimal printf implementation.  Supports:
//  % [+] [0] [<width>] [.precision] [l] [s|c|x|X|i|d|u|b|f]
//
//  0           == Fill field with zeroes
//  width       == Minimum field width
//  .precision  == digits of fraction
//  l           == Doubleword for x|i|d|u|o|b
//  s           == string pointer
//  S           == flash string pointer
//  c           == single character
//  x           == hex number
//  X           == uppercase string character
//  i,d         == signed decimal number
//  u           == unsigned number
//  b           == binary
//  f           == floating point
/////////////////////////////////////////////////////////////////////////
int16_t printf(const char *format, ...)
{
  char print_buffer[MAX_OUTPUT_SIZE + 1];
  va_list ap;
  int16_t retVal;

  va_start(ap, format);
  retVal = ssprintf(print_buffer, sizeof(print_buffer), format, ap);
  va_end(ap);

  Serial.print(print_buffer);

  return (retVal);
}

int16_t printf(const __FlashStringHelper *format, ...)
{
  char print_buffer[MAX_OUTPUT_SIZE + 1];
  va_list ap;
  int16_t retVal;

  va_start(ap, format);
  retVal = ssprintf(print_buffer, sizeof(print_buffer), (const char *)format, ap);
  va_end(ap);

  Serial.print(print_buffer);

  return (retVal);
}

// Like sprintf, except size of the buffer is passed in so that it won't get overrun.
int16_t ssprintf(char *print_buffer, int16_t buffer_size, const char *format, va_list ap)
{
  PARSEOPTION_TYPE options;
  char format_char;
  uint16_t out_size = 0;

  do
  {
    format_char = pgm_read_byte(format++);

    if (format_char == '%')
    {
      uint16_t size;

      init_option_data(&options);
      parse_format(&format, &options);
      size = expand_data(&ap, &options, print_buffer, buffer_size - out_size);
      print_buffer += size;
      out_size += size;
    }
    else
    {
      // just a normal character, move it in the buffer.
      *print_buffer++ = format_char;
      out_size++;
    }

  }
  while ((format_char != '\0') && (out_size < buffer_size));

  // return how many characters we put into the buffer.
  return out_size;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Local Functions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// hex_digit() returns the correct hexadecimal digit for a number
static inline int8_t hex_digit(uint8_t value, boolean upperCase)
{
  if (value < 10) return (int8_t) (value + '0');
  return (int8_t) ((value - 10) + ((upperCase == true) ? 'A' : 'a'));
}

// set all printf options to their defaults.
static void init_option_data(PARSEOPTION_TYPE *options)
{
  options->ParseType = PARSE_INCOMPLETE;
  options->MinWidth = 0;
  options->Precision = DEFAULT_PRECISION;
  options->ZeroFill = false;
  options->Long = false;
  options->ForceSign = false;
  options->Signed = false;
  options->Decimal = false;
  options->UpperCase = false;
}

// parse the format.  This routine assumes that the pointer to the format is
// just after the "%" character.  This routine will return the pointer of
// the format from where it left off.
static void parse_format(const char **format, PARSEOPTION_TYPE *options)
{
  char ch;

  options->ParseType = PARSE_INCOMPLETE;
  while (options->ParseType == PARSE_INCOMPLETE)
  {
    ch = pgm_read_byte((*format)++);
    switch (ch)
    {
      case '\0': // ran out of format text
        (*format)--;
        options->ParseType = PARSE_NONSENSE;
        break;

      case '+':
        options->ForceSign = true;
        break;

      case '.':
        options->Decimal = true;
        options->Precision = 0;
        break;

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        if (options->Decimal)
        {
          options->Precision = (options->Precision * 10) + ch - '0';
        }
        else
        {
          options->MinWidth = (options->MinWidth * 10) + ch - '0';
          if (options->MinWidth == 0) options->ZeroFill = true;
        }
        break;

      case 'l':
        options->Long = true;
        break;

      case 'c':
        options->ParseType = PARSE_CHARACTER;
        break;

      case 's':
        options->ParseType = PARSE_STRING;
        break;

      case 'S':
        options->ParseType = PARSE_FLASH_STRING;
        break;

      case 'b': /* binary */
        options->ParseType = PARSE_BINARY;
        break;

      case 'X': /* hex  */
        options->UpperCase = true;
        // fall through
      case 'x':
        options->ParseType = PARSE_HEX;
        break;

      case 'd': /*   signed decimal */
      case 'i': /*   signed decimal */
        options->Signed = true;
        options->ParseType = PARSE_DECIMAL;
        break;

      case 'u': /* unsigned decimal */
        options->ForceSign = false; // 'u' beats '+'
        options->ParseType = PARSE_DECIMAL;
        break;

      case 'f': /*   float */
        options->ParseType = PARSE_FLOAT;
        break;

      case '%':
        options->ParseType = PARSE_PERCENT;
        break;

      default:
        options->ParseType = PARSE_NONSENSE;
        break;
    }
  }

  return;
}

// Take each parsed format and expands the result into the 'print_buffer' based on the given 'options'.
// The 'print_buffer' size  is passed in so it not to overflow it.
static uint16_t expand_data(va_list *ap, PARSEOPTION_TYPE *options, char *print_buffer, int16_t buffer_size)
{
  uint16_t out_size = 0;

  if (!buffer_size) return 0; // We can probably trust outer loop won't call when there's no buffer left, but just in case

  if (options->MinWidth > buffer_size) options->MinWidth = buffer_size;

  switch (options->ParseType)
  {

    case PARSE_NONE:
      break;

    case PARSE_PERCENT:
      {
        *print_buffer++ = '%';
        out_size++;
      }
      break;

    case PARSE_NONSENSE:
      // write error code as a complete unit or not at all
      if (buffer_size > 4)
      {
        *print_buffer++ = '%';
        *print_buffer++ = '?';
        *print_buffer++ = '?';
        *print_buffer++ = '?';
        out_size += 4;
      }
      break;

    case PARSE_BINARY:
      {
        uint8_t temp_num[BITS_PER_BYTE * sizeof(uint32_t)];
        uint8_t *temp_num_ptr = temp_num;
        int16_t temp_num_size = 0;

        // Get value from argument list and convert it to digits
        // Decode the number to temporary storage as we need to write Most Significant Digit first
        // to the output buffer and we don't know how much to pad yet for MinWidth.
        if (options->Long == true)
        {
          uint32_t value = va_arg(*ap, uint32_t);
          do
          {
            *temp_num_ptr++ = (value & 1);
            value >>= 1;
            temp_num_size++;
          }
          while (value != 0);
        }
        else
        {
          uint16_t value = va_arg(*ap, uint16_t);
          do
          {
            *temp_num_ptr++ = (value & 1);
            value >>= 1;
            temp_num_size++;
          }
          while (value != 0);
        }

        // Pad and zero fill if the user wants it.
        {
          // calculate what's left for leading zero's.
          int16_t num_zeros = options->MinWidth - (temp_num_size + out_size);
          while ((num_zeros-- > 0) && (out_size < buffer_size))
          {
            *print_buffer++ = (options->ZeroFill == true) ? '0' : ' ';
            out_size++;
          }
        }

        // Write the converted number.
        while ((temp_num_size-- > 0) && (out_size < buffer_size))
        {
          // Write the digit converted to ascii
          *print_buffer++ = *(--temp_num_ptr) ? '1' : '0';
          out_size++;
        }
      }
      break;

    case PARSE_HEX:
      {
        uint8_t temp_num[NIBBLES_PER_BYTE * sizeof(uint32_t)];
        uint8_t *temp_num_ptr = temp_num;
        int16_t temp_num_size = 0;

        // Get value from argument list and convert it to digits
        // Decode the number to temporary storage as we need to write Most Significant Digit first
        // to the output buffer and we don't know how much to pad yet for MinWidth.
        if (options->Long == true)
        {
          uint32_t value = va_arg(*ap, uint32_t);
          do
          {
            *temp_num_ptr++ = (value & MASK(BITS_PER_NIBBLE,0));
            value >>= BITS_PER_NIBBLE;
            temp_num_size++;
          }
          while (value != 0);
        }
        else
        {
          uint16_t value = va_arg(*ap, uint16_t);
          do
          {
            *temp_num_ptr++ = (value & MASK(BITS_PER_NIBBLE,0));
            value >>= BITS_PER_NIBBLE;
            temp_num_size++;
          }
          while (value != 0);
        }

        // Pad and zero fill if the user wants it.
        {
          // calculate what's left for leading zero's.
          int16_t num_zeros = options->MinWidth - (temp_num_size + out_size);
          while ((num_zeros-- > 0) && (out_size < buffer_size))
          {
            *print_buffer++ = (options->ZeroFill == true) ? '0' : ' ';
            out_size++;
          }
        }

        // Write the converted number.
        while ((temp_num_size-- > 0) && (out_size < buffer_size))
        {
          // Write the digit converted to ascii
          *print_buffer++ = hex_digit(*(--temp_num_ptr), options->UpperCase);
          out_size++;
        }
      }
      break;

    case PARSE_DECIMAL:
      {
        uint8_t temp_num[10];
        uint8_t *temp_num_ptr = &temp_num[0];
        int16_t temp_num_size = 0;
        LT_union_uint32_2uint16s value;

        // Get value from argument list and convert it to digits
        // Decode the number to temporary storage as we need to write Most Significant Digit first
        // to the output buffer and we don't know how much to pad yet for MinWidth.
        if (options->Long == true)
        {
          value.LT_uint32 = va_arg(*ap, uint32_t);

          // Handle sign if necessary
          if (options->Signed == true)
          {
            if (value.LT_uint32 >= 0x80000000UL)
            {
              value.LT_uint32 = 0 - value.LT_uint32;
            }
            else
            {
              options->Signed = false;
            }
          }

          // Just convert using 32 bit math as long (haha) as we have to.
          while (value.LT_uint16[MSB] != 0)
          {
            *temp_num_ptr++ = value.LT_uint32 % 10;
            value.LT_uint32 /= 10;
            temp_num_size++;
          }

        }
        else
        {
          value.LT_uint16[LSB] = va_arg(*ap, uint16_t);
          value.LT_uint16[MSB] = 0;

          // Handle sign if necessary
          if (options->Signed == true)
          {
            if (value.LT_uint16[LSB] >= 0x8000U)
            {
              value.LT_uint16[LSB] = 0 - value.LT_uint16[LSB];
            }
            else
            {
              options->Signed = false;
            }
          }
        }

        // Convert using 16 bit math when you can.
        do
        {
          *temp_num_ptr++ = value.LT_uint16[LSB] % 10;
          value.LT_uint16[LSB] /= 10;
          temp_num_size++;
        }
        while (value.LT_uint16[LSB] != 0);

        // Add the negative sign.
        if (options->ZeroFill == true)
        {
          if ((options->Signed == true) && (out_size < buffer_size))
          {
            *print_buffer++ = '-';
            out_size++;
          }
          // Add the positive sign.
          else if ((options->ForceSign == true) && (out_size < buffer_size))
          {
            *print_buffer++ = '+';
            out_size++;
          }
        }

        // Pad and zero fill if the user wants it.
        {
          // calculate what's left for leading zero's.
          int16_t num_zeros = options->MinWidth - (temp_num_size + (options->Signed || options->ForceSign ? 1 : 0));
          while ((num_zeros-- > 0) && (out_size < buffer_size))
          {
            *print_buffer++ = (options->ZeroFill == true) ? '0' : ' ';
            out_size++;
          }
        }

        if (options->ZeroFill == false)
        {
          // Add the negative sign.
          if ((options->Signed == true) && (out_size < buffer_size))
          {
            *print_buffer++ = '-';
            out_size++;
          }
          // Add the positive sign.
          else if ((options->ForceSign == true) && (out_size < buffer_size))
          {
            *print_buffer++ = '+';
            out_size++;
          }
        }

        // Write the converted number.
        while ((temp_num_size-- > 0) && (out_size < buffer_size))
        {
          // Write the digit converted to ascii
          *print_buffer++ = *(--temp_num_ptr) + '0';
          out_size++;
        }
      }
      break;

    case PARSE_FLOAT:
      {
        // Convert float up to 32 integer part, plus MAX_PRECISION fractional part
        uint8_t temp_num[10 + MAX_PRECISION];
        uint8_t *temp_num_ptr = &temp_num[0];
        int16_t temp_num_size = 0;
        int16_t temp_num_fraction_size = 0;
        LT_union_uint32_2uint16s value;
        float fvalue;
        uint32_t int_value;

        // Get value from argument list and convert it to digits
        // Decode the number to temporary storage as we need to write Most Significant Digit first
        // to the output buffer and we don't know how much to pad yet for MinWidth.
        fvalue = va_arg(*ap, double);

        // Handle sign if necessary
        if (fvalue < 0.0)
        {
          fvalue = -fvalue;
          options->Signed = true;
        }
        else
        {
          options->Signed = false;
        }

        // Convert the integer part of the floating point number once
        int_value = fvalue;

        if (fvalue >= 1.0 * 0x100000000ULL)
        {
          // write error code as a complete unit or not at all
          if (buffer_size > 8)
          {
            *print_buffer++ = 'T';
            *print_buffer++ = 'o';
            *print_buffer++ = 'o';
            *print_buffer++ = 'L';
            *print_buffer++ = 'a';
            *print_buffer++ = 'r';
            *print_buffer++ = 'g';
            *print_buffer++ = 'e';
            out_size += 8;
          }
          break;
        }

        // First do Fractional Portion of Float (if desired)
        if (options->Precision != 0)
        {
          if (options->Precision > MAX_PRECISION) options->Precision = MAX_PRECISION;
          value.LT_uint32 = (fvalue - int_value) * PrecisionTable[options->Precision] + 0.5;

          // Just convert using 32 bit math as long (haha) as we have to.
          while ((value.LT_uint16[MSB] != 0) && (options->Precision > temp_num_fraction_size))
          {
            *temp_num_ptr++ = value.LT_uint32 % 10;
            value.LT_uint32 /= 10;
            temp_num_fraction_size++;
          }

          // Convert using 16 bit math when you can.
          do
          {
            *temp_num_ptr++ = value.LT_uint16[LSB] % 10;
            value.LT_uint16[LSB] /= 10;
            temp_num_fraction_size++;
          }
          while (options->Precision > temp_num_fraction_size);

        }
        else
        {
          temp_num_fraction_size = -1; // Set to -1 so that it balances the +1 for the decimal point when subtracting from options->MinWidth
        }

        // Next do Integer Portion of Float
        value.LT_uint32 = int_value;

        // Just convert using 32 bit math as long (haha) as we have to.
        while (value.LT_uint16[MSB] != 0)
        {
          *temp_num_ptr++ = value.LT_uint32 % 10;
          value.LT_uint32 /= 10;
          temp_num_size++;
        };

        // Convert using 16 bit math when you can.
        do
        {
          *temp_num_ptr++ = value.LT_uint16[LSB] % 10;
          value.LT_uint16[LSB] /= 10;
          temp_num_size++;
        }
        while (value.LT_uint16[LSB] != 0);

        // Add the negative sign.
        if (options->ZeroFill == true)
        {
          if ((options->Signed == true) && (out_size < buffer_size))
          {
            *print_buffer++ = '-';
            out_size++;
          }
          // Add the positive sign.
          else if ((options->ForceSign == true) && (out_size < buffer_size))
          {
            *print_buffer++ = '+';
            out_size++;
          }
        }

        // Pad and zero fill if the user wants it.
        {
          // calculate what's left for leading zero's.
          int16_t num_zeros = options->MinWidth - (temp_num_size + temp_num_fraction_size + 1 + (options->Signed || options->ForceSign ? 1 : 0)); // +1 is for decimal point
          while ((num_zeros-- > 0) && (out_size < buffer_size))
          {
            *print_buffer++ = (options->ZeroFill == true) ? '0' : ' ';
            out_size++;
          }
        }

        // Add the negative sign.
        if (options->ZeroFill == false)
        {
          if ((options->Signed == true) && (out_size < buffer_size))
          {
            *print_buffer++ = '-';
            out_size++;
          }
          // Add the positive sign.
          else if ((options->ForceSign == true) && (out_size < buffer_size))
          {
            *print_buffer++ = '+';
            out_size++;
          }
        }

        // Write the converted integer.
        while ((temp_num_size-- > 0) && (out_size < buffer_size))
        {
          // Write the digit converted to ascii
          *print_buffer++ = *(--temp_num_ptr) + '0';
          out_size++;
        }

        // Write the converted fraction(if desired)
        if (options->Precision != 0)
        {

          if (out_size < buffer_size)
          {
            *print_buffer++ = '.';
            out_size++;
          }

          while ((temp_num_fraction_size-- > 0) && (out_size < buffer_size))
          {
            // Write the digit converted to ascii
            *print_buffer++ = *(--temp_num_ptr) + '0';
            out_size++;
          }
        }
      }
      break;

    case PARSE_CHARACTER:
      {
        // Pad and zero fill if the user wants it.
        {
          // calculate what's left for leading zero's.
          int16_t num_zeros = options->MinWidth - (sizeof(char) + out_size);
          while ((num_zeros-- > 0) && (out_size < buffer_size))
          {
            *print_buffer++ = (options->ZeroFill == true) ? '0' : ' ';
            out_size++;
          }
        }

        *print_buffer++ = (int8_t) va_arg(*ap, int16_t);
        out_size++;
      }
      break;

    case PARSE_STRING:
      {
        char *string_ptr = va_arg( *ap, char *);
        char *string_ptr2 = string_ptr;
        int16_t string_size = 0;

        while (*string_ptr2++ != '\0') string_size++;

        // Pad and zero fill if the user wants it.
        {
          // calculate what's left for leading zero's.
          int16_t num_zeros = options->MinWidth - (string_size + out_size);
          while ((num_zeros-- > 0) && (out_size < buffer_size))
          {
            *print_buffer++ = (options->ZeroFill == true) ? '0' : ' ';
            out_size++;
          }
        }

        while ((*string_ptr != '\0') && (out_size < buffer_size))
        {
          *print_buffer++ = *string_ptr++;
          out_size++;
        }

      }
      break;

    case PARSE_FLASH_STRING:
      {
        PGM_P string_ptr = va_arg( *ap, PGM_P);
        PGM_P string_ptr2 = string_ptr;
        int16_t string_size = 0;

        while (pgm_read_byte(string_ptr2++) != '\0') string_size++;

        // Pad and zero fill if the user wants it.
        {
          // calculate what's left for leading zero's.
          int16_t num_zeros = options->MinWidth - (string_size + out_size);
          while ((num_zeros-- > 0) && (out_size < buffer_size))
          {
            *print_buffer++ = (options->ZeroFill == true) ? '0' : ' ';
            out_size++;
          }
        }

        while ((pgm_read_byte(string_ptr) != '\0') && (out_size < buffer_size))
        {
          *print_buffer++ = pgm_read_byte(string_ptr++);
          out_size++;
        }

      }
      break;

    default:
      return 0;
  }

  return out_size;
}

