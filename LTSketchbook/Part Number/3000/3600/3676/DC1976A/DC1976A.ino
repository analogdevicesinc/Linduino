/*!
Linear Technology DC1976A Demonstration Board
LTC3676: Power Management Solution for Application Processors

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.
   External power supply is required. Any assembly option may be used:
   DC1976A-A, DC1976A-B.

@endverbatim

http://www.linear.com/product/LTC3676

http://www.linear.com/product/LTC3676#demoboards


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

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC3676.h"

// Function Declarations
int8_t menu_1_read_write_registers();
int8_t menu_2_regulator_settings();
int8_t menu_3_control_settings();
int8_t menu_4_sequencing(uint8_t *reg_phase);

void print_title();
void print_prompt();
void print_warning_prompt();
int8_t LTC3676_print_all_registers(uint8_t i2c_address);
void read_reg_map(uint8_t *reg_map_array);

// Global variables
static uint8_t demo_board_connected;     //!< Set to 1 if the board is connected
static uint8_t i2c_address;              //!< I2C address set for either -A or -B demo board
static char board_option;                //!< Demo board option of the attached demo board
char demo_name[] = "DC1976";             //!< Demo Board Name stored in QuikEval EEPROM
uint8_t reg_phase[7] = {1,1,1,1,1,1,1};  //!< Power-up sequence phases for every regulator output
float delay_ms = 0;                      //!< Delay between power-up phases

//! Initialize Linduino
void setup()
{
  char eeprom_string[50];
  uint8_t char_count;
  quikeval_I2C_init();          //! Initializes Linduino I2C port.
  quikeval_I2C_connect();       //! Connects I2C port to the QuikEval connector
  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  demo_board_connected = discover_demo_board(demo_name);  //! Checks if correct demo board is connected.
  if (demo_board_connected)
  {

    char_count = read_quikeval_id_string(eeprom_string);
    board_option = demo_board_option(eeprom_string);
    if (board_option == 'B') i2c_address = LTC3676_1_I2C_ADDRESS;
    else i2c_address = LTC3676_I2C_ADDRESS;
    print_prompt();
  }
  else
  {
    print_warning_prompt();
    demo_board_connected = true;
    i2c_address = LTC3676_I2C_ADDRESS;
  }
}

//! Repeats Linduino loop
void loop()
{
  int8_t ack=0;
  uint8_t user_command;
  if (demo_board_connected)                          //! Does nothing if the demo board is not connected
  {
    if (Serial.available())                          //! Checks for user input
    {
      user_command = read_int();                     //! Reads the user command
      if (user_command != 'm')
        Serial.println(user_command);
      ack = 0;
      switch (user_command)                               //! Prints the appropriate submenu
      {
        case 1:
          ack |= menu_1_read_write_registers();          // Print single-ended voltage menu
          break;
        case 2:
          ack |= menu_2_regulator_settings();            // Differential voltage menu
          break;
        case 3:
          ack |= menu_3_control_settings();              // Control Settings menu
          break;
        case 4:
          ack |= menu_4_sequencing(reg_phase);           // Sequencing menu
          break;
        default:
          Serial.println("Incorrect Option");
          break;
      }
      if (ack != 0)
      {
        Serial.print(F("Error: No Acknowledge. \n"));
      }
      print_prompt();
    }
  }
}

// Function Definitions

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC1976A Demonstration Program                                 *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send and receive data from   *\n"));
  Serial.print(F("* the LTC3676 Power Management Solution for                     *\n"));
  Serial.print(F("* Application Processors.                                       *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("\n  1-Read/Write Registers\n"));
  Serial.print(F("  2-Regulator Settings\n"));
  Serial.print(F("  3-Control Settings\n"));
  Serial.print(F("  4-Sequencing\n"));
  Serial.print(F("\nEnter a command:"));
}

//! Prints a warning if the demo board is not detected.
void print_warning_prompt()
{
  Serial.println(F("\nWarning: Demo board not detected. Linduino will attempt to proceed."));
}

int8_t LTC3676_print_all_registers(uint8_t i2c_address)
{
  int8_t ack = 0;
  uint8_t user_register = 0x01;
  uint8_t data;
  do
  {
    ack |= LTC3676_register_read(i2c_address, user_register, &data);   //! Read register
    Serial.print("Register 0x");
    Serial.print(user_register, HEX);
    Serial.print(":\t0x");
    Serial.println(data, HEX);
    user_register++;
  }
  while ((user_register <= 0x17) && (ack != 1));
  return ack;
}

char demo_board_option(char *eeprom_string)
{
  char option='0';
  uint8_t i;
  for (i=0; i < strlen(eeprom_string); i++)
  {
    if (eeprom_string[i] == 'A' && option == '0')
      option = eeprom_string[i+2];
  }
  return option;
}

int8_t menu_1_read_write_registers()
{
  int8_t ack=0;
  uint8_t user_command;
  uint8_t data;
  uint8_t user_register;
  uint8_t user_bit;
  do
  {
    //! Displays the Read/Write Registers menu
    Serial.print(F("\nRead/Write Registers\n\n"));
    Serial.print(F("  1-Read All Registers\n"));
    Serial.print(F("  2-Read Single Register\n"));
    Serial.print(F("  3-Write Single Register\n"));
    Serial.print(F("  4-Set Bit\n"));
    Serial.print(F("  5-Clear Bit\n"));
    Serial.print(F("  6-Clear IRQ\n"));
    Serial.print(F("  7-Send Hard Reset Command\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
    {
      Serial.print(F("m\n"));
    }
    else
      Serial.println(user_command);                         // Print user command

    //! Reads or writes to a LTC3676 and prints result.
    switch (user_command)
    {
      case 1:
        // Read every register and print it's data.
        ack |= LTC3676_print_all_registers(i2c_address);
        break;
      case 2:
        // Read a single register and print it's data.
        Serial.print(F("\nAddress (in hex with '0x' prefix) of register to read: "));
        user_register = read_int();
        Serial.print("0x");
        Serial.println(user_register, HEX);
        if (user_register < 0x01 || user_register > 0x17)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        ack |= LTC3676_register_read(i2c_address, user_register, &data);
        Serial.print("Register data: 0x");
        Serial.println(data, HEX);
        break;
      case 3:
        // Write a byte to a chosen register.
        Serial.print(F("\nAddress (in hex with '0x' prefix) of register to write: "));
        user_register = read_int();
        Serial.print("0x");
        Serial.println(user_register, HEX);
        if (user_register == 0x1E)
        {
          Serial.println(F("  Hard Reset command sent."));
          ack |= LTC3676_register_write(i2c_address, user_register, 0x01);
          break;
        }
        else if (user_register == 0x1F)
        {
          Serial.println(F("  Clear IRQ command sent."));
          ack |= LTC3676_register_write(i2c_address, user_register, 0x01);
          break;
        }
        else if (user_register < 0x01 || user_register > 0x17)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.print(F("Data (in hex with '0x' prefix) to write: "));
        data = read_int();
        Serial.println(data, HEX);
        ack |= LTC3676_register_write(i2c_address, user_register, data);
        Serial.print("0x");
        Serial.print(data, HEX);
        Serial.print(" written to register 0x");
        Serial.println(user_register, HEX);
        break;
      case 4:
        // Set a single bit within a chosen register.
        Serial.print(F("\nAddress (in hex with '0x' prefix) of register: "));
        user_register = read_int();
        Serial.print("0x");
        Serial.println(user_register, HEX);
        if (user_register < 0x01 || user_register > 0x17)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.print(F("Bit position (0-7) to set: "));
        data = read_int();
        if (data < 0 || data > 7)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.println(data, DEC);
        ack |= LTC3676_bit_set(i2c_address, user_register, data);
        Serial.print("Bit set. Register data is now 0x");
        ack |= LTC3676_register_read(i2c_address, user_register, &data);
        Serial.println(data, HEX);
        break;
      case 5:
        // Clear a single bit within a chosen register.
        Serial.print(F("\nAddress (in hex with '0x' prefix) of register: "));
        user_register = read_int();
        Serial.print("0x");
        Serial.println(user_register, HEX);
        if (user_register < 0x01 || user_register > 0x17)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.print(F("Bit position (0-7) to clear: "));
        data = read_int();
        if (data < 0 || data > 7)
        {
          Serial.println(F("Invalid input."));
          break;
        }
        Serial.println(data, DEC);
        ack |= LTC3676_bit_clear(i2c_address, user_register, data);
        Serial.print("Bit cleared. Register data is now 0x");
        ack |= LTC3676_register_read(i2c_address, user_register, &data);
        Serial.println(data, HEX);
        break;
      case 6:
        // Clear IRQ register
        Serial.println(F("\n  Clear IRQ command sent."));
        ack |= LTC3676_register_write(i2c_address, LTC3676_REG_CLIRQ, 0x01);
        break;
      case 7:
        // Initiate a hard reset
        Serial.println(F("\n  Hard Reset command sent."));
        ack |= LTC3676_register_write(i2c_address, LTC3676_REG_HRST, 0x01);
        break;
      default:
        if (user_command != 'm')
          Serial.println("Invalid Selection");
        break;
    }
  }
  while ((user_command != 'm') && (ack != 1));
  return(ack);
}

int8_t menu_2_regulator_settings()
{
  int8_t ack=0;
  uint8_t user_command;
  uint8_t data;
  uint8_t user_register;
  uint8_t user_int;
  uint8_t user_buck;
  uint8_t user_ldo;
  int8_t user_char;
  do
  {
    //! Displays the Regulator Settings menu
    Serial.print(F("\nRegulator Settings\n\n"));
    Serial.print(F("  1-Enable/Disable Bucks\n"));
    Serial.print(F("  2-Enable/Disable LDOs\n"));
    Serial.print(F("  3-Set Buck Output Voltage\n"));
    Serial.print(F("  4-Set Buck Feedback Reference\n"));
    Serial.print(F("  5-Select Buck Reference\n"));
    Serial.print(F("  6-Set Buck Switching Mode\n"));
    Serial.print(F("  7-Set Start-Up Mode (300mV check)\n"));
    Serial.print(F("  8-Set PGOOD Slewing Mask\n"));
    Serial.print(F("  9-Exit Software Control Mode\n"));
    if (board_option == 'B')
      Serial.print(F("  10-Set LDO4 Output Voltage\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
      Serial.print(F("m\n"));
    else
      Serial.println(user_command);                         // Print user command

    //! Reads or writes to a LTC3676 and prints result.
    switch (user_command)
    {
      case 1:
        //Enter software control mode
        if (!LTC3676_bit_is_set(i2c_address, LTC3676_REG_CNTRL, LTC3676_SOFTWARE_CNTRL))
        {
          Serial.print(F("\n********** Note: LTC3676 is now in Sofware Control Mode **********\n"));
          Serial.print(F("***************Select Option 9 to resume Pin Control *************\n"));
          ack |= LTC3676_bit_set(i2c_address, LTC3676_REG_CNTRL, LTC3676_SOFTWARE_CNTRL);
        }
        // Enable/Disable Bucks
        Serial.print(F("\nSelect Buck(1-4, 5=all): "));
        user_buck = read_int();
        Serial.println(user_buck, DEC);
        if (user_buck < 1 || user_buck > 5)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.println(F("0=Disable, 1=Enable"));
        Serial.print(F("Enter selection: "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int > 1)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        if (user_int == 1)
          ack |= LTC3676_bit_set(i2c_address, LTC3676_REG_CNTRL, LTC3676_PWR_ON);
        if (user_buck == 5)
        {
          ack |= LTC3676_bit_write(i2c_address, LTC3676_REG_BUCK1, 7, user_int);
          ack |= LTC3676_bit_write(i2c_address, LTC3676_REG_BUCK2, 7, user_int);
          ack |= LTC3676_bit_write(i2c_address, LTC3676_REG_BUCK3, 7, user_int);
          ack |= LTC3676_bit_write(i2c_address, LTC3676_REG_BUCK4, 7, user_int);
        }
        else
          ack |= LTC3676_bit_write(i2c_address, user_buck, 7, user_int);
        Serial.println(F("Done."));
        break;
      case 2:
        //Enter software control mode
        if (!LTC3676_bit_is_set(i2c_address, LTC3676_REG_CNTRL, LTC3676_SOFTWARE_CNTRL))
        {
          Serial.print(F("\n********** Note: LTC3676 is now in Sofware Control Mode **********\n"));
          Serial.print(F("***************Select Option 8 to resume Pin Control *************\n"));
          ack |= LTC3676_bit_set(i2c_address, LTC3676_REG_CNTRL, LTC3676_SOFTWARE_CNTRL);
        }
        // Enable/Disable LDOs
        Serial.print(F("\nSelect LDO(2-4, 5=all): "));
        user_ldo = read_int();
        Serial.println(user_ldo, DEC);
        if (user_ldo < 2 || user_ldo > 5)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.println(F("0=Disable, 1=Enable"));
        Serial.print(F("Enter selection: "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int > 1)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        if (user_int == 1)
          ack |= LTC3676_bit_set(i2c_address, LTC3676_REG_CNTRL, LTC3676_PWR_ON);
        if (user_ldo == 5)
        {
          ack |= LTC3676_bit_write(i2c_address, LTC3676_REG_LDOA, 2, user_int);
          ack |= LTC3676_bit_write(i2c_address, LTC3676_REG_LDOA, 5, user_int);
          ack |= LTC3676_bit_write(i2c_address, LTC3676_REG_LDOB, 2, user_int);
        }
        else if (user_ldo == 2)
          ack |= LTC3676_bit_write(i2c_address, LTC3676_REG_LDOA, 2, user_int);
        else if (user_ldo == 3)
          ack |= LTC3676_bit_write(i2c_address, LTC3676_REG_LDOA, 5, user_int);
        else if (user_ldo == 4)
          ack |= LTC3676_bit_write(i2c_address, LTC3676_REG_LDOB, 2, user_int);
        Serial.println(F("Done."));
        break;
      case 3:
        // Set buck output voltage in mV
        float user_output;
        float new_output;
        Serial.print(F("\nSelect Buck(1-4): "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 1 || user_int > 4)
        {
          Serial.println(F("Invalid input."));
          break;
        }
        Serial.print(F("Select Reference (A=DVB"));
        Serial.print(user_int, DEC);
        Serial.print(F("A, B=DVB"));
        Serial.print(user_int, DEC);
        Serial.print(F("B): "));
        user_char = read_char();
        Serial.write(user_char);
        if (user_char == 'A' || user_char == 'a')
          user_register = (user_int*2) + 8;    //Converts selection to appropriate register address.
        else if (user_char == 'B' || user_char == 'b')
          user_register = (user_int*2) + 9;    //Converts selection of appropriate register address.
        else
        {
          Serial.println(F("\nInvalid input."));
          break;
        }
        Serial.print(F("\nPotential output voltage range: "));
        Serial.print(LTC3676_buck_vout_min(user_int), 0);
        Serial.print(F("mV to "));
        Serial.print(LTC3676_buck_vout_max(user_int), 0);
        Serial.print(F("mV."));
        Serial.print(F("\nNew output voltage in mV: "));
        user_output = read_float();
        Serial.println(user_output, 0);
        if (user_output < LTC3676_buck_vout_min(user_int) | user_output > LTC3676_buck_vout_max(user_int))
        {
          Serial.println(F("\nInvalid input."));
          break;
        }
        new_output = LTC3676_set_buck_output_voltage(i2c_address, user_register, user_output);
        Serial.print(F("Output voltage set to "));
        Serial.print(new_output, 0);
        Serial.println(F("mV."));
        Serial.print(F("New Feedback Reference Bits: 0x"));
        ack |= LTC3676_register_read(i2c_address, user_register, &data);
        Serial.println(data & 0x1F, HEX);
        break;
      case 4:
        // Set buck feedback reference voltage in mV
        float user_reference;
        float new_reference;
        Serial.print(F("\nSelect Buck(1-4): "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 1 || user_int > 4)
        {
          Serial.println(F("Invalid input."));
          break;
        }
        Serial.print(F("Select Reference (A=DVB"));
        Serial.print(user_int);
        Serial.print(F("A, B=DVB"));
        Serial.print(user_int);
        Serial.print(F("B): "));
        user_char = read_char();
        Serial.write(user_char);
        if (user_char == 'A' || user_char == 'a')
          user_register = (user_int*2) + 8;    //Converts selection to appropriate register address.
        else if (user_char == 'B' || user_char == 'b')
          user_register = (user_int*2) + 9;    //Converts selection of appropriate register address.
        else
        {
          Serial.println(F("\nInvalid input."));
          break;
        }
        Serial.print(F("\nNew feedback reference input in mV (412.5-800): "));
        user_reference = read_float();
        Serial.println(user_reference);
        if (user_reference < 412.5 | user_reference > 800)
        {
          Serial.println(F("\nInvalid input."));
          break;
        }
        new_reference = LTC3676_set_buck_fb_ref(i2c_address, user_register, user_reference);
        Serial.print(F("Feedback reference input set to "));
        Serial.print(new_reference, 0);
        Serial.println(F("mV"));
        Serial.print(F("New Feedback Reference Bits: 0x"));
        ack |= LTC3676_register_read(i2c_address, user_register, &data);
        Serial.println(data & 0x1F, HEX);
        break;
      case 5:
        // Select buck reference (A or B)
        Serial.print(F("\nSelect Buck(1-4, 5=all): "));
        user_buck = read_int();
        Serial.println(user_buck, DEC);
        if (user_buck < 1 || user_buck > 5)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.print(F("Select Reference (A or B): "));
        user_char = read_char();
        Serial.write(user_char);
        if (user_char == 'A' || user_char == 'a' || user_char == 'B' || user_char == 'b')
        {
          if (user_buck == 5)
            ack |= LTC3676_select_buck_reference(i2c_address, 0xFF, user_char);
          else
            ack |= LTC3676_select_buck_reference(i2c_address, user_buck, user_char);
          Serial.println(F("\nDone."));
          break;
        }
        Serial.println(F("\n  Invalid input."));
        break;
      case 6:
        // Set buck switching mode
        Serial.print(F("\nSelect Buck(1-4, 5=all): "));
        user_register = read_int();
        Serial.println(user_register, DEC);
        if (user_register < 1 || user_register > 5)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.print(F("Select mode (0=Pulse Skipping, 1=Burst, 2=Forced Continuous): "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 0 || user_int > 2)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        if (user_register == 5)
          ack |= LTC3676_set_buck_mode(i2c_address, 0xFF, user_int);
        else
          ack |= LTC3676_set_buck_mode(i2c_address, user_register, user_int);
        Serial.println(F("Switching mode(s) set."));
        break;
      case 7:
        // Sets the startup mode for all bucks
        Serial.print(F("\nSelect Buck(1-4, 5=all): "));
        user_buck = read_int();
        Serial.println(user_buck, DEC);
        if (user_buck < 1 || user_buck > 5)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.println(F("0=Enable at any output voltage, 1=Enable only if output <300mV"));
        Serial.print(F("Enter selection: "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 0 || user_int > 1)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        if (user_buck == 5)
          ack |= LTC3676_set_startup_mode(i2c_address, 0xFF, user_int);
        else
          ack |= LTC3676_set_startup_mode(i2c_address, user_buck, user_int);
        Serial.println(F("Start-up mode(s) set."));
        break;
      case 8:
        // Sets PGOOD masks bit for all bucks.
        Serial.print(F("\nSelect Buck(1-4, 5=all): "));
        user_buck = read_int();
        Serial.println(user_buck, DEC);
        if (user_buck < 1 || user_buck > 5)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.println(F("0=PGOOD low when slewing, 1=PGOOD not forced low when slewing"));
        Serial.print(F("Enter selection: "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 0 || user_int > 1)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        if (user_buck == 5)
          ack |= LTC3676_set_buck_pgood_mask(i2c_address, 0xFF, user_int);
        else
          ack |= LTC3676_set_buck_pgood_mask(i2c_address, user_buck, user_int);
        Serial.println(F("PGOOD Mask bit(s) set."));
        break;
      case 9:
        // Exit software control mode
        ack |= LTC3676_bit_clear(i2c_address, LTC3676_REG_CNTRL, LTC3676_SOFTWARE_CNTRL);
        break;
      case 10:
        // Set LDO4 Output Voltage for DC1976A-B
        if (board_option == 'B')
        {
          Serial.print(F("Select LDO4 Voltage (0=1.2V, 1=2.5V, 2=2.8V, 3=3.0V): "));
          user_int = read_int();
          Serial.println(user_int, DEC);
          if (user_int > 3)
          {
            Serial.println(F("  Invalid input."));
            break;
          }
          ack |= LTC3676_1_set_ldo4_voltage(i2c_address, user_int);
          Serial.println(F("LDO4 Voltage Set."));
          break;
        }
        Serial.println(" Invalid Selection");
        break;
      default:
        if (user_command != 'm')
          Serial.println(" Invalid Selection");
        break;
    }
  }
  while ((user_command != 'm') && (ack != 1));
  return (ack);
}

int8_t menu_3_control_settings()
{
  int8_t ack=0;
  uint8_t user_command;
  uint8_t data;
  uint8_t user_register;
  uint8_t user_int;
  int8_t user_char;
  do
  {
    //! Displays the Control Settings menu
    Serial.print(F("\nControl Settings\n\n"));
    Serial.print(F("  1-Set UV Warning Threshold\n"));
    Serial.print(F("  2-Set Over-Temperature Warning Level\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              // Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
      Serial.print(F("m\n"));
    else
      Serial.println(user_command);                         // Print user command

    //! Reads or writes to a LTC3676 and prints result.
    switch (user_command)
    {
      case 1:
        // Set UV warning threshold
        float user_threshold;
        Serial.print(F("\nEnter new UV warning threshold in Volts (2.7 - 3.4): "));
        user_threshold = read_float();
        Serial.println(user_threshold, 1);
        if (user_threshold < 2.7 | user_threshold > 3.4)
        {
          Serial.println(F("Invalid input."));
          break;
        }
        ack = LTC3676_set_uv_warning_threshold(i2c_address, user_threshold);
        Serial.print(F("UV warning threshold set to "));
        Serial.print(user_threshold,1);
        Serial.println(F("V"));
        break;
      case 2:
        // Set over-temperature warning level
        Serial.print(F("Enter warning level in deg C below over-temperature shutdown(10,20,30,40): "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 10 | user_int > 40)
        {
          Serial.println(F("Invalid input."));
          break;
        }
        ack = LTC3676_set_overtemp_warning_level(i2c_address, user_int);
        Serial.print(F("Warning level set to "));
        Serial.print(((uint8_t)((user_int + 5)/10))*10);
        Serial.println(F("C below Over-temperature"));
        break;
      default:
        if (user_command != 'm')
          Serial.println(" Invalid Selection");
        break;
    }
  }
  while ((user_command != 'm') && (ack != 1));
  return (ack);
}


int8_t menu_4_sequencing(uint8_t *reg_phase)
{
  int8_t ack=0;
  uint8_t user_command;
  uint8_t data;
  uint8_t user_sequence;
  uint8_t user_int;
  uint8_t reg_map[6];
  uint8_t reg_address[7] = {1,2,3,4,5,5,6};
  uint8_t reg_bit_position[7] = {7,7,7,7,2,5,2};
  int count;

  //Enter software control mode
  if (!LTC3676_bit_is_set(i2c_address, LTC3676_REG_CNTRL, LTC3676_SOFTWARE_CNTRL))
  {
    Serial.print(F("\n********** Note: LTC3676 is now in Sofware Control Mode **********\n"));
    Serial.print(F("************** Select Option 8 to resume Pin Control *************\n"));
    ack |= LTC3676_bit_set(i2c_address, LTC3676_REG_CNTRL, LTC3676_SOFTWARE_CNTRL);
  }
  do
  {
    //! Displays the Startup Sequencing menu

    Serial.print(F("\nPowerup Sequencing\n\n"));
    Serial.print(F("  1-Set Power-up Sequence\n"));
    Serial.print(F("  2-Print Power-up Sequence\n"));
    Serial.print(F("  3-Set Buck Power-down Sequence\n"));
    Serial.print(F("  4-Set LDO Power-down Sequence\n"));
    Serial.print(F("  5-Print Power-down Sequence\n"));
    Serial.print(F("  6-Power-up Regulators\n"));
    Serial.print(F("  7-Power-down Regulators\n"));
    Serial.print(F("  8-Exit Software Control Mode\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
      Serial.print(F("m\n"));
    else
      Serial.println(user_command);                         // Print user command

    switch (user_command)
    {
      case 1:
        Serial.println(F("\nRegulators will power-up in 4 phases"));
        Serial.println(F("Select a phase for each"));
        Serial.println(F("1=First phase, 2=Second phase, 3=Third phase, 4=Fourth phase, 0=Don't enable"));
        Serial.print(F("Buck1: "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 5)
          reg_phase[0] = user_int;
        Serial.print(F("Buck2: "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 5)
          reg_phase[1] = user_int;
        Serial.print(F("Buck3: "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 5)
          reg_phase[2] = user_int;
        Serial.print(F("Buck4: "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 5)
          reg_phase[3] = user_int;
        Serial.print(F("LDO2: "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 5)
          reg_phase[4] = user_int;
        Serial.print(F("LDO3: "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 5)
          reg_phase[5] = user_int;
        Serial.print(F("LDO4: "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 5)
          reg_phase[6] = user_int;
        Serial.print(F("Enter delay between phases in milliseconds: "));
        delay_ms = read_float();
        if (delay_ms >= 0 && delay_ms <= 10000)
          Serial.println(delay_ms, 1);
        else
          Serial.println(F("Values less than 0 or greater than 10s not allowed."));
        break;
      case 2:
        Serial.print(F("\nBuck1: "));
        Serial.println(reg_phase[0]);
        Serial.print(F("Buck2: "));
        Serial.println(reg_phase[1]);
        Serial.print(F("Buck3: "));
        Serial.println(reg_phase[2]);
        Serial.print(F("Buck4: "));
        Serial.println(reg_phase[3]);
        Serial.print(F("LDO2:  "));
        Serial.println(reg_phase[4]);
        Serial.print(F("LDO3:  "));
        Serial.println(reg_phase[5]);
        Serial.print(F("LDO4:  "));
        Serial.println(reg_phase[6]);
        Serial.print(F("Delay: "));
        Serial.print(delay_ms, 1);
        Serial.println(F("ms"));
        break;
      case 3:
        // Set Buck Power-down Sequence
        Serial.print(F("\nSelect Buck(1-4): "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 1 || user_int > 4)
        {
          Serial.println(F("Invalid input."));
          break;
        }
        Serial.println(F("0=with WAKE, 1=WAKE+100ms, 2=WAKE+200ms, 3=WAKE+300ms"));
        Serial.print(F("Enter selection: "));
        user_sequence = read_int();
        Serial.println(user_sequence, DEC);
        if (user_sequence > 3)
        {
          Serial.println(F("Invalid input."));
          break;
        }
        ack |= LTC3676_set_buck_sequence_down(i2c_address, user_int, user_sequence);
        Serial.print(F("Sequence down set for Buck"));
        Serial.println(user_int, DEC);
        break;
      case 4:
        // Set LDO Power-down Sequence
        Serial.print(F("\nSelect LDO(2-4): "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 2 || user_int > 4)
        {
          Serial.println(F("Invalid input."));
          break;
        }
        Serial.println(F("0=with WAKE, 1=WAKE+100ms, 2=WAKE+200ms, 3=WAKE+300ms"));
        Serial.print(F("Enter selection: "));
        user_sequence = read_int();
        Serial.println(user_sequence, DEC);
        if (user_sequence > 3)
        {
          Serial.println(F("Invalid input."));
          break;
        }
        ack |= LTC3676_set_ldo_sequence_down(i2c_address, user_int, user_sequence);
        Serial.print(F("Sequence down set for LDO"));
        Serial.println(user_int, DEC);
        break;
      case 5:
        //Print power-down sequence
        ack |= LTC3676_register_read(i2c_address, LTC3676_REG_SQD1, &data);
        Serial.print(F("\nBuck1: "));
        Serial.println(data & LTC3676_BUCK1_SEQ_MASK, DEC);
        Serial.print(F("Buck2: "));
        Serial.println((data & LTC3676_BUCK2_SEQ_MASK)>>2, DEC);
        Serial.print(F("Buck3: "));
        Serial.println((data & LTC3676_BUCK3_SEQ_MASK)>>4, DEC);
        Serial.print(F("Buck4: "));
        Serial.println((data & LTC3676_BUCK4_SEQ_MASK)>>6, DEC);
        ack |= LTC3676_register_read(i2c_address, LTC3676_REG_SQD2, &data);
        Serial.print(F("LDO2:  "));
        Serial.println(data & LTC3676_LDO2_SEQ_MASK, DEC);
        Serial.print(F("LDO3:  "));
        Serial.println((data & LTC3676_LDO3_SEQ_MASK)>>2, DEC);
        Serial.print(F("LDO4:  "));
        Serial.println((data & LTC3676_LDO4_SEQ_MASK)>>4, DEC);
        Serial.print(F("Delay between phases(fixed): 100ms\n"));
        break;
      case 6:
        //Power-up regulators
        ack |= LTC3676_bit_set(i2c_address, LTC3676_REG_CNTRL, LTC3676_PWR_ON);
        delay(100);
        read_reg_map(reg_map);
        //Start Phase 1 Regulators

        for (count=0; count<7; count++)
        {
          if (reg_phase[count] == 1)
            ack |= LTC3676_register_write(i2c_address, reg_address[count], ((0x01<<reg_bit_position[count]) | reg_map[reg_address[count]-1]));
        }

        delay(delay_ms);


        //Start Phase 2 Regulators

        for (count=0; count<7; count++)
        {
          if (reg_phase[count] == 2)
            ack |= LTC3676_register_write(i2c_address, reg_address[count], ((0x01<<reg_bit_position[count]) | reg_map[reg_address[count]-1]));
        }

        delay(delay_ms);

        //Start Phase 3 Regulators



        for (count=0; count<7; count++)
        {
          if (reg_phase[count] == 3)
            ack |= LTC3676_register_write(i2c_address, reg_address[count], ((0x01<<reg_bit_position[count]) | reg_map[reg_address[count]-1]));
        }

        delay(delay_ms);

        //Start Phase 4 Regulators


        for (count=0; count<7; count++)
        {
          if (reg_phase[count] == 4)
            ack |= LTC3676_register_write(i2c_address, reg_address[count], ((0x01<<reg_bit_position[count]) | reg_map[reg_address[count]-1]));
        }

        break;
      case 7:
        // Power-down by clearing the PWR_ON bit
        Serial.print(F("\n******** Note: On DC1976, set SW2 for uC control of PWR_ON *******\n"));
        Serial.print(F("*** Otherwise, PWR_ON bit may not initiate power-down sequence ***\n\n"));
        ack |= LTC3676_bit_clear(i2c_address, LTC3676_REG_CNTRL, LTC3676_PWR_ON);
        break;
      case 8:
        // Exit software control mode
        ack |= LTC3676_bit_clear(i2c_address, LTC3676_REG_CNTRL, LTC3676_SOFTWARE_CNTRL);
        break;
      default:
        if (user_command != 'm')
          Serial.println(" Invalid Selection");
        break;
    }
  }
  while ((user_command != 'm') && (ack != 1));
  return (ack);
}

//! Reads the first 6 registers and stores them in an array
void read_reg_map(uint8_t *reg_map_array)
{
  uint8_t count;
  uint8_t data;
  int8_t ack;
  for (count=0; count<6; count++)
  {
    ack |= LTC3676_register_read(i2c_address, count+1, &data);
    reg_map_array[count] = data;
  }
}


