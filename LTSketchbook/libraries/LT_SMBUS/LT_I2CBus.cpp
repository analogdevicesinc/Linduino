/*!
LTC I2CBus Support: Routines to communicate to I2C by Wire Library.

@verbatim

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
//! @defgroup LT_I2CBus LT_I2CBus: Routines to communicate to I2C by Wire Library.
//! @}

/*! @file
    @ingroup LT_I2CBus
    Library File for LT_I2CBus
*/

#include <Arduino.h>
#include <stdint.h>
//#include <util/delay.h>
#include "Linduino.h"
#include "LT_I2CBus.h"

LT_I2CBus::LT_I2CBus()
{
  speed_ = 100000;
  LT_Wire.begin(speed_);
  inGroupProtocol_ = false;
}

LT_I2CBus::LT_I2CBus(uint32_t speed)
{
  speed_ = speed;
  LT_Wire.begin(speed_);
  inGroupProtocol_ = false;
}

void LT_I2CBus::changeSpeed(uint32_t speed)
{
  LT_Wire.begin(speed);
}

uint32_t LT_I2CBus::getSpeed()
{
  return speed_;
}

// Read a byte, store in "value".
int8_t LT_I2CBus::readByte(uint8_t address, uint8_t *value)
{
  uint8_t ret = 0;
  LT_Wire.beginTransmission(address);
  LT_Wire.requestFrom(address, value, (uint16_t)1);


  return ret;
}

// Write "value" byte to device at "address"
int8_t LT_I2CBus::writeByte(uint8_t address, uint8_t value)
{
  int8_t ret = 1;

  LT_Wire.beginTransmission(address);
  LT_Wire.write(value);
  ret = LT_Wire.endTransmission(!inGroupProtocol_);
  return ret;
}

// Read a byte of data at register specified by "command", store in "value"
int8_t LT_I2CBus::readByteData(uint8_t address, uint8_t command, uint8_t *value)
{
  int8_t ret = 1;
  LT_Wire.beginTransmission(address);
  LT_Wire.write(command);
  ret = LT_Wire.endTransmission(false);
  LT_Wire.beginTransmission(address);
  LT_Wire.requestFrom(address, value, (uint16_t)1);

  return ret;                             // Return result
}

// Write a byte of data to register specified by "command"
int8_t LT_I2CBus::writeByteData(uint8_t address, uint8_t command, uint8_t value)
{
  int8_t ret = 1;

  LT_Wire.beginTransmission(address);
  LT_Wire.write(command);
  LT_Wire.write(value);
  ret = LT_Wire.endTransmission(!inGroupProtocol_);
  return ret;
}

// Read a 16-bit word of data from register specified by "command"
int8_t LT_I2CBus::readWordData(uint8_t address, uint8_t command, uint16_t *value)
{
  int8_t ret = 1;

  LT_Wire.beginTransmission(address);
  LT_Wire.write(command);
  ret = LT_Wire.endTransmission(false);
  LT_Wire.beginTransmission(address);
  uint8_t tempHolder[2];
  LT_Wire.requestFrom(address, tempHolder, (uint16_t)2);
  *value = tempHolder[0] << 8;
  *value |= tempHolder[1];

  return ret;
}

// Write a 16-bit word of data to register specified by "command"
int8_t LT_I2CBus::writeWordData(uint8_t address, uint8_t command, uint16_t value)
{
  int8_t ret = 1;

  LT_Wire.beginTransmission(address);
  LT_Wire.write(command);
  LT_Wire.write(value >> 8);
  LT_Wire.write(value & 0xFF);
  ret = LT_Wire.endTransmission(!inGroupProtocol_);

  return ret;
}

int8_t LT_I2CBus::readBlockData(uint8_t address, uint8_t command, uint16_t length, uint8_t *values)
{
  int8_t ret = 0;
  LT_Wire.beginTransmission(address);
  LT_Wire.write(command);
  ret = LT_Wire.endTransmission(false);
  LT_Wire.beginTransmission(address);
  LT_Wire.requestFrom(address, values, length);

  return ret;
}


// Read a block of data, no command byte, reads length number of bytes and stores it in values.
int8_t LT_I2CBus::readBlockData(uint8_t address, uint16_t length, uint8_t *values)
{
  int8_t ret = 0;

  LT_Wire.beginTransmission(address);
  LT_Wire.requestFrom(address, values, length);

  return ret;
}


// Write a block of data, starting at register specified by "command" and ending at (command + length - 1)
int8_t LT_I2CBus::writeBlockData(uint8_t address, uint8_t command, uint16_t length, uint8_t *values)
{
  uint8_t i = length;
  int8_t ret = 1;

  LT_Wire.beginTransmission(address);
  LT_Wire.write(command);
  do
  {
    i--;
  }
  while (LT_Wire.write(values[length - 1 - i]) == 1 && i > 0);

  ret = LT_Wire.endTransmission(!inGroupProtocol_);

  return ret;
}

// Write two command bytes, then receive a block of data
int8_t LT_I2CBus::twoByteCommandReadBlock(uint8_t address, uint16_t command, uint16_t length, uint8_t *values)
{
  int8_t ret = 0;

  LT_Wire.beginTransmission(address);
  LT_Wire.write(command >> 8);
  LT_Wire.write(command & 0xFF);
  ret = LT_Wire.endTransmission(false);
  LT_Wire.beginTransmission(address);
  LT_Wire.requestFrom(address, values, length);


  return ret;
}

// Initializes Linduino I2C port.
// Before communicating to the I2C port throught the QuikEval connector, you must also run
// quikeval_I2C_connect to connect the I2C port to the QuikEval connector throught the
// QuikEval MUX (and disconnect SPI).
void LT_I2CBus::quikevalI2CInit(void)
{
//  enable();  //! 1) Enable the I2C port;
}

// Switch MUX to connect I2C pins to QuikEval connector.
// This will disconnect SPI pins.
void LT_I2CBus::quikevalI2CConnect(void)
{
  // Enable I2C
  pinMode(QUIKEVAL_MUX_MODE_PIN, OUTPUT);       //! 1) Set Mux pin as an output
  digitalWrite(QUIKEVAL_MUX_MODE_PIN, HIGH);    //! 2) Set the Mux pin to high
}


void LT_I2CBus::startGroupProtocol()
{
  inGroupProtocol_ = true;
}
void LT_I2CBus::endGroupProtocol()
{
  inGroupProtocol_ = false;
}
