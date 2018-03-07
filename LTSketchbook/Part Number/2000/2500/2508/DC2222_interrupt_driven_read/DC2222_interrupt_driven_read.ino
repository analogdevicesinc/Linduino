/*!
Linear Technology DC2222AB Demonstration Board.
LTC2508: 32-Bit Over-Sampling ADC with Configurable Digital Filter

@verbatim

NOTES

   This code is used in the case where DRDY triggers a hardware interrupt on Linduino,
   and then the data is read. This makes data acquisition faster.
   Set the terminal baud rate to 115200 and select the newline terminator.
   Provide an external voltage supply of +/-9V or 10 V.
   Drive MCLK with an external CLK source. Connect DRDY to INT1 pin. Connect
   QUIKEVAL_GPIO pin to the SYNC pin.
   16 bits of configuration data: 1 0 0 0   0 0 0 0   0 SEL0 SEL1 0   0 0 0 0

   In the Linduino code, data is continuously captured in the interrupt service
   routine and printed out to the serial console as hex values. Converting the data
   to voltage can be done later. The serially printed out data is logged into a
   file using TeraTerm. The data is read out, converted to voltage and plotted
   using a Python script.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2508

http://www.linear.com/product/LTC2508#demoboards


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
    @ingroup LTC2508
*/


#include <Arduino.h>
#include "LT_I2C.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "Linduino.h"
#include <Wire.h>
#include <stdint.h>
#include <SPI.h>
#include "LTC2508.h"

float VREF = 5.0;
uint16_t global_config_data = CONFIG_DF_256;

// Function Declarations
void sneaker_port_init();
void initialise_i2c_data(uint16_t value, uint8_t i2c_data[48]);

const byte interruptPin = 0;

// Setup function
void setup()
{
  Serial.begin(115200);             // Initialize the serial port to the PC
  TIMSK0 &= ~(1<<TOIE0);        // Timer Interrupt Overflow Enable = 0
  sneaker_port_init();          // Send configuration data through sneaker port and a SYNC pulse
  attachInterrupt(interruptPin, read_data_ISR, FALLING);
}


// Loop function
void loop()
{

}

void read_data_ISR()
{
  uint8_t data;
  output_low(QUIKEVAL_CS);                 //! 1) Pull CS low

  data = SPI.transfer(0);
  if (data < 16)
    Serial.print("0");
  Serial.print(data, HEX);    //! 2) Read and send byte array

  data = SPI.transfer(0);
  if (data < 16)
    Serial.print("0");
  Serial.print(data, HEX);    //! 2) Read and send byte array

  data = SPI.transfer(0);
  if (data < 16)
    Serial.print("0");
  Serial.print(data, HEX);    //! 2) Read and send byte array

  data = SPI.transfer(0);
  if (data < 16)
    Serial.print("0");
  Serial.print(data, HEX);    //! 2) Read and send byte array

  data = SPI.transfer(0);
  if (data < 16)
    Serial.print("0");
  Serial.print(data, HEX);    //! 2) Read and send byte array

  /*Serial.print(SPI.transfer(0), HEX);    //! 2) Read and send byte array
  Serial.print(SPI.transfer(0), HEX);    //! 2) Read and send byte array
  Serial.print(SPI.transfer(0), HEX);    //! 2) Read and send byte array
  Serial.print(SPI.transfer(0), HEX);    //! 2) Read and send byte array
  Serial.print(SPI.transfer(0), HEX);    //! 2) Read and send byte array
  */
  Serial.print("\n");

  output_high(QUIKEVAL_CS);                //! 3) Pull CS high
}


//! Send configuration data through sneaker port
// Send I2C data to sneaker port to bit bang WRIN_I2C(CS) at P2,
// SCK at P5 and SDI at P6.
//! Send a SYNC pulse after configuration.
void sneaker_port_init()
{
  uint8_t i;
  uint16_t config_value = global_config_data;
  uint8_t i2c_data[48];                 // 48 bytes of data: 3 bytes each for 16 bits

  quikeval_I2C_init();                      // I2C Mode at 100kHz
  quikeval_I2C_connect();                   // Connect I2C to main data port
  initialise_i2c_data(config_value, i2c_data);      // Populate i2c_data array with values to bit bang P3, P5, P6

  i2c_write_byte(SNEAKER_PORT_ADDRESS, CS_LOW);     // P3 = 0 pulls WRIN_CS low

  for (i = 0; i < 48; ++i)
  {
    i2c_write_byte(SNEAKER_PORT_ADDRESS, i2c_data[i]);  // Sending configuration data
  }
  i2c_write_byte(SNEAKER_PORT_ADDRESS, CS_HIGH);    // P3 = 1 pulls WRIN_CS high

  // Send a SYNC pulse and leave it low
  pinMode(QUIKEVAL_GPIO, OUTPUT);   // Make SYNC pin on output mode
  digitalWrite(QUIKEVAL_GPIO, LOW);   // SYNC is connected to GPIO pin of the QuikEval connector
  digitalWrite(QUIKEVAL_GPIO, HIGH);
  digitalWrite(QUIKEVAL_GPIO, LOW);

  //quikeval_SPI_init();          // SPI Mode for QuikEval connector - default is 1 MHz
  spi_enable(SPI_CLOCK_DIV2);  //! 1) Configure the spi port for 8MHz SCK
  quikeval_SPI_connect();
}

// Create the array of 3 bytes each for 16 bits sneaker port data
void initialise_i2c_data(uint16_t value, uint8_t i2c_data[48])
{
  uint8_t i;
  uint8_t set_bit;
  uint8_t *p = i2c_data;
  for (i = 0; i < 16; ++i)
  {
    set_bit = (value >> i) & 0x01;
    if (set_bit)
    {
      *p++ = 0x40;
      *p++ = 0x60;
      *p++ = 0x40;
    }
    else
    {
      *p++ = 0x00;
      *p++ = 0x20;
      *p++ = 0x00;
    }
  }

  Serial.print("\n");
}


