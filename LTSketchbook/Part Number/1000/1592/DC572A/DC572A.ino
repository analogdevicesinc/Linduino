/*!
Linear Technology DC572A Demonstration Board.
LTC1592: 16-bit SoftSpan DAC with Programmable Output Range

@verbatim
NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

   An external +/- 15V power supply is required to power the circuit.

  Explanation of Commands:
   1- Voltage Output
   Displays the calculated voltage depending on the code input from user and
   voltage range selected.

   2- Square Wave Output
   Generates a square wave on the output pin. This function helps to measure
   settling time and glitch impulse.

   3- Change Range
   |   Command   | Range Selected |
   | C3 C2 C1 C0 |                |
   |------------------------------|
   | 1  0  0  0  |    0V - 5V     |
   | 1  0  0  1  |    0V - 10V    |
   | 1  0  1  0  |   -5V - +5V    |
   | 1  0  1  1  |  -10V - +10V   |
   | 1  1  0  0  | -2.5V - +2.5V  |
   | 1  1  0  1  | -2.5V - 7.5V   |


USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC1592

http://www.linear.com/product/LTC1592#demoboards


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
    @ingroup LTC1592
*/

// Headerfiles
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "Linduino.h"
#include <SPI.h>
#include "LTC1592.h"
#include <Stream.h>

// Global variables
static uint8_t demo_board_connected;   //!< Set to 1 if the board is connected
float LTC1592_RANGE_LOW = 0;
float LTC1592_RANGE_HIGH = 5;
uint16_t data = 0x0000;
uint8_t range_selected = 0x00;

// Function Declarations
void print_title();
void print_prompt();
uint8_t menu_1_voltage_output();
uint8_t menu_2_square_wave_output();
void menu_3_change_range();

//! Initialize Linduino
void setup()
{
  char demo_name[] = "DC572";  // Demo Board Name stored in QuikEval EEPROM

  quikeval_SPI_init();          // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();       // Connect SPI to main data port
  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100kHz

  Serial.begin(115200);         // Initialize the serial port to the PC
  print_title();

  read_quikeval_id_string(&ui_buffer[0]);
  ui_buffer[48] = 0;
  Serial.println(ui_buffer);

  demo_board_connected = discover_demo_board(demo_name);
  if (demo_board_connected)
  {
    Serial.println("Connected...");
    print_prompt();
  }
}

//! Repeats Linduino loop
void loop()
{
  int16_t user_command;
  if (Serial.available())               // Check for user input
  {
    user_command = read_int();          // Read the user command
    Serial.println(user_command);
    Serial.flush();
    switch (user_command)
    {
      case 1:
        menu_1_voltage_output();
        break;
      case 2:
        menu_2_square_wave_output();
        break;
      case 3:
        menu_3_change_range();
        break;
      default:
        Serial.println(F("Incorrect Option"));
        break;
    }
    Serial.println(F("\n************************************************************"));
    print_prompt();
  }
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.println();
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC572A Demonstration Program                                  *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to send data to the LTC1592     *"));
  Serial.println(F("* 16-bit SoftSpan DAC found on the DC572A demo board.           *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.println(F("\nCommand Summary:"));
  Serial.println(F("  1-Voltage Output"));
  Serial.println(F("  2-Square wave output"));
  Serial.println(F("  3-Change Range"));

  Serial.println(F("\nPresent Values:"));
  Serial.print(F("  Range: "));
  switch (range_selected)
  {
    case 0:
      Serial.print(F("0V to 5V"));
      break;

    case 1:
      Serial.print(F("0V to 10V"));
      break;

    case 2:
      Serial.print(F("-5V to +5V"));
      break;

    case 3:
      Serial.print(F("-10V to +10V"));
      break;

    case 4:
      Serial.print(F("-2.5V to +2.5V"));
      break;

    case 5:
      Serial.print(F("-2.5V to 7.5V"));
      break;

    default:
      Serial.println("0V to 5V");
  }

  Serial.print(F("\n\nEnter a command: "));
  Serial.flush();
}

//! Function to enter a digital value and get the analog output
uint8_t menu_1_voltage_output()
{
  float voltage;
  uint8_t dac_command;
  Serial.print("\nEnter the 16-bit data as decimal or hex: ");
  data = read_int();
  Serial.print("0x");
  Serial.println(data, HEX);

  switch (range_selected)
  {
    case 0:
      dac_command = LTC1592_RANGE_0_TO_5V;
      break;

    case 1:
      dac_command = LTC1592_RANGE_0_TO_10V;
      break;

    case 2:
      dac_command = LTC1592_RANGE_PLUS_MINUS_5V;
      break;

    case 3:
      dac_command = LTC1592_RANGE_PLUS_MINUS_10V;
      break;

    case 4:
      dac_command = LTC1592_RANGE_PLUS_MINUS_2_5V;
      break;

    case 5:
      dac_command = LTC1592_RANGE_MINUS_2_5V_TO_PLUS_7_5V;
      break;
  }

  LTC1592_write(LTC1592_CS, dac_command, data);
  voltage = LTC1592_code_to_voltage(data, LTC1592_RANGE_HIGH, LTC1592_RANGE_LOW);

  Serial.print("\nOutput voltage = ");
  Serial.print(voltage);
  Serial.println(" V");
  return 0;
}

//! Function to generate a square wave of desired frequency and voltage ranges
uint8_t menu_2_square_wave_output()
{
  uint16_t freq;
  float time;
  float voltage_high, voltage_low;
  uint16_t data_voltage_high, data_voltage_low;
  uint8_t receive_enter;  // To receive enter key pressed
  uint8_t dac_command;

  Serial.print("\nEnter voltage_high: ");
  while (!Serial.available());
  voltage_high = read_float();
  Serial.print(voltage_high);
  Serial.println(" V");
  receive_enter = read_int();

  Serial.print("\nEnter voltage_low: ");
  while (!Serial.available());
  voltage_low = read_float();
  Serial.print(voltage_low);
  Serial.println(" V");
  receive_enter = read_int();

  Serial.print("\nEnter the required frequency in Hz: ");
  freq = read_int();
  Serial.print(freq);
  Serial.println(" Hz");

  time = (float)1000/freq;
  Serial.print("\nT = ");
  Serial.print(time);
  Serial.println(" ms");

  //! Converting data into voltage
  data_voltage_high = LTC1592_voltage_to_code(voltage_high, LTC1592_RANGE_HIGH, LTC1592_RANGE_LOW);
  data_voltage_low = LTC1592_voltage_to_code(voltage_low, LTC1592_RANGE_HIGH, LTC1592_RANGE_LOW);

  switch (range_selected)
  {
    case 0:
      dac_command = LTC1592_RANGE_0_TO_5V;
      break;

    case 1:
      dac_command = LTC1592_RANGE_0_TO_10V;
      break;

    case 2:
      dac_command = LTC1592_RANGE_PLUS_MINUS_5V;
      break;

    case 3:
      dac_command = LTC1592_RANGE_PLUS_MINUS_10V;
      break;

    case 4:
      dac_command = LTC1592_RANGE_PLUS_MINUS_2_5V;
      break;

    case 5:
      dac_command = LTC1592_RANGE_MINUS_2_5V_TO_PLUS_7_5V;
      break;
  }

  while (!Serial.available()) //! Generate square wave until a key is pressed
  {
    LTC1592_write(LTC1592_CS, dac_command, data_voltage_high);
    delayMicroseconds(time * 500);
    LTC1592_write(LTC1592_CS, dac_command, data_voltage_low);
    delayMicroseconds(time * 500);
  }
  receive_enter = read_int();
  return 0;
}

//! Function to choose the range of voltages to be used
void menu_3_change_range()
{
  Serial.println("\n| Choice | Range         |");
  Serial.println("|--------|---------------|");
  Serial.println("|    0   | 0 - 5 V       |");
  Serial.println("|    1   | 0 - 10 V      |");
  Serial.println("|    2   | -5 - +5 V     |");
  Serial.println("|    3   | -10 - +10 V   |");
  Serial.println("|    4   | -2.5 - +2.5 V |");
  Serial.println("|    5   | -2.5 - 7.5 V  |");

  Serial.print("\nEnter your choice: ");
  range_selected = read_int();
  Serial.println(range_selected);

  switch (range_selected)
  {
    case 0:
      LTC1592_RANGE_LOW = 0;
      LTC1592_RANGE_HIGH = 5;
      break;

    case 1:
      LTC1592_RANGE_LOW = 0;
      LTC1592_RANGE_HIGH = 10;
      break;

    case 2:
      LTC1592_RANGE_LOW = -5;
      LTC1592_RANGE_HIGH = 5;
      break;

    case 3:
      LTC1592_RANGE_LOW = -10;
      LTC1592_RANGE_HIGH = 10;
      break;

    case 4:
      LTC1592_RANGE_LOW = -2.5;
      LTC1592_RANGE_HIGH = 2.5;
      break;

    case 5:
      LTC1592_RANGE_LOW = -2.5;
      LTC1592_RANGE_HIGH = 7.5;
      break;

    default:
      Serial.println("\nWrong choice!");
  }
}
