/*!
Linear Technology DC1880A Demonstration Board

@verbatim

SETUP:
   Set the terminal baud rate to 115200 and select the newline terminator.
   External power supply is required.

USER INPUT DATA FORMAT:
 decimal : 14
 hex     : 0x0E (Data), 0xE (Reg)

@endverbatim

http://www.linear.com/product/LTC2874

http://www.linear.com/product/LTC2874#demoboards


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

IO-Link is a registered trademark of PROFIBUS User Organization (PNO).
*/

/*! @file
    @ingroup LTC2874
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "LT_SPI.h"
#include "LTC2874.h"
#include "QuikEval_EEPROM.h"
#include <Wire.h>
#include <SPI.h>


// Function Prototypes
void print_title();
void print_prompt();
uint8_t reg_test();
void print_warning_prompt();

// Menu selections from print_prompt
void menu_1_cq();
void menu_2_lplus();
void menu_3_wakeup_cq();
void menu_4_24v_mode();
void menu_5_sio_mode();
void menu_6_set_illm();
void menu_7_clear_event();
void menu_8_update_all();
void menu_9_reset();
void menu_10_read_register();
void menu_11_write_register();
void menu_12_read_bit();
void menu_13_write_bit_set();
void menu_14_write_bit_clr();


// Demo Board Name
char demo_name[] = "DC1880";            //!< Demo Board Name stored in QuikEval EEPROM

// ***Global Variables***

uint8_t demo_board_connected;           //!< Set to 1 if the board is connected

//******************************** Initialize Linduino *******************************
//! Initialize Linduino

void setup()
// Setup the Program
{
  quikeval_I2C_init();                    //! Initializes Linduino I2C port.

  output_high(LTC2874_CS);                //! Pulls LTC2874 Chip Select High

  quikeval_SPI_init();                    //! Configures the SPI port for 4Mhz SCK
  quikeval_SPI_connect();                 //! Connects SPI to QuikEval port
  Serial.begin(115200);                   //! Initializes the serial port to the PC
  print_title();                          //! Displays the title
  quikeval_I2C_connect();

  //! Checks if correct demo board is connected.
  demo_board_connected = discover_demo_board(demo_name);
  quikeval_SPI_connect();
  //! Prints the prompt if the correct demo board is connected
  if (demo_board_connected)
  {
    print_prompt();
  }
  else
  {
    print_warning_prompt();
    demo_board_connected = true;
  }
}


//*************************************** LOOP ***************************************
//! Repeats Linduino loop

void loop()
{
  // The main control loop
  if (demo_board_connected)       //! Does nothing if the demo board is not connected.
  {
    if (Serial.available())       //! Checks if user input is available.
    {
      int16_t user_command;       // User input command

      user_command = read_int();  //! Reads the user command (if available).
      Serial.println(user_command);
      Serial.flush();
      switch (user_command)
      {
        case 1:
          menu_1_cq();
          break;

        case 2:
          menu_2_lplus();
          break;

        case 3:
          menu_3_wakeup_cq();
          break;

        case 4:
          menu_4_24v_mode();
          break;

        case 5:
          menu_5_sio_mode();
          break;

        case 6:
          menu_6_set_illm();
          break;

        case 7:
          menu_7_clear_event();
          break;

        case 8:
          menu_8_update_all();
          break;

        case 9:
          menu_9_reset();
          break;

        case 10:
          menu_10_read_register();
          break;

        case 11:
          menu_11_write_register();
          break;

        case 12:
          menu_12_read_bit();
          break;

        case 13:
          menu_13_write_bit_set();
          break;

        case 14:
          menu_14_write_bit_clr();
          break;

        default:
          Serial.println("Incorrect Option");
          break;
      }

      Serial.println("");
      Serial.println("*************************");
      print_prompt();
    }
  }
}

//************************************************************************************
//! Menu 1: Enable/Disable CQ output for specified port
//************************************************************************************
void menu_1_cq(void)
{
  uint8_t port, value = 0;

  Serial.println(F("Select CQ output (1,2,3,4, 5=ALL)"));  //get port
  port = read_int();
  if (port == 5)
  {
    Serial.println("All");
  }
  else
  {
    Serial.println(port);
  }

  if ((port < 1) || (port > 5))                            //check limits
  {
    Serial.println(F("Selection out of range"));
    return;
  }

  Serial.println(F("Select 1 to Enable or 0 to Disable"));    //get value
  value = read_int();
  Serial.println(value);

  if ((value < 0) || (value > 1))                          //check limits for bit
  {
    Serial.println(F("Selection out of range"));
    return;
  }

  LTC2874_cq_output(port, value);                        //write to register

  Serial.print(F("CQ output"));
  if (port ==5)
  {
    Serial.print("s are ");
  }
  else
  {
    Serial.print(" ");
    Serial.print(port);
    Serial.print(F(" is "));
  }
  Serial.println(value);
}

//************************************************************************************
//! Menu 2: Enable/Disable L+ output for specified port
//************************************************************************************
void menu_2_lplus(void)
{
  uint8_t port, value = 0;

  Serial.println(F("Select L+ output (1,2,3,4, 5=ALL)"));
  port = read_int();
  if (port == 5)
  {
    Serial.println("All");
  }
  else
  {
    Serial.println(port);
  }

  if ((port < 1) || (port > 5))                            //check limits
  {
    Serial.println(F("Selection out of range"));
    return;
  }

  Serial.println(F("Select 1 to Enable or 0 to Disable"));
  value = read_int();
  Serial.println(value);

  if ((value < 0) || (value > 1))                          //check limits for bit
  {
    Serial.println(F("Selection out of range"));
    return;
  }

  LTC2874_lplus_output(port, value);

  Serial.print(F("L+ output"));
  if (port ==5)
  {
    Serial.print("s are ");
  }
  else
  {
    Serial.print(" ");
    Serial.print(port);
    Serial.print(F(" is "));
  }
  Serial.println(value);
}

//************************************************************************************
//! Menu 3: Generate Wake-Up Request (WURQ) on CQ output
//************************************************************************************
void menu_3_wakeup_cq(void)
{
  uint8_t data, port = 0;

  Serial.println(F("Generate Wakeup for CQ output (1,2,3,4)"));
  port = read_int();
  Serial.println(port);

  if ((port < 1) || (port > 4))                            //check limits
  {
    Serial.println(F("Selection out of range"));
    return;
  }
  LTC2874_wakeup_request(port);
  Serial.print("Wake-Up Request generated for CQ");
  Serial.print(port);
}

//************************************************************************************
//! Menu 4: Change value of 24VMODE bit
//************************************************************************************
void menu_4_24v_mode(void)
{
  uint8_t value = 0;
  Serial.println(F("Enable (1) or Disable (0) 24V Mode"));
  value = read_int();

  if ((value < 0) || (value > 1))                          //check limits
  {
    Serial.println(F("Selection out of range"));
    return;
  }

  LTC2874_24v_mode(value);
  Serial.print(F("24VMODE = "));
  Serial.println(value);
}

//************************************************************************************
//! Menu 5: Change SIO_MODE setting for specified port.
//************************************************************************************
void menu_5_sio_mode(void) //!<
{
  uint8_t port = 0;

  Serial.println(F("Enable SIO mode for CQ output (1,2,3,4, 5=ALL)"));
  port = read_int();
  if (port == 5)
  {
    Serial.println("All");
  }
  else
  {
    Serial.println(port);
  }

  if ((port < 1) || (port > 5))                            //check limits
  {
    Serial.println(F("Selection out of range"));
    return;
  }
  Serial.print(F("SIO mode set for "));
  if (port ==5)
  {
    Serial.print("ALL");
  }
  else
  {
    Serial.print("port ");
    Serial.print(port);
  }
  LTC2874_sio_mode(port);
}

//************************************************************************************
//! Menu 6: Change ILLM setting for specified port.
//************************************************************************************
void menu_6_set_illm(void)
{
  uint8_t port, value = 0;

  Serial.println(F("Select port for ILLM (1,2,3,4, 5=All)"));
  port = read_int();

  if ((port < 1) || (port > 5))                            //check limits
  {
    Serial.println(F("Selection out of range"));
    return;
  }
  if (port == 5)
  {
    Serial.println("All");
  }
  else
  {
    Serial.println(port);
  }

  Serial.println(F("Enter value to write (0x0-0x3); use Hex 0xn format."));
  value = read_int();
  Serial.print("0x");
  Serial.println(value, HEX);

  if ((value < 0x0) || (value > 0x3))                      //check limits
  {
    Serial.println(F("Value out of range"));
    return;
  }
  //! Update internal register
  LTC2874_write_ILLM_value(port, value);

  return;
}

//************************************************************************************
//! Menu 7: Clear all Event registers
//************************************************************************************
void menu_7_clear_event(void)
{
  Serial.println(F("Event registers cleared"));
  LTC2874_write_register( LTC2874_EVENT1_REG2, 0x00);
  LTC2874_write_register( LTC2874_EVENT2_REG3, 0x00);
  LTC2874_write_register( LTC2874_EVENT3_REG4, 0x00);
  LTC2874_write_register_update_all( LTC2874_EVENT4_REG5, 0x00);
}
//************************************************************************************
//! Menu 8: Update all registers
//************************************************************************************
void menu_8_update_all(void)
{
  Serial.println(F("All registers updated"));
  LTC2874_update_all();
  return;
}

//************************************************************************************
//! Menu 9: Reset LTC2874
//************************************************************************************
void menu_9_reset(void)
{
  Serial.println(F("LTC2874 has been reset"));
  LTC2874_reset();
  return;
}

//************************************************************************************
//! Menu 10: Read byte of data from a register
//************************************************************************************
void menu_10_read_register(void)
{
  uint8_t address, data = 0;

  Serial.println(F("Select Register to be read (0x0-0xE); use Hex 0xn format."));
  address = read_int();
  if ((address > 0xE ) || (address < 0x0))        //If user enters an invalid option
  {
    Serial.println("No registers in this range");
    return;
  }

  data = LTC2874_read_reg(address);

  Serial.print("Register 0x");
  Serial.print(address, HEX);
  Serial.print(" reads 0x");
  Serial.println (data, HEX);
  return;
}

//************************************************************************************
//! Menu 11: Write byte of data to a register
//************************************************************************************
void menu_11_write_register(void)
{
  uint8_t data, address = 0;

  address = reg_test();
  if (address == 0)
  {
    return;
  }

  Serial.print("Selected Register is 0x");
  Serial.println(address, HEX);

  Serial.println(F("Enter data to load in register; use Hex 0xnn format."));
  data = read_int();

  Serial.print("Data loaded is 0x");
  Serial.println(data, HEX);

  LTC2874_write_register_update_all(address, data);
  return;
}

//************************************************************************************
//! Menu 12: Read one bit from a register
//************************************************************************************
void menu_12_read_bit(void)
{
  uint8_t value, address = 0;

  Serial.println(F("Select Register that contains bit to read (0x0-0xE); use Hex 0xn format."));
  Serial.println();
  address = read_int();

  if ((address > 0xE ) || (address < 0x0))        //If user enters an invalid option
  {
    Serial.println("No registers in this range");
    return;
  }

  Serial.print("Selected Register is 0x");
  Serial.println(address, HEX);

  Serial.println(F("Enter bit position (0-7) for bit to read"));
  value = read_int();
  Serial.print("Bit position ");
  Serial.print(value);
  value = 0x01 << value;

  value = LTC2874_read_bit(address, value );
  Serial.print(" = ");
  Serial.println(value);
  return;
}

//************************************************************************************
//! Menu 13: Set one bit in writable register
//************************************************************************************
void menu_13_write_bit_set(void)
{
  uint8_t data, address = 0;

  address = reg_test();
  if (address == 0)
  {
    return;
  }

  Serial.print("Selected Register is 0x");
  Serial.println(address, HEX);

  Serial.println(F("Enter bit position (0-7) to Set"));
  data = read_int();

  Serial.print("Bit position ");
  Serial.print(data);
  data = 0x01 << data;
  LTC2874_write_bit_set_update_all(address, data);         //! Update
  Serial.print(" has been Set");
  return;
}

//************************************************************************************
//! Menu 14: Clear one bit in writable register
//************************************************************************************
void menu_14_write_bit_clr(void)
{
  uint8_t data, address = 0;

  address = reg_test();
  if (address == 0)
  {
    return;
  }

  Serial.print("Selected Register is 0x");
  Serial.println(address, HEX);

  Serial.println(F("Enter bit position (0-7) to Clear"));
  data = read_int();

  Serial.print("Bit position ");
  Serial.print(data);
  data = 0x01 << data;
  LTC2874_write_bit_clr_update_all(address, data);         //! Update
  Serial.print(" has been Cleared");
  return;
}

//************************************************************************************
//! Print the title block
//************************************************************************************
void print_title()
{
  Serial.println("*****************************************************************");
  Serial.println("* DC1880A Demonstration Program                                 *");
  Serial.println("*                                                               *");
  Serial.println("* This program demonstrates communication with the LTC2874      *");
  Serial.println("* Quad IO-Link Master Hot Swap Controller and PHY               *");
  Serial.println("*                                                               *");
  Serial.println("*                                                               *");
  Serial.println("*                                                               *");
  Serial.println("* Set the baud rate to 115200 select the newline terminator.    *");
  Serial.println("*****************************************************************");
}

//************************************************************************************
//! Print main menu and prompt user for an input command
//************************************************************************************
void print_prompt()
{
  Serial.println("\nLTC2874 Demo Menu:");

  Serial.println("  1-CQ Output Enable/Disable");
  Serial.println("  2-L+ Output Enable/Disable");
  Serial.println("  3-Generate Wake-Up Request (WURQ)");
  Serial.println("  4-24V IO-Link Mode");
  Serial.println("  5-SIO Mode");
  Serial.println("  6-Select ILLM Sinking Current");
  Serial.println("  7-Clear Event Registers");
  Serial.println("  8-Update All");
  Serial.println("  9-Reset");
  Serial.println(" 10-Read Register");
  Serial.println(" 11-Write Register");
  Serial.println(" 12-Read Bit");
  Serial.println(" 13-Set Bit");
  Serial.println(" 14-Clear Bit");

  Serial.println();

  Serial.print("Enter a command: ");
}

//************************************************************************************
//! Test register number against valid range for Write
//************************************************************************************
uint8_t reg_test()
{
  uint8_t address;

  Serial.println(F("Select Register to write to; use Hex 0xn format."));
  Serial.println(F("NOTE: 0x0,0x6 and 0x7 are read only."));
  Serial.println();
  address = read_int();

  if ((address > 0xF ) || (address < 0x0))        //If user enters an invalid option
  {
    Serial.println("Error: Invalid register");
    return(0);
  }
  else if (address == 0)
  {
    Serial.println("Error: Register 0x0 is read only");
    return(0);
  }
  else if (address == 5)
  {
    Serial.println("Error: Only lower nibble of 0x5 is writable");
  }
  else if (address == 6)
  {
    Serial.println("Error: Register 0x6 is read only");
    return(0);
  }
  else if (address == 7)
  {
    Serial.println("Error: Register 0x7 is read only");
    return(0);
  }

  return(address);
}

//************************************************************************************
//! Print warning prompt
//************************************************************************************
void print_warning_prompt()
{
  Serial.println(F("No demo board connected."));
  Serial.println(F("However, the Linduino will attempt to proceed..."));
}

