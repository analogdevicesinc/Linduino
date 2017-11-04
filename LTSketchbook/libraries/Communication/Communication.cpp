/***************************************************************************/ /**
 *   @file   Communication.c
 *   @brief  Implementation of Communication Driver for the Linduino. Includes
 *           added functions that do the same things as the functions in LT_SPI
 *           and LT_I2C
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Greg Brisebois (gregbrisebois@linear.com)
********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
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

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
#include <Arduino.h>
#include <Linduino.h>
//#include "LT_SPI.h"
#include <SPI.h>
#include <Wire.h>
#include "Communication.h"


/*******************************************
 * Arduino pin wrappers
 *******************************************/

 /*! pinMode */
void CommPinMode(uint8_t pin, uint8_t mode)
{
    pinMode(pin, mode);
}

/*! digitalWrite */
void CommDigitalWrite(uint8_t pin, uint8_t mode)
{
    digitalWrite(pin, mode);
}

/*! digitalRead */
int CommDigitalRead(uint8_t pin)
{
    return digitalRead(pin);
}

/**
 * Serial print function compatible with C
 * Prints a char array
 */
void printc(char* line)
{
    String sline = line;
    Serial.println(line);
}

/**
 * Serial print function compatible with c
 * Prints a long (int)
 */
void printi(long num)
{
    Serial.println(num);
}

/***************************************************************************/ /**
 * @brief Initializes the I2C communication peripheral.
 *
 * @param clockFreq - I2C clock frequency (Hz).
 *                    Example: 100000 - I2C clock frequency is 100 kHz.
 * @return status - Result of the initialization procedure.
 *                  Example: 1 - if initialization was successful;
 *                           0 - if initialization was unsuccessful.
*******************************************************************************/
unsigned char I2C_Init(unsigned long clockFreq)
{
    // Add your code here.
    Wire_Connect();
    return 0;
}

/***************************************************************************/ /**
 * @brief Writes data to a slave device.
 *
 * @param slaveAddress - Adress of the slave device.
 * @param dataBuffer - Pointer to a buffer storing the transmission data.
 * @param bytesNumber - Number of bytes to write.
 * @param stopBit - Stop condition control.
 *                  Example: 0 - A stop condition will not be sent;
 *                           1 - A stop condition will be sent.
 *
 * @return status - Number of written bytes.
*******************************************************************************/
unsigned char I2C_Write(unsigned char slaveAddress,
                        unsigned char *dataBuffer,
                        unsigned char bytesNumber,
                        unsigned char stopBit)
{
    return Wire_Write(slaveAddress, dataBuffer, bytesNumber, stopBit);
    // Add your code here.
    //i2c_write_block_data(slaveAddress, dataBuffer[0], bytesNumber, dataBuffer);
}

/***************************************************************************/ /**
 * @brief Reads data from a slave device.
 *
 * @param slaveAddress - Adress of the slave device.
 * @param dataBuffer - Pointer to a buffer that will store the received data.
 * @param bytesNumber - Number of bytes to read.
 * @param stopBit - Stop condition control.
 *                  Example: 0 - A stop condition will not be sent;
 *                           1 - A stop condition will be sent.
 *
 * @return status - Number of read bytes.
*******************************************************************************/
unsigned char I2C_Read(unsigned char slaveAddress,
                       unsigned char *dataBuffer,
                       unsigned char bytesNumber,
                       unsigned char stopBit)
{
    // Add your code here.
    return Wire_Read(slaveAddress, dataBuffer, bytesNumber, stopBit);
}

/***************************************************************************/ /**
 * @brief Initializes the SPI communication peripheral.
 *
 * @param lsbFirst - Transfer format (0 or 1).
 *                   Example: 0x0 - MSB first.
 *                            0x1 - LSB first.
 * @param clockFreq - SPI clock frequency (Hz).
 *                    Example: 1000 - SPI clock frequency is 1 kHz.
 * @param clockPol - SPI clock polarity (0 or 1).
 *                   Example: 0x0 - Idle state for clock is a low level; active
 *                                  state is a high level;
 *	                      0x1 - Idle state for clock is a high level; active
 *                                  state is a low level.
 * @param clockEdg - SPI clock edge (0 or 1).
 *                   Example: 0x0 - Serial output data changes on transition
 *                                  from idle clock state to active clock state;
 *                            0x1 - Serial output data changes on transition
 *                                  from active clock state to idle clock state.
 *
 * @return status - Result of the initialization procedure.
 *                  Example: 1 - if initialization was successful;
 *                           0 - if initialization was unsuccessful.
*******************************************************************************/
unsigned char SPI_Init(unsigned char lsbFirst,
                       unsigned long clockFreq,
                       unsigned char clockPol,
                       unsigned char clockEdg)
{
    // @TODO pick the right spi
    SPI.setDataMode(SPI_MODE3);
    Lin_SPI_Init();
    Lin_SPI_Connect();
    //quikeval_SPI_init();
    //quikeval_SPI_connect();
}

/***************************************************************************/ /**
 * @brief Reads data from SPI.
 *
 * @param slaveDeviceId - The ID of the selected slave device.
 * @param data - Data represents the write buffer as an input parameter and the
 *               read buffer as an output parameter.
 * @param bytesNumber - Number of bytes to read.
 *
 * @return Number of read bytes.
*******************************************************************************/
unsigned char SPI_Read(unsigned char slaveDeviceId,
                       unsigned char *data,
                       unsigned char bytesNumber)
{
    unsigned char tx[bytesNumber];

    for (int i = 0; i < bytesNumber; i++)
    {
        tx[i] = data[i];
        data[i] = 0;
    }

    //spi_transfer_block(slaveDeviceId, tx, data, bytesNumber);
    Lin_SPI_Transfer_Block(slaveDeviceId, tx, data, bytesNumber);
}

/***************************************************************************/ /**
 * @brief Writes data to SPI.
 *
 * @param slaveDeviceId - The ID of the selected slave device.
 * @param data - Data represents the write buffer.
 * @param bytesNumber - Number of bytes to write.
 *
 * @return Number of written bytes.
*******************************************************************************/
unsigned char SPI_Write(unsigned char slaveDeviceId,
                        unsigned char *data,
                        unsigned char bytesNumber)
{
    unsigned char tx[bytesNumber];

    // ADI driver gives us the data backwards
    tx[0] = data[2];
    tx[1] = data[1];
    tx[2] = data[0];

    uint8_t rx[bytesNumber];
    Lin_SPI_Transfer_Block(slaveDeviceId, tx, rx, bytesNumber);
    //spi_transfer_block(slaveDeviceId, tx, data, bytesNumber);

    // Some ADI functions rely on data being the read value as well (they
    // should probably have used SPI_Read instead but oh well)
    data[0] = rx[2];
    data[1] = rx[1];
    data[2] = rx[0];
}

/***************************************************************************************************
 * *************************************************************************************************
 * LINDUINO WIRE I2C FUNCTIONS
 */

 /***************************************************************************/ /**
 * @brief Connects to I2C device
 * 
*******************************************************************************/
void Wire_Connect()
{
    Wire.begin();
}

/***************************************************************************/ /**
 * @brief Write to I2C device.
 *
 * @param address - Address of device
 * @param data - Data to write, includes command
 * @param length - Length of data byte array
 * @param stop - Stop bit
 * 
 * @return Transmission acknowledged by device
 * 
*******************************************************************************/
uint8_t Wire_Write(unsigned char address, unsigned char* data, unsigned char length, unsigned char stop)
{
    Wire.beginTransmission(address);
    
    //Serial.print(F("Wire Writing Data:"));
    for(int i = 0; i < length; i++)
    {
        //Serial.print(data[i]);
        Wire.write(data[i]);
    }
    //Serial.println("");
    
    bool wireStop = (stop > 0);
    
    uint8_t ack = Wire.endTransmission(wireStop);
    
    return ack;
}

/***************************************************************************/ /**
 * @brief Read from I2C device.
 *
 * @param address - Address of device
 * @param data - Data read back
 * @param length - Length of data byte array to read
 * @param stop - Stop bit
 * 
 * @return Number of read bytes
 * 
*******************************************************************************/
uint8_t Wire_Read(unsigned char address, unsigned char* data, unsigned char length, unsigned char stop)
{
    Wire.requestFrom(address, length, stop);
    
    uint8_t i = 0;
    
    //Serial.print(F("Wire Reading Data: "));
    while(Wire.available())
    {
        data[i] = Wire.read();
        //Serial.print(data[i]);
        i++;
    }
    //Serial.println("");
    
    return i;
}

/***************************************************************************************************
 * *************************************************************************************************
 * LINDUINO SPI FUNCTIONS
 */

/***************************************************************************/ /**
 * @brief Enables SPI.
 *
 * @param spi_clock_divider - The ID of the selected slave device.
 * 
*******************************************************************************/
void Lin_SPI_Enable(uint8_t spi_clock_divider)
{
    SPI.begin();
    SPI.setClockDivider(spi_clock_divider);
}

/***************************************************************************/ /**
 * @brief Initializes SPI on the Linduino.
 * 
*******************************************************************************/
void Lin_SPI_Init(void)
{
    Lin_SPI_Enable(SPI_CLOCK_DIV16); //! 1) Configure the spi port for 4MHz SCK
}

/***************************************************************************/ /**
 * @brief Connect SPI pins to QuikEval connector through the Linduino MUX.
 *        This will disconnect I2C.
 * 
*******************************************************************************/
void Lin_SPI_Connect()
{
    output_high(QUIKEVAL_CS); //! 1) Pull Chip Select High

    //! 2) Enable Main SPI
    pinMode(QUIKEVAL_MUX_MODE_PIN, OUTPUT);
    digitalWrite(QUIKEVAL_MUX_MODE_PIN, LOW);
}

/***************************************************************************/ /**
 * @brief Reads and sends a byte array.
 *
 * @param cs_pin - Chipselect pin
 * @param tx - Byte array to transfer/send
 * @param rx - Byte array returned by device
 * @param length - length of tx array
 *
*******************************************************************************/
void Lin_SPI_Transfer_Block(uint8_t cs_pin, uint8_t *tx, uint8_t *rx, uint8_t length)
{
    int8_t i;

    output_low(cs_pin); //! 1) Pull CS low

    for (i = length; i > 0; i--)
    {
        rx[i - 1] = SPI.transfer(tx[i - 1]); //! 2) Read and send byte array
    }

    output_high(cs_pin); //! 3) Pull CS high
}