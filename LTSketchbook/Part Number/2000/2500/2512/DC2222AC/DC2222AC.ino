/*!
Linear Technology DC2222AC Demonstration Board.
LTC2512: 24-Bit Over-Sampling ADC with Configurable Digital Filter

@verbatim

NOTES

   Set the terminal baud rate to 115200 and select the newline terminator.
   Provide an external voltage supply of +/-9V or 10 V.
   QUIKEVAL_GPIO is connected to the SYNC pin.
   16 bits of configuration data: 1 0 0 0   0 0 0 0   0 SEL0 SEL1 0   0 0 0 0

   Steps involved:
   1. Set I2C mode on QuikEval connector.
   2. Send configuration data through sneaker port to set DF.
   3. Send a SYNC pulse.
   4. Set SPI mode on QuikEval connector.
   5. Send DF number of MCLK pulses.
   6. Read data out through SPI.

  Explanation of Commands:

   Option 1: Read 32 bits of data and calculate input analog voltage.
   Option 2: Set the DF value for filter.
   Option 3: Set the reference voltage.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2512-24

http://www.linear.com/product/LTC2512-24#demoboards


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
    @ingroup LTC2512
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
#include "LTC2512.h"

float VREF = 5.0;
uint16_t global_config_data = CONFIG_DF_8;
uint16_t num_of_mclk_pulses = 8;

// Function Declarations
void sneaker_port_init();
void initialise_i2c_data(uint16_t value, uint8_t i2c_data[48]);
int8_t i2c_write_block_data_no_command(uint8_t address, uint8_t length, uint8_t *values);
uint16_t decode_DF_value(uint8_t byte0);
float code_to_voltage(uint8_t rx[5]);

// Setup function
void setup()
{
  Serial.begin(115200);         // Initialize the serial port to the PC
  print_title();

  sneaker_port_init();          // Send configuration data through sneaker port and a SYNC pulse

  print_prompt();
}


// Loop function
void loop()
{
  uint8_t user_command;
  if (Serial.available())               // Check for user input
  {
    user_command = read_int();                     //! Reads the user command
    if (user_command != 'm')
      Serial.println(user_command);
    switch (user_command)                               //! Prints the appropriate submenu
    {
      case 1:
        menu_1_read_voltage();          // Print single-ended voltage menu
        break;
      case 2:
        menu_2_change_DF();            // Differential voltage menu
        break;
      case 3:
        menu_3_set_VREF();           // Sequencing menu
        break;
      default:
        Serial.println("Incorrect Option");
        break;
    }
    print_prompt();
  }
}

//! Displays the ADC output and calculated voltage
void menu_1_read_voltage()
{
  uint16_t DF;
  float voltage;
  uint32_t code = 0;

  send_pulses(MCLK_pin, num_of_mclk_pulses);        // Send MCLK pulses
  code = LTC2512_read_data(QUIKEVAL_CS, &DF);
  voltage = LTC2512_code_to_voltage(code, VREF);

  Serial.print(F("\n  Data          : 0x"));
  Serial.println(code, HEX);
  Serial.print(F("  DF            : "));
  Serial.println(DF);
  Serial.print(F("  Input Voltage : "));
  Serial.print(voltage, 9);
  Serial.println(F(" V"));

}

//! Function to change the DF value for filter
void menu_2_change_DF()
{
  uint8_t user_command;
  Serial.print(F("\n  Enter the new DF option (1-4, 2-8, 3-16, 4-32) : "));
  user_command = read_int();                     //! Reads the user command
  Serial.println(user_command);
  switch (user_command)                               //! Prints the appropriate submenu
  {
    case 1:
      global_config_data = CONFIG_DF_4;
      num_of_mclk_pulses = 4;
      break;
    case 2:
      global_config_data = CONFIG_DF_8;
      num_of_mclk_pulses = 8;
      break;
    case 3:
      global_config_data = CONFIG_DF_16;
      num_of_mclk_pulses = 16;
      break;
    case 4:
      global_config_data = CONFIG_DF_32;
      num_of_mclk_pulses = 32;
      break;
    default:
      Serial.println("Incorrect Option");
      break;
  }

  sneaker_port_init();
}

//! Function to change the voltage reference
void menu_3_set_VREF()
{
  Serial.println(F("\n  Remove the R37 resistor and probe in the new reference voltage at VREF"));
  Serial.print(F("\n  Enter the new reference voltage : "));
  VREF = read_float();
  Serial.print(VREF);
  Serial.println(F(" V"));
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

  delay(1);
  quikeval_SPI_init();          // SPI Mode for QuikEval connector
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

  /* Serial.println("\nI2C values to configure DF: ");
  for(i = 0; i<48; ++i)
  {
    if(i%3 == 0)
    Serial.print("\n");
    Serial.print("\t0x");
    Serial.print(i2c_data[i], HEX);
  }  */
  Serial.print("\n");
}


//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n************************************************************************\n"));
  Serial.print(F("* DC2222AB Demonstration Program                                       *\n"));
  Serial.print(F("*                                                                      *\n"));
  Serial.print(F("* This program demonstrates how to configure LTC2512 to set DF value,  *\n"));
  Serial.print(F("* and to read out 24 bits of data and configuration information        *\n"));
  Serial.print(F("*                                                                      *\n"));
  Serial.print(F("* Setup:                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.       *\n"));
  Serial.print(F("* Power the board with +/- 9V or 10 V.                                 *\n"));
  Serial.print(F("*                                                                      *\n"));
  Serial.print(F("* NOTES:                                                               *\n"));
  Serial.print(F("* Please note that LTC2512 requires 10 samples to settle and hence,    *\n"));
  Serial.print(F("* the first few samples do not reflect the exact input voltage.        *\n"));
  Serial.print(F("*                                                                      *\n"));
  Serial.print(F("************************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("\nMain Menu:\n"));
  Serial.print(F("  1-Read Voltage input\n"));
  Serial.print(F("  2-Change DF\n"));
  Serial.print(F("  3-Set VREF\n"));
  Serial.print(F("\nEnter a command:"));
}