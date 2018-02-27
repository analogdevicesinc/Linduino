//! @todo Review this file.
/*
Linduino.h

This file contains the hardware definitions for the Linduino.


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
    @ingroup Linduino
    @ingroup QuikEval
    Header File for Linduino Libraries and Demo Code
*/

#ifndef LINDUINO_H
#define LINDUINO_H

#include <Arduino.h>  // typedefs use types defined in this header file.

//! @name LINDUINO PIN ASSIGNMENTS
//! @{

#define QUIKEVAL_GPIO 9          //!< Linduino QuikEval GPIO pin (QuikEval connector pin 14) connects to Arduino pin 9
#define QUIKEVAL_CS SS           //!< QuikEval CS pin (SPI chip select on QuikEval connector pin 6) connects to Arduino SS pin.
#define QUIKEVAL_MUX_MODE_PIN 8  /*!< QUIKEVAL_MUX_MODE_PIN defines the control pin for the QuikEval MUX.
The I2C port's SCL and the SPI port's SCK signals share the same pin on the Linduino's QuikEval connector.
Additionally, the I2C port's SDA and the SPI port's MOSI signals share the same pin on the Linduino's QuikEval connector.
The pair of pins connected to the QuikEval connector is switched using a MUX on the Linduino board.
The control pin to switch the MUX is defined as QUIKEVAL_MUX_MODE_PIN (Arduino pin 8). */
//! @}

// Macros
//! Set "pin" low
//! @param pin pin to be driven LOW
#define output_low(pin)   digitalWrite(pin, LOW)
//! Set "pin" high
//! @param pin pin to be driven HIGH
#define output_high(pin)  digitalWrite(pin, HIGH)
//! Return the state of pin "pin"
//! @param pin pin to be read (HIGH or LOW).
//! @return the state of pin "pin"
#define input(pin)        digitalRead(pin)

//! @name ENDIAN DEPENDENT BYTE INDEXES
//! @{
//! Arduino/Linduino is a Little Endian Device, where the least significant byte is stored in the first byte of larger data types.
#ifdef BIG_ENDIAN
#define LSB 1 //!< Location of Least Signficant Byte when Word is accessed as Byte Array
#define MSB 0 //!< Location of Most Signficant Byte when Word is accessed as Byte Array
#define LSW 1 //!< Location of Least Signficant Word when Long Word is accessed as Byte Array
#define MSW 0 //!< Location of most Signficant Word when Long Word is accessed as Byte Array
#else
#define LSB 0 //!< Location of Least Signficant Byte when Word is accessed as Byte Array
#define MSB 1 //!< Location of Most Signficant Byte when Word is accessed as Byte Array
#define LSW 0 //!< Location of Least Signficant Word when Long Word is accessed as Byte Array
#define MSW 1 //!< Location of most Signficant Word when Long Word is accessed as Byte Array
#endif
//! @}

//! This union splits one int16_t (16-bit signed integer) or uint16_t (16-bit unsigned integer)
//! into two uint8_t's (8-bit unsigned integers) and vice versa.
  union LT_union_int16_2bytes
  {
    int16_t LT_int16;    //!< 16-bit signed integer to be converted to two bytes
    uint16_t LT_uint16;  //!< 16-bit unsigned integer to be converted to two bytes
    uint8_t LT_byte[2];  //!< 2 bytes (unsigned 8-bit integers) to be converted to a 16-bit signed or unsigned integer
  };

//! This union splits one int32_t (32-bit signed integer) or uint32_t (32-bit unsigned integer)
//! four uint8_t's (8-bit unsigned integers) and vice versa.
union LT_union_int32_4bytes
{
  int32_t LT_int32;       //!< 32-bit signed integer to be converted to four bytes
  uint32_t LT_uint32;     //!< 32-bit unsigned integer to be converted to four bytes
  uint8_t LT_byte[4];     //!< 4 bytes (unsigned 8-bit integers) to be converted to a 32-bit signed or unsigned integer
};

//! This union splits one int32_t (32-bit signed integer) or uint32_t (32-bit unsigned integer)
//! into two uint16_t's (16-bit unsigned integers) and vice versa.
union LT_union_uint32_2uint16s
{
  int32_t LT_int32;       //!< 32-bit signed integer to be converted to four bytes
  uint32_t LT_uint32;     //!< 32-bit unsigned integer to be converted to four bytes
  uint16_t LT_uint16[2];  //!< 2 words (unsigned 16-bit integers) to be converted to a 32-bit signed or unsigned integer
};

//! This union splits one float into four uint8_t's (8-bit unsigned integers) and vice versa.
union LT_union_float_4bytes
{
  float LT_float;      //!< float to be converted to four bytes
  uint8_t LT_byte[4];  //!< 4 bytes (unsigned 8-bit integers) to be converted to a float
};


#endif  // LINDUINO_H
