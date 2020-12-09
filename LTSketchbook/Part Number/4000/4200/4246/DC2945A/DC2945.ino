#define CODE_REVISION_NUM 1

/*!

-----------------------------------------------------------------------
Set serial port to:  115200,8,N,1, eol=LF
-----------------------------------------------------------------------

Configure all of the parts on the daisy-chained SPI bus with the same 0.5A current limit.
Change the limit with the LTC4246_THRESHOLD variable.
To set AFFINTY modes:
Affinity ALL, set AFFINITY_ALL=0xFF. Affinity NONE define LTC4246_AFFINITY_ALL=0x00.
Enable/disable all channels on all parts in unison.
This code automatically discovers all configured IC on the bus. You must manually set the board jumpers correctly.


@verbatim

The LTC4246 is an 8 channel 12V 1.5A circuit breaker with a SPI 4-wire interface. 
The SPI interface in this case is configured as a daisy-chain of multiple devices.
  IF USING THE DC2945 DEMO BOARD USE JUMPERS JP4 and JP5.

@endverbatim

http://www.linear.com/product/LTC4246

http://www.linear.com/product/LTC4246#demoboards


Copyright 2020(c) Analog Devices, Inc.

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
    @ingroup LTC4246
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"

#include "LTC4246.h"
#ifndef LT_SPI_H
#include <SPI.h>
#include <LT_SPI.h>
#endif

// define some default values
#define LTC4246_AFFINITY_ALL 0x00
#define LTC4246_THRESHOLD 0x0A

// Function Declaration
void print_title(int num_devices);              // Print the title block
void print_prompt();                            // Prompt the user for an input command
// read a byte from each device on the SPI daisy chain
void LTC4246_read_byte(uint8_t reg_addr, uint8_t *byte_buf, int N);
// read a byte from each device on the SPI daisy chain and print the bytes
void LTC4246_read_byte_print(uint8_t reg_addr, uint8_t *byte_buf, int N);
// write the same byte to all devices on the daisy chain
void LTC4246_write_byte(uint8_t reg_addr, uint8_t out_byte, int N);
//int LTC4246_daisy_chain_length()

static uint8_t demo_board_connected;            //!< Set to 1 if the board is connected

static int code_revision_number = CODE_REVISION_NUM;

int num_devices;

//! Initialize Linduino
void setup()
{
  char demo_name[]="DC2945";     // Demo Board Name stored in QuikEval EEPROM

  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100kHz
  quikeval_SPI_init();          // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();       // Connect SPI to main data port

  Serial.begin(115200);         // Initialize the serial port to the PC

  num_devices = LTC4246_daisy_chain_length(); // count the number of devices in the daisy chain
  if (num_devices == 0) {
    Serial.print(F("]n***ERROR DETECTING DEVICES. ASSUMING "));
    num_devices = 1;
  }

  print_title(num_devices);
  print_prompt();
  
}

//! Repeats Linduino loop
void loop()
{
  uint16_t user_command;
  uint16_t out_word;
  uint16_t word_buf;
  uint8_t  byte_buf, byte_buf0, byte_buf1;
  uint8_t  bytes_buf[num_devices]; // a buffer to hold one byte per part in the daisy chain
  
  int i; // loop counter
  int n;
  
  {
    if (Serial.available())
    {
      user_command = read_int();        // Read the user command
      if (user_command != 'm')
        Serial.println(user_command);   // Prints the user command to com port

      switch (user_command) {

      case 0:
	// SCAN THE BUS TO SEE HOW MANY DEVICES ARE DAISY-CHAINED
	Serial.print(F("\nCOUNTING DEVICES ON THE SPI DAISY CHAIN: "));
	num_devices = LTC4246_daisy_chain_length(); // count the number of devices in the daisy chain
	if (num_devices == 0) {
	  Serial.print(F("]n***ERROR DETECTING DEVICES. ASSUMING "));
	  num_devices = 1;
	}
	Serial.println(num_devices);
	break;
	
      case 1:
	// RESET & CONFIGURE
	Serial.println(F("\nRESETTING..."));
	// DISABLE (0x00) ALL 
	LTC4246_write_byte(LTC4246_ENABLE, 0x00, num_devices);
	LTC4246_write_byte(LTC4246_RESET, LTC4246_RESET_DIGITAL, num_devices);
	delay(1);

	Serial.println(F("CONFIGURING..."));
	// FAULT PIN
	LTC4246_write_byte(LTC4246_FAULT_PIN, 0xFF, num_devices);
	// THRESHOLDS
	for (i=0; i < 8; i++) { // 1 for each channel
	  LTC4246_write_byte(LTC4246_CH0_THRESHOLD+i, LTC4246_THRESHOLD, num_devices);
	}
	// AFFINITY
	for (i=0; i < 8; i++) { // 1 for each channel
	  LTC4246_write_byte(LTC4246_CH0_AFFINITY+i, LTC4246_AFFINITY_ALL, num_devices);
	  //	  LTC4246_read_byte_print(LTC4246_CH0_AFFINITY+i, bytes_buf, num_devices);
	  //	  for(n = 0; n < num_devices; n++) {
	  //	    Serial.print(bytes_buf[n], HEX);
	  //	    Serial.print(F(" ; "));
	  //	  }
	  //	  Serial.println(F(""));
	}
	// FAULT_PIN (ALL = 0xFF)
	LTC4246_write_byte(LTC4246_FAULT_PIN, 0xFF, num_devices);
	// CLEAR FAULT_REG
	LTC4246_write_byte(LTC4246_FAULT_REG, 0x00, num_devices);
	// ENABLE (0xFF) ALL IF DESIRED
	// LTC4246_write_byte(LTC4246_ENABLE, 0xFF, num_devices);

	Serial.println(F("\nDONE."));
	break;
      case 2:
	// ENABLE (0xFF) ALL CHANNELS
	LTC4246_write_byte(LTC4246_ENABLE, 0xFF, num_devices);
	break;
      case 3:
	// DISABLE (0x00) ALL CHANNELS
	LTC4246_write_byte(LTC4246_ENABLE, 0x00, num_devices);
	break;
      case 4:
	// ENABLE CHANNELS BY SELECTING BITS
	Serial.println(F(" "));
	Serial.print(F("\n  Enter the DEVICE number (0 - N): "));
	byte_buf0 = read_serial_byte();
	Serial.println(byte_buf0, HEX);
	Serial.print(F("\n  Enter the ENABLE BYTE: "));
	byte_buf = read_serial_byte();
	Serial.println(byte_buf, HEX);

	// construct a NOOP-packed string with just the desired write
	LTC4246_write_single_byte(byte_buf0, LTC4246_ENABLE, byte_buf, num_devices);
	break;
      case 5:
	// PRINT ALL REGISTERS
	LTC4246_print_device_status(num_devices);
	break;
      case 6:
	// WRITE ARBITRARY VALUE TO ARBITRARY REGISTER
	Serial.println(F(" "));
	Serial.print(F("\n  Enter the DEVICE number (0 - N): "));
	byte_buf0 = read_serial_byte();
	Serial.println(byte_buf0, HEX);
	Serial.print(F("\n  Enter the ADDR byte in HEX: "));
	byte_buf = read_serial_byte();
	Serial.println(byte_buf, HEX);
	Serial.print(F("\n  Enter the DATA byte in HEX: "));
	byte_buf1 = read_serial_byte();
	Serial.println(byte_buf1, HEX);

	// construct a NOOP-packed string with just the desired write
	LTC4246_write_single_byte(byte_buf0, byte_buf, byte_buf1, num_devices);
 	break;
      case 7:
	// READ AN ARBITRARY REGISTER (IN EACH DEVICE)
	Serial.print(F("\nEnter the ADDR byte in HEX: "));
	byte_buf = read_serial_byte();
	Serial.print(F("0x"));
	Serial.println(byte_buf, HEX);

	
	Serial.print(F("DEVICE:\t    "));
	for (n=0; n < num_devices; n++) {
	  Serial.print(n);
	  Serial.print(F("  :  "));
	}
	Serial.print(F("\nVALUE:\t"));
        LTC4246_read_byte_print(byte_buf, bytes_buf, num_devices);
	break;
      case 8:
	// WRITE ARBITRARY VALUE TO ARBITRARY REGISTER
	Serial.println(F(" "));
	Serial.println(F("\n  Enter the ADDR byte in HEX: "));
	byte_buf = read_serial_byte();
	Serial.println(byte_buf, HEX);
	Serial.println(F("\n  Enter the DATA byte in HEX: "));
	byte_buf1 = read_serial_byte();
	Serial.println(byte_buf1, HEX);
	LTC4246_write_byte(byte_buf, byte_buf1, num_devices);
 	break;
      case 100:
	// PRINT CODE VERSION
	Serial.print(  F("\n  CODE REVISION: "));
	Serial.println(code_revision_number);
	break;
      default:	
	Serial.println(F("Invalid Option"));
	break;
      }
      Serial.println();
      print_prompt();
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

//! Prints the title block when program first starts.
void print_title( int num_devices)
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC2945A Engineering Eval Program, rev "));
  Serial.print(code_revision_number);
  Serial.print(                                         F("                       *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send data and receive data   *\n"));
  Serial.print(F("* from multiple LTC4246s daisy chained on the SPI bus.          *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Addressing a SPI daisy chain of "));
  Serial.print(num_devices);
  Serial.print(                                   F(" devices                     *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("0- SCAN THE BUS TO ESTABLISH DAISY-CHAIN LENGTH\n"));
  Serial.print(F("1- RESET AND CONFIGURE (ALL CHANNELS OFF)\n"));
  Serial.print(F("2- ENABLE ALL CHANNELS\n"));
  Serial.print(F("3- DISABLE ALL CHANNELS\n"));
  Serial.print(F("4- ENABLE CHANNELS BY A BYTE\n"));
  Serial.print(F("5- READ ALL REGISTERS\n"));
  Serial.print(F("6- WRITE ONE REGISTER IN ONE CHIP\n"));
  Serial.print(F("7- READ REGISTER\n"));
  Serial.print(F("8- WRITE ARBITRARY VALUE TO ONE REGISTER IN ALL CHIPS\n"));
  Serial.print(F("\nEnter a Command: "));
}

int LTC4246_daisy_chain_length()
// scan the bus to establish how long daisy chain length
// Note that the LTC4246 is word-oriented (1 byte ADDR and 1 byte data) so we transmit byte pairs
{
  uint8_t rx = 0x00;
  int ct = 0x00;

  // begin by requesting a read from 255 devices
  // (assuming that there must be fewer, but we don't know how many)
  output_low (QUIKEVAL_CS); // open a SPI transaction by setting the CSb pin low
  for (ct = 255; ct <= 0xFF; ct++) {
    SPI.transfer(0x80); // the NOOP register address with READ bit set
    SPI.transfer(ct); // NOOP contents doesn't matter
  }
  output_high (QUIKEVAL_CS); // end the SPI transaction by setting the CSb pin high

  // now receive the contents
  // the trick is to note that all devices in the chain
  // will return 0x25 for the ADDR, so we keep shifting until we see 0x00 instead of 0x25
  ct = 0x00;
  output_low (QUIKEVAL_CS); // open a SPI transaction by setting the CSb pin low
  do {
    rx = SPI.transfer(0x00); // send in 0x00 and look for 0x25
    SPI.transfer(0x00); // send in 0x00 and don't care about this byte return value
    ct++;
  } while ((ct < 255)&&(rx == 0x25));
  output_high (QUIKEVAL_CS); // end the SPI transaction by setting the CSb pin high

  return --ct;
}

void LTC4246_write_byte(uint8_t reg_addr, uint8_t out_byte, int N)
// write out_byte (the same byte) to reg_addr for each of N devices in the daisy chain
{
  //  uint16_t word_buf = 0x0000; // return value is stored into word_buf, then discarded
  uint8_t in_byte_buf[2*N];  // a buffer big enough to hold the data coming back on MISO
  uint8_t out_byte_buf[2*N]; // a buffer big enough to hold the data going out on MOSI
                             // size=2*N because each byte comes with an extra byte of address
  int i = 0;
  
  spi_enable(SPI_CLOCK_DIV16);

  // construct a block of addresses and data to send to the daisy chain
  for (i = (N-1); i >= 0; i--) {
    out_byte_buf[(2*i)+1] = reg_addr; // register address
    out_byte_buf[(2*i)] = out_byte; // 8'b data
  }
  spi_transfer_block(QUIKEVAL_CS, out_byte_buf, in_byte_buf, (2*N));
  spi_disable();
  
}

void LTC4246_write_single_byte(uint8_t device_num, uint8_t reg_addr, uint8_t out_byte, int N)
// write out_byte only to device_num (0 to num_devices-1) and reg_addr for only one device in the daisy chain
{
  uint8_t in_byte_buf[2*N]; // a buffer big enough to hold the data coming back on MISO
  uint8_t out_byte_buf[2*N]; // a buffer big enough to hold the data going out on MOSI
                            // size=2*N because each byte comes with an extra byte of chaff (address)
  int i = 0;
  
  // SPI_CLK_DIVnn is a required argument, but apparently does nothing
  // The Linduino SPI runs at a constant 1MHz
  spi_enable(SPI_CLOCK_DIV128);
  

  for (i = (N-1); i >= 0; i--) {
    if(i == device_num) {
      // for the chosen device do a real write
      out_byte_buf[(2*i)+1] = reg_addr; // register address
      out_byte_buf[(2*i)] = out_byte; // 8'b data
    }
    else {
      // for other devices do NOOP
      out_byte_buf[(2*i)+1] = LTC4246_NO_OP; // register address
      out_byte_buf[(2*i)] = 0x00; // 8'b data
    }
  }
  spi_transfer_block(QUIKEVAL_CS, out_byte_buf, in_byte_buf, (2*N));
  spi_disable();
  
}

void LTC4246_write_bytes(uint8_t reg_addr, uint8_t *out_byte, int N)
// write a unique out_byte to reg_addr for each of N devices in the daisy chain
// out_byte is an array of bytes, byte 0 goes first
{
  uint8_t in_byte_buf[2*N]; // a buffer big enough to hold the data coming back on MISO
  uint8_t out_byte_buf[2*N]; // a buffer big enough to hold the data going out on MOSI
                            // size=2*N because each byte comes with an extra byte of chaff (address)
  int i = 0;
  
  spi_enable(SPI_CLOCK_DIV16);

  // construct a block of addresses and data to send to the daisy chain
  for (i = (N-1); i >= 0; i--) {
    out_byte_buf[(2*i)+1] = reg_addr; // register address
    out_byte_buf[(2*i)] = out_byte[i]; // 8'b data
  }
  spi_transfer_block(QUIKEVAL_CS, out_byte_buf, in_byte_buf, (2*N));
  spi_disable();
  
}

void LTC4246_read_byte(uint8_t reg_addr, uint8_t *byte_buf, int N)
// read bytes from reg_addr into the byte_buf array
// for daisy-chained devices read the same byte from each device in the chain of N
{
  uint8_t in_byte_buf[2*N]; // a buffer big enough to hold the data coming back on MISO
  uint8_t out_byte_buf[2*N]; // a buffer big enough to hold the data going out on MOSI
                            // size=2*N because each byte comes with an extra byte of chaff (address)
  int i = 0;
  
  spi_enable(SPI_CLOCK_DIV16);

  // construct a block of addresses and data to send to the daisy chain
  // construct and send the command string for N devices
  for (i = (N-1); i >= 0; i--) {
    out_byte_buf[(2*i)+1] = (0x80 | reg_addr); // register address
    out_byte_buf[(2*i)] = 0x00; // data is ignored
  }
  spi_transfer_block(QUIKEVAL_CS, out_byte_buf, in_byte_buf, (2*N));

  // construct and send the read string for N devices (nothing that we send on MOSI matters)
  for (i = (N-1); i >= 0; i--) {
    out_byte_buf[(2*i)+1] = 0x80; // register address
    out_byte_buf[(2*i)] = 0x00; // data is ignored
  }
  spi_transfer_block(QUIKEVAL_CS, out_byte_buf, in_byte_buf, (2*N));
  // in_byte_buf now contains a mix of data and padding
  // remove the padding
  for(i = (N-1); i >= 0; i--) {
    byte_buf[i] = in_byte_buf[2*i];
  }
  spi_disable();
}

uint8_t LTC4246_read_single_byte(int device_num, uint8_t reg_addr, uint8_t *byte_buf, int N)
// read bytes from reg_addr into the byte_buf array
// for daisy-chained devices read the same byte from each device in the chain of N
// return only the requested byte
{
  uint8_t in_byte_buf[2*N]; // a buffer big enough to hold the data coming back on MISO
  uint8_t out_byte_buf[2*N]; // a buffer big enough to hold the data going out on MOSI
                            // size=2*N because each byte comes with an extra byte of chaff (address)
  int i = 0;
  
  spi_enable(SPI_CLOCK_DIV16);

  // construct a block of addresses and data to send to the daisy chain
  // construct and send the command string for N devices
  for (i = (N-1); i >= 0; i--) {
    out_byte_buf[(2*i)+1] = (0x80 | reg_addr); // register address
    out_byte_buf[(2*i)] = 0x00; // data is ignored
  }
  spi_transfer_block(QUIKEVAL_CS, out_byte_buf, in_byte_buf, (2*N));

  // construct and send the read string for N devices (nothing that we send on MOSI matters)
  for (i = (N-1); i >= 0; i--) {
    out_byte_buf[(2*i)+1] = 0x80; // register address
    out_byte_buf[(2*i)] = 0x00; // data is ignored
  }
  spi_transfer_block(QUIKEVAL_CS, out_byte_buf, in_byte_buf, (2*N));
  // in_byte_buf now contains a mix of data and padding
  // remove the padding
  for(i = (N-1); i >= 0; i--) {
    byte_buf[i] = in_byte_buf[2*i];
  }
  spi_disable();
  return byte_buf[device_num];
}

void LTC4246_read_byte_print(uint8_t reg_addr, uint8_t *byte_buf, int N)
// read bytes from reg_addr into the byte_buf array and prints the results to the serial stream
// for daisy-chained devices read the same byte from each device in the chain of N
{
  uint8_t in_byte_buf[2*N]; // a buffer big enough to hold the data coming back on MISO
  uint8_t out_byte_buf[2*N]; // a buffer big enough to hold the data going out on MOSI
                            // size=2*N because each byte comes with an extra byte of chaff (address)
  int i = 0;
  
  spi_enable(SPI_CLOCK_DIV16);

  // construct a block of addresses and data to send to the daisy chain
  // construct and send the command string for N devices
  for (i = (N-1); i >= 0; i--) {
    out_byte_buf[(2*i)+1] = (0x80 | reg_addr); // register address
    out_byte_buf[(2*i)] = 0x00; // data is ignored
  }
  spi_transfer_block(QUIKEVAL_CS, out_byte_buf, in_byte_buf, (2*N));

  // construct and send the read string for N devices (nothing that we send on MOSI matters)
  for (i = (N-1); i >= 0; i--) {
    out_byte_buf[(2*i)+1] = 0x80; // register address
    out_byte_buf[(2*i)] = 0x00; // data is ignored
  }
  spi_transfer_block(QUIKEVAL_CS, out_byte_buf, in_byte_buf, (2*N));
  // in_byte_buf now contains a mix of data and padding
  // remove the padding and print the results
  Serial.print(F("  "));
  for(i = 0; i < N; i++) {
    byte_buf[i] = in_byte_buf[2*i];
    // print the results to the serial stream
    Serial.print(F("0x"));
    Serial.print(byte_buf[i], HEX);
    if(i < (N-1)) // print formatting
      Serial.print(F(" ; "));
    else
      Serial.print(F("\n"));
  }
  spi_disable();
}

void LTC4246_print_device_status(int num_devices)
{
  int n = 0;
  uint8_t  bytes_buf[num_devices];
  Serial.println(F("\nLTC4246 REGISTERS:"));
  Serial.println(F(" daisy\t|  en\t| fault\t| reset\t| fault\t| thr\t| thr\t| thr\t| thr\t| thr\t| thr\t| thr\t| thr\t| aff\t| aff\t| aff\t| aff\t| aff\t| aff\t| aff\t| aff\t| rev\t|"));
  Serial.println(F(" chain \t|   \t| reg  \t|      \t| pin  \t| 0  \t| 1  \t| 2  \t| 3  \t| 4  \t| 5  \t| 6  \t| 7  \t| 0  \t| 1  \t| 2  \t| 3  \t| 4  \t| 5  \t| 6  \t| 7  \t| id \t|"));
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"));
  for(n = 0; n < num_devices; n++) {
    Serial.print(F("  "));
    Serial.print(n);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_ENABLE, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_FAULT_REG, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_RESET, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_FAULT_PIN, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_CH0_THRESHOLD, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_CH1_THRESHOLD, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_CH2_THRESHOLD, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_CH3_THRESHOLD, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_CH4_THRESHOLD, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_CH5_THRESHOLD, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_CH6_THRESHOLD, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_CH7_THRESHOLD, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_CH0_AFFINITY, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_CH1_AFFINITY, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_CH2_AFFINITY, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_CH3_AFFINITY, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_CH4_AFFINITY, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_CH5_AFFINITY, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_CH6_AFFINITY, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| 0x"));
    LTC4246_read_byte(LTC4246_CH7_AFFINITY, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.print(F("\t| "));
    LTC4246_read_byte(LTC4246_ID_VER, bytes_buf, num_devices);
    Serial.print(bytes_buf[n], HEX);
    Serial.println(F("\t|"));
  }
  Serial.println(F("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------"));
}


uint8_t read_serial_byte()
// read a HEX value 0x(0-F)(0-F) from the serial stream, convert it to a byte, return the byte
{
  uint8_t retval = 0x00;
  uint8_t byte_buf[8];
  int howmany = 0;
  
  byte_buf[0] = 0;
  byte_buf[1] = 0;
  byte_buf[2] = 0;
  do {
    // wait patiently until something happens
  } while((howmany = Serial.available()) <= 0 );
    
  howmany = Serial.readBytesUntil('\n', byte_buf, 8);
  switch (howmany) {
  case 0:
    // no valid data
    retval = 0x00;
    Serial.println(F("\nNO DATA! PLEASE ENTER EXACTLY 2 HEX CHARACTERS. "));
    break;
  case 1:
    // 1 valid data byte
    retval = (0x00 | ishex(byte_buf[0]));
    break;
  case 2:
    // 2 valid data bytes
    retval = (ishex(byte_buf[0]) << 4) | ishex(byte_buf[1]);
    break;
  default:
    retval = 0x00;
    Serial.println(F("\nPLEASE ENTER EXACTLY 2 HEX CHARACTERS. "));
    break;
  }
  return retval;
}


uint8_t ishex(char inchar)
{
  switch (inchar)
    {
    case '0':
      return 0x0;
      break;
    case '1':
      return 0x1;
      break;
    case '2':
      return 0x2;
      break;
    case '3':
      return 0x3;
      break;
    case '4':
      return 0x4;
      break;
    case '5':
      return 0x5;
      break;
    case '6':
      return 0x6;
      break;
    case '7':
      return 0x7;
      break;
    case '8':
      return 0x8;
      break;
    case '9':
      return 0x9;
      break;
    case 'A':
    case 'a':
      return 0xA;
      break;
    case 'B':
    case 'b':
      return 0xB;
      break;
    case 'C':
    case 'c':
      return 0xC;
      break;
    case 'D':
    case 'd':
      return 0xD;
      break;
    case 'E':
    case 'e':
      return 0xE;
      break;
    case 'F':
    case 'f':
      return 0xF;
      break;
    default:
      return 0x0;
      break;
    }
}

