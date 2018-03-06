
/*!
LT_I2C: Routines to communicate with ATmega328P's hardware I2C port.


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
    @ingroup LT_I2C
    Library Header File for LT_I2C: Routines to communicate with ATmega328P's hardware I2C port.
*/

#ifndef LT_I2C_H
#define LT_I2C_H

#include <stdint.h>
//#include <Wire.h>

//! @name HARDWARE I2C PRESCALER VALUES
//! @{
#define HARDWARE_I2C_PRESCALER_1  0
#define HARDWARE_I2C_PRESCALER_4  1
#define HARDWARE_I2C_PRESCALER_16 2
#define HARDWARE_I2C_PRESCALER_64 3
//! @}

//! @name I2C READ and WRITE BITS
//! @{
//! Eighth bit (LSB) of I2C address indicates a "read" or "write".
//! (The first seven bits are the 7-bit I2C address.)
#define I2C_READ_BIT    0x01
#define I2C_WRITE_BIT   0x00
//! @}

//! @name STATUS BITS
//! @{
#define STATUS_START               0x08
#define STATUS_REPEATED_START      0x10
#define STATUS_ADDRESS_WRITE_ACK   0x18
#define STATUS_ADDRESS_WRITE_NACK  0x20
#define STATUS_WRITE_ACK           0x28
#define STATUS_WRITE_NACK          0x30
#define STATUS_ARBITRATION_LOST    0x38
#define STATUS_ADDRESS_READ_ACK    0x40
#define STATUS_ADDRESS_READ_NACK   0x48
#define STATUS_READ_ACK            0x50
#define STATUS_READ_NACK           0x58
//! @}

//! @name TIMEOUT AND DELAY IN US
//! @{
#define HW_I2C_DELAY  1
#define HW_I2C_TIMEOUT  20000
//! @}

//! @name ACK OR NACK PARAMETER PASSED TO I2C_READ
//! @{
#define WITH_ACK  0  //!<  Use with i2c_read(WITH_ACK) to read with an acknowledge
#define WITH_NACK 1  //!<  Use with i2c_read(WITH_NACK) to read without an acknowledge.  Normally used after the last byte of a multi-byte read.
//! @}

//! @name OPTIONAL I2C Address MACRO
//! @{
#define I2C_8ADDR(address) (address >> 1)  //!< Use to convert an 8-bit I2C address to 7 bits.
//! @}

//! Read a byte, store in "value".
//! @return 0 on success, 1 on failure
int8_t i2c_read_byte(uint8_t address,  //!< 7-bit I2C address
                     uint8_t *value  //!< Byte to be read
                    );

//! Write "value" byte to device at "address"
//! @return 0 on success, 1 on failure
int8_t i2c_write_byte(uint8_t address, //!< 7-bit I2C address
                      uint8_t value  //!< Byte to be written
                     );

//! Read a byte of data at register specified by "command", store in "value"
//! @return 0 on success, 1 on failure
int8_t i2c_read_byte_data(uint8_t address,     //!< 7-bit I2C address
                          uint8_t command,   //!< Command byte
                          uint8_t *value     //!< Byte to be read
                         );

//! Write a byte of data to register specified by "command"
//! @return 0 on success, 1 on failure
int8_t i2c_write_byte_data(uint8_t address,    //!< 7-bit I2C address
                           uint8_t command,  //!< Command byte
                           uint8_t value     //!< Byte to be written
                          );

//! Read a 16-bit word of data from register specified by "command"
//! @return 0 on success, 1 on failure
int8_t i2c_read_word_data(uint8_t address,     //!< 7-bit I2C address
                          uint8_t command,   //!< Command byte
                          uint16_t *value    //!< Word to be read
                         );

//! Write a 16-bit word of data to register specified by "command"
//! @return 0 on success, 1 on failure
int8_t i2c_write_word_data(uint8_t address,    //!< 7-bit I2C address
                           uint8_t command,  //!< Command byte
                           uint16_t value    //!< Word to be written
                          );

//! Read a block of data, starting at register specified by "command" and ending at (command + length - 1)
//! @return 0 on success, 1 on failure
int8_t i2c_read_block_data(uint8_t address,     //!< 7-bit I2C address
                           uint8_t command,  //!< Command byte
                           uint8_t length,   //!< Length of array
                           uint8_t *values   //!< Byte array to be read
                          );

//! Read a block of data, no command byte, reads length number of bytes and stores it in values.
//! @return 0 on success, 1 on failure
int8_t i2c_read_block_data(uint8_t address,     //!< 7-bit I2C address
                           uint8_t length,   //!< Length of array
                           uint8_t *values   //!< Byte array to be read
                          );


//! Write a block of data, starting at register specified by "command" and ending at (command + length - 1)
//! @return 0 on success, 1 on failure
int8_t i2c_write_block_data(uint8_t address,       //!< 7-bit I2C address
                            uint8_t command,     //!< Command byte
                            uint8_t length,      //!< Length of array
                            uint8_t *values      //!< Byte array to be written
                           );

//! Write a two command bytes, then receive a block of data
//! @return 0 on success, 1 on failure
int8_t i2c_two_byte_command_read_block(uint8_t address,    //!< 7-bit I2C address
                                       uint16_t command,   //!< Command word
                                       uint8_t length,     //!< Length of array
                                       uint8_t *values     //!< Byte array to be read
                                      );

//! Initializes Linduino I2C port.
//! Before communicating to the I2C port through the QuikEval connector, you must also run
//! quikeval_I2C_connect to connect the I2C port to the QuikEval connector through the
//! QuikEval MUX (and disconnect SPI).
void quikeval_I2C_init(void);

//! Switch MUX to connect I2C pins to QuikEval connector.
//! This will disconnect SPI pins.
void quikeval_I2C_connect(void);

//! i2c_enable or quikeval_I2C_init must be called before using any of the other I2C routines.
void i2c_enable(void);

//! Write start bit to the hardware I2C port
//! @return 0 if successful, 1 if not successful
int8_t i2c_start();

//! Write a repeat start bit to the hardware I2C port
//! @return 0 if successful, 1 if not successful
int8_t i2c_repeated_start();

//! Write stop bit to the hardware I2C port
void i2c_stop();

//! Send a data byte to hardware I2C port
//! @return 0 if successful, 1 if not successful
int8_t i2c_write(uint8_t data   //!< byte that will be written to hardware I2C port.
                );

//! Read a data byte from the hardware I2C port.
//! If ack is 0 then an acknowledge (ACK) is generated, else a NACK is generated.
//! @return the data byte read.
uint8_t i2c_read(int8_t ack //!< If ACK is 0 then an acknowledge is generated, else a NACK is generated.
                );

//! Poll the I2C port and look for an acknowledge
//! @return Returns 0 if successful, 1 if not successful
int8_t i2c_poll(uint8_t i2c_address //!< i2c_address is the address of the slave being polled.
               );


#endif  // LT_I2C_H
