//! @todo Review this document.
/*!
LT_SPI: Routines to communicate with ATmega328P's hardware SPI port.

@verbatim

LT_SPI implements the low level master SPI bus routines using
the hardware SPI port.

SPI Frequency = (CPU Clock frequency)/(16+2(TWBR)*Prescaler)
SPCR = SPI Control Register (SPIE SPE DORD MSTR CPOL CPHA SPR1 SPR0)
SPSR = SPI Status Register (SPIF WCOL - - - - - SPI2X)

Data Modes:
CPOL  CPHA  Leading Edge    Trailing Edge
0      0    sample rising   setup falling
0      1    setup rising    sample falling
1      0    sample falling  setup rising
1      1    sample rising   setup rising

CPU Frequency = 16MHz on Arduino Uno
SCK Frequency
SPI2X  SPR1  SPR0  Frequency  Uno_Frequency
  0      0     0     fosc/4     4 MHz
  0      0     1     fosc/16    1 MHz
  0      1     0     fosc/64    250 kHz
  0      1     1     fosc/128   125 kHz
  0      0     0     fosc/2     8 MHz
  0      0     1     fosc/8     2 MHz
  0      1     0     fosc/32    500 kHz

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
//! @defgroup LT_SPI LT_SPI: Routines to communicate with ATmega328P's hardware SPI port.
//! @}

/*! @file
    @ingroup LT_SPI
    Library for LT_SPI: Routines to communicate with ATmega328P's hardware SPI port.
*/

#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include "Linduino.h"
#include "LT_SPI.h"

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
  pinMode(QUIKEVAL_CS, OUTPUT);
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
  spi_enable(SPI_CLOCK_DIV16);  //! 1) Configure the spi port for 4MHz SCK
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
#if defined(ARDUINO_ARCH_AVR)
  SPDR = data;                  //! 1) Start the SPI transfer
  while (!(SPSR & _BV(SPIF)));  //! 2) Wait until transfer complete
#else
  SPI.transfer(data);
#endif

}

// Read and write a data byte using the SPI hardware
// Returns the data byte read
int8_t spi_read(int8_t  data) //!The data byte to be written
{
#if defined(ARDUINO_ARCH_AVR)
  SPDR = data;                  //! 1) Start the SPI transfer
  while (!(SPSR & _BV(SPIF)));  //! 2) Wait until transfer complete
  return SPDR;                  //! 3) Return the data read
#else
  return SPI.transfer(data);
#endif


}

// Below are implementations of spi_read, etc. that do not use the
// Arduino SPI library.  To use these functions, uncomment them and comment out
// the correcsponding function above.
//
// // Reads and sends a byte
// // Return 0 if successful, 1 if failed
// uint8_t spi_transfer_byte(uint8_t cs_pin, uint8_t tx, uint8_t *rx)
// {
//     output_low(cs_pin);                 //! 1) Pull CS low
//
//     *rx = spi_read(tx);                 //! 2) Read byte and send byte
//
//     output_high(cs_pin);                //! 3) Pull CS high
//
//     return(0);
// }
//
// // Reads and sends a word
// // Return 0 if successful, 1 if failed
// uint8_t spi_transfer_word(uint8_t cs_pin, uint16_t tx, uint16_t *rx)
// {
//     union
//     {
//         uint8_t b[2];
//         uint16_t w;
//     } data_tx;
//
//     union
//     {
//         uint8_t b[2];
//         uint16_t w;
//     } data_rx;
//
//     data_tx.w = tx;
//
//     output_low(cs_pin);                     //! 1) Pull CS low
//
//     data_rx.b[1] = spi_read(data_tx.b[1]);  //! 2) Read MSB and send MSB
//     data_rx.b[0] = spi_read(data_tx.b[0]);  //! 3) Read LSB and send LSB
//     *rx = data_rx.w;
//
//     output_high(cs_pin);                    //! 4) Pull CS high
//
//     return(0);
// }
//
// // Reads and sends a byte array
// // Return 0 if successful, 1 if failed
// uint8_t spi_transfer_block(uint8_t cs_pin, uint8_t *tx, uint8_t *rx, uint8_t length)
// {
//     int8_t i;
//
//     output_low(cs_pin);                 //! 1) Pull CS low
//
//     for(i=0;  i < length; i++)
//         rx[i] = spi_read(tx[i]);        //! 2) Read and send byte array
//
//     output_high(cs_pin);                //! 3) Pull CS high
//
//     return(0);
// }
//
// // Connect SPI pins to QuikEval connector through the Linduino MUX. This will disconnect I2C.
// void quikeval_SPI_connect()
// {
//   output_high(QUIKEVAL_CS); //! 1) Pull Chip Select High
//
//   //! 2) Enable Main SPI
//   pinMode(QUIKEVAL_MUX_MODE_PIN, OUTPUT);
//   digitalWrite(QUIKEVAL_MUX_MODE_PIN, LOW);
// }
//
// // Configure the SPI port for 4MHz SCK.
// // This function or spi_enable() must be called
// // before using the other SPI routines.
// void quikeval_SPI_init(void)  // Initializes SPI
// {
//   spi_enable(SPI_CLOCK_DIV32);  //! 2) Configure the spi port for 4MHz SCK
// }
//
// // Setup the processor for hardware SPI communication.
// // Must be called before using the other SPI routines.
// // Alternatively, call quikeval_SPI_connect(), which automatically
// // calls this function.
// void spi_enable(uint8_t spi_clock_divider) // Configures SCK frequency. Use constant defined in header file.
// {
//   pinMode(SCK, OUTPUT);             //! 1) Setup SCK as output
//   pinMode(MOSI, OUTPUT);            //! 2) Setup MOSI as output
//   pinMode(QUIKEVAL_CS, OUTPUT);     //! 3) Setup CS as output
//   output_low(SCK);
//   output_low(MOSI);
//   output_high(QUIKEVAL_CS);
//   SPCR |= _BV(MSTR);                //! 4) Set the SPI port to master mode
//   //! 5) Set the SPI hardware rate
//   SPCR = (SPCR & ~SPI_CLOCK_MASK) | (spi_clock_divider & SPI_CLOCK_MASK);
//   SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((spi_clock_divider >> 2) & SPI_2XCLOCK_MASK);
//   SPCR |= _BV(SPE);                 //! 5) Enable the SPI port
// }
//
// // Disable the SPI hardware port
// void spi_disable()
// {
//   SPCR &= ~_BV(SPE);
// }