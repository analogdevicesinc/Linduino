/*
  LT_TwoWire.cpp - TWI/I2C library for Wiring & Arduino
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

//! @ingroup PMBus_SMBus
//! @{
//! @defgroup LT_Wire LT_Wire: Routines to Communicate With the hardware I2C port.
//@ @}

/*! @file
  @ingroup LT_Wire
  Library File for LT_Wire
*/
extern "C" {
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <utility/twi.h>

}

#include "LT_Wire.h"
#include "Arduino.h" //for serial debug

// Constructors ////////////////////////////////////////////////////////////////

LT_TwoWire::LT_TwoWire()
{
}

// Public Methods //////////////////////////////////////////////////////////////

void LT_TwoWire::begin(uint32_t speed)
{
  TwoWire::begin();
}


uint8_t LT_TwoWire::requestFrom(uint8_t address, uint8_t *acceptBuffer, uint16_t quantity, uint8_t sendStop)
{

  // perform blocking read into buffer
  uint16_t read = twi_readFrom(address, acceptBuffer, quantity, sendStop);

  return read;
}

uint8_t LT_TwoWire::requestFrom(uint8_t address, uint8_t *acceptBuffer, uint16_t quantity)
{
  return requestFrom((uint8_t)address, (uint8_t *) acceptBuffer, (uint16_t)quantity, (uint8_t)true);
}

uint8_t LT_TwoWire::requestFrom(int address, uint8_t *acceptBuffer, int quantity)
{
  return requestFrom((uint8_t)address, (uint8_t *) acceptBuffer, (uint16_t)quantity, (uint8_t)true);
}

uint8_t LT_TwoWire::requestFrom(int address, uint8_t *acceptBuffer, int quantity, int sendStop)
{
  return requestFrom((uint8_t)address, (uint8_t *) acceptBuffer, (uint16_t)quantity, (uint8_t)sendStop);
}

// Preinstantiate Objects //////////////////////////////////////////////////////

LT_TwoWire LT_Wire = LT_TwoWire();

