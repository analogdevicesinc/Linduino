/*!
LT_I2C_Wire: I2C Routines based on Wire library to communicate with Arduino based boards.

@verbatim


  Wire.endTransmission() returns 0, 1, 2, 3, or 4. To maintain consistency with our Legacy LT_I2C library,
  a zero(false) or a non-zero(true) is tested.

  0 .. success
  1 .. length to long for buffer
  2 .. address send, NACK received
  3 .. data send, NACK received
  4 .. other twi error (lost bus arbitration, bus error, ..)


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


#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C_Wire.h"
#include <Wire.h>

#if defined(ARDUINO_ARCH_AVR)
#include <util/delay.h>
#elif defined(ARDUINO_ARCH_SAM)
#define Wire Wire1
#define _delay_us delayMicroseconds
#elif defined(ARDUINO_ARCH_SAMD)
#include <delay.h>
#define _delay_us delayMicroseconds
#endif

// Read a byte, store in "value".
int8_t i2c_read_byte(uint8_t address, uint8_t *value)
{
  // Wire.requestFrom returns the number of bytes that were requested from the slave.
  // If the address NAcked, the function returns 0. 
  int8_t ret = 0;
  Wire.requestFrom((uint8_t)address, (uint8_t)1, (uint8_t)true);
  while (Wire.available())
  {
    *value = Wire.read();       // Read MSB from buffer
  }
  delay(100);
  if(ret == 0)
	return (1);		// Unsuccessful
  else
	return(0);		// Successful

}

// Write "value" byte to device at "address"
int8_t i2c_write_byte(uint8_t address, uint8_t value)
{
  Wire.beginTransmission(address);    // Transmit to device
  Wire.write(byte(value));              // Sends one byte
  if (Wire.endTransmission())     // stop transmitting
  {
    // endTransmission returns zero on success
    return(1);
  }
  return(0);
}

// Read a byte of data at register specified by "command", store in "value"
int8_t i2c_read_byte_data(uint8_t address, uint8_t command, uint8_t *value)
{
  int8_t ret = 0;
  Wire.beginTransmission(address);
  Wire.write(byte(command));
  if (Wire.endTransmission())     // stop transmitting
  {
    // endTransmission returns zero on success
    return(1);
  }
  ret = Wire.requestFrom((uint8_t)address, (uint8_t)1, (uint8_t)true);
  while (Wire.available())
  {
    *value = Wire.read();               // Read MSB from buffer
  }
  delay(100);
  if(ret == 0)
	return (1);		// Unsuccessful
  else
	return(0);		// Successful
}

// Write a byte of data to register specified by "command"
int8_t i2c_write_byte_data(uint8_t address, uint8_t command, uint8_t value)
{
  Wire.beginTransmission(address);      // transmit to device
  Wire.write(byte(command));            // send instruction byte
  Wire.write(byte(value));            // send value byte
  if (Wire.endTransmission())     // stop transmitting
  {
    // endTransmission returns zero on success
    return(1);
  }
  return(0);
}

// Read a 16-bit word of data from register specified by "command"
int8_t i2c_read_word_data(uint8_t address, uint8_t command, uint16_t *value)
{
  int8_t ret = 0;

  union
  {
    uint8_t b[2];
    uint16_t w;
  } data;

  Wire.beginTransmission(address);
  Wire.write(byte(command));
  if (Wire.endTransmission())     // stop transmitting
  {
    // endTransmission returns zero on success
    return(1);
  }
  ret = Wire.requestFrom((uint8_t)address, (uint8_t)2, (uint8_t)true);
  int i = 1;
  while (Wire.available())
  {
    data.b[i] = Wire.read();                        // Read MSB from buffer
    i--;
    if (i == 0)
      break;
  }
  delay(100);
  *value = data.w;
  if(ret == 0)
	return (1);		// Unsuccessful
  else
	return(0);		// Successful
}

// Write a 16-bit word of data to register specified by "command"
int8_t i2c_write_word_data(uint8_t address, uint8_t command, uint16_t value)
{
  int8_t ret=0;

  union
  {
    uint8_t b[2];
    uint16_t w;
  } data;
  data.w = value;

  Wire.beginTransmission(address);      // transmit to device
  Wire.write(byte(command));            // sends instruction byte
  Wire.write(byte(data.b[1]));          // send value byte
  Wire.write(byte(data.b[0]));          // send value byte
  if (Wire.endTransmission())     // stop transmitting
  {
    // endTransmission returns zero on success
    return(1);
  }
  return(0);
}

// Read a block of data, starting at register specified by "command" and ending at (command + length - 1)
int8_t i2c_read_block_data(uint8_t address, uint8_t command, uint8_t length, uint8_t *values)
{
  uint8_t i = (length-1);
  int8_t ret = 0;

  Wire.beginTransmission(address);
  Wire.write(byte(command));
  if (Wire.endTransmission())     // stop transmitting
  {
    // endTransmission returns zero on success
    return(1);
  }
  ret = Wire.requestFrom((uint8_t)address, (uint8_t)length, (uint8_t)true);

  while (Wire.available())
  {
    values[i] = Wire.read();
    i--;
    if (i == 0)
      break;
  }
  delay(100);
  if(ret == 0)
	return (1);		// Unsuccessful
  else
	return(0);		// Successful
}


// Read a block of data, no command byte, reads length number of bytes and stores it in values.
int8_t i2c_read_block_data(uint8_t address, uint8_t length, uint8_t *values)
{
  uint8_t i = (length-1);
  int8_t ret = 0;

  ret = Wire.requestFrom((uint8_t)address, (uint8_t)length, (uint8_t)true);

  while (Wire.available())
  {
    values[i] = Wire.read();
    i--;
    if (i == 0)
      break;
  }
  delay(100);
  if(ret == 0)
	return (1);		// Unsuccessful
  else
	return(0);		// Successful
}


// Write a block of data, starting at register specified by "command" and ending at (command + length - 1)
int8_t i2c_write_block_data(uint8_t address, uint8_t command, uint8_t length, uint8_t *values)
{
  int8_t i = length-1;
  int8_t ret = 0;

  Wire.beginTransmission(address);    // transmit to device
  Wire.write(byte(command));            // sends instruction byte
  while (i>=0)
  {
    Wire.write(byte(values[i]));        // send values
    i--;
  }
  if (Wire.endTransmission())     // stop transmitting
  {
    // endTransmission returns zero on success
    return(1);
  }
  return(0);
}

// Write two command bytes, then receive a block of data
int8_t i2c_two_byte_command_read_block(uint8_t address, uint16_t command, uint8_t length, uint8_t *values)
{
  int8_t ret = 0;
  union
  {
    uint8_t b[2];
    uint16_t w;
  } comm;
  comm.w = command;
  uint8_t i = (length-1);
  uint8_t readBack = 0;

#if defined(ARDUINO_ARCH_SAM)
  //Wire.beginTransmission(address);
  readBack = Wire.requestFrom((uint8_t)address, (uint8_t)length, (uint32_t)command, (uint8_t)2, (uint8_t)false);
#elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_AVR)
  Wire.beginTransmission(address);
  Wire.write(byte(comm.b[1]));
  Wire.write(byte(comm.b[0]));

  if (Wire.endTransmission(false)) // endTransmission(false) is a repeated start
  {
    // endTransmission returns zero on success
    Wire.endTransmission();
    return(1);
  }
  readBack = Wire.requestFrom((uint8_t)address, (uint8_t)length, (uint8_t)true);
#endif

  if (readBack == length)
  {
    while (Wire.available())
    {
      values[i] = Wire.read();
      if (i == 0)
        break;
      i--;
    }
    return (0);
  }
  else
  {
    return (1);
  }
}

// Initializes Linduino I2C port.
// Before communicating to the I2C port throught the QuikEval connector, you must also run
// quikeval_I2C_connect to connect the I2C port to the QuikEval connector throught the
// QuikEval MUX (and disconnect SPI).
void quikeval_I2C_init(void)
{
  i2c_enable();  //! 1) Enable the I2C port;
}

// Switch MUX to connect I2C pins to QuikEval connector.
// This will disconnect SPI pins.
void quikeval_I2C_connect(void)
{
  // Enable I2C
  pinMode(QUIKEVAL_MUX_MODE_PIN, OUTPUT);       //! 1) Set Mux pin as an output
  if (digitalRead(QUIKEVAL_MUX_MODE_PIN) == LOW) //! 2) If pin is already high, do nothing
  {
    digitalWrite(QUIKEVAL_MUX_MODE_PIN, HIGH);  //! 3) Set the Mux pin to high
    delay(55);                                  //! 4) And wait for LTC4315 to connect (required for rev B)
  }
}

// Setup the hardware I2C interface.
// i2c_enable or quikeval_I2C_init must be called before using any of the other I2C routines.
void i2c_enable()
{
  Wire.begin();
}

/*

// Write start bit to the hardware I2C port
// return 0 if successful, 1 if not successful
int8_t i2c_start()
{
  uint8_t result;
  uint16_t timeout;
  TWCR=(1<<TWINT) | (1<<TWSTA) | (1<<TWEN);                 //! 1) I2C start
  for (timeout = 0; timeout < HW_I2C_TIMEOUT; timeout++)    //! 2) START the timeout loop
  {
    _delay_us(1);
    if (TWCR & (1 << TWINT)) break;                         //! 3) Check the TWINT bit in TWCR
  }
  result=(TWSR & 0xF8);                                     //! 4) Mask the status
  if ((result == STATUS_START) || (result == STATUS_REPEATED_START))
    return(0);                                              //! 5) Return status
  else
    return(1);
}

// Write a repeat start bit to the hardware I2C port
// return 0 if successful, 1 if not successful
int8_t i2c_repeated_start()
{
  uint8_t result;
  uint16_t timeout;
  TWCR=(1<<TWINT) | (1<<TWSTA) | (1<<TWEN);                 //! 1) I2C repeated start
  for (timeout = 0; timeout < HW_I2C_TIMEOUT; timeout++)    //! 2) START the timeout loop
  {
    _delay_us(1);
    if (TWCR & (1 << TWINT)) break;                         //! 3) Check the TWINT bit in TWCR
  }
  result=(TWSR & 0xF8);                                     //! 4) Mask the status
  if (result == STATUS_REPEATED_START)
    return(0);                                              //! 5) Return status
  else
    return(1);
}
// Write stop bit to the hardware I2C port
void i2c_stop()
{
  TWCR=(1<<TWINT) | (1<<TWEN) | (1<<TWSTO);  //! 1) I2C stop
  while (TWCR & (1<<TWSTO));                 //! 2) Wait for stop to complete
}

// Send a data byte to hardware I2C port
// return 0 if successful, 1 if not successful
int8_t i2c_write(uint8_t data)
{
  uint8_t result;
  uint16_t timeout;
  TWDR = data;                                              //! 1) Load data register
  TWCR =(1<<TWINT) | (1<<TWEN);                             //! 2) START transaction
  for (timeout = 0; timeout < HW_I2C_TIMEOUT; timeout++)    //! 3) START the timeout loop
  {
    _delay_us(1);
    if (TWCR & (1 << TWINT)) break;                         //! 4) Check the TWINT bit in TWCR
  }
  result=(TWSR & 0xF8);                                     //! 5) Update status
  // For a generic write, need to consider all three of these cases (processor specific, some may not be this detailed.)
  if ((result == STATUS_WRITE_ACK) || (result == STATUS_ADDRESS_WRITE_ACK) || (result == STATUS_ADDRESS_READ_ACK))
    return(0);                                              //! 6) Return status
  else
    return(1);
}
// Read a data byte from the hardware I2C port.
// Returns the data byte read.
uint8_t i2c_read(int8_t ack)
{
  uint8_t result;
  uint8_t return_value = 1;
  uint16_t timeout;
  uint8_t data;
  if (ack == 0)
  {
    TWCR=(1<<TWINT) | (1<<TWEN) | (1<<TWEA);                //! 1) START transaction with ack
    for (timeout = 0; timeout < HW_I2C_TIMEOUT; timeout++)  //! 2) START timeout loop
    {
      _delay_us(1);
      if (TWCR & (1 << TWINT)) break;                       //! 3) Check the TWINT bit in TWCR
    }
    data = TWDR;                                            //! 4) Get data
    result = TWSR & 0xF8;                                   //! 5) Update status
    if (result == STATUS_READ_ACK) return_value = 0;
  }
  else
  {
    TWCR=(1<<TWINT) | (1<<TWEN);                            //! 6) START transaction with NACK
    for (timeout = 0; timeout < HW_I2C_TIMEOUT; timeout++)
    {
      _delay_us(1);
      if (TWCR & (1 << TWINT)) break;                       //! 7) Check the TWINT bit in TWCR
    }
    data = TWDR;                                            //! 8) Get data
    result = TWSR & 0xF8;                                   //! 9) Update status
    if (result == STATUS_READ_NACK) return_value = 0;
  }
  return(data);
}

// Poll the I2C port and look for an acknowledge
// Returns 0 if successful, 1 if not successful
int8_t i2c_poll(uint8_t i2c_address)

{
  int8_t ack=0;
  ack |= i2c_start();                                  //! 1) I2C start
  ack |= i2c_write((i2c_address<<1) | I2C_WRITE_BIT); //! 2) I2C address + !write
  i2c_stop();                                   //! 3) I2C stop
  return(ack);                                  //! 4) Return ack status
}
*/