/***************************************************************************//**
 *   @file   Communication.h
 *   @brief  Header file of Communication Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
 *   SVN Revision: 775
*******************************************************************************/
#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/

#include <stdint.h>

// Our communication driver needs to be C-compatible to work with the drivers
#ifdef __cplusplus
extern "C" {
#endif

// Arduino COMM values, redefnied here since Arduino.h is a C++ library
#define COMM_INPUT          0
#define COMM_OUTPUT         1
#define COMM_LOW            0
#define COMM_HIGH           1

// Added by Greg, these should probably go upstream at least commented out
// GPIO Pin functions, used by drivers.

// Linduino pin 5
#define GPIO0_PIN_OUT       CommPinMode(5, COMM_OUTPUT)
#define GPIO0_LOW           CommDigitalWrite(5, COMM_LOW)
#define GPIO0_HIGH          CommDigitalWrite(5, COMM_HIGH)

// Linduino pin 4
#define GPIO1_PIN_OUT       CommPinMode(4, COMM_OUTPUT)
#define GPIO1_LOW           CommDigitalWrite(4, COMM_LOW)
#define GPIO1_HIGH          CommDigitalWrite(4, COMM_HIGH)

// Linduino pin 6
#define GPIO2_PIN_OUT       CommPinMode(6, COMM_OUTPUT)
#define GPIO2_LOW           CommDigitalWrite(6, COMM_LOW)
#define GPIO2_HIGH          CommDigitalWrite(6, COMM_HIGH)

// Linduino pin 2
#define GPIO3_PIN_OUT       CommPinMode(2, COMM_OUTPUT)
#define GPIO3_LOW           CommDigitalWrite(2, COMM_LOW)
#define GPIO3_HIGH          CommDigitalWrite(2, COMM_HIGH)
#define GPIO3_STATE         CommDigitalRead(2)

// Need to define each part numbers' CS/address, because we don't want
// them hard-coded in the driver header

// AD5686
#define AD5686_SLAVE_ID     10 // Quikeval CS on Linduino is 10

// ADT7420
#define ADT7420_A0_PIN      0
#define ADT7420_A1_PIN      0

/******************************************************************************/
/* Functions Prototypes                                                       */
/******************************************************************************/

/*! C functions for printing out to the serial from inside .c drivers */
void printc(char* line); // Prints char array
void printi(long num); // Prints integer

/*! C wrappers for the arduino pinmode, digitalread, and digitalwrite functions */
void CommPinMode(uint8_t pin, uint8_t mode);
void CommDigitalWrite(uint8_t pin, uint8_t mode);
int CommDigitalRead(uint8_t pin);

/*! Initializes the I2C communication peripheral. */
unsigned char I2C_Init(unsigned long clockFreq);

/*! Writes data to a slave device. */
unsigned char I2C_Write(unsigned char slaveAddress,
                        unsigned char* dataBuffer,
                        unsigned char bytesNumber,
                        unsigned char stopBit);

/*! Reads data from a slave device. */
unsigned char I2C_Read(unsigned char slaveAddress,
                       unsigned char* dataBuffer,
                       unsigned char bytesNumber,
                       unsigned char stopBit);

/*! Initializes the SPI communication peripheral. */
unsigned char SPI_Init(unsigned char lsbFirst,
                       unsigned long clockFreq,
                       unsigned char clockPol,
                       unsigned char clockEdg);

/*! Reads data from SPI. */
unsigned char SPI_Read(unsigned char slaveDeviceId,
                       unsigned char* data,
                       unsigned char bytesNumber);

/*! Writes data to SPI. */
unsigned char SPI_Write(unsigned char slaveDeviceId,
                        unsigned char* data,
                        unsigned char bytesNumber);

/*************************
 * Linduino I2C functions for ADI use
 * These functions are wrappers around the Arduino Wire library
**/

/*! Connects and initializes I2C */
void Wire_Connect();

/*! Write I2C */
uint8_t Wire_Write(unsigned char address, unsigned char* data, unsigned char length, unsigned char stop);

/*! Read I2C */
uint8_t Wire_Read(unsigned char address, unsigned char* data, unsigned char length, unsigned char stop);

                        
/*************************
 * Linduino SPI functions for ADI use:
 * These functions are derived from functions in LT_SPI and modified
 * to work on ADI parts. They are copied here so we don't have to
 * depend on the LT_SPI library to avoid naming conflicts.
 */
 
/*! Enable SPI, matches spi_enable */
void Lin_SPI_Enable();
 
/*! Initialize SPI, matches spi_init */
void Lin_SPI_Init();
 
/*! Connect to SPI, matches quikeval_spi_connect */
void Lin_SPI_Connect();

/*! Transfer block of SPI data, matches spi_transfer_block */
void Lin_SPI_Transfer_Block(uint8_t cs_pin, uint8_t *tx, uint8_t *rx, uint8_t length);

#ifdef __cplusplus // Closing extern c
}
#endif

#endif // _COMMUNICATION_H
