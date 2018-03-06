//! @todo Review this file?  Document with Doxygen?  Time permitting...
/*
DC590B USB to Serial Controller

This file contains the routines to emulate the DC590B USB to Serial Converter. All commands
are supported except Uxxy the Write Port D bus. Added the 'D' delay ms command.
With this program, the Linduino can be used by the QuikEval program running on a PC
to communicate with QuikEval compatible demo boards.

The Kxy bit bang command uses the following pin mappings :
0-Linduino 2
1-Linduino 3
2-Linduino 4
3-Linduino 5
4-Linduino 6
5-Linduino 7


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
#include "QuikEval_EEPROM.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include <Wire.h>
#include <SPI.h>


// timeouts
#define READ_TIMEOUT  20
#define MISO_TIMEOUT  1000

// recording mode constants
#define RECORDING_SIZE 50
const byte off = 0;
const byte playback = 1;

// serial mode constants
const byte spi_mode = 0;
const byte i2c_mode = 1;
const byte i2c_auxiliary_mode = 2;

// hex conversion constants
char hex_digits[16]=
{
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

// spi clock divider
const char spi_divider = SPI_CLOCK_DIV32;  // configure the spi port for 4MHz SCK (500kHz@div32??)

// global variables
byte serial_mode = spi_mode;  // current serial mode
byte recording_mode = off;        // recording mode off
char id_string[51]="USBSPI,PIC,01,01,DC,DC590,----------------------\n\0"; // id string
char hex_to_byte_buffer[5]=
{
  '0', 'x', '0', '0', '\0'
};               // buffer for ASCII hex to byte conversion
char byte_to_hex_buffer[3]=
{
  '\0','\0','\0'
};                     // buffer for byte to ASCII hex conversion
char recording_buffer[RECORDING_SIZE]=
{
  '\0'
}; // buffer for saving recording loop
byte recording_index = 0;                // index to the recording buffer

char get_char();

void byte_to_hex(byte value)
// convert a byte to two hex characters
{
  byte_to_hex_buffer[0]=hex_digits[value>>4];        // get upper nibble
  byte_to_hex_buffer[1]=hex_digits[(value & 0x0F)];  // get lower nibble
  byte_to_hex_buffer[2]='\0';                        // add NULL at end
}

byte read_hex()
// read 2 hex characters from the serial buffer and convert
// them to a byte
{
  byte data;
  hex_to_byte_buffer[2]=get_char();
  hex_to_byte_buffer[3]=get_char();
  data = strtol(hex_to_byte_buffer, NULL, 0);
  return(data);
}

char get_char()
// get the next character either from the serial port
// or the recording buffer
{
  char command='\0';
  if (recording_mode != playback)
  {
    // read a command from the serial port
    while (Serial.available() <= 0);
    return(Serial.read());
  }
  else
  {
    // read a command from the recording buffer
    if (recording_index < RECORDING_SIZE)
    {
      command = recording_buffer[recording_index++];
      // disregard loop commands during playback
      if (command == 'w') command='\1';
      if (command == 't') command='\1';
      if (command == 'v') command='\1';
      if (command == 'u') command='\1';
    }
    else
      command = '\0';
    if (command == '\0')
    {
      recording_index = 0;
      recording_mode = off;
    }
    return(command);
  }
}
int i = 0;
unsigned char pseudo_reset = 0;

void setup()
// Setup the program
{
  digitalWrite(QUIKEVAL_GPIO, LOW);
  digitalWrite(QUIKEVAL_CS, HIGH);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  pinMode(QUIKEVAL_GPIO, OUTPUT);
  pinMode(QUIKEVAL_CS, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

  Serial.begin(115200);          // enable the serial port for 115200 baud

  quikeval_SPI_init();
  quikeval_SPI_connect();   // Connect SPI to main data port

  quikeval_I2C_init();           // Configure the EEPROM I2C port for 100kHz SCK
  Serial.print("hello\n");
  Serial.flush();
}

void loop()
{
  byte tx_data;
  byte rx_data;
  byte pin_value;
  int delay_value;
  int pin;
  char command;
  int byte_count;
  long delay_count;
  command = get_char();
  switch (command)
  {
    case 'D':
      // delay milliseconds
      delay_value = read_hex();
      delay_value<<=8;
      delay_value|=read_hex();
      delay(delay_value);
      break;
    case 'g':
      // IO pin low
      output_low(QUIKEVAL_GPIO);
      break;
    case 'G':
      // IO pin high
      output_high(QUIKEVAL_GPIO);
      break;
    case 'H':     // wait for MISO to go high with a timeout
      delay_count = 0;
      while (1)
      {
        if (input(MISO)==1) break;   // MISO is high so quit
        if (delay_count++>MISO_TIMEOUT)
        {
          //Serial.print('T');         // timeout occurred. Print 'T'
          break;
        }
        else delay(1);
      }
      break;
    case 'i':
      // send controller id string
      pseudo_reset = 0;
      Serial.print(id_string);
      Serial.print('\0');
      Serial.flush();
      break;
    case 'I':
      // get controller id string
      quikeval_SPI_connect();   // Connect SPI to main data port
      pseudo_reset = 0;
      byte_count = read_quikeval_id_string(&ui_buffer[0]);
      if (byte_count!=0)
      {
        Serial.print(ui_buffer);
        //Serial.print("LTC4261,Cls,D4261,01,01,DC,DC998A,--------------\n\0");
        Serial.print('\0');
        Serial.flush();
      }
      break;
    case 'K':
      // Bang pin. The pin assignments are :
      // 0: PIND2, Arduino 2
      // 1: PIND3, Arduino 3
      // 2: PIND4, Arduino 4
      // 3: PIND5, Arduino 5
      // 4: PIND6, Arduino 6
      // 5: PIND6, Arduino 7
      pin_value = get_char();          // read the value
      pin = get_char()-0x30;         // read the pin
      if (pin_value == '0') digitalWrite(pin+2, LOW);
      else digitalWrite(pin+2, HIGH);
      break;
    case 'L':
      // wait for MISO to go low with a timeout
      delay_count = 0;
      while (1)
      {
        if (input(MISO)==0) break;   // MISO is low so quit
        if (delay_count++>MISO_TIMEOUT)
        {
          //Serial.print('T');         // timeout occurred. Print 'T'
          break;
        }
        else delay(1);
      }
      break;
    case 'M':
      // change the serial mode
      command = get_char();
      switch (command)
      {
        case 'I':
          // I2C mode
          serial_mode = i2c_mode;
          // enable_main_I2C();
          quikeval_I2C_connect();
          break;
        case 'S':
          // spi mode
          serial_mode = spi_mode;
          // Need to send command to disable LTC4302
          // enable_main_SPI();
          quikeval_SPI_connect();
          break;
        case 'X':
          // axillary I2C mode - no hardware action necessary, always available.
          serial_mode = i2c_auxiliary_mode;
          quikeval_SPI_connect();
          break;
      }
      //delay(1);
      break;
    case 'p':
      // I2C stop
      if (serial_mode!=spi_mode)   TWCR=(1<<TWINT) | (1<<TWEN) | (1<<TWSTO);  // I2C stop //i2c_stop();
      // if(serial_mode == i2c_auxiliary_mode) i2c_stop();
      break;
    case 'P':
      // ping
      Serial.print('P');
      delay(5);
      break;
    case 'Q':
      // Read byte in I2C mode only. Add ACK
      switch (serial_mode)
      {
        case i2c_mode:
          rx_data = i2c_read(WITH_ACK);
          byte_to_hex(rx_data);
          Serial.print(byte_to_hex_buffer);
          break;
        case i2c_auxiliary_mode:
          rx_data = i2c_read(WITH_ACK);
          byte_to_hex(rx_data);
          Serial.print(byte_to_hex_buffer);
          break;
      }
      break;

    case 'r':
      rx_data = spi_read(0);
      byte_to_hex(rx_data);
      Serial.print(byte_to_hex_buffer);
      break;

    case 'R':
      // Read byte, add NACK in I2C mode
      switch (serial_mode)
      {
        case spi_mode:
          rx_data = spi_read(0);
          byte_to_hex(rx_data);
          Serial.print(byte_to_hex_buffer);
          break;
        case i2c_mode:
          rx_data = i2c_read(WITH_NACK);
          byte_to_hex(rx_data);
          Serial.print(byte_to_hex_buffer);
          break;
        case i2c_auxiliary_mode:
          rx_data = i2c_read(WITH_NACK);
          byte_to_hex(rx_data);
          Serial.print(byte_to_hex_buffer);
          break;
      }
      break;
    case 's':   // I2C start
      if (serial_mode == i2c_mode) i2c_start();
      if (serial_mode == i2c_auxiliary_mode) i2c_start();
      break;
    case 'S':   // send byte
      tx_data = read_hex();
      switch (serial_mode)
      {
        case spi_mode:
          spi_write(tx_data);
          break;
        case i2c_mode:
          if (i2c_write(tx_data)==1) Serial.print('N');
          break;
        case i2c_auxiliary_mode:
          if (i2c_write(tx_data)==1) Serial.print('N');
          break;
      }
      break;
    case 't':   // recording loop
      recording_index = 0;
      do
      {
        command = get_char();
        if (command == 'u')   // stop recording
        {
          recording_buffer[recording_index]='\0';
          recording_index = 0;
          break;
        }
        else            // add character to recording buffer
        {
          if (recording_index < RECORDING_SIZE) recording_buffer[recording_index++]=command;
        }
      }
      while (1);
      break;
    case 'T':   // transceive byte
      tx_data = read_hex();
      if (serial_mode == spi_mode)
      {
        rx_data = spi_read(tx_data);
        byte_to_hex(rx_data);
        Serial.print(byte_to_hex_buffer);
      }
      break;
    case 'v':    // echo recording loop
      Serial.print(recording_buffer);
      break;
    case 'w':
      recording_mode = playback;
      break;
    case 'x':
      output_low(QUIKEVAL_CS);
      break;
    case 'X':
      output_high(QUIKEVAL_CS);
      break;
    case 'Z':    // line feed
      Serial.print('\n');
      Serial.flush();
      break;
    case 0x80:    // Reset
      if (pseudo_reset == 0)
      {
        delay(500); // The delay is needed for older GUI's
        Serial.print("hello\n");
        pseudo_reset = 1;
      }
      break;
  }
}
