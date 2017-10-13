/*!
LT_SPI: SPI Routines to communicate with Arduino based boards.

@verbatim

Defined in SPI.h:

SPI_CLOCK_DIV4    0x00  // 4 Mhz
SPI_CLOCK_DIV16   0x01  // 1 Mhz
SPI_CLOCK_DIV64   0x02  // 250 khz
SPI_CLOCK_DIV128  0x03  // 125 khz
SPI_CLOCK_DIV2    0x04  // 8 Mhz
SPI_CLOCK_DIV8    0x05  // 2 Mhz
SPI_CLOCK_DIV32   0x06  // 500 khz

@endverbatim

Copyright (c) 2013, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/

#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include "Linduino.h"
#include "LT_SPI_Zero.h"

// Reads and sends a byte
// Return 0 if successful, 1 if failed
void spi_transfer_byte(uint8_t cs_pin, uint8_t tx, uint8_t *rx)
{
  output_low(cs_pin);                 //! 1) Pull CS low

  *rx = SPI.transfer(tx);             //! 2) Read byte and send byte

  output_high(cs_pin);                //! 3) Pull CS high
}

// Reads and sends a word
// Return 0 if successful, 1 if failed
void spi_transfer_word(uint8_t cs_pin, uint16_t tx, uint16_t *rx)
{
  union
  {
    uint8_t b[2];
    uint16_t w;
  } data_tx;

  union
  {
    uint8_t b[2];
    uint16_t w;
  } data_rx;

  data_tx.w = tx;

  output_low(cs_pin);                         //! 1) Pull CS low

  data_rx.b[1] = SPI.transfer(data_tx.b[1]);  //! 2) Read MSB and send MSB
  data_rx.b[0] = SPI.transfer(data_tx.b[0]);  //! 3) Read LSB and send LSB

  *rx = data_rx.w;

  output_high(cs_pin);                        //! 4) Pull CS high
}

// Reads and sends a byte array
void spi_transfer_block(uint8_t cs_pin, uint8_t *tx, uint8_t *rx, uint8_t length)
{
  int8_t i;

  output_low(cs_pin);                 //! 1) Pull CS low

  for (i=(length-1);  i >= 0; i--)
    rx[i] = SPI.transfer(tx[i]);    //! 2) Read and send byte array

  output_high(cs_pin);                //! 3) Pull CS high
}

// Connect SPI pins to QuikEval connector through the Linduino MUX. This will disconnect I2C.
void quikeval_SPI_connect()
{
  output_high(QUIKEVAL_CS); //! 1) Pull Chip Select High

  //! 2) Enable Main SPI
  pinMode(QUIKEVAL_MUX_MODE_PIN, OUTPUT);
  digitalWrite(QUIKEVAL_MUX_MODE_PIN, LOW);
}

// Configure the SPI port for 4MHz SCK.
// This function or spi_enable() must be called
// before using the other SPI routines.
void quikeval_SPI_init(void)  // Initializes SPI
{
  spi_enable(SPI_CLOCK_DIV4);  //! 1) Configure the spi port for 4MHz SCK
}

// Setup the processor for hardware SPI communication.
// Must be called before using the other SPI routines.
// Alternatively, call quikeval_SPI_connect(), which automatically
// calls this function.
void spi_enable(uint8_t spi_clock_divider) // Configures SCK frequency. Use constant defined in header file.
{
  //pinMode(SCK, OUTPUT);             //! 1) Setup SCK as output
  //pinMode(MOSI, OUTPUT);            //! 2) Setup MOSI as output
  //pinMode(QUIKEVAL_CS, OUTPUT);     //! 3) Setup CS as output
  SPI.begin();
  SPI.setClockDivider(spi_clock_divider);
}

// Disable the SPI hardware port
void spi_disable()
{
  SPI.end();
}

// Write a data byte using the SPI hardware
void spi_write(int8_t  data)  // Byte to be written to SPI port
{
  SPI.transfer(data);
}

// Read and write a data byte using the SPI hardware
// Returns the data byte read
int8_t spi_read(int8_t  data) //!The data byte to be written
{
  return SPI.transfer(data);;                  //! Return the data read
}