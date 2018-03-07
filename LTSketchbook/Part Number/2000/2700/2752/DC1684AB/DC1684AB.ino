/*!
Linear Technology DC1684AB Demonstration Board.
LTC2752: Dual Serial 16-Bit SoftSpan IOUT DAC

@verbatim
NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

   An external +/- 15V power supply is required to power the circuit.

  Explanation of Commands:
   1- Select DAC
   Select between DAC A, DAC B, or both.

   2- Change Span of Selected DAC
   |   Command   | Range Selected |
   | C3 C2 C1 C0 |                |
   |-------------|----------------|
   | 1  0  0  0  |    0V - 5V     |
   | 1  0  0  1  |    0V - 10V    |
   | 1  0  1  0  |   -5V - +5V    |
   | 1  0  1  1  |  -10V - +10V   |
   | 1  1  0  0  | -2.5V - +2.5V  |
   | 1  1  0  1  | -2.5V - +7V    |

   3- Voltage Output
   Displays the calculated voltage depending on the code input from user and
   voltage range selected.

   4- Square wave output
   Generates a square wave on the output pin. This function helps to measure
   settling time and glitch impulse.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2752

http://www.linear.com/product/LTC2752#demoboards


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
    @ingroup LTC2752
*/

// Headerfiles
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "Linduino.h"
#include <SPI.h>
#include <Stream.h>
#include "LTC2752.h"

// Global variables
static uint8_t demo_board_connected;   //!< Set to 1 if the board is connected

float DACA_RANGE_LOW = 0;
float DACA_RANGE_HIGH = 5;

float DACB_RANGE_LOW = 0;
float DACB_RANGE_HIGH = 5;

uint8_t DAC_SELECTED = ADDRESS_DACA;

// Function Declarations
void print_title();
void print_prompt();
uint8_t menu1_select_dac();
void menu2_change_range();
uint8_t menu3_voltage_output();
uint8_t menu4_square_wave_output();

//! Initialize Linduino
void setup()
{
  char product_name[] = "LTC2752";  // Product Name stored in QuikEval EEPROM
  char board_name[] = "DC1684";     // Demo Board Name stored in QuikEval EEPROM

  quikeval_SPI_init();              // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();           // Connect SPI to main data port
  quikeval_I2C_init();              // Configure the EEPROM I2C port for 100kHz

  Serial.begin(115200);             // Initialize the serial port to the PC
  print_title();

  demo_board_connected = discover_DC1684AB(product_name, board_name);
  if (demo_board_connected)
  {
    print_prompt();
  }
}

//! Read the ID string from the EEPROM and determine if the correct board is connected.
//! Returns 1 if successful, 0 if not successful
uint8_t discover_DC1684AB(char *product_name, char *board_name)
{
  Serial.print(F("\nChecking EEPROM contents..."));
  read_quikeval_id_string(&ui_buffer[0]);
  ui_buffer[48] = 0;
  // Serial.println(ui_buffer);

  if (!strcmp(demo_board.product_name, product_name) && !strcmp(demo_board.name, board_name))
  {
    Serial.print("\nDemo Board Name: ");
    Serial.println(demo_board.name);
    Serial.print("Product Name: ");
    Serial.println(demo_board.product_name);
    if (demo_board.option)
    {
      Serial.print("Demo Board Option: ");
      Serial.println(demo_board.option);
    }
    Serial.println(F("Demo board connected..."));
    Serial.println(F("\n\n\t\t\t\tPress Enter to Continue..."));
    read_int();
    return 1;
  }
  else
  {
    Serial.print("Demo board ");
    Serial.print(board_name);
    Serial.print(" not found, \nfound ");
    Serial.print(demo_board.name);
    Serial.println(" instead. \nConnect the correct demo board, then press the reset button.");
    return 0;
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
        menu1_select_dac();
        break;
      case 2:
        menu2_change_range();
        break;
      case 3:
        menu3_voltage_output();
        break;
      case 4:
        menu4_square_wave_output();
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
  Serial.println(F("* DC1684A-B Demonstration Program                               *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to send data to the LTC2752     *"));
  Serial.println(F("* Dual Serial 16-bit Soft Span DAC                              *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.println(F("\nCommand Summary:"));
  Serial.println(F("\n  1. Select DAC"));
  Serial.println(F("  2. Change Span of selected DAC"));
  Serial.println(F("  3. Voltage Output"));
  Serial.println(F("  4. Square wave output"));

  Serial.println(F("\nPresent Values:\n"));
  Serial.print(F("  DAC A Range: "));
  Serial.print(DACA_RANGE_LOW);
  Serial.print(F(" V to "));
  Serial.print(DACA_RANGE_HIGH);
  Serial.println(F(" V"));

  Serial.print(F("  DAC B Range: "));
  Serial.print(DACB_RANGE_LOW);
  Serial.print(F(" V to "));
  Serial.print(DACB_RANGE_HIGH);
  Serial.println(F(" V"));

  Serial.print(F("\n  Selected DAC: "));
  switch (DAC_SELECTED)
  {
    case ADDRESS_DACA:
      Serial.println(F("DAC A"));
      break;
    case ADDRESS_DACB:
      Serial.println(F("DAC B"));
      break;
    case ADDRESS_DAC_ALL:
      Serial.println(F("ALL DACs"));
      break;
  }

  Serial.print(F("\n\nEnter a command: "));
  Serial.flush();
}

//! Function to select DAC and set DAC address
uint8_t menu1_select_dac()
{
  uint8_t choice;
  Serial.println(F("\n1. DAC A"));
  Serial.println(F("2. DAC B"));
  Serial.println(F("3. All DACs"));
  Serial.print(F("\nEnter a choice: "));
  choice = read_int();          // Read the user command

  switch (choice)
  {
    case 1:
      DAC_SELECTED = ADDRESS_DACA;
      Serial.println("DAC A");
      break;
    case 2:
      DAC_SELECTED = ADDRESS_DACB;
      Serial.println("DAC B");
      break;
    default:
      DAC_SELECTED = ADDRESS_DAC_ALL;
      Serial.println("ALL DACs");
      break;
  }
  Serial.flush();
}

//! Function to choose the range of voltages to be used
void menu2_change_range()
{
  uint16_t span;
  Serial.println("\n| Choice | Range         |");
  Serial.println("|--------|---------------|");
  Serial.println("|    0   |    0 - 5 V    |");
  Serial.println("|    1   |    0 - 10 V    |");
  Serial.println("|    2   |   -5 - +5 V   |");
  Serial.println("|    3   |  -10 - +10 V  |");
  Serial.println("|    4   | -2.5 - +2.5 V |");
  Serial.println("|    5   | -2.5 - +7.5 V |");

  Serial.print("\nEnter your choice: ");
  span = read_int();

  if (DAC_SELECTED == ADDRESS_DACA || DAC_SELECTED == ADDRESS_DAC_ALL)
  {
    switch (span)
    {
      case 0:
        DACA_RANGE_LOW = 0;
        DACA_RANGE_HIGH = 5;
        break;

      case 1:
        DACA_RANGE_LOW = 0;
        DACA_RANGE_HIGH = 10;
        break;

      case 2:
        DACA_RANGE_LOW = -5;
        DACA_RANGE_HIGH = 5;
        break;

      case 3:
        DACA_RANGE_LOW = -10;
        DACA_RANGE_HIGH = 10;
        break;

      case 4:
        DACA_RANGE_LOW = -2.5;
        DACA_RANGE_HIGH = 2.5;
        break;

      case 5:
        DACA_RANGE_LOW = -2.5;
        DACA_RANGE_HIGH = 7.5;
        break;

      default:
        Serial.println("\nWrong choice!");
    }
    Serial.print(F("Span Changed!"));
  }
  if (DAC_SELECTED == ADDRESS_DACB || DAC_SELECTED == ADDRESS_DAC_ALL)
  {
    switch (span)
    {
      case 0:
        DACB_RANGE_LOW = 0;
        DACB_RANGE_HIGH = 5;
        break;

      case 1:
        DACB_RANGE_LOW = 0;
        DACB_RANGE_HIGH = 10;
        break;

      case 2:
        DACB_RANGE_LOW = -5;
        DACB_RANGE_HIGH = 5;
        break;

      case 3:
        DACB_RANGE_LOW = -10;
        DACB_RANGE_HIGH = 10;
        break;

      case 4:
        DACB_RANGE_LOW = -2.5;
        DACB_RANGE_HIGH = 2.5;
        break;

      case 5:
        DACB_RANGE_LOW = -2.5;
        DACB_RANGE_HIGH = 7.5;
        break;

      default:
        Serial.println("\nWrong choice!");
    }
    Serial.print(F("Span Changed!"));
  }
  LTC2752_write(LTC2752_CS, LTC2752_WRITE_SPAN_DAC, DAC_SELECTED, span);
}

//! Function to enter a digital value and get the analog output
uint8_t menu3_voltage_output()
{
  uint8_t choice;
  uint16_t data;
  float voltage;

  Serial.println(F("\n1. Enter Voltage"));
  Serial.println(F("2. Enter Code"));
  Serial.print(F("\nEnter a choice: "));
  choice = read_int();          // Read the user command
  Serial.print(choice);

  if (choice == 2)
  {
    Serial.print("\nEnter the 16-bit data as decimal or hex: ");
    data = read_int();
    Serial.print("0x");
    Serial.println(data, HEX);

    if (DAC_SELECTED == ADDRESS_DACA || DAC_SELECTED == ADDRESS_DAC_ALL)
    {
      voltage = LTC2752_code_to_voltage(data, DACA_RANGE_LOW, DACA_RANGE_HIGH);
      Serial.print("\nDACA Output voltage = ");
      Serial.print(voltage);
      Serial.println(" V");
    }

    if (DAC_SELECTED == ADDRESS_DACB || DAC_SELECTED == ADDRESS_DAC_ALL)
    {
      voltage = LTC2752_code_to_voltage(data, DACB_RANGE_LOW, DACB_RANGE_HIGH);
      Serial.print("\nDACB Output voltage = ");
      Serial.print(voltage);
      Serial.println(" V");
    }
    LTC2752_write(LTC2752_CS, LTC2752_WRITE_CODE_UPDATE_DAC, DAC_SELECTED, data);
  }
  else if (choice == 1)
  {
    Serial.print("\nEnter voltage: ");
    while (!Serial.available());
    voltage = read_float();
    Serial.print(voltage);
    Serial.println(" V");

    if (DAC_SELECTED == ADDRESS_DACA || DAC_SELECTED == ADDRESS_DAC_ALL)
    {
      data = LTC2752_voltage_to_code(voltage, DACA_RANGE_LOW, DACA_RANGE_HIGH);
      LTC2752_write(LTC2752_CS, LTC2752_WRITE_CODE_UPDATE_DAC, DAC_SELECTED, data);
      Serial.print("\nDACA Output voltage = ");
      Serial.print(voltage);
      Serial.println(" V");
      Serial.print("\nDAC CODE: 0x");
      Serial.println(data, HEX);
    }

    if (DAC_SELECTED == ADDRESS_DACB || DAC_SELECTED == ADDRESS_DAC_ALL)
    {
      data = LTC2752_voltage_to_code(voltage, DACB_RANGE_LOW, DACB_RANGE_HIGH);
      LTC2752_write(LTC2752_CS, LTC2752_WRITE_CODE_UPDATE_DAC, DAC_SELECTED, data);
      Serial.print("\nDACB Output voltage = ");
      Serial.print(voltage);
      Serial.println(" V");
      Serial.print("\nDAC CODE: 0x");
      Serial.println(data, HEX);
    }
  }
  return 0;
}

//! Function to generate a square wave of desired frequency and voltage ranges
uint8_t menu4_square_wave_output()
{
  uint16_t freq;
  float time;
  float voltage_high, voltage_low;
  uint16_t code_high, code_low;
  uint8_t receive_enter;  // To receive enter key pressed

  Serial.print("\nEnter voltage_high: ");
  while (!Serial.available());
  voltage_high = read_float();
  Serial.print(voltage_high);
  Serial.println(" V");

  Serial.print("\nEnter voltage_low: ");
  while (!Serial.available());
  voltage_low = read_float();
  Serial.print(voltage_low);
  Serial.println(" V");

  Serial.print("\nEnter the required frequency in Hz: ");
  freq = read_int();
  Serial.print(freq);
  Serial.println(" Hz");

  time = (float)1000/freq;
  Serial.print("\nT = ");
  Serial.print(time);
  Serial.println(" ms");

  //! Converting voltage into data
  if (DAC_SELECTED == ADDRESS_DACA || DAC_SELECTED == ADDRESS_DAC_ALL)
  {
    code_high = LTC2752_voltage_to_code(voltage_high, DACA_RANGE_LOW, DACA_RANGE_HIGH);
    code_low = LTC2752_voltage_to_code(voltage_low, DACA_RANGE_LOW, DACA_RANGE_HIGH);
  }
  if (DAC_SELECTED == ADDRESS_DACB || DAC_SELECTED == ADDRESS_DAC_ALL)
  {
    code_high = LTC2752_voltage_to_code(voltage_high, DACB_RANGE_LOW, DACB_RANGE_HIGH);
    code_low = LTC2752_voltage_to_code(voltage_low, DACB_RANGE_LOW, DACB_RANGE_HIGH);
  }

  while (!Serial.available()) //! Generate square wave until a key is pressed
  {
    LTC2752_write(LTC2752_CS, LTC2752_WRITE_CODE_UPDATE_DAC, DAC_SELECTED, code_high);
    delayMicroseconds(time * 500);
    LTC2752_write(LTC2752_CS, LTC2752_WRITE_CODE_UPDATE_DAC, DAC_SELECTED, code_low);
    delayMicroseconds(time * 500);
  }
  receive_enter = read_int();
  return 0;
}

