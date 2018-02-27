/*!
Linear Technology DC2094AA Demonstration Board.
LTC2348-18: Octal, 18-Bit, 200ksps Differential ±10.24V Input SoftSpan ADC with Wide Input Common Mode Range.

Linear Technology DC2094AD Demonstration Board.
LTC2348-16: Octal, 16-Bit, 200ksps Differential ±10.24V Input SoftSpan ADC with Wide Input Common Mode Range.

Linear Technology DC2520AA Demonstration Board.
LTC2344-18: Quad, 18-Bit, 400ksps/ch Differential SoftSpan ADC with Wide Input Common Mode Range.

Linear Technology DC2520AB Demonstration Board.
LTC2344-16: Quad, 16-Bit, 400ksps/ch Differential SoftSpan ADC with Wide Input Common Mode Range.

Linear Technology DC2365AD Demonstration Board.
LTC2333-18: Buffered 8-Channel, 18-Bit, 800ksps Differential ±10.24V ADC with 30VP-P Common Mode Range.

Linear Technology DC2365AH Demonstration Board.
LTC2333-16: Buffered 8-Channel, 16-Bit, 800ksps Differential ±10.24V ADC with 30VP-P Common Mode Range.

Linear Technology DC2365AB Demonstration Board.
LTC2357-18: Buffered Quad, 18-Bit, 350ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.

Linear Technology DC2365AF Demonstration Board.
LTC2357-16: Buffered Quad, 16-Bit, 350ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.

Linear Technology DC2365AA Demonstration Board.
LTC2358-18: Buffered Octal, 18-Bit, 200ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.

Linear Technology DC2365AE Demonstration Board.
LTC2358-16: Buffered Octal, 16-Bit, 200ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.

Linear Technology DC2365AC Demonstration Board.
LTC2353-18: Buffered Dual, 18-Bit, 550ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.

Linear Technology DC2365AG Demonstration Board.
LTC2353-16: Buffered Dual, 16-Bit, 550ksps/Ch Differential ±10.24V ADC with 30VP-P Common Mode Range.


@verbatim

NOTES
 Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.
   Ensure all jumpers on the demo board are installed in their default positions
   as described in Demo Manual DC2094A. Apply +/- 16V to the indicated terminals.
   Make sure the input range of channels are configured to measure according to
   the input range required.

   This code can be used for all the Family supported parts by changing num_of_channels
   and num_of_bits corresponding to each of those parts.

 Menu Entry 1: Display ADC Output Data and Calculated ADC input voltage

 Menu Entry 2: Change Configuration Settings

@endverbatim

http://www.linear.com/product/LTC2348-18
http://www.linear.com/product/LTC2348-16
http://www.linear.com/product/LTC2344-18
http://www.linear.com/product/LTC2344-16
http://www.linear.com/product/LTC2333-18
http://www.linear.com/product/LTC2333-16
http://www.linear.com/product/LTC2357-18
http://www.linear.com/product/LTC2357-16
http://www.linear.com/product/LTC2358-18
http://www.linear.com/product/LTC2358-16
http://www.linear.com/product/LTC2353-18
http://www.linear.com/product/LTC2353-16

http://www.linear.com/product/LTC2348-18#demoboards
http://www.linear.com/product/LTC2348-16#demoboards
http://www.linear.com/product/LTC2344-18#demoboards
http://www.linear.com/product/LTC2344-16#demoboards
http://www.linear.com/product/LTC2333-18#demoboards
http://www.linear.com/product/LTC2333-16#demoboards
http://www.linear.com/product/LTC2357-18#demoboards
http://www.linear.com/product/LTC2357-16#demoboards
http://www.linear.com/product/LTC2358-18#demoboards
http://www.linear.com/product/LTC2358-16#demoboards
http://www.linear.com/product/LTC2353-18#demoboards
http://www.linear.com/product/LTC2353-16#demoboards


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
    @ingroup LTC23XX
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
#include "LTC2348.h"

// Global variable
static uint8_t demo_board_connected;   //!< Set to 1 if the board is connected
union LT_union_int32_4bytes configuration_bits;
union LT_union_int32_4bytes data;

// LTC2348 specific values
uint8_t num_of_channels = 8;  // Number of channels
uint8_t num_of_bits = 18;

// Function declarations
void sneaker_port_init();
uint8_t discover_DC2094(char *demo_name);
void print_title();
void print_prompt();
void decode_values(uint32_t channel_data);
void menu1_display_adc_output();
void menu2_change_softspan_range();

//! Initialize Linduino
void setup()
{
  char demo_name[] = "LTC2348-18";  //! Demo Board Name stored in QuikEval EEPROM
  quikeval_I2C_init();              //! Initializes Linduino I2C port.
  quikeval_SPI_init();            //! Initializes Linduino SPI port.

  Serial.begin(115200);             //! Initialize the serial port to the PC
  print_title();

  demo_board_connected = discover_DC2094(demo_name);
  if (demo_board_connected)
  {
    print_prompt();
  }
  sneaker_port_init();        //! Configure sneaker port to talk to the ADC
  quikeval_SPI_connect();           //! Connects to main SPI port
}

//! Repeats Linduino Loop
void loop()
{
  int8_t user_command;                 // The user input command
  uint8_t acknowledge = 0;
  if (Serial.available())             // Check for user input
  {
    user_command = read_int();        // Read the user command
    if (user_command != 'm')
      Serial.println(user_command);   // Prints the user command to com port
    Serial.flush();
    switch (user_command)
    {
      case 1:
        menu1_display_adc_output();
        break;
      case 2:
        menu2_change_softspan_range();
        break;
      default:
        Serial.println(F("Incorrect Option"));
    }
    if (acknowledge)
      Serial.println(F("***** I2C ERROR *****"));
    Serial.print(F("\n****************************** Press Enter to Continue ******************************\n"));
    read_int();
    print_prompt();
  }
}

//! Configure sneaker port to talk to the ADC
void sneaker_port_init()
{
  uint8_t value = 0;
  i2c_read_byte(SNEAKER_PORT_I2C_ADDRESS, &value);
  delay(100);
  value = value & 0x7F;             // P7 = WRIN = 0
  value = value | 0x04;             // P2 = WRIN2 = 1
  i2c_write_byte(SNEAKER_PORT_I2C_ADDRESS, value);
  delay(100);
}

//! Read the ID string from the EEPROM and determine if the correct board is connected.
//! Returns 1 if successful, 0 if not successful
uint8_t discover_DC2094(char *demo_name)
{
  Serial.print(F("\nChecking EEPROM contents..."));
  read_quikeval_id_string(&ui_buffer[0]);
  ui_buffer[48] = 0;
  Serial.println(ui_buffer);

  if (!strcmp(demo_board.product_name, demo_name))
  {
    Serial.print("Demo Board Name: ");
    Serial.println(demo_board.name);
    Serial.print("Product Name: ");
    Serial.println(demo_board.product_name);
    if (demo_board.option)
    {
      Serial.print("Demo Board Option: ");
      Serial.println(demo_board.option);
    }
    Serial.println(F("Demo board connected"));
    Serial.println(F("\n\n\t\t\t\tPress Enter to Continue..."));
    read_int();
    return 1;
  }
  else
  {
    Serial.print("Demo board ");
    Serial.print(demo_name);
    Serial.print(" not found, \nfound ");
    Serial.print(demo_board.name);
    Serial.println(" instead. \nConnect the correct demo board, then press the reset button.");
    return 0;
  }
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC2094A Demonstration Program                                 *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send data and receive data   *\n"));
  Serial.print(F("* from the 18-bit ADC.                                          *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.println(F("\n\n\n\t\t\t\tCONFIGURATION SETTINGS (Vref = 4.096V)\n"));
  Serial.println(F("|Config Number| SS2 | SS1 | SS0 | ANALOG INPUT RANGE      | DIGITAL COMPRESSION | RESULT BINARY FORMAT |"));
  Serial.println(F("|-------------|-----------------|-------------------------|---------------------|----------------------|"));
  Serial.println(F("|      0      |  0  |  0  |  0  | Disable Channel         | N/A                 | All Zeros            |"));
  Serial.println(F("|      1      |  0  |  0  |  1  | 0 - 1.25 Vref           | 1                   | Straight Binary      |"));
  Serial.println(F("|      2      |  0  |  1  |  0  | -1.25 Vref - +1.25 Vref | 1/1.024             | Two's Complement     |"));
  Serial.println(F("|      3      |  0  |  1  |  1  | -1.25 Vref - +1.25 Vref | 1                   | Two's Complement     |"));
  Serial.println(F("|      4      |  1  |  0  |  0  | 0 - 2.5 Vref            | 1/1.024             | Straight Binary      |"));
  Serial.println(F("|      5      |  1  |  0  |  1  | 0 - 2.5 Vref            | 1                   | Straight Binary      |"));
  Serial.println(F("|      6      |  1  |  1  |  0  | -2.5 Vref - +2.5 Vref   | 1/1.024             | Two's Complement     |"));
  Serial.println(F("|      7      |  1  |  1  |  1  | -2.5 Vref - +2.5 Vref   | 1                   | Two's Complement     |"));

  Serial.print(F("\n\n\n\t\t\t\tOPTIONS\n"));
  Serial.print(F("\n1 - Display ADC output\n"));
  Serial.print(F("2 - Change configuration setting\n"));

  Serial.print(F("\nENTER A COMMAND: "));
}

void decode_values(uint32_t channel_data)
{
  uint8_t channel, config_number;
  float voltage;
  uint32_t code;

  channel = (channel_data & CHANNEL_NUMBER) >> 3;
  config_number = channel_data & SOFTSPAN;
  code = (channel_data & 0xFFFFC0) >> 6;
  voltage = LTC23XX_voltage_calculator(code, config_number);

  Serial.print(F("\nChannel  : "));
  Serial.print(channel);
  Serial.print(F("\t\tData       : 0x"));
  Serial.println(code, HEX);
  Serial.print(F("Voltage  : "));
  Serial.print(voltage, 6);
  Serial.print(F(" V"));
  Serial.print(F("\tConfig Number: "));
  Serial.println(config_number);
}

//! Displays the ADC output and calculated voltage for all channels
void menu1_display_adc_output()
{
  uint8_t display_channels;
  uint8_t Result[24];
  uint8_t pos;
  data.LT_uint32 = 0;

  Serial.print("\nEnter the number of channels to be displayed: ");
  display_channels = read_int();
  if (display_channels < 1)
    display_channels = 1;
  else if (display_channels > num_of_channels)
    display_channels = num_of_channels;

  while (!Serial.available())
  {
    LTC23XX_read(QUIKEVAL_CS, configuration_bits.LT_uint32, Result);
    Serial.println(F("\n********************************************"));

    pos = 23;
    for (int i = 0; i < display_channels; ++i)
    {
      data.LT_byte[2] = Result[pos--];
      data.LT_byte[1] = Result[pos--];
      data.LT_byte[0] = Result[pos--];

      decode_values(data.LT_uint32);
    }
  }
}

//! Function to change the configuration setting
void menu2_change_softspan_range()
{
  uint32_t config_word = 0;
  uint8_t num;
  for (int i = 0; i < num_of_channels; ++i)
  {
    Serial.print(F("\nEnter configuration number for channel "));
    Serial.print(i);
    Serial.print(F(": "));
    num = read_int();
    if (num < 0)
      num = 0;
    else if (num > 7)
      num = 7;
    Serial.print(num);
    LTC23XX_create_config_word(i, num, &config_word);
  }
  Serial.print("\nConfig word: 0x");
  Serial.println(config_word, HEX);
  configuration_bits.LT_uint32 = config_word;
  Serial.print(F("\nCONFIGURATION CHANGED!"));
}