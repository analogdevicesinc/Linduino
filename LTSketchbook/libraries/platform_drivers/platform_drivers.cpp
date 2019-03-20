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
#include <Arduino.h>
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
 * @param desc - The I2C descriptor.
 * @param init_param - The structure that contains the I2C parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_init(i2c_desc **desc,
		 const struct i2c_init_param *param)
{
	// Create the i2c description object for the device
	i2c_desc * new_desc = (i2c_desc*) malloc(sizeof(*new_desc));
	new_desc->type = param->type;
	new_desc->id = param->id;
	new_desc->max_speed_hz = param->max_speed_hz;
	new_desc->slave_address = param->slave_address;
	
	*desc = new_desc;
	
	// Set the mux to use I2C
	quikeval_set_mux(MUX_I2C);
	
	// Enable the I2C Wire functionality
	Wire_Connect();
	
	// Set clock speed
	Wire.setClock(new_desc->max_speed_hz);
	
	return SUCCESS;
}

/**
 * @brief Free the resources allocated by i2c_init().
 * @param desc - The I2C descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_remove(i2c_desc *desc)
{
	return SUCCESS;
}

/**
 * @brief Write data to a slave device.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that stores the transmission data.
 * @param bytes_number - Number of bytes to write.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_write(i2c_desc *desc,
		  uint8_t *data,
		  uint8_t bytes_number,
		  uint8_t stop_bit)
{
	return Wire_Write(desc->slave_address, data, bytes_number, stop_bit);
}

/**
 * @brief Read data from a slave device.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that will store the received data.
 * @param bytes_number - Number of bytes to read.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_read(i2c_desc *desc,
		 uint8_t *data,
		 uint8_t bytes_number,
		 uint8_t stop_bit)
{
	return Wire_Read(desc->slave_address, data, bytes_number, stop_bit);
}


// Converts 1-3 into arduino SPI codes
const uint8_t arduino_spi_modes[4] = {
	SPI_MODE0, //0x00, //SPI_MODE0
	SPI_MODE1, //0x04, //SPI_MODE1
	SPI_MODE2, //0x08, //SPI_MODE2
	SPI_MODE3 //0x0C  //SPI_MODE3
};



/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param init_param - The structure that contains the SPI parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_init(spi_desc **desc,
		 const struct spi_init_param *param)
{
	// Create the spi description object for the device
	spi_desc * new_desc = (spi_desc*) malloc(sizeof(*new_desc));
	new_desc->chip_select = param->chip_select;
	new_desc->mode = param->mode;
	new_desc->max_speed_hz = param->max_speed_hz;
	
	*desc = new_desc;
	
	// Set the Quikeval mux pin to use SPI
    quikeval_set_mux(MUX_SPI);
	
	// Get SPI initialization settings from the description
	uint8_t spi_mode = arduino_spi_modes[new_desc->mode];
	uint32_t spi_speed = new_desc->max_speed_hz;
	
	// Create initialization object
	SPISettings * spi_settings = new SPISettings(spi_speed, MSBFIRST, spi_mode);
	
	// Set the SPI bus to use our settings
	SPI.beginTransaction(*spi_settings);
	SPI.endTransaction();
	
	// Connect to the bus
	SPI.begin();
	
	// Pull Chip Select High
	output_high(new_desc->chip_select); 
	
	return SUCCESS;
}

/**
 * @brief Free the resources allocated by spi_init().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_remove(spi_desc *desc)
{
	SPI.end();

	return SUCCESS;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t spi_write_and_read(spi_desc *desc,
			   uint8_t *data,
			   uint8_t bytes_number)
{
	// Set chip select
	uint8_t id = desc->chip_select;
	
	// transfer & read arrays
	uint8_t tx[bytes_number];
	uint8_t rx[bytes_number];
	
	// Copy data to tx
	for(int i = 0; i < bytes_number; i++)
	{
		tx[i] = data[i];
	}

	// if(false)
	// {
		// Serial.print(F("SPI writing: "));
		// Serial.print(tx[0], HEX);
		// Serial.print(tx[1], HEX);
		// Serial.println(tx[2], HEX);
	// }

//	Lin_SPI_Transfer_Block(id, tx, rx, bytes_number);

	// Copy rx back into data
//	Serial.println(F("read from SPI"));
//	for(int i = 0; i < bytes_number; i++)
//	{
//		data[i] = rx[i];
//		Serial.print(data[i], HEX);
//	}

	
	
    output_low(id); //! 1) Pull CS low

	//for (i = length; i > 0; i--)
	for (int i = 0; i < bytes_number; i++)
    {
        data[i] = SPI.transfer(tx[i]); //! 2) Read and send byte array
    }

    output_high(id); //! 3) Pull CS high
	
	
	
	return SUCCESS;
}

/**
 * @brief Obtain the GPIO decriptor.
 * @param desc - The GPIO descriptor.
 * @param gpio_number - The number of the GPIO.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get(gpio_desc **desc,
		 uint8_t gpio_number)
{
	gpio_desc * new_gpio = new gpio_desc;
	new_gpio->type = GENERIC_GPIO;
	new_gpio->id = 0;
	new_gpio->number = gpio_number;

	*desc = new_gpio;

	return 0;
}

/**
 * @brief Free the resources allocated by gpio_get().
 * @param desc - The SPI descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_remove(gpio_desc *desc)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	return SUCCESS;
}

/**
 * @brief Enable the input direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_direction_input(gpio_desc *desc)
{
	pinMode(desc->number, GPIO_IN);

	return SUCCESS;
}

/**
 * @brief Enable the output direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_direction_output(gpio_desc *desc,
			      uint8_t value)
{
	pinMode(desc->number, GPIO_OUT);
	digitalWrite(desc->number, value);

	return SUCCESS;
}

/**
 * @brief Get the direction of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param direction - The direction.
 *                    Example: GPIO_OUT
 *                             GPIO_IN
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get_direction(gpio_desc *desc,
			   uint8_t *direction)
{
	if (desc) {
		// Unused variable - fix compiler warning
	}

	if (direction) {
		// Unused variable - fix compiler warning
	}

	return 0;
}

/**
 * @brief Set the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_set_value(gpio_desc *desc,
		       uint8_t value)
{
	digitalWrite(desc->number, value);

	return 0;
}

/**
 * @brief Get the value of the specified GPIO.
 * @param desc - The GPIO descriptor.
 * @param value - The value.
 *                Example: GPIO_HIGH
 *                         GPIO_LOW
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t gpio_get_value(gpio_desc *desc,
		       uint8_t *value)
{
	uint8_t ret = digitalRead(desc->number);
	*value = ret;

	return SUCCESS;
}

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 * @return None.
 */
void mdelay(uint32_t msecs)
{
	if (msecs) {
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
    
    while(Wire.available())
    {
        data[i] = Wire.read();
        i++;
    }
    
    return i;
}

/***************************************************************************************************
 * *************************************************************************************************
 * LINDUINO SPI FUNCTIONS
 */

/***************************************************************************/ /**
 * @brief Sets the Linduino MUX that selects SPI or I2C for the 
 *        quikeval port
 * 
 * @param mux - New mux selection
 *              Example: MUX_I2C - Enable I2C
 *                       MUX_SPI - Enable SPI
 * 
*******************************************************************************/
void quikeval_set_mux(uint8_t mux)
{
	
	pinMode(QUIKEVAL_MUX_MODE_PIN, OUTPUT);
	
	if(mux == MUX_I2C)
	{
		digitalWrite(QUIKEVAL_MUX_MODE_PIN, HIGH);
		delay(50);
	}
	else
	{
		digitalWrite(QUIKEVAL_MUX_MODE_PIN, LOW);
		delay(1);
	}
}

// Simple wrapper around Arduino Serial.print method.
void uartTX(char *buf)
{
  Serial.print(buf);
}