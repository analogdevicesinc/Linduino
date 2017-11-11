/***************************************************************************//**
 *   @file   platform_drivers.c
 *   @brief  Implementation of Generic Platform Drivers.
 *   @author DBogdan (dragos.bogdan@analog.com)
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
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <Linduino.h>
#include <SPI.h>
#include <Wire.h>
#include "platform_drivers.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the I2C communication peripheral.
 * @param dev - The device structure.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_init(i2c_device *dev)
{
	if(dev) {
		// Unused variable - fix compiler warning
		Wire_Connect();
	}

	return SUCCESS;
}

/**
 * @brief Write data to a slave device.
 * @param dev - The device structure.
 * @param data - Buffer that stores the transmission data.
 * @param bytes_number - Number of bytes to write.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_write(i2c_device *dev,
		  uint8_t *data,
		  uint8_t bytes_number,
		  uint8_t stop_bit)
{
	/*if(dev) {
		// Unused variable - fix compiler warning
	}

	if(data) {
		// Unused variable - fix compiler warning
	}

	if(bytes_number) {
		// Unused variable - fix compiler warning
	}

	if(stop_bit) {
		// Unused variable - fix compiler warning
	}*/

	return Wire_Write(dev->slave_address, data, bytes_number, stop_bit);

	//return SUCCESS;
}

/**
 * @brief Read data from a slave device.
 * @param dev - The device structure.
 * @param data - Buffer that will store the received data.
 * @param bytes_number - Number of bytes to read.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_read(i2c_device *dev,
		 uint8_t *data,
		 uint8_t bytes_number,
		 uint8_t stop_bit)
{
	return Wire_Read(dev->slave_address, data, bytes_number, stop_bit);

	/*if(dev) {
		// Unused variable - fix compiler warning
	}

	if(data) {
		// Unused variable - fix compiler warning
	}

	if(bytes_number) {
		// Unused variable - fix compiler warning
	}

	if(stop_bit) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;*/
}

/**
 * @brief Initialize the SPI communication peripheral.
 * @param dev - The device structure.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_init(spi_device *dev)
{
	// @TODO fix this, shouldn't be hardcoded, need to get info from *dev
	SPI.setDataMode(SPI_MODE3);

	Lin_SPI_Init();
	Lin_SPI_Connect();

	return SUCCESS;
}

/**
 * @brief Write and read data to/from SPI.
 * @param dev - The device structure.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_write_and_read(spi_device *dev,
			   uint8_t *data,
			   uint8_t bytes_number)
{
	uint8_t id = dev->chip_select;
	uint8_t tx[bytes_number];
	uint8_t rx[bytes_number];

	/*for(int i = 0; i < bytes_number; i++)
	{
		uint8_t inverse_i = (bytes_number - i - 1);
		tx[i] = data[i];
		data[i] = 0;
	}*/

	// @TODO make this work with any bytesnumber, not just 3
	tx[0] = data[2];
	tx[1] = data[1];
	tx[2] = data[0];

	Lin_SPI_Transfer_Block(id, tx, rx, bytes_number);

	data[0] = rx[2];
    data[1] = rx[1];
    data[2] = rx[0];

	return SUCCESS;
}

/**
 * @brief Initialize the GPIO controller.
 * @param dev - The device structure.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_init(gpio_device *dev)
{
	if(dev) {
		// Unused variable - fix compiler warning
	}

	return 0;
}

/**
 * @brief Set the direction of the specified GPIO.
 * @param dev - The device structure.
 * @param gpio_num - The GPIO number.
 * @param direction - The direction.
 *                    Example: GPIO_OUT
 *                             GPIO_IN
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_set_direction(gpio_device *dev,
			  uint8_t gpio_num,
			  uint8_t direction)
{
	pinMode(gpio_num, direction);

	return 0;
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param dev - The device structure.
 * @param gpio_num - The GPIO number.
 * @param direction - The direction.
 *                    Example: GPIO_OUT
 *                             GPIO_IN
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get_direction(gpio_device *dev,
			  uint8_t gpio_num,
			  uint8_t *direction)
{
	if(dev) {
		// Unused variable - fix compiler warning
	}

	if(gpio_num) {
		// Unused variable - fix compiler warning
	}

	if(direction) {
		// Unused variable - fix compiler warning
	}

	return 0;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param dev - The device structure.
 * @param gpio_num - The GPIO number.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_set_value(gpio_device *dev,
		       uint8_t gpio_num,
		       uint8_t value)
{
	digitalWrite(gpio_num, value);

	return 0;
}

/**
 * @brief Get the value of the specified GPIO.
 * @param dev - The device structure.
 * @param gpio_num - The GPIO number.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get_value(gpio_device *dev,
		       uint8_t gpio_num,
		       uint8_t *value)
{
	uint8_t ret = digitalRead(gpio_num);
	value = &ret;

	return 0;
}

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 * @return None.
 */
void mdelay(uint32_t msecs)
{
	if(msecs) {
		delay(msecs);
	}
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