/*!
Linear Technology DC1558 and DC1808 Demonstration Boards.
LTC3589: 8-Output Regulator with Sequencing and I2C

@verbatim

SETUP:
   Set the terminal baud rate to 115200 and select the newline terminator.
   Set all switches in SW1 to their default position.
   Tie PWR_ON high by connecting it to VIN.
   Power VIN from an external power supply.

NOTE:
   For proper IRQ LED behavior, move resistor R39 to the optional R38 position
   on the DC2026B.

@endverbatim

http://www.linear.com/product/LTC3589

http://www.linear.com/product/LTC3589#demoboards


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
    @ingroup LTC3589
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC3589.h"

// Global variables
static uint8_t demo_board_connected;     //!< Set to 1 if the board is connected
static uint8_t i2c_address;              //!< I2C address set for all LTC3589 options
static char board_option;                //!< Demo board option of the attached demo board
char demo_name_1[] = "DC1558";           //!< Demo Board Name stored in QuikEval EEPROM
char demo_name_2[] = "DC1808";           //!< Demo Board Name stored in QuikEval EEPROM
uint8_t reg_phase[7] = {1,1,1,1,1,1,1};  //!< Power-up sequence phases for every regulator output
float delay_ms = 0;                      //!< Delay between power-up phases
uint8_t reg_read_list[15] = {0x07,0x010,0x12,0x20,0x23,0x24,0x25,0x26,0x27,0x29,0x2A,0x32,0x33,0x02,0x13};
uint8_t reg_write_list[14] = {0x07,0x010,0x12,0x20,0x21,0x23,0x24,0x25,0x26,0x27,0x29,0x2A,0x32,0x33};

void setup()
//  Setup the program
{
  quikeval_I2C_init();          //! Initializes Linduino I2C port.
  quikeval_I2C_connect();       //! Connects I2C port to the QuikEval connector
  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  demo_board_connected = discover_demo_boards(demo_name_1, demo_name_2);        //! Checks if correct demo board is connected.
  if (demo_board_connected)
  {
    i2c_address = LTC3589_I2C_ADDRESS;
    print_prompt();
  }
  else
  {
    print_warning_prompt();
    demo_board_connected = true;
    i2c_address = LTC3589_I2C_ADDRESS;
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
          ack |= menu_3_sequencing(reg_phase);           // Sequencing menu
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

//! Prints the Read/Write Registers menu and handles the user response.
//! @return State of the acknowledge bit after menu selection. 0=valid selection, 1=invalid selection.
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
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
    {
      Serial.print(F("m\n"));
    }
    else
      Serial.println(user_command);                         // Print user command

    //! Reads or writes to a LTC3589 and prints result.
    switch (user_command)
    {
      case 1:
        // Read every register and print it's data.
        ack |= LTC3589_print_all_registers(i2c_address);
        break;
      case 2:
        // Read a single register and print it's data.
        Serial.print(F("\nAddress (in hex with '0x' prefix) of register to read: "));
        user_register = read_int();
        Serial.print("0x");
        Serial.println(user_register, HEX);
        if (!valid_register(user_register, reg_read_list, sizeof(reg_read_list)))
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        ack |= LTC3589_register_read(i2c_address, user_register, &data);
        Serial.print("Register data: 0x");
        Serial.println(data, HEX);
        break;
      case 3:
        // Write a byte to a chosen register.
        Serial.print(F("\nAddress (in hex with '0x' prefix) of register to write: "));
        user_register = read_int();
        Serial.print("0x");
        Serial.println(user_register, HEX);
        if (user_register == 0x21)
        {
          Serial.println(F("  Clear IRQ command sent."));
          ack |= LTC3589_register_write(i2c_address, user_register, 0x01);
          break;
        }
        else if (!valid_register(user_register, reg_write_list, sizeof(reg_write_list)))
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.print(F("Data (in hex with '0x' prefix) to write: "));
        data = read_int();
        Serial.println(data, HEX);
        ack |= LTC3589_register_write(i2c_address, user_register, data);
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
        if (!valid_register(user_register, reg_write_list, sizeof(reg_write_list)))
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
        ack |= LTC3589_bit_set(i2c_address, user_register, data);
        Serial.print("Bit set. Register data is now 0x");
        ack |= LTC3589_register_read(i2c_address, user_register, &data);
        Serial.println(data, HEX);
        break;
      case 5:
        // Clear a single bit within a chosen register.
        Serial.print(F("\nAddress (in hex with '0x' prefix) of register: "));
        user_register = read_int();
        Serial.print("0x");
        Serial.println(user_register, HEX);
        if (!valid_register(user_register, reg_write_list, sizeof(reg_write_list)))
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
        ack |= LTC3589_bit_clear(i2c_address, user_register, data);
        Serial.print("Bit cleared. Register data is now 0x");
        ack |= LTC3589_register_read(i2c_address, user_register, &data);
        Serial.println(data, HEX);
        break;
      case 6:
        // Clear IRQ register
        Serial.println(F("\n  Clear IRQ command sent."));
        ack |= LTC3589_register_write(i2c_address, LTC3589_REG_CLIRQ, 0x01);
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

//! Prints the Regulator Settings menu and handles the user response.
//! @return State of the acknowledge bit after menu selection. 0=valid selection, 1=invalid selection.
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
    Serial.print(F("  1-Enable/Disable Switchers\n"));
    Serial.print(F("  2-Enable/Disable LDOs\n"));
    Serial.print(F("  3-Set Buck Output Voltage\n"));
    Serial.print(F("  4-Set Buck Feedback Reference\n"));
    Serial.print(F("  5-Select Buck Reference\n"));
    Serial.print(F("  6-Set LDO2 Output Voltage\n"));
    Serial.print(F("  7-Set LDO2 Feedback Reference\n"));
    Serial.print(F("  8-Select LDO2 Reference\n"));
    Serial.print(F("  9-Set Buck Switching Mode\n"));
    Serial.print(F(" 10-Set Buck-Boost Switching Mode\n"));
    Serial.print(F(" 11-Set Start-Up Mode (300mV check)\n"));
    Serial.print(F(" 12-Set PGOOD Slewing Mask Bits\n"));
    Serial.print(F(" 13-Set LDO4 Voltage\n"));
    Serial.print(F(" 14-Set Switch DV/DT Control\n"));
    Serial.print(F(" 15-Set Regulator Slew Rate\n"));
    Serial.print(F(" 16-Exit Software Control Mode\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
      Serial.print(F("m\n"));
    else
      Serial.println(user_command);                         // Print user command

    //! Reads or writes to a LTC3589 and prints result.
    switch (user_command)
    {
      case 1:
        //Enter software control mode
        if (!LTC3589_bit_is_set(i2c_address, LTC3589_REG_OVEN, LTC3589_SOFTWARE_CNTRL))
        {
          Serial.print(F("\n********** Note: LTC3589 is now in Sofware Control Mode **********\n"));
          Serial.print(F("***************Select Option 16 to resume Pin Control ************\n"));
          ack |= LTC3589_bit_set(i2c_address, LTC3589_REG_OVEN, LTC3589_SOFTWARE_CNTRL);
        }
        // Enable/Disable Bucks
        Serial.print(F("\nSelect Regulator(1-3=Bucks, 4=Buck-boost, 5=All): "));
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
        if (user_buck == 5)
        {
          ack |= LTC3589_bit_write(i2c_address, LTC3589_REG_OVEN, LTC3589_EN1, user_int);
          ack |= LTC3589_bit_write(i2c_address, LTC3589_REG_OVEN, LTC3589_EN2, user_int);
          ack |= LTC3589_bit_write(i2c_address, LTC3589_REG_OVEN, LTC3589_EN3, user_int);
          ack |= LTC3589_bit_write(i2c_address, LTC3589_REG_OVEN, LTC3589_EN4, user_int);
        }
        else
          ack |= LTC3589_bit_write(i2c_address, LTC3589_REG_OVEN, user_buck-1, user_int);
        Serial.println(F("Done."));
        break;
      case 2:
        //Enter software control mode
        if (!LTC3589_bit_is_set(i2c_address, LTC3589_REG_OVEN, LTC3589_SOFTWARE_CNTRL))
        {
          Serial.print(F("\n********** Note: LTC3589 is now in Sofware Control Mode **********\n"));
          Serial.print(F("***************Select Option 16 to resume Pin Control ************\n"));
          ack |= LTC3589_bit_set(i2c_address, LTC3589_REG_OVEN, LTC3589_SOFTWARE_CNTRL);
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
        if (user_ldo == 5)
        {
          ack |= LTC3589_bit_write(i2c_address, LTC3589_REG_OVEN, LTC3589_EN_LDO2, user_int);
          ack |= LTC3589_bit_write(i2c_address, LTC3589_REG_OVEN, LTC3589_EN_LDO3, user_int);
          ack |= LTC3589_bit_write(i2c_address, LTC3589_REG_OVEN, LTC3589_EN_LDO4, user_int);
        }
        ack |= LTC3589_bit_write(i2c_address, LTC3589_REG_OVEN, user_ldo+2, user_int);
        Serial.println(F("Done."));
        break;
      case 3:
        // Set buck output voltage in mV
        float user_output;
        float new_output;
        Serial.print(F("\nSelect Buck(1-3): "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 1 || user_int > 3)
        {
          Serial.println(F("Invalid input."));
          break;
        }
        if (user_int == 1)
          user_register = LTC3589_REG_B1DTV1;    //Converts selection to appropriate register address.
        else if (user_int == 2)
          user_register = LTC3589_REG_B2DTV1;    //Converts selection to appropriate register address.
        else if (user_int == 3)
          user_register = LTC3589_REG_B3DTV1;    //Converts selection to appropriate register address.
        Serial.print(F("Select Reference (1=B"));
        Serial.print(user_int, DEC);
        Serial.print(F("DTV1, 2=B"));
        Serial.print(user_int, DEC);
        Serial.print(F("DTV2): "));
        user_char = read_char();
        Serial.write(user_char);
        if (user_char < '1' || user_char > '2')
        {
          Serial.println(F("\nInvalid input."));
          break;
        }
        else if (user_char == '2')
          user_register += 1;    //Converts selection of appropriate register address.
        Serial.print(F("\nPotential output voltage range: "));
        Serial.print(LTC3589_buck_vout_min(user_int), 0);
        Serial.print(F("mV to "));
        Serial.print(LTC3589_buck_vout_max(user_int), 0);
        Serial.print(F("mV."));
        Serial.print(F("\nNew output voltage in mV: "));
        user_output = read_float();
        Serial.println(user_output, 0);
        if (user_output < LTC3589_buck_vout_min(user_int) | user_output > LTC3589_buck_vout_max(user_int))
        {
          Serial.println(F("\nInvalid input."));
          break;
        }
        new_output = LTC3589_set_buck_output_voltage(i2c_address, user_register, user_output);
        Serial.print(F("Output voltage set to "));
        Serial.print(new_output, 0);
        Serial.println(F("mV"));
        Serial.print(F("New Feedback Reference Bits: 0x"));
        ack |= LTC3589_register_read(i2c_address, user_register, &data);
        Serial.println(data & 0x1F, HEX);
        break;
      case 4:
        // Set buck feedback reference voltage in mV
        float user_reference;
        float new_reference;
        Serial.print(F("\nSelect Buck(1-3): "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 1 || user_int > 3)
        {
          Serial.println(F("Invalid input."));
          break;
        }
        if (user_int == 1)
          user_register = LTC3589_REG_B1DTV1;    //Converts selection to appropriate register address.
        else if (user_int == 2)
          user_register = LTC3589_REG_B2DTV1;    //Converts selection to appropriate register address.
        else if (user_int == 3)
          user_register = LTC3589_REG_B3DTV1;    //Converts selection to appropriate register address.
        Serial.print(F("Select Reference (1=B"));
        Serial.print(user_int);
        Serial.print(F("DTV1, 2=B"));
        Serial.print(user_int);
        Serial.print(F("DTV2): "));
        user_char = read_char();
        Serial.write(user_char);
        if (user_char < '1' || user_char > '2')
        {
          Serial.println(F("\nInvalid input."));
          break;
        }
        else if (user_char == '2')
          user_register += 1;    //Converts selection of appropriate register address.
        Serial.print(F("\nNew feedback reference input in mV (362.5-750): "));
        user_reference = read_float();
        Serial.println(user_reference);
        if (user_reference < 362.5 | user_reference > 750)
        {
          Serial.println(F("\nInvalid input."));
          break;
        }
        new_reference = LTC3589_set_buck_fb_ref(i2c_address, user_register, user_reference);
        Serial.print(F("Feedback reference input set to "));
        Serial.print(new_reference, 0);
        Serial.println(F("mV"));
        Serial.print(F("New Feedback Reference Bits: 0x"));
        ack |= LTC3589_register_read(i2c_address, user_register, &data);
        Serial.println(data & 0x1F, HEX);
        break;
      case 5:
        // Select buck reference (1 or 2)
        Serial.print(F("\nSelect Buck(1-3, 4=all): "));
        user_buck = read_int();
        Serial.println(user_buck, DEC);
        if (user_buck < 1 || user_buck > 4)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.print(F("Select Reference 1 or 2): "));
        user_char = read_char();
        Serial.write(user_char);
        if (user_char == '1' || user_char == '2')
        {
          if (user_buck == 4)
            ack |= LTC3589_select_buck_reference(i2c_address, 0xFF, user_char);
          else
            ack |= LTC3589_select_buck_reference(i2c_address, user_buck, user_char);
          Serial.println(F("\nDone."));
          break;
        }
        Serial.println(F("\n  Invalid input."));
        break;
      case 6:
        // Set LDO2 output voltage in mV
        user_register = LTC3589_REG_L2DTV1;
        Serial.print(F("Select Reference (1=L2DTV1, 2=L2DTV2): "));
        user_char = read_char();
        Serial.write(user_char);
        if (user_char < '1' || user_char > '2')
        {
          Serial.println(F("\nInvalid input."));
          break;
        }
        else if (user_char == '2')
          user_register += 1;    //Converts selection of appropriate register address.
        Serial.print(F("\nPotential output voltage range: "));
        Serial.print(LTC3589_ldo2_vout_min(), 0);
        Serial.print(F("mV to "));
        Serial.print(LTC3589_ldo2_vout_max(), 0);
        Serial.print(F("mV"));
        Serial.print(F("\nNew output voltage in mV: "));
        user_output = read_float();
        Serial.println(user_output, 0);
        if (user_output < LTC3589_ldo2_vout_min() | user_output > LTC3589_ldo2_vout_max())
        {
          Serial.println(F("\nInvalid input."));
          break;
        }
        new_output = LTC3589_set_ldo2_output_voltage(i2c_address, user_register, user_output);
        Serial.print(F("Output voltage set to "));
        Serial.print(new_output, 0);
        Serial.println(F("mV"));
        Serial.print(F("New Feedback Reference Bits: 0x"));
        ack |= LTC3589_register_read(i2c_address, user_register, &data);
        Serial.println(data & 0x1F, HEX);
        break;
      case 7:
        // Set LDO2 feedback reference voltage in mV
        Serial.print(F("Select Reference (1=L2DTV1, 2=L2DTV2): "));
        user_char = read_char();
        Serial.write(user_char);
        if (user_char < '1' || user_char > '2')
        {
          Serial.println(F("\nInvalid input."));
          break;
        }
        else if (user_char == '2')
          user_register += 1;    //Converts selection of appropriate register address.
        Serial.print(F("\nNew feedback reference input in mV (362.5-750): "));
        user_reference = read_float();
        Serial.println(user_reference);
        if (user_reference < 362.5 | user_reference > 750)
        {
          Serial.println(F("\nInvalid input."));
          break;
        }
        new_reference = LTC3589_set_ldo2_fb_ref(i2c_address, user_register, user_reference);
        Serial.print(F("Feedback reference input set to "));
        Serial.print(new_reference, 0);
        Serial.println(F("mV"));
        Serial.print(F("New Feedback Reference Bits: 0x"));
        ack |= LTC3589_register_read(i2c_address, user_register, &data);
        Serial.println(data & 0x1F, HEX);
        break;
      case 8:
        // Select LDO2 reference (1 or 2)
        Serial.print(F("Select Reference 1 or 2): "));
        user_char = read_char();
        Serial.write(user_char);
        if (user_char == '1' || user_char == '2')
        {
          ack |= LTC3589_select_ldo2_reference(i2c_address, user_char);
          Serial.println(F("\nDone."));
          break;
        }
        Serial.println(F("\n  Invalid input."));
        break;
      case 9:
        // Set buck switching mode
        Serial.print(F("\nSelect Buck(1-3, 4=all): "));
        user_register = read_int();
        Serial.println(user_register, DEC);
        if (user_register < 1 || user_register > 4)
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
        if (user_register == 4)
          ack |= LTC3589_set_buck_mode(i2c_address, 0xFF, user_int);
        else
          ack |= LTC3589_set_buck_mode(i2c_address, user_register, user_int);
        Serial.println(F("Switching mode(s) set."));
        break;
      case 10:
        // Set buck-boost switching mode
        Serial.print(F("Select mode (0=Continuous, 1=Burst): "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 0 || user_int > 1)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        ack |= LTC3589_bit_write(i2c_address, LTC3589_REG_SCR1, LTC3589_BUCKBOOST_MODE, user_int);
        Serial.println(F("Switching mode(s) set."));
        break;
      case 11:
        // Sets the startup mode for all bucks
        Serial.println(F("0=Wait for output <300mV to enable, 1=Don't wait & disable discharge resistor"));  //Standard and -1 Option
        // Serial.println(F("0=Enable at any output voltage, 1=Wait for output <300mV to enable"));    //-2 Option
        Serial.print(F("\nSelect start-up mode: "));
        Serial.print(F("Enter selection: "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 0 || user_int > 1)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.println(F("Start-up modes set."));
        break;
      case 12:
        // Sets PGOOD mask bit.
        Serial.println(F("0=PGOOD low when slewing, 1=PGOOD not forced low when slewing"));
        Serial.print(F("Enter selection: "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 0 || user_int > 1)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        ack |= LTC3589_set_pgood_mask(i2c_address, user_int);
        Serial.println(F("PGOOD Mask bits set."));
        break;
      case 13:
        // Set LDO4 Output Voltage
        //if (board_option == 'B')
        //{
        if (demo_board.option == 'A' || demo_board.option == 'B')
        {
          Serial.print(F("Select LDO4 Voltage (0=1.2V, 1=1.8V, 2=2.5V, 3=3.2V): "));
        }
        else
        {
          Serial.print(F("Select LDO4 Voltage (0=2.8V, 1=2.5V, 2=1.8V, 3=3.3V): "));
        }
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int > 3)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        ack |= LTC3589_set_ldo4_voltage(i2c_address, user_int);
        Serial.println(F("LDO4 Voltage Set."));
        break;
        //}
        //Serial.println(" Invalid Selection");
        //break;
      case 14:
        // Set switch DV/DT control
        Serial.print(F("Select DV/DT Control (0=1ns, 1=2ns, 2=4ns, 3=8ns): "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int > 3)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        ack |= LTC3589_set_switch_dvdt_control(i2c_address, user_int);
        Serial.println(F("Switch DV/DT Set."));
        break;
      case 15:
        // Set regulator slew rate
        Serial.print(F("\nSelect Regulator(1-3=Bucks, 4=LDO2, 5=All): "));
        user_buck = read_int();
        Serial.println(user_buck, DEC);
        if (user_buck < 1 || user_buck > 5)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.print(F("Select Slew Rate (0=0.88mV/us, 1=1.75mV/us, 2=3.5mV/us, 3=7mV/us): "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int > 3)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        if (user_buck == 5)
        {
          ack |= LTC3589_set_regulator_slew(i2c_address, 0xFF, user_int);
        }
        else
          ack |= LTC3589_set_regulator_slew(i2c_address, 3<<((user_buck-1)*2), user_int);
        Serial.println(F("Done."));
        break;
      case 16:
        // Exit software control mode
        ack |= LTC3589_bit_clear(i2c_address, LTC3589_REG_OVEN, LTC3589_SOFTWARE_CNTRL);
        Serial.println(F("\nDone."));
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

//! Prints the Powerup Sequencing menu and handles the user response.
//! @return State of the acknowledge bit after menu selection. 0=valid selection, 1=invalid selection.
int8_t menu_3_sequencing(uint8_t *reg_phase)
{
  int8_t ack=0;
  uint8_t user_command;
  uint8_t data;
  uint8_t user_sequence;
  uint8_t user_int;
  uint8_t reg_map[6];
  uint8_t reg_oven;
  uint8_t reg_bit_position[7] = {0,1,2,3,4,5,6};
  int count;

  //Enter software control mode
  if (!LTC3589_bit_is_set(i2c_address, LTC3589_REG_OVEN, LTC3589_SOFTWARE_CNTRL))
  {
    Serial.print(F("\n********** Note: LTC3589 is now in Sofware Control Mode **********\n"));
    Serial.print(F("************** Select Option 5 to resume Pin Control *************\n"));
    ack |= LTC3589_bit_set(i2c_address, LTC3589_REG_OVEN, LTC3589_SOFTWARE_CNTRL);
  }
  do
  {
    //! Displays the Startup Sequencing menu

    Serial.print(F("\nPowerup Sequencing\n\n"));
    Serial.print(F("  1-Set Power-up Sequence\n"));
    Serial.print(F("  2-Print Power-up Sequence\n"));
    Serial.print(F("  3-Power-up Regulators\n"));
    Serial.print(F("  4-Power-down Regulators\n"));
    Serial.print(F("  5-Exit Software Control Mode\n"));
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
        Serial.print(F("BB:    "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 5)
          reg_phase[3] = user_int;
        Serial.print(F("LDO2:  "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 5)
          reg_phase[4] = user_int;
        Serial.print(F("LDO3:  "));
        user_int = read_int();
        Serial.println(user_int, DEC);
        if (user_int < 5)
          reg_phase[5] = user_int;
        Serial.print(F("LDO4:  "));
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
        Serial.print(F("BB:    "));
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
        ack |= LTC3589_register_read(i2c_address, LTC3589_REG_OVEN, &reg_oven);
        //Start Phase 1 Regulators
        for (count=0; count<7; count++)
        {
          if (reg_phase[count] == 1)
          {
            reg_oven |= 0x01<<reg_bit_position[count];
            ack |= LTC3589_register_write(i2c_address, LTC3589_REG_OVEN, reg_oven);
          }
        }
        delay(delay_ms);

        //Start Phase 2 Regulators
        for (count=0; count<7; count++)
        {
          if (reg_phase[count] == 2)
          {
            reg_oven |= 0x01<<reg_bit_position[count];
            ack |= LTC3589_register_write(i2c_address, LTC3589_REG_OVEN, reg_oven);
          }
        }
        delay(delay_ms);

        //Start Phase 3 Regulators
        for (count=0; count<7; count++)
        {
          if (reg_phase[count] == 3)
          {
            reg_oven |= 0x01<<reg_bit_position[count];
            ack |= LTC3589_register_write(i2c_address, LTC3589_REG_OVEN, reg_oven);
          }
        }
        delay(delay_ms);

        //Start Phase 4 Regulators
        for (count=0; count<7; count++)
        {
          if (reg_phase[count] == 4)
          {
            reg_oven |= 0x01<<reg_bit_position[count];
            ack |= LTC3589_register_write(i2c_address, LTC3589_REG_OVEN, reg_oven);
          }
        }
        break;
      case 4:
        // Power-down by clearing all enable bits
        ack |= LTC3589_register_write(i2c_address, LTC3589_REG_OVEN, 0x80);
        break;
      case 5:
        // Exit software control mode
        ack |= LTC3589_bit_clear(i2c_address, LTC3589_REG_OVEN, LTC3589_SOFTWARE_CNTRL);
        user_command = 'm';
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

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n******************************************************************\n"));
  Serial.print(F("* DC1558A Demonstration Program                                  *\n"));
  Serial.print(F("*                                                                *\n"));
  Serial.print(F("* This program demonstrates how to send and receive data from    *\n"));
  Serial.print(F("* the LTC3589 8-Output Regulator with Sequencing and I2C.        *\n"));
  Serial.print(F("*                                                                *\n"));
  Serial.print(F("* Setup:                                                         *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator. *\n"));
  Serial.print(F("* Set all switches in SW1 to their default position.             *\n"));
  Serial.print(F("* Tie PWR_ON high by connecting it to VIN.                       *\n"));
  Serial.print(F("* Power VIN from an external power supply.                       *\n"));
  Serial.print(F("*                                                                *\n"));
  Serial.print(F("* Note:                                                          *\n"));
  Serial.print(F("* For proper IRQ LED behavior, move resistor R39 to the          *\n"));
  Serial.print(F("* optional R38 position on the DC2026B.                          *\n"));
  Serial.print(F("******************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("\n  1-Read/Write Registers\n"));
  Serial.print(F("  2-Regulator Settings\n"));
  Serial.print(F("  3-Sequencing\n"));
  Serial.print(F("\nEnter a command:"));
}

//! Prints a warning that no demo board was not detected.
void print_warning_prompt()
{
  Serial.println(F("\nWarning: Demo board not detected. Linduino will attempt to proceed."));
}

//! Reads and prints the data in every register
//! @return State of the acknowledge bit. 0=acknowledge, 1=no acknowledge.
int8_t LTC3589_print_all_registers(uint8_t i2c_address)
{
  int8_t ack = 0;
  uint8_t data;
  uint8_t i;
  for (i=0; i<sizeof(reg_read_list); i++)
  {
    ack |= LTC3589_register_read(i2c_address, reg_read_list[i], &data);   //! Read register
    Serial.print("Register 0x");
    Serial.print(reg_read_list[i], HEX);
    Serial.print(":\t0x");
    Serial.println(data, HEX);
  }
  return ack;
}

//! Checks to see if a register address is a valid address in this device.
//! @return State of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
boolean valid_register(uint8_t user_register, uint8_t register_array[], uint8_t array_length)
{
  uint8_t i=0;
  for (i=0; i<array_length; i++)
  {
    if (register_array[i] == user_register)
    {
      return true;
    }
  }
  return false;
}

//! Read the ID string from the EEPROM to identify the connected demo board.
//! @return Returns 1 if successful, 0 if not successful
int8_t discover_demo_boards(char *demo_name_1, char *demo_name_2)
{
  int8_t connected;
  connected = 1;
  // read the ID from the serial EEPROM on the board
  // reuse the buffer declared in UserInterface
  if (read_quikeval_id_string(&ui_buffer[0]) == 0) connected = 0;
  // make sure it is the correct demo board
  if (strcmp(demo_board.name, demo_name_1) == 0) connected = 1;
  else if (strcmp(demo_board.name, demo_name_2) == 0) connected = 1;
  else connected = 0;
  if (connected != 0)
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
  }
  else
  {
    Serial.print("Demo board not found, \nfound ");
    Serial.print(demo_board.name);
    Serial.println(" instead. \nConnect the correct demo board, then press the reset button.");
  }
  return(connected);
}

