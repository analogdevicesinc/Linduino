/*!
Linear Technology DC1096B Demonstration Board.
LTC2642: 16-/14-/12-Bit VOUT DAC with SPI Interface

LTC2641: 16-/14-/12-Bit VOUT DAC with SPI Interface
@verbatim

NOTES

  Set the terminal baud rate to 115200 and select the newline terminator.
  No external power supply is required. Two on-board reference voltages of
  2.5V and 5V are available. The board features LTC2642A with direct and
  buffered output.

  The program displays calculated voltages which are based on the reference
  voltage used. A precision voltmeter is needed to verify the actual
  measured voltages against the calculated voltage displayed.

  There is also an option of generating a square wave of required voltage
  levels and frequency. This proves the fast settling time of the corresponding
  parts. Setting the software to unipolar or bipolar will not set the hardware
  to bipolar mode. The jumpers need to be placed in their proper positions in
  order to accomplish this.

  Jumper settings:
   JP3: Sets DAC2 to either unipolar or bipolar mode. In bipolar mode, set JP6
   and JP7 to EXT and apply an appropriate supply to AMP V+, AMP V–, and GND
   turret posts.

   JP4: Sets DAC3 to either unipolar or bipolar mode. In bipolar mode, set JP6
   and JP7 to EXT and apply an appropriate supply to AMP V+, AMP V–, and GND
   turret posts.

  Explanation of Commands:
   Option 1: Enter a digital value or voltage to obtain analog voltage output.
   Option 2: Enter the voltage levels to obtain a square wave.
   Option 3: Vary the reference voltage used.
   Option 4: Select Bipolar/Unipolar.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2642
http://www.linear.com/product/LTC2641

http://www.linear.com/product/LTC2642#demoboards
http://www.linear.com/product/LTC2641#demoboards


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
    @ingroup LTC2642
*/

// Headerfiles
#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include "LT_SPI.h"
#include "LT_I2C.h"
#include "Linduino.h"
#include "QuikEval_EEPROM.h"
#include "UserInterface.h"
#include "LTC2642.h"

// Macros
#define VREF1 5.0
#define VREF2 2.5

// Global Variables
float reference_voltage = VREF1;   // Set the reference voltage
uint8_t range = UNIPOLAR; // set to unipolar

// Function Declarations
void print_title();
void print_prompt();
uint8_t menu1_voltage_output();
uint8_t menu2_square_wave_output();
void menu3_change_reference_voltage();
void menu4_select_range();

//! Used to choose between voltage and code
enum
{
  PROMPT_VOLTAGE = 0, /**< 0 */
  PROMPT_CODE = 1     /**< 1 */
};

//! Initialize Linduino
void setup()
{
  uint8_t demo_board_connected;

  char demo_name[] = "DC1096";           // Demo Board Name stored in QuikEval EEPROM
  output_high(LTC2642_CS);                //! Pulls LTC2642 Chip Select (LD pin) High

  quikeval_I2C_init();                    //! Initializes Linduino I2C port.

  quikeval_SPI_init();                    //! Configures the SPI port for 4MHz SCK
  quikeval_SPI_connect();                 //! Connects SPI to QuikEval port

  Serial.begin(115200);                   //! Initializes the serial port to the PC
  print_title();                          //! Displays the title

  demo_board_connected = discover_demo_board(demo_name); //! Checks if correct demo board is connected.
  if (demo_board_connected)               //! Prints the prompt if the correct demo board is connected
  {
    print_prompt();                       // Prints prompt and indicates that "A" is selected.
  }
}

//! Repeats Linduino loop
void loop()
{
  int8_t user_command;                // The user input command
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
        acknowledge |= menu1_voltage_output();
        break;
      case 2:
        acknowledge |= menu2_square_wave_output();
        break;
      case 3:
        menu3_change_reference_voltage();
        break;
      case 4:
        menu4_select_range();
        break;
      default:
        Serial.println(F("Incorrect Option"));
    }
    if (acknowledge)
      Serial.println(F("***** I2C ERROR *****"));
    print_prompt();
  }
}

//! Function to enter a digital value and get the analog output
uint8_t menu1_voltage_output()
{
  uint16_t dac_code;
  float voltage;
  if (prompt_voltage_or_code() == PROMPT_VOLTAGE)
    dac_code = get_voltage();
  else
    dac_code = get_code();

  LTC2642_write(QUIKEVAL_CS, dac_code);
  voltage = LTC2642_code_to_voltage(dac_code, reference_voltage, range);
  Serial.print("\nDac Code = 0x");
  Serial.print(dac_code, HEX);
  Serial.print("\nOutput voltage = ");
  Serial.print(voltage, 3);
  Serial.print(" V");
  return 0;
}

//! Function to generate a square wave of desired frequency and voltage ranges
uint8_t menu2_square_wave_output()
{
  float voltage_high, voltage_low;
  uint16_t code_high, code_low;
  //uint8_t receive_enter;  // To receive enter key pressed

  Serial.print("\nEnter voltage_high: ");
  while (!Serial.available());
  voltage_high = read_float();
  Serial.print(voltage_high);
  Serial.println(" V");

  Serial.print("Enter voltage_low: ");
  while (!Serial.available());
  voltage_low = read_float();
  Serial.print(voltage_low);
  Serial.println(" V");

  //! Converting data into voltage
  code_high = LTC2642_voltage_to_code(voltage_high, reference_voltage, range);
  code_low = LTC2642_voltage_to_code(voltage_low, reference_voltage, range);

  Serial.print(F("Enter any character to stop. "));
  while (!Serial.available()) //! Generate square wave until a key is pressed
  {
    LTC2642_write(LTC2642_CS, code_high);
    delay(30);  // 30 ms delay
    LTC2642_write(LTC2642_CS, code_low);
    delay(30);  // 30 ms delay
  }
  read_int();
  return 0;
}

//! Function to change the reference voltage to be used
void menu3_change_reference_voltage()
{
  Serial.print(F("\nEnter the reference voltage: "));
  reference_voltage = read_float();
  Serial.print(reference_voltage);
  Serial.print(F(" V"));
}

//! Select between unipolar/bipolar
void menu4_select_range()
{
  uint8_t choice;
  Serial.print("\nEnter your choice (1 - Unipolar, 2 - Bipolar): ");
  choice = read_int();
  Serial.print(choice);

  if (choice == 1)
  {
    range = UNIPOLAR;
    Serial.print(F("\nSet jumpers JP3 and JP4 to UNI."));
  }
  else
  {
    range = BIPOLAR;
    Serial.print(F("\nSet jumpers JP3 and JP4 to BIP."));
    Serial.print(F("\nIn bipolar mode, set JP6 and JP7 to EXT and apply an appropriate supply"));
    Serial.print(F("\nto AMP V+, AMP V–, and GND turret posts."));
  }
}

//! Prompt user to enter a voltage or digital code to send to DAC
int16_t prompt_voltage_or_code()
{
  int16_t user_input;
  Serial.print(F("\nType 1 to enter voltage, 2 to enter code:"));
  Serial.flush();
  user_input = read_int();
  Serial.println(user_input);

  if (user_input != 2)
    return(PROMPT_VOLTAGE);
  else
    return(PROMPT_CODE);
}

//! Get voltage from user input, calculate DAC code based on lsb, offset
uint16_t get_voltage()
{
  float dac_voltage;

  Serial.print(F("Enter Desired DAC output voltage: "));
  dac_voltage = read_float();
  Serial.print(dac_voltage, 3);
  Serial.println(" V");
  Serial.flush();
  return(LTC2642_voltage_to_code(dac_voltage, reference_voltage, range));
}

//! Get code to send to DAC directly, in decimal, hex, or binary
uint16_t get_code()
{
  uint16_t returncode;
  Serial.println("Enter Desired DAC Code");
  Serial.print("(Format 32768, 0x8000, 0100000, or B1000000000000000): ");
  returncode = (uint16_t) read_int();
  Serial.print("0x");
  Serial.println(returncode, HEX);
  Serial.flush();
  return(returncode);
}


//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC1096B Demonstration Program                                 *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send data to the 16-bit DAC. *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("\nSelected Range: "));
  if (range == UNIPOLAR)
    Serial.print(F("Unipolar"));
  else
    Serial.print(F("Bipolar"));
  Serial.print(F("\nSelected Reference Voltage: "));
  Serial.print(reference_voltage);
  Serial.print(F(" V"));

  Serial.print(F("\n\nMenu:"));
  Serial.print(F("\n  1 - Set output\n"));
  Serial.print(F("  2 - Square Wave output\n"));
  Serial.print(F("  3 - Set Reference Voltage\n"));
  Serial.print(F("  4 - Select Unipolar/Bipolar\n"));
  Serial.print(F("Enter a command: "));
}
