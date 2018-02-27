/*!
QuikEval EEPROM Library

Routines to read and parse the ID string from the EEPROM
on a QuikEval-compatible demo board.

The QuikEval ID string has the following 50 byte format :
@verbatim
LTCxxxx,cls,Dxxxx,xx,yy,DC,DCxxxx--------------,\n\0
Example: LTC2309,Cls,D1859,01,01,DC,DC1337A-A,------------\n\0
@endverbatim

Also included are the routines to communicate with an external serial I2C EEPROM
such as the 24LC025 found on all LTC QuikEval compatible demo boards. The routines
use the ATMega328 hardware I2C port. The routines require the use of the hardware I2C
routines found in LT_I2C.h


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
//! @defgroup QuikEval QuikEval EEPROM library
//! @}

/*! @file
    @ingroup QuikEval
    Library for reading from and writing to the EEPROM on QuikEval-compatible demo boards.
*/

#include <Arduino.h>
#include <stdint.h>
#include <util/delay.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"

struct demo_board_type demo_board;

// Read the id string from the EEPROM, then parse the
// product name, demo board name, and demo board option
// from the id string into the global demo_board variable.
// Returns the number of characters read from the information string.
uint8_t read_quikeval_id_string(char *buffer)
{
  uint8_t i, j;                // Iteration variables
  uint8_t comma_position[8] = {0};  // Contains the position of the commas in the buffer
  uint8_t comma_count = 0;      // The number of commas found in the buffer
  uint8_t buffer_count;       // The number of characters read
  int8_t option;              // Temporary demo board option
  // read the id string from the demo board EEPROM
  // starting EEPROM address=0, terminator=0x0a, max buffer length=52 bytes
  // Enable I2C
  pinMode(QUIKEVAL_MUX_MODE_PIN, OUTPUT);   // Configure MUX to disconnect I2C pins on QuikEval connector. Remember EEPROM has its own I2C pins on the the connector.
  int8_t QUIKEVAL_MUX_MODE_PIN_state;
  QUIKEVAL_MUX_MODE_PIN_state = digitalRead(QUIKEVAL_MUX_MODE_PIN);
  digitalWrite(QUIKEVAL_MUX_MODE_PIN, LOW);
  buffer_count = eeprom_read_buffer_with_terminator(EEPROM_I2C_ADDRESS, buffer, 0, QUIKEVAL_ID_TERMINATOR, QUIKEVAL_ID_SIZE+2);
  digitalWrite(QUIKEVAL_MUX_MODE_PIN, QUIKEVAL_MUX_MODE_PIN_state);  // Restore the QUIKEVAL_MUX_MODE_PIN (8) to its previous state.  (But, it will be left as output.)
  if (buffer_count == 0) return(0);   // quit if no data read
  // find comma positions
  for (i = 0; i < buffer_count; i++)
  {
    if (buffer[i] == ',') comma_position[comma_count++]=i;
  }

  if (comma_position[6] < comma_position[5])// comma_position[6]=strlen(buffer);  // some demo boards are missing the last comma
  {
    for (i = buffer_count - 2; i > comma_position[5]; i--)
    {
      if (buffer[i] != '-')
      {
        comma_position[6] = i+1;
        break;
      }
    }
  }



  // get product name. All characters before the 1st comma.
  for (i = 0; i < comma_position[0]; i++)
  {
    demo_board.product_name[i]=buffer[i];
  }
  demo_board.product_name[i]='\0';  // add terminator
  // Get demo board name between the 5th and 6th comma
  j = 0;

  for (i = comma_position[5]+1; i < comma_position[6]; i++)
  {
    demo_board.name[j++]=buffer[i];
  }
  demo_board.name[j]='\0';  // add terminator
  // get demo board option from the demo board name
  for (i = 0; i < strlen(demo_board.name); i++)
  {
    if (demo_board.name[i] == '-') option = demo_board.name[i+1];
  }
  if ((option >= 0x41) && (option <= 0x5A)) demo_board.option = option;
  // final demo board name is 6 characters without the option
  demo_board.name[6]='\0';
  return(buffer_count);
}

// Read the ID string from the EEPROM and determine if the correct board is connected.
// Returns 1 if successful, 0 if not successful
int8_t discover_demo_board(char *demo_name)
{
  int8_t connected;
  connected = 1;
  // read the ID from the serial EEPROM on the board
  // reuse the buffer declared in UserInterface
  if (read_quikeval_id_string(&ui_buffer[0]) == 0) connected = 0;
  // make sure it is the correct demo board
  if (strcmp(demo_board.name, demo_name) != 0) connected = 0;
  if (connected != 0)
  {
    Serial.print("Demo Board Name: ");
    Serial.println(demo_board.name);
    Serial.print("Product Name: ");
    Serial.println(demo_board.product_name);
    if (demo_board.option)
    {
      Serial.print("Demo Board Option: ");
      Serial.println(demo_board.option);
    }
  }
  else
  {
    Serial.print("Demo board ");
    Serial.print(demo_name);
    Serial.print(" not found, \nfound ");
    Serial.print(demo_board.name);
    Serial.println(" instead. \nConnect the correct demo board, then press the reset button.");
  }
  return(connected);
}



// Determine if the EEPROM is ready for communication by writing
// the address+!write byte and looking for an acknowledge. This is
// repeated every 1ms until an acknowledge occurs, or a timeout occurs.
// If a timeout occurs, an I2C stop is generated.
// Returns 0 if an acknowledge is generated and 1 if not.
int8_t eeprom_poll(uint8_t i2c_address)
{
  uint8_t timer_count;
  int8_t ack;
  i2c_start();                            // I2C start
  ack = i2c_write(i2c_address | I2C_WRITE_BIT);     // I2C address + !write
  if (ack != 0)
  {
    // acknowledge not received, enter timer loop
    for (timer_count = 0; timer_count < EEPROM_TIMEOUT; timer_count++)
    {
      i2c_repeated_start();               // I2C repeated start
      ack = i2c_write(i2c_address | I2C_WRITE_BIT);  // I2C address + !write
      if (ack == 0)
        break;                  // If the EEPROM acknowledges, jump out of delay loop
      else
        delay(1);
    }
    if (ack) i2c_stop();                    // Generate a stop if no acknowledge
  }
  return(ack);
}

// Wait for the EEPROM write cycle to complete by executing
// the acknowledge polling loop.
// Returns 0 if an acknowledge is generated and 1 if not.
int8_t eeprom_write_poll(uint8_t i2c_address)
{
  _delay_us(10);
  return(eeprom_poll(i2c_address));
}

// Write the data byte to the EEPROM with i2c_address starting at EEPROM address.
// Returns the total number of bytes written
uint8_t eeprom_write_byte(uint8_t i2c_address, char data, uint16_t address)
{
  uint8_t byte_count = 0;
  if (address < EEPROM_DATA_SIZE)     // Make sure the address is in bounds
  {
    if (!eeprom_poll(i2c_address))  // Check if EEPROM is ready
    {
      if (EEPROM_DATA_SIZE > 0x100)
        i2c_write(address>>8);   // Send upper byte of address if size > 256 bytes
      i2c_write(address);        // Send lower byte of address
      i2c_write(data);           // Send byte to EEPROM
      i2c_stop();                // I2C stop
      byte_count = 1;
    }
  }
  return(byte_count);
}

// Write the data byte array to the EEPROM with i2c_address starting at EEPROM address.
// Returns the total number of bytes written
uint8_t eeprom_write_byte_array(uint8_t i2c_address, char data[], uint16_t address, uint8_t num_bytes)
{
  uint8_t i;
  for (i = 0; i < num_bytes; i++)
  {
    if (eeprom_write_byte(i2c_address, data[i], address+i) != 1) break;
  }
  return (i);
}

// Write the buffer to the EEPROM with i2c_address starting at EEPROM address in blocks of EEPROM_PAGE_SIZE bytes.
// Returns the total number of byte written
uint8_t eeprom_write_buffer(uint8_t i2c_address, char *buffer, uint16_t address)
{
  uint8_t i, j;
  uint8_t index = 0;
  uint8_t page_count;
  uint8_t byte_count;
  page_count=((strlen(buffer)-1)/EEPROM_PAGE_SIZE)+1; // Calculate page count
  if (address < EEPROM_DATA_SIZE)                       // Make sure the address is in bounds
  {
    for (i = 0; i < page_count; i++)
    {
      if (!eeprom_poll(i2c_address))                  // Check if EEPROM is ready
      {
        if (EEPROM_DATA_SIZE > 0x100)
          i2c_write(address>>8);                      // Send upper byte of address if size > 256 bytes
        i2c_write(address);                           // Send lower byte of address
        byte_count = strlen(buffer)-index;              // Calculate the remaining byte count
        if (byte_count > EEPROM_PAGE_SIZE)              // Limit to EEPROM_PAGE_SIZE
          byte_count = EEPROM_PAGE_SIZE;
        for (j = 0; j < byte_count; j++)
        {
          if (address >= EEPROM_DATA_SIZE) break;       // Quit if the address is at the end
          i2c_write(buffer[index++]);                 // Write byte to EEPROM
          address++;                                  // Increment the EEPROM address
        }
        i2c_stop();                                   // The stop bit starts the write process
      }
      else
        break;
    }
  }
  return(index);                                      // Return the number of bytes written
}

// Read a data byte at address from the EEPROM with i2c_address.
// Returns the number of bytes read.
uint8_t eeprom_read_byte(uint8_t i2c_address, char *data, uint16_t address)
{
  uint8_t byte_count = 0;
  if (address < EEPROM_DATA_SIZE)          // Make sure the address is in bounds
  {
    if (!eeprom_poll(i2c_address))        // Check if EEPROM is ready
    {
      if (EEPROM_DATA_SIZE > 0x100)
        i2c_write(address>>8);          // Send upper byte of address if size > 256 bytes
      i2c_write(address);               // Send lower byte of address
      i2c_repeated_start();             // Repeated start
      i2c_write(i2c_address | I2C_READ_BIT);    // I2C address + read
      *data = i2c_read(WITH_NACK);                // Read last byte from EEPROM with NACK
      i2c_stop();                       // I2C stop
      byte_count = 1;
    }
  }
  return(byte_count);
}

// Read a data byte at address from the EEPROM with i2c_address.
// Returns the number of bytes read.
uint8_t eeprom_read_byte_array(uint8_t i2c_address, char *data, uint16_t address, uint8_t num_bytes)
{
  uint8_t i;
  for (i = 0; i < num_bytes; i++)
  {
    if (eeprom_read_byte(i2c_address, data+i, address+i) != 1) break;
  }
  return (i);
}

// Read data bytes from the EEPROM starting at address until number bytes read equals count. A null terminator is
// added to the end of the buffer.
// Returns the number of bytes read.
uint8_t eeprom_read_buffer(uint8_t i2c_address, char *buffer, uint16_t address, uint8_t count)
{
  uint8_t i = 0;
  uint8_t data;
  *buffer='\0';                        // Initialize buffer with null
  if (!eeprom_poll(i2c_address))    // Check if the EEPROM is ready
  {
    if (EEPROM_DATA_SIZE > 0x100)
      i2c_write(address>>8);        // Send upper byte of address if size > 256 bytes
    i2c_write(address);             // Send lower byte of address
    i2c_repeated_start();           // I2C repeated start
    i2c_write(i2c_address | I2C_READ_BIT);  // I2C address + read
    for (i = 0; i < count-1; i++)       // Read count-1 bytes with ACK
    {
      data = i2c_read(WITH_ACK);             // Receive 1 EEPROM data byte with ACK
      *buffer++=data;               // Place data byte in buffer and increment pointer
    }
    data = i2c_read(WITH_NACK);               // Read last byte from EEPROM with NACK
    *buffer++=data;
    i2c_stop();                     // I2C stop
    *buffer='\0';                      // Place null terminator at end of buffer
    return(i+1);
  }
  else
    return(0);
}

// Read data bytes from the EEPROM starting at address until the terminator is read
// or the number bytes read equals count. A null terminator is placed at the end of the buffer.
// Returns the number of bytes read.
uint8_t eeprom_read_buffer_with_terminator(uint8_t i2c_address, char *buffer, uint16_t address, char terminator, uint8_t count)
{
  uint8_t i = 0;
  uint8_t data;
  *buffer='\0';                            // Initialize buffer with null
  if (eeprom_poll(i2c_address) == 0)      // Check if the EEPROM is ready
  {
    if (EEPROM_DATA_SIZE > 0x100)
      i2c_write(address>>8);            // Send upper byte of address if size > 256 bytes
    i2c_write(address);                 // Send lower byte of address
    i2c_repeated_start();               // I2C repeated start
    i2c_write(i2c_address | I2C_READ_BIT);      // I2C address + read
    for (i = 0; i < count-1; i++)           // Read count-1 bytes with ACK
    {
      data = i2c_read(WITH_ACK);                 // Receive 1 EEPROM data byte with ACK
      *buffer++=data;                   // Place data byte in buffer and increment pointer
      if (data == terminator) break;
    }
    data = i2c_read(WITH_NACK);                   // Read one more byte from EEPROM with NACK
    i2c_stop();                         // I2C stop
    *buffer = 0;                          // Place null terminator at end of buffer
    return(i+1);
  }
  else
    return(0);
}

// Write the 2 byte integer data to the EEPROM starting at address. Use the eeprom_write_byte
// routine to avoid keeping track of page boundaries with the eeprom_write_buffer routine.
// Returns the total number of bytes written.
uint8_t eeprom_write_int16(uint8_t i2c_address, int16_t write_data, uint16_t address)
{
  union
  {
    int16_t a;
    int8_t b[2];
  } data;
  uint8_t byte_count;
  data.a = write_data;                                              // get the data
  byte_count = eeprom_write_byte(i2c_address, data.b[0], address++);  // write the LSB to EEPROM
  if (eeprom_write_poll(i2c_address)) return(0);                  // poll EEPROM until complete
  byte_count+=eeprom_write_byte(i2c_address, data.b[1], address);   // write the MSB to EEPROM
  if (eeprom_write_poll(i2c_address)) return(1);                  // poll EEPROM until complete
  return(byte_count);
}

// Read the two byte integer data from the EEPROM starting at address.
// Returns the total number of bytes read.
uint8_t eeprom_read_int16(uint8_t i2c_address, int16_t *read_data, uint16_t address)
{
  union
  {
    int16_t a;
    char b[2];
  } data;
  uint8_t byte_count = 0;
  byte_count = eeprom_read_byte(i2c_address, &data.b[0], address++);  // read the LSB from EEPROM
  byte_count+=eeprom_read_byte(i2c_address, &data.b[1], address++);  // read the MSB from EEPROM
  *read_data = data.a;
  return(byte_count);
}

// Write the 4 byte float data to the EEPROM starting at address. Use the eeprom_write_byte
// routine to avoid keeping track of page boundaries with the eeprom_write_buffer routine.
// Returns the total number of bytes written.
uint8_t eeprom_write_float(uint8_t i2c_address, float write_data, uint16_t address)
{
  union
  {
    float a;
    char b[4];
  } data;
  uint8_t byte_count;

  data.a = write_data;
  byte_count = eeprom_write_byte(i2c_address, data.b[0], address++);
  if (eeprom_write_poll(i2c_address)) return(0);
  byte_count+=eeprom_write_byte(i2c_address, data.b[1], address++);
  if (eeprom_write_poll(i2c_address)) return(1);
  byte_count+=eeprom_write_byte(i2c_address, data.b[2], address++);
  if (eeprom_write_poll(i2c_address)) return(2);
  byte_count+=eeprom_write_byte(i2c_address, data.b[3], address);
  if (eeprom_write_poll(i2c_address)) return(3);
  return(byte_count);
}

// Read the four byte float data from the EEPROM starting at address.
// Returns the total number of bytes written
uint8_t eeprom_read_float(uint8_t i2c_address, float *read_data, uint16_t address)
{
  union
  {
    float a;
    char b[4];
  } data;
  uint8_t byte_count = 0;

  byte_count = eeprom_read_byte(i2c_address, &data.b[0], address++);
  byte_count+=eeprom_read_byte(i2c_address, &data.b[1], address++);
  byte_count+=eeprom_read_byte(i2c_address, &data.b[2], address++);
  byte_count+=eeprom_read_byte(i2c_address, &data.b[3], address++);
  *read_data = data.a;
  return(byte_count);
}

// Write the 4 byte int32 data to the EEPROM starting at address. Use the eeprom_write_byte
// routine to avoid keeping track of page boundaries with the eeprom_write_buffer routine.
// Returns the total number of bytes written.
uint8_t eeprom_write_int32(uint8_t i2c_address, int32_t write_data, uint16_t address)
{
  union
  {
    int32_t a;
    char b[4];
  } data;
  uint8_t byte_count;

  data.a = write_data;
  byte_count = eeprom_write_byte(i2c_address, data.b[0], address++);
  if (eeprom_write_poll(i2c_address)) return(0);
  byte_count+=eeprom_write_byte(i2c_address, data.b[1], address++);
  if (eeprom_write_poll(i2c_address)) return(1);
  byte_count+=eeprom_write_byte(i2c_address, data.b[2], address++);
  if (eeprom_write_poll(i2c_address)) return(2);
  byte_count+=eeprom_write_byte(i2c_address, data.b[3], address);
  if (eeprom_write_poll(i2c_address)) return(3);
  return(byte_count);
}

// Read the four byte int32 data from the EEPROM starting at address.
// Returns the total number of bytes written
uint8_t eeprom_read_int32(uint8_t i2c_address, int32_t *read_data, uint16_t address)
{
  union
  {
    int32_t a;
    char b[4];
  } data;
  uint8_t byte_count = 0;

  byte_count = eeprom_read_byte(i2c_address, &data.b[0], address++);
  byte_count+=eeprom_read_byte(i2c_address, &data.b[1], address++);
  byte_count+=eeprom_read_byte(i2c_address, &data.b[2], address++);
  byte_count+=eeprom_read_byte(i2c_address, &data.b[3], address++);
  *read_data = data.a;
  return(byte_count);
}

// Write Cal Key back
uint8_t enable_calibration()
{
  return(eeprom_write_int16(EEPROM_I2C_ADDRESS, EEPROM_CAL_KEY, EEPROM_CAL_STATUS_ADDRESS));   // Enable Cal key
}

// Reset key to default value
uint8_t disable_calibration()
{
  return(eeprom_write_int16(EEPROM_I2C_ADDRESS, 0xFFFF, EEPROM_CAL_STATUS_ADDRESS));          // Wipe Cal key
}
