/*!
Linear Technology DC935A Demonstration Board
LTC2605: Octal 16-/14-/12-Bit Rail-to Rail DACs in 16-Lead SSOP

@verbatim
NOTES
  Setup:
    Set the terminal baud rate to 115200 and select the newline terminator.
     The program displays calculated voltages which are based on the voltage
     of the reference used, be it internal or external. A precision voltmeter
     is needed to verify the actual measured voltages against the calculated
     voltage displayed. If an external reference is used, a precision voltage
     source is required to apply the external reference voltage. A precision
     voltmeter is also required to measure the external reference voltage.
     No external power supply is required. Any assembly option may be used:

  Explanation of Commands:
    1- Select DAC: Select one of four DACs to test: A, B, C, D.

     2- Write to DAC input register: Value is stored in the DAC for updating
      later, allowing multiple channels to be updated at once, either
      through a software "Update All" command or by asserting the LDAC# pin.
      User will be prompted to enter either a code in hex or decimal, or a
      voltage. If a voltage is entered, a code will be calculated based on
      the active scaling and reference parameters - ideal values if no
      calibration was ever stored.

     3- Write and Update: Similar to item 1, but DAC is updated immediately.

     4- Update DAC: Copies the value from the input register into the DAC
      Register. Note that a "write and update" command writes the code to
      BOTH the input register and DAC register, so subsequent "update" commands
      will simply re-copy the same data (no change in output).

     5- Power Down DAC: Disable DAC output. Power supply current is reduced. DAC
      code present in DAC registers at time of shutdown are preserved.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim
http://ww.linear.com/product/LTC2605

http://www.linear.com/product/LTC2605#demoboards



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
    @ingroup LTC2605
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC2605.h"
#include <Wire.h>
#include <SPI.h>

// DAC Reference State
// Could have been 0 or 1. This allows you to use the
// variable "reference_mode" as the command argument to a write
#define REF_INTERNAL LTC2605_CMD_INTERNAL_REFERENCE   //!< Stored reference state is Internal
#define REF_EXTERNAL LTC2605_CMD_EXTERNAL_REFERENCE   //!< Stored reference state is External

//Function Declaration
void print_title();                         // Print the title block
void print_prompt(int16_t selected_dac);    // Prompt the user for an input command
int16_t prompt_voltage_or_code();
uint16_t get_voltage(float LTC2605_lsb, int16_t LTC2605_offset);
uint16_t get_code();
int8_t calibrate_dac(uint8_t index);        // Calibrate the selected DAC using a voltmeter. The routine does a linear curve fit given two data points.

int8_t menu_1_select_dac(int16_t *selected_dac);
int8_t menu_2_write_to_input_register(int16_t selected_dac);
int8_t menu_3_write_and_update_dac(int16_t selected_dac);
int8_t menu_4_update_power_up_dac(int16_t selected_dac);
int8_t menu_5_power_down_dac(int16_t selected_dac);

// Global variables
static uint8_t demo_board_connected;               //!< Set to 1 if the board is connected
static uint8_t shift_count = 0;                    //!< The data align shift count. For 16-bit=0, for 14 bits=2 for 12-bits=4
static uint8_t reference_mode = REF_INTERNAL;                     //!< Tells whether to set internal or external reference

// Global calibration variables
static float reference_voltage= 5;              //!< Reference voltage, either internal or external
static int16_t LTC2605_offset = 0;  //!< DAC offset
static float LTC2605_lsb = (reference_voltage / (pow(2,16) -1)); // least signifigant bit value of the dac

// Constants

//! Lookup table for DAC address. Allows the "All DACs" address to be indexed right after DAC D in loops.
//! This technique is very useful for devices with non-monotonic channel addresses.
const uint8_t address_map[9] = {LTC2605_DAC_A, LTC2605_DAC_B, LTC2605_DAC_C, LTC2605_DAC_D, LTC2605_DAC_E, LTC2605_DAC_F , LTC2605_DAC_G, LTC2605_DAC_H, LTC2605_DAC_ALL};  //!< Map entered option 0..2 to DAC address

//! Used to keep track to print voltage or print code
enum
{
  PROMPT_VOLTAGE = 0, /**< 0 */
  PROMPT_CODE = 1     /**< 1 */
};

//! Initialize Linduino
void setup()
// Setup the program
{
  quikeval_I2C_init();              // Configure the EEPROM I2C port for 100khz
  quikeval_I2C_connect();           // Connect I2C to main data port
  Serial.begin(115200);
  print_title();
  //if(demo_board_connected)
  demo_board_connected = true;
  print_prompt(0);
  LTC2605_write(LTC2605_I2C_ADDRESS, LTC2605_CMD_NO_OPERATION, LTC2605_DAC_ALL, 0x0000);
  Serial.flush();
}

//! Repeats Linduino loop
void loop()
{
  int8_t ack =0;
  int16_t user_command;
  static int16_t selected_dac =0; // The selected DAC to be updated (0=A, 1=B ... 9=All).  Initialized to "A"
  // the main control loop
  if (demo_board_connected) // do nothing if board is not connected
  {
    if (Serial.available()) // check for user input
    {
      user_command = read_int();
      Serial.println(user_command);
      Serial.flush();
      ack =0;
      switch (user_command)
      {
        case 1:
          ack |= menu_1_select_dac(&selected_dac);
          break;
        case 2:
          ack |= menu_2_write_to_input_register(selected_dac);
          break;
        case 3:
          ack |= menu_3_write_and_update_dac(selected_dac);
          break;
        case 4:
          ack |= menu_4_update_power_up_dac(selected_dac);
          break;
        case 5:
          ack |= menu_5_power_down_dac(selected_dac);
          break;
        default:
          Serial.println("Incorrect Option");
          break;
      }
      if (ack)Serial.println("I2C NACK received, check address\n");
      Serial.println("\n*****************************************************************");
      print_prompt(selected_dac);
    }
  }
}

// Function Definitions

//! Select which DAC to operate on
//! @return 0
int8_t menu_1_select_dac(int16_t *selected_dac) //!< what DAC to operate on
{
  // Select a DAC to operate on
  Serial.print("Select DAC to operate on (0=A, 1=B, 2=C, 3=D, 4=E, 5=F, 6=G, 7=H, 8=All)");
  *selected_dac = read_int();
  if (*selected_dac == 8)
    Serial.println("All");
  else
    Serial.println(*selected_dac);
  return(0);
}

//! Write data to input register, but do not update DAC output
//! @return ACK bit (0=acknowledge, 1=no acknowledge)
int8_t menu_2_write_to_input_register(int16_t selected_dac) //!< what DAC to operate on
{
  int8_t ack=0;
  uint16_t dac_code;

  if (prompt_voltage_or_code() == PROMPT_VOLTAGE)
    dac_code = get_voltage(LTC2605_lsb, LTC2605_offset);
  else
    dac_code = get_code();

  Serial.print("DAC code transmitted: 0x");
  Serial.println(dac_code,HEX);

  ack |= LTC2605_write(LTC2605_I2C_ADDRESS, LTC2605_CMD_WRITE, address_map[selected_dac], dac_code << shift_count);
  return (ack);
}
//!Write data to DAC register (which updates output immediately)
//! @return ACK bit (0=acknowledge, 1=no acknowledge)
int8_t menu_3_write_and_update_dac(int16_t selected_dac) //!< what DAC to operate on
{
  int8_t ack=0;
  uint16_t dac_code;

  if (prompt_voltage_or_code() == PROMPT_VOLTAGE)
    dac_code = get_voltage(LTC2605_lsb, LTC2605_offset);
  else
    dac_code = get_code();
  //Serial.print("lsb:");
  //Serial.println(LTC2605_lsb);
  Serial.print("DAC code transmitted: 0x");
  Serial.println(dac_code,HEX);


  ack |= LTC2605_write(LTC2605_I2C_ADDRESS, LTC2605_CMD_WRITE_UPDATE, address_map[selected_dac], dac_code << shift_count);
  return (ack);
}

//! Update DAC with data that is stored in input register, power up if sleeping
//! @return ACK bit (0=acknowledge, 1=no acknowledge)
int8_t menu_4_update_power_up_dac(int16_t selected_dac) //!< what DAC to operate on
{
  // Update DAC
  int8_t ack=0;
  ack |= LTC2605_write(LTC2605_I2C_ADDRESS, LTC2605_CMD_UPDATE, address_map[selected_dac], 0x0000);
  return (ack);
}

//! Power down DAC
//! @return ACK bit (0=acknowledge, 1=no acknowledge)
int8_t menu_5_power_down_dac(int16_t selected_dac) //!< what DAC to operate on
{
  // Power down DAC
  int8_t ack=0;
  ack |= LTC2605_write(LTC2605_I2C_ADDRESS, LTC2605_CMD_POWER_DOWN, address_map[selected_dac], 0x0000);
  return (ack);
}


//! Prompt user to enter a voltage or digital code to send to DAC
//! @return prompt type
int16_t prompt_voltage_or_code()
{
  int16_t user_input;
  Serial.print(F("Type 1 to enter voltage, 2 to enter code:"));
  Serial.flush();
  user_input = read_int();
  Serial.println(user_input);

  if (user_input != 2)
    return(PROMPT_VOLTAGE);
  else
    return(PROMPT_CODE);
}
//! Get voltage from user input, calculate DAC code based on lsb, offset
//! @return voltage
uint16_t get_voltage(float LTC2605_lsb, //!< the voltage LSB of the DAC
                     int16_t LTC2605_offset) //!< the Offset of the DAC
{
  float dac_voltage;

  Serial.print(F("Enter Desired DAC output voltage: "));
  dac_voltage = read_float();
  Serial.print(dac_voltage);
  Serial.println(" V");
  Serial.flush();
  return(LTC2605_voltage_to_code(dac_voltage, LTC2605_lsb, LTC2605_offset));
}

//! Get code to send to DAC directly, in decimal, hex, or binary
//! @return DAC code
uint16_t get_code()
{
  uint16_t returncode;
  Serial.println("Enter Desired DAC Code");
  Serial.print("(Format 32768, 0x8000, 0100000, or B1000000000000000): ");
  returncode = (uint16_t) read_int();
  Serial.print("0x");
  Serial.println(returncode, HEX);
  Serial.print("code corresponding to voltage: ");
  Serial.print(LTC2605_code_to_voltage( returncode, LTC2605_lsb,LTC2605_offset));
  Serial.println(" V");
  Serial.flush();
  return(returncode);
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.println("");
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC935 Demonstration Program                                   *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to send data to the LTC2605     *"));
  Serial.println(F("* octo 16/14/12-bit DAC found on the DC935 demo board.          *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************"));
}

//! Prints main menu.
void print_prompt(int16_t selected_dac) //!< what DAC to operate on
{
  Serial.println(F("\nCommand Summary:"));
  Serial.println(F("  1-Select DAC"));
  Serial.println(F("  2-Write to input register (no update)"));
  Serial.println(F("  3-Write and update DAC"));
  Serial.println(F("  4-Update / power up DAC"));
  Serial.println(F("  5-Power down DAC"));


  Serial.println("\nPresent Values:");
  Serial.print("  Selected DAC: ");
  if (selected_dac != 4)
    Serial.println((char) (selected_dac + 0x41));
  else
    Serial.println("All");
  //Serial.print("  DAC Code: 0x");
  // Serial.println(code[selected_dac], HEX);
  // Serial.print("  Calculated DAC Output Voltage = ");
  // Serial.print(voltage, 4);
  // Serial.println(" V");
  Serial.print("  DAC Reference: ");
  if (reference_mode == REF_INTERNAL)
    Serial.println("Internal");
  else
  {
    Serial.print(F("External "));
    Serial.print(reference_voltage, 5);
    Serial.println(F("V reference, please verify"));
    Serial.print(F("Enter a command:"));

  }
  Serial.flush();
}