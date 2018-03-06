
/*!
LT_I2C: Routines to communicate with ATmega328P's hardware I2C port.

@verbatim

LT_I2C contains the low level routines to communicate with devices using the
ATMega328's onboard hardware I2C port. Each routine checks the Two Wire Status
Register (TWSR) at the end of the transaction and returns 0 if successful and 1
if not successful.

I2C Frequency = (CPU Clock frequency)/(16+2(TWBR)*Prescaler)

TWBR-Two Wire Bit Rate Register
TWCR=Two Wire Control Register (TWINT TWEA TWSTA TWSTO TWWC TWEN - TWIE)
TWSR=Two Wire Status Register

Prescaler Values:
TWSR1  TWSR0  Prescaler
   0      0      1
   0      1      4
   1      0      16
   1      1      64

Examples:
CPU Frequency = 16Mhz on Arduino Uno
I2C Frequency  Prescaler  TWSR1  TWSR0  TWBR
  1khz         64         1      1      125
  10khz        64         1      1      12
  50khz        16         1      0      10
  100khz        4         0      1      18
  400khz        1         0      0      12

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

//! @ingroup Linduino
//! @{
//! @defgroup LT_I2C LT_I2C: Routines to Communicate With ATmega328P's hardware I2C port.
//! @}

/*! @file
    @ingroup LT_I2C
    Library for LT_I2C: Routines to Communicate With ATmega328P's hardware I2C port.
*/

#include <Arduino.h>
#include <stdint.h>
#include <util/delay.h>
#include "Linduino.h"
#include "LT_I2C.h"

//! CPU master clock frequency
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

// Read a byte, store in "value".
int8_t i2c_read_byte(uint8_t address, uint8_t *value)
{
  uint8_t ret=0;
  if (i2c_start()!=0)                                //I2C START
    return(1);                                  //Stop and return 0 if START fail
  ret |= i2c_write((address<<1)|I2C_READ_BIT);                // Write the I2C 7-bit address with R bit
  if (ret != 0)                                   // Returns 1 if failed
    return(1);

  *value = i2c_read(WITH_NACK);                           // Read byte from I2C Bus with NAK
  i2c_stop();                                     //I2C STOP
  return(0);                                      // Return success

}

// Write "value" byte to device at "address"
int8_t i2c_write_byte(uint8_t address, uint8_t value)
{
  int8_t ret= 0 ;

  if (i2c_start()!=0)                                //I2C START
    return(1);                                  //Stop and return 0 if START fail
  ret |= i2c_write((address<<1)|I2C_WRITE_BIT);        //Write the I2C 7 bit address with W bit
  ret |= i2c_write(value);            //Write value
  i2c_stop();                         //I2C STOP
  if (ret!=0)                            // Returns 1 if failed
    return(1);
  return(0);                          // Returns 0 if success
}

// Read a byte of data at register specified by "command", store in "value"
int8_t i2c_read_byte_data(uint8_t address, uint8_t command, uint8_t *value)
{
  int8_t ret = 0;
  if (i2c_start()!=0)                                //I2C START
    return(1);                                  //Stop and return 0 if START fail
  ret |= i2c_write((address<<1)|I2C_WRITE_BIT);        // Write 7 bit address with W bit
  if (ret!=0)   //If NACK return 1
  {
    i2c_stop();                         //I2C STOP
    return(1);
  }
  ret|= i2c_write(command);                 // Set register to be read to command
  if (ret!=0)   //If NACK return 1
  {
    i2c_stop();                         //I2C STOP
    return(1);
  }
  if (i2c_start()!=0)                                 //I2C repeated START
  {
    i2c_stop();                                 //Attempt to issue I2C STOP
    return(1);                                  //Stop and return 0 if START fail
  }
  ret |= i2c_write((address<<1)|I2C_READ_BIT);     // Write 7 bit address with R bit
  *value = i2c_read(WITH_NACK);               // Read byte from buffer with NAK
  i2c_stop();                         // I2C STOP
  if (ret!=0)                         //If there was a NAK return 1
    return(1);
  return(0);                     // Return success
}

// Write a byte of data to register specified by "command"
int8_t i2c_write_byte_data(uint8_t address, uint8_t command, uint8_t value)
{
  int8_t ret = 0;

  if (i2c_start()!=0)                                //I2C START
    return(1);                                  //Stop and return 0 if START fail
  ret |= i2c_write((address<<1)|I2C_WRITE_BIT);        // Write 7 bit address with W bit
  ret|= i2c_write(command);                 // Set register to be read to command
  ret|= i2c_write(value);
  i2c_stop();                        // I2C STOP

  if (ret!=0)                         //If there was a NAK return 1
    return(1);
  return(0);                     // Return success
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


  if (i2c_start()!=0)                                //I2C START
    return(1);                                  //Stop and return 0 if START fail
  ret |= i2c_write((address<<1)|I2C_WRITE_BIT);        // Write 7 bit address with W bit
  if (ret!=0)   //If NACK return 1
  {
    i2c_stop();                         //I2C STOP
    return(1);
  }
  ret |= i2c_write(command);                 // Set register to be read to command
  if (ret!=0)   //If NACK return 1
  {
    i2c_stop();                         //I2C STOP
    return(1);
  }
  if (i2c_start()!=0)                                 //I2C START
  {
    i2c_stop();                                 //Attempt to issue I2C STOP
    return(1);                                  //Stop and return 0 if START fail
  }
  ret |= i2c_write((address<<1)|I2C_READ_BIT);     // Write 7 bit address with R bit

  data.b[1] = i2c_read(WITH_ACK);                        // Read MSB from buffer
  data.b[0] = i2c_read(WITH_NACK);                        // Read LSB from buffer
  i2c_stop();                         //I2C STOP

  *value = data.w;

  if (ret!=0)                         //If NAK
    return (1);                     //return 1
  return(0);                                      // Return success
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

  if (i2c_start()!=0)                                //I2C START
    return(1);                                  //Stop and return 0 if START fail
  ret |= i2c_write((address<<1)|I2C_WRITE_BIT);       // Write 7 bit address with W bit
  ret|= i2c_write(command);                 // Set register to be read to command
  ret|= i2c_write(data.b[1]);         //Write MSB
  ret|= i2c_write(data.b[0]);         //Write LSB;

  i2c_stop();                        // I2C STOP

  if (ret!=0)                         //If there was a NAK return 1
    return(1);
  return(0);


}

// Read a block of data, starting at register specified by "command" and ending at (command + length - 1)
int8_t i2c_read_block_data(uint8_t address, uint8_t command, uint8_t length, uint8_t *values)
{
  uint8_t i = (length-1);
  int8_t ret = 0;

  if (i2c_start()!=0)                                //I2C START
    return(1);                                  //Stop and return 0 if START fail
  ret |= i2c_write((address<<1)|I2C_WRITE_BIT);       //Write 7-bit address with W bit
  if (ret!=0)   //If NACK return 1
  {
    i2c_stop();                         //I2C STOP
    return(1);
  }
  ret|= i2c_write(command);                           //Write 8 bit command word
  if (ret!=0)   //If NACK return 1
  {
    i2c_stop();                         //I2C STOP
    return(1);
  }
  if (i2c_start()!=0)                                 //I2C repeated START
  {
    i2c_stop();                                 //Attempt to issue I2C STOP
    return(1);                                  //Stop and return 0 if START fail
  }
  ret |= i2c_write((address<<1)|I2C_READ_BIT);        //Write 7-bit address with R bit

  if (ret!=0)   //If NACK return 1
  {
    i2c_stop();                         //I2C STOP
    return(1);
  }
  while (i>0)                         //Begin read loop
  {
    values[i] = i2c_read(WITH_ACK); //Read from bus with ACK
    i--;
  }

  values[0] = i2c_read(WITH_NACK);    //Read from bus with NACK for the last one;

  i2c_stop();                         //I2C STOP


  return(0);                           // Success!
}


// Read a block of data, no command byte, reads length number of bytes and stores it in values.
int8_t i2c_read_block_data(uint8_t address, uint8_t length, uint8_t *values)
{
  uint8_t i = (length-1);
  int8_t ret = 0;

  if (i2c_start()!=0)                                //I2C START
    return(1);                                  //Stop and return 0 if START fail
  ret |= i2c_write((address<<1)|I2C_READ_BIT);        //Write 7-bit address with R bit

  if (ret!=0)   //If NACK return 1
  {
    i2c_stop();                         //I2C STOP
    return(1);
  }
  while (i>0)                         //Begin read loop
  {
    values[i] = i2c_read(WITH_ACK); //Read from bus with ACK
    i--;
  }

  values[0] = i2c_read(WITH_NACK);    //Read from bus with NACK for the last one;

  i2c_stop();                         //I2C STOP


  return(0);                           // Success!
}


// Write a block of data, starting at register specified by "command" and ending at (command + length - 1)
int8_t i2c_write_block_data(uint8_t address, uint8_t command, uint8_t length, uint8_t *values)
{
  int8_t i = length-1;
  int8_t ret = 0;

  if (i2c_start()!=0)                                //I2C START
    return(1);                                  //Stop and return 0 if START fail
  ret |= i2c_write((address<<1)|I2C_WRITE_BIT);        // Write 7 bit address with W bit
  ret|= i2c_write(command);           // Set register to be read to command

  while (i>=0)
  {
    ret|= i2c_write(values[i]);     //Write Value
    i--;
  }
  i2c_stop();                        // I2C STOP

  if (ret!=0)
    return(1);
  else
    return(0);                      // Success!
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


  if (i2c_start()!=0)                                //I2C START
    return(1);                                  //Stop and return 0 if START fail
  ret |= i2c_write((address<<1)|I2C_WRITE_BIT);           //Write 7-bit address with W bit
  ret |= i2c_write(comm.b[1]);                             //Write MSB command word
  ret |= i2c_write(comm.b[0]);                             // Write LSB of command
  if (i2c_start()!=0)                                 //I2C repeated START
  {
    i2c_stop();                                 //Attempt to issue I2C STOP
    return(1);                                  //Stop and return 0 if START fail
  }
  ret |= i2c_write((address<<1)|I2C_READ_BIT);            //Write 7-bit address with R bit
  if (ret!=0)                                              //If NACK return 1
  {
    i2c_stop();
    return(1);
  }
  while (i> 0)                         //Begin read loop
  {
    values[i] = i2c_read(WITH_ACK); //Read from bus with ACK
    i--;
  }

  values[0] = i2c_read(WITH_NACK);    //Read from bus with NACK for the last one;
  i2c_stop();                         //I2C STOP


  return(0);                           // Success!

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
  // set these for 100KHz to match the DC590
  TWSR = (HARDWARE_I2C_PRESCALER_4 & 0x03);  //! 1) set the prescaler bits
  TWBR = 18;                                 //! 2) set the bit rate

}


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
