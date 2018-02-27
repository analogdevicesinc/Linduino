/*!
DC1954A
LTC6954: Low Phase Noise, Triple Output Clock Distribution Divider/Driver

@verbatim

  Setup:
    Set the terminal baud rate to 115200 and select the newline terminator.
    Refer to Demo Manual DC1954A.
    Ensure all jumpers are installed in the factory default positions.
    One 3.3V power supplies ia needed for this demo board
    An input signal is also needed for this demo board, refer to the
    DC1954 Demo Manual for details.


Command Description:

                             *****Main Menu*****
    1-  Load Default Settings- Loads the SPI map that is identical to file
        LTC6954.6954set that is supplied with the LTC6954_GUI and mentioned
        in the DC1954A user's manual.

    2-  READ/WRITE to Registers Addresses- Selecting this option will cause all the registers to
        be read, stored to variables, and displayed.  The user will then have the option
        to write to one register address at a time.

    3-  READ/WRITE to Registers Fields- Selecting this option will allow the user
        to read or write to one register field name at a time.

    4-  This function stores the current SPI settings in the demo boards EEPROM

    5-  This function loads SPI settings from the demo boards EEPROM to the device


USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC6954

http://www.linear.com/product/LTC6954#demoboards


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
    @ingroup LTC6954
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC6954.h"
#include <SPI.h>
#include <Wire.h>

// Function Declaration
void print_title();             // Print the title block
void print_prompt();            // Print the main menu
void menu_1_load_default_settings();  // Sub-menus
void menu_2_RW_to_reg_addresss();
void menu_3_RW_to_reg_field();
void menu_4_store_settings();
void menu_5_restore_settings();

// Global Variables
static uint8_t ref_out = 0;            //!< Used to keep track of reference out status
static int8_t demo_board_connected;    //!< Demo Board Name stored in QuikEval EEPROM
uint8_t First_Run=0;                   //!< if first time through loop = 0, otherwise=1


/* ------------------------------------------------------------------------- */
//! Initialize Linduino
//! @return void
void setup()
{
  char demo_name[] = "DC1954";    // Demo Board Name stored in QuikEval EEPROM
  uint8_t data;

  quikeval_SPI_init();      //! Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();   //! Connect SPI to main data port
  quikeval_I2C_init();      //! Configure the EEPROM I2C port for 100kHz
  Serial.begin(115200);     //! Initialize the serial port to the PC
  LTC6954_init();
  print_title();

  demo_board_connected = discover_demo_board(demo_name);  //! Checks if correct demo board is connected.

  if (!demo_board_connected)
    while (1);                  //! Does nothing if the demo board is not connected

  Serial.print(demo_board.name);
  Serial.println(F(" was found"));

  print_prompt();
} // end of setup()


/* ------------------------------------------------------------------------- */
//! Repeats Linduino loop
//! @return void
void loop()
{
  uint16_t user_command;          // User input command

  if (Serial.available())          // Check for user input
  {
    if (First_Run==0)
    {
      First_Run=1;
    }

    user_command = read_int();  //! Reads the user command
    if (user_command != 'm')
      Serial.println(user_command);

    switch (user_command)       //! Prints the appropriate submenu
    {
      case 1:
        menu_1_load_default_settings();
        break;

      case 2:
        menu_2_RW_to_reg_addresss();
        break;

      case 3:
        menu_3_RW_to_reg_field();
        break;

      case 4:
        menu_4_store_settings();
        break;

      case 5:
        menu_5_restore_settings();
        break;
      default:
        Serial.println(F("Incorrect Option"));
        break;
    } // end of switch statement
    Serial.println(F("\n*****************************************************************"));
    print_prompt();
  } // end of if statement
} // end of loop()

// Function Definitions
/* ------------------------------------------------------------------------- */
//! Menu 1: Load Default SPI Register Settings
//!  This function identifies which of the 4 LTC6954 frequency versions are connected.
//!  Based on the version connected, this function loads the register settings referenced
//!  in the DC1954A demo manual's quick start section.
//!  The register settings loaded are the same as the LTC6954_GUI 6954set files
//!  LTC6954.6954set.
//!  The setting loaded with this function assume the DC1954A's loop filter has
//!  not been modified.
//! @return void
void menu_1_load_default_settings()
{

// select which default register setting to load based on part number
  if (demo_board.product_name[8]=='1')   // if this is a LTC6954-1
    set_LTC6954_ALLREGS(LTC6954_CS,0x00,0x80,0x04,0x80,0x04,0x80,0x04);
  else if (demo_board.product_name[8]=='2')  // if this is a LTC6954-2
    set_LTC6954_ALLREGS(LTC6954_CS,0x00,0x80,0x04,0x80,0x04,0x80,0x04);
  else if (demo_board.product_name[8]=='3')  // if this is a LTC6954-3
    set_LTC6954_ALLREGS(LTC6954_CS,0x00,0x80,0x04,0x80,0x04,0x80,0x04);
  else if (demo_board.product_name[8]=='4')  // if this is a LTC6954-4
    set_LTC6954_ALLREGS(LTC6954_CS,0x00,0x80,0x04,0x80,0x04,0x80,0x04);
  else
  {
    Serial.print("No default file for this board:  ");
    Serial.println(demo_board.product_name);
  }  // end if-then-else statement

  Serial.println(F("Registers Have Been Written"));
} // end menu_1_load_default_settings function


/* ------------------------------------------------------------------------- */
//! Menu 2: Reads and/or Writes the SPI register address
//!  This function reads and displays all SPI register address settings in HEX format.
//!  It then provides an option to modify(write to) individual registers one at time
//!
//!  EXAMPLE:
//!  - 0- ADDR00 = 0x00
//!  - 1- ADDR01 = 0x08
//!  - ....
//!  - 6- ADDR06 = 0x04
//!  - 7- ADDR07 = 0x21 (read only)
//!  - -1 - Return to Main Menu
//!  - Enter a command (0-6 to modify register, or '-1' to return to Main Menu):
//! @return void
void menu_2_RW_to_reg_addresss()
{
  uint8_t i, regval, user_regval, num_regs;
  uint16_t user_address;          // User input command

  num_regs = get_LTC6954_REGSIZE();
// Read/Write loop, can exit loop by choosing '0'
  user_address=99;
  while  (user_address != -1)
  {
    Serial.println(F("\n*****************************************************************"));
    // Read All Registers and display results
    for (i=0; i<num_regs; i++)
    {
      regval = LTC6954_read(LTC6954_CS,i);
      Serial.print(i);
      if (i<16)
        Serial.print(F("- ADDR0"));
      else
        Serial.print(F("- ADDR"));
      Serial.print(i, HEX);
      Serial.print(F(" = 0x"));
      if (regval<16) Serial.print(F("0"));
      Serial.print(regval, HEX);
      if (i==(num_regs-1)) Serial.print(" (read only) ");
      Serial.println("");
    }  // end for loop
    Serial.print("-1 - Return to Main Menu\n\n");
    // User input: Select which register to modify, or return to main menu
    Serial.print("Enter a command (0-6 to modify register, or '-1' to return to Main Menu): ");
    user_address = read_int();  //! Reads the user command
    Serial.println(user_address);

    // User input: enter new setting for selected register
    if (user_address >=0 && user_address<(num_regs-1))
    {
      Serial.print("What value should ADDR");
      Serial.print(user_address);
      Serial.print(" be set to (ex: HEX format 0xff): ");
      user_regval = read_int();  //! Reads the user command
      Serial.println(user_regval);

      // writes new setting to part
      LTC6954_write(LTC6954_CS, (uint8_t)user_address, user_regval);
    } // end if statement
  } // end while loop
}  // end menu_2_RW_to_reg_addresss


/* ------------------------------------------------------------------------- */
//! Support function for function menu_3_RW_to_reg_field
//!  displays current state of select field
//!  provides user the option to write to that field or return to menu
//!  @return field value (user input) that will be written to part
long field_menu_RW(long field_val,       //!< current state of the selected field
                   char field_name[],    //!< SPI Field name selected
                   uint8_t f             //!< SPI field identifier identifies selected fields information in SPI MAP arrays
                  )
{
  long usr_field_val;
  uint8_t field_size, i;
  long max_num=1, pow2=1;

  Serial.print("CURRENT STATE (HEX): ");
  Serial.print(field_name);
  Serial.print("= 0x");
  Serial.println(field_val, HEX);

  if (get_LTC6954_SPI_FIELD_RW(f)==0)
  {
    field_size=get_LTC6954_SPI_FIELD_NUMBITS(f);
    for (i=1; i<field_size; i++)
    {
      pow2=pow2*2;
      max_num=max_num + pow2;
    }

    Serial.print("What value should ");
    Serial.print(field_name);
    Serial.print(" be set to or type '-1' to exit: (ex: HEX format 0x00 to 0x");
    Serial.print(max_num, HEX);
    Serial.print(")");
    usr_field_val = read_int();  //! Reads the user command

    if (usr_field_val>=0 && usr_field_val<=max_num)
    {
      Serial.println(usr_field_val);
      return usr_field_val;
    }
    else
    {
      return field_val;
    } // end of if statement
  } // end of if statement
} // end of field_menu_RW


/* ------------------------------------------------------------------------- */
//! Menu 3: Reads and/or Writes individual SPI fields
//!  This function provides the user with a list of all SPI fields.
//!  The user can select a SPI field to read its current value.
//!  Then the user will be provided with an option to write to that field
//!  or return to the selection menu.
//!
//!  EXAMPLE:
//!  - 1-CMSINV0      9-LVCS2        17-PD_DIV2
//!  - 2-CMSINV1      10-M0          18-PD_OUT0
//!  - ....
//!  - 7-LVCS0        15-PD_DIV0     23-SYNC_EN1
//!  - 8-LVCS1        16-PD_DIV1     24-SYNC_EN2
//!  - 0 - Return to Main Menu
//!  - * = READ ONLY FIELD
//!  - Enter a command (1-24 to modify register, or '0' to return to Main Menu):
//! @return void
void menu_3_RW_to_reg_field()
{
  uint8_t  field_num;
  long field_val;

// Read/Write loop, can exit loop by choosing 'm'
  field_num=1;
  while  (field_num != 0)
  {
    Serial.println(F("\n*****************************************************************"));
    // Select Fields to read and write to
    Serial.print(F("1-CMSINV0     9-LVCS2        17-PD_DIV2\n"));
    Serial.print(F("2-CMSINV1     10-M0          18-PD_OUT0\n"));
    Serial.print(F("3-CMSINV2     11-M1          19-PD_OUT1\n"));
    Serial.print(F("4-DEL0        12-M2          20-PD_OUT2\n"));
    Serial.print(F("5-DEL1        13-PART *      21-REV *\n"));
    Serial.print(F("6-DEL2        14-PDALL       22-SYNC_EN0\n"));
    Serial.print(F("7-LVCS0       15-PD_DIV0     23-SYNC_EN1\n"));
    Serial.print(F("8-LVCS1       16-PD_DIV1     24-SYNC_EN2\n"));
    Serial.print("0 - Return to Main Menu\n");
    Serial.print("* = READ ONLY FIELD\n\n");

    Serial.print("Enter a command (1-24 to modify register, or '0' to return to Main Menu): ");
    field_num = read_int();  //! Reads the user command
    Serial.println(field_num);

    // User input: enter new setting for selected register
    if (field_num > 0)
    {
      switch (field_num)        //! Prints the appropriate submenu
      {
        case LTC6954_CMSINV0:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_CMSINV0);    // reads selected field
          field_val=field_menu_RW(field_val,"CMSINV0",LTC6954_CMSINV0);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_CMSINV0, field_val); // updates selected field
          }
          break;

        case LTC6954_CMSINV1:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_CMSINV1);    // reads selected field
          field_val=field_menu_RW(field_val,"CMSINV1",LTC6954_CMSINV1);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_CMSINV1, field_val); // updates selected field
          }
          break;

        case LTC6954_CMSINV2:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_CMSINV2);    // reads selected field
          field_val=field_menu_RW(field_val,"CMSINV2",LTC6954_CMSINV2);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_CMSINV2, field_val); // updates selected field
          }
          break;

        case LTC6954_DEL0:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_DEL0);    // reads selected field
          field_val=field_menu_RW(field_val,"DEL0",LTC6954_DEL0);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_DEL0, field_val); // updates selected field
          }
          break;

        case LTC6954_DEL1:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_DEL1);    // reads selected field
          field_val=field_menu_RW(field_val,"DEL1",LTC6954_DEL1);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_DEL1, field_val); // updates selected field
          }
          break;

        case LTC6954_DEL2:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_DEL2);    // reads selected field
          field_val=field_menu_RW(field_val,"DEL2",LTC6954_DEL2);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_DEL2, field_val); // updates selected field
          }
          break;

        case LTC6954_LVCS0:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_LVCS0);    // reads selected field
          field_val=field_menu_RW(field_val,"LVCS0",LTC6954_LVCS0);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_LVCS0, field_val); // updates selected field
          }
          break;

        case LTC6954_LVCS1:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_LVCS1);    // reads selected field
          field_val=field_menu_RW(field_val,"LVCS1",LTC6954_LVCS1);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_LVCS1, field_val); // updates selected field
          }
          break;

        case LTC6954_LVCS2:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_LVCS2);    // reads selected field
          field_val=field_menu_RW(field_val,"LVCS2",LTC6954_LVCS2);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_LVCS2, field_val); // updates selected field
          }
          break;

        case LTC6954_M0:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_M0);    // reads selected field
          field_val=field_menu_RW(field_val,"M0",LTC6954_M0);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_M0, field_val); // updates selected field
          }
          break;

        case LTC6954_M1:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_M1);    // reads selected field
          field_val=field_menu_RW(field_val,"M1",LTC6954_M1);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_M1, field_val); // updates selected field
          }
          break;

        case LTC6954_M2:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_M2);    // reads selected field
          field_val=field_menu_RW(field_val,"M2",LTC6954_M2);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_M2, field_val); // updates selected field
          }
          break;

        case LTC6954_PART:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_PART);    // reads selected field
          field_val=field_menu_RW(field_val,"PART",LTC6954_PART);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_PART, field_val); // updates selected field
          }
          break;

        case LTC6954_PDALL:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_PDALL);    // reads selected field
          field_val=field_menu_RW(field_val,"PDALL",LTC6954_PDALL);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_PDALL, field_val); // updates selected field
          }
          break;

        case LTC6954_PD_DIV0:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_PD_DIV0);    // reads selected field
          field_val=field_menu_RW(field_val,"PD_DIV0",LTC6954_PD_DIV0);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_PD_DIV0, field_val); // updates selected field
          }
          break;

        case LTC6954_PD_DIV1:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_PD_DIV1);    // reads selected field
          field_val=field_menu_RW(field_val,"PD_DIV1",LTC6954_PD_DIV1);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_PD_DIV1, field_val); // updates selected field
          }
          break;

        case LTC6954_PD_DIV2:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_PD_DIV2);    // reads selected field
          field_val=field_menu_RW(field_val,"PD_DIV2",LTC6954_PD_DIV2);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_PD_DIV2, field_val); // updates selected field
          }
          break;

        case LTC6954_PD_OUT0:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_PD_OUT0);    // reads selected field
          field_val=field_menu_RW(field_val,"PD_OUT0",LTC6954_PD_OUT0);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_PD_OUT0, field_val); // updates selected field
          }
          break;

        case LTC6954_PD_OUT1:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_PD_OUT1);    // reads selected field
          field_val=field_menu_RW(field_val,"PD_OUT1",LTC6954_PD_OUT1);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_PD_OUT1, field_val); // updates selected field
          }
          break;

        case LTC6954_PD_OUT2:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_PD_OUT2);    // reads selected field
          field_val=field_menu_RW(field_val,"PD_OUT2",LTC6954_PD_OUT2);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_PD_OUT2, field_val); // updates selected field
          }
          break;

        case LTC6954_REV:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_REV);    // reads selected field
          field_val=field_menu_RW(field_val,"REV",LTC6954_REV);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_REV, field_val); // updates selected field
          }
          break;

        case LTC6954_SYNC_EN0:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_SYNC_EN0);    // reads selected field
          field_val=field_menu_RW(field_val,"SYNC_EN0",LTC6954_SYNC_EN0);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_SYNC_EN0, field_val); // updates selected field
          }
          break;

        case LTC6954_SYNC_EN1:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_SYNC_EN1);    // reads selected field
          field_val=field_menu_RW(field_val,"SYNC_EN1",LTC6954_SYNC_EN1);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_SYNC_EN1, field_val); // updates selected field
          }
          break;

        case LTC6954_SYNC_EN2:
          field_val=get_LTC6954_SPI_FIELD(LTC6954_CS,LTC6954_SYNC_EN2);    // reads selected field
          field_val=field_menu_RW(field_val,"SYNC_EN2",LTC6954_SYNC_EN2);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6954_SPI_FIELD(LTC6954_CS, LTC6954_SYNC_EN2, field_val); // updates selected field
          }
          break;
      }  // end of switch statement
    } // end if user_command != 0 statement
  } // end while loop
}  // end menu_3_RW_to_reg_field function

/* ------------------------------------------------------------------------- */
//! Store PLL settings to nonvolatile EEPROM on demo board
//! @return void
void menu_4_store_settings()
{
// Store the PLL Settings to the EEPROM
  uint8_t regval;

  uint8_t addr_offset;
  uint8_t num_reg;

  addr_offset=2;
  num_reg = get_LTC6954_REGSIZE();

  eeprom_write_int16(EEPROM_I2C_ADDRESS, EEPROM_CAL_KEY, EEPROM_CAL_STATUS_ADDRESS);         // Cal key

  for (uint8_t i = 0; i <= num_reg ; i++)
  {
    regval = LTC6954_read(LTC6954_CS,i);
    eeprom_write_byte(EEPROM_I2C_ADDRESS,(char) regval, EEPROM_CAL_STATUS_ADDRESS+ i+addr_offset);
  }
  Serial.println(F("PLL Settings Stored to EEPROM"));

}


/* ------------------------------------------------------------------------- */
//! Read stored PLL settings from nonvolatile EEPROM on demo board
//! @return void
void menu_5_restore_settings()
{
// Read the PLL settings from EEPROM
  int16_t cal_key;
  uint8_t regval;
  uint8_t user_address;

  uint8_t addr_offset;
  uint8_t num_reg;

  addr_offset=2;
  num_reg = get_LTC6954_REGSIZE();

// read the cal key from the EEPROM
  eeprom_read_int16(EEPROM_I2C_ADDRESS, &cal_key, EEPROM_CAL_STATUS_ADDRESS);
  if (cal_key == EEPROM_CAL_KEY)
  {
    // PLL Settings has been stored, read PLL Settings
    user_address=2;
    for (uint8_t i = 0; i <= num_reg ; i++)
    {
      eeprom_read_byte(EEPROM_I2C_ADDRESS,(char *) &regval, EEPROM_CAL_STATUS_ADDRESS + i+addr_offset);
      LTC6954_write(LTC6954_CS, (uint8_t)i, regval);
      user_address++;
    }
    Serial.println(F("PLL Settings Restored"));
  }
  else
  {
    Serial.println(F("PLL Settings not found"));
  }

}

/* ------------------------------------------------------------------------- */
//!    Prints the title block when program first starts.
void print_title()
{

  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC1954 Demonstration Program                                  *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to send data to the LTC6954     *"));
  Serial.println(F("* Low Phase Noise, Triple Output Clock Distribution             *"));
  Serial.println(F("* Divider/Driver.                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************"));
  Serial.println();
} // end of print_title


/* ------------------------------------------------------------------------- */
//!    Prints main menu.
void print_prompt()
{

  Serial.println(F("\nCommand Summary:"));
  Serial.println(F("  1-Load Default Settings (same as the LTC6954 GUI's LTC6954.6954set settings)"));
  Serial.println(F("  2-READ/WRITE to Registers Addresses"));
  Serial.println(F("  3-READ/WRITE to Registers Fields"));
  Serial.println(F("  4-Store LTC6954 SPI settings to the DC1954's EEPROM"));
  Serial.println(F("  5-Restore LTC6954 SPI settings from the DC1954's EEPROM"));
  Serial.println("");
  Serial.print(F("Enter a command: "));
} // end of print_prompt


