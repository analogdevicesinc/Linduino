/*!
  TwoWire.h - TWI/I2C library for Arduino & Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts

  Modified 2014 to conserve memory for master mode and PMBus compliance
*/

/*! @file
  @ingroup LT_Wire
  Library Header File for LT_Wire
*/

#ifndef LT_TwoWire_h
#define LT_TwoWire_h

#include <inttypes.h>
#include <Wire.h>


class LT_TwoWire : public TwoWire
{
  public:
    LT_TwoWire();

    //! Initiate Prep
    //! @speed 10000 to 400000
    //! @return void
    void begin(uint32_t speed);

    //! Read from a slave I2C device.
    //! @return number of bytes read.  If different from quantity, something bad happened.
    uint8_t requestFrom(uint8_t address,  //!< 7-bit I2C address
                        uint8_t *acceptBuffer,   //!< buffer pointer to fill
                        uint16_t quantity  //!< anticipated length of read
                       );

    //! Read from a slave I2C device.
    //! @return number of bytes read.  If different from quantity, something bad happened.
    uint8_t requestFrom(uint8_t address,  //!< 7-bit I2C address
                        uint8_t *acceptBuffer,   //!< buffer pointer to fill
                        uint16_t quantity, //!< anticipated length of read
                        uint8_t sendStop //!< whether to STOP or anticipate a repeated START
                       );

    //! Read from a slave I2C device.
    //! @return number of bytes read.  If different from quantity, something bad happened.
    uint8_t requestFrom(int address,    //!< 7-bit I2C address
                        uint8_t *acceptBuffer, //!< buffer pointer to fill
                        int quantity //!< anticipated length of read
                       );

    //! Read from a slave I2C device.
    //! @return number of bytes read.  If different from quantity, something bad happened.
    uint8_t requestFrom(int address,    //!< 7-bit I2C address
                        uint8_t *acceptBuffer,   //!< buffer pointer to fill
                        int quantity,  //!< anticipated length of read
                        int sendStop //!< whether to STOP or anticipate a repeated START
                       );

};

extern LT_TwoWire LT_Wire;

#endif

