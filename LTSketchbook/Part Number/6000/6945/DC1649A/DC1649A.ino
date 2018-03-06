/*!
DC1649A
LTC6945: Ultralow Noise and Spurious 0.35GHz to 6GHz Integer-N Synthesizer

@verbatim

  Setup:
    Set the terminal baud rate to 115200 and select the newline terminator.
    Refer to Demo Manual DC1649A.
    Ensure all jumpers are installed in the factory default positions.
    Two power supplies are needed for this demo board: a 5v and a 3.3v supply.
    The 5V powers the 5V supply and V+VCO turret.
    A reference frequency is also needed for this demo board, refer to the
    DC1649 Demo Manual for details.


Command Description:

                             *****Main Menu*****
    1-  Load Default Settings- Loads the SPI map that is identical to file
        DC1649_100MHz.pllset that is supplied with the PllWizard and mentioned
        in the DC1649A user's manual.  It assumes a 100MHz reference input and the
        default DC1649A BOM.  It should output a 914MHz signal on RF+/-.

        ** If you want to use a different loop filter, reference frequency or different
        register settings.  Please use PllWizard for the loop filter design and initial
        device setup.  The register settings from PllWizard can be entered into menu option 2.

    2-  READ/WRITE to Registers Addresses- Selecting this option will cause all the registers to
        be read, stored to variables, and displayed.  The user will then have the option
        to write to one register address at a time.

    3-  READ/WRITE to Registers Fields- Selecting this option will allow the user
        to read or write to one register field name at a time.

    4-  This function calculates and programs OD, ND based on the desired Frf,
        the reference frequency, and the current RD value.  Linduino One (Arduino Uno) are
        limited to 32 bit floats, int and doubles.  Significant rounding errors are created
        with this 32 bit limitation.  Therefore, this function uses 64bit math functions
        specifically created to overcome this limitation.  After OD and ND, are programmed,
        the program calibrates the LTC6945.  If other registers need change see menu 2 or menu 3.

    5-  This function stores the current SPI settings in the demo boards EEPROM

    6-  This function loads SPI settings from the demo boards EEPROM to the device


USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC6945

http://www.linear.com/product/LTC6945#demoboards


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
    @ingroup LTC6945
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC6945.h"
#include <SPI.h>
#include <Wire.h>

// Function Declaration
void print_title();             // Print the title block
void print_prompt();            // Print the main menu
void menu_1_load_default_settings();  // Sub-menus
void menu_2_RW_to_reg_addresss();
void menu_3_RW_to_reg_field();
void menu_4_set_frf();
void menu_5_store_settings();
void menu_6_restore_settings();

// Global Variables
static uint8_t ref_out = 0;            //!< Used to keep track of reference out status
static int8_t demo_board_connected;    //!< Demo Board Name stored in QuikEval EEPROM
uint8_t First_Run=0;                   //!< if first time through loop = 0, otherwise=1


/* ------------------------------------------------------------------------- */
//! Initialize Linduino
//! @return void
void setup()
{
  char demo_name[] = "DC1649";    // Demo Board Name stored in QuikEval EEPROM
  uint8_t data;

  quikeval_SPI_init();      //! Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();   //! Connect SPI to main data port
  quikeval_I2C_init();      //! Configure the EEPROM I2C port for 100kHz
  Serial.begin(115200);     //! Initialize the serial port to the PC
  LTC6945_init();
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
        menu_4_set_frf();
        break;

      case 5:
        menu_5_store_settings();
        break;

      case 6:
        menu_6_restore_settings();
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
//!  This function loads the register settings referenced
//!  in the DC1649A demo manual's quick start section.
//!  The register settings loaded are the same as PllWizards
//!  pllset files DC1649_100MHz.pllset
//!  The setting loaded with this function assume the LTC6945's
//!  reference is set to 100MHz and
//!  the DC1649A's BOM has not been modified.
//! @return void
void menu_1_load_default_settings()
{

  set_LTC6945_ALLREGS(LTC6945_CS, 0x04,0x0C,0x01,0x90,0x0E,0x48,0x03,0x99,0xDB,0x00);
  Serial.println(F("Registers Have Been Written"));
} // end menu_1_load_default_settings function


/* ------------------------------------------------------------------------- */
//! Menu 2: Reads and/or Writes the SPI register address
//!  This function reads and displays all SPI register address settings in HEX format.
//!  It then provides an option to modify(write to) individual registers one at time
//!
//!  EXAMPLE:
//!  - 0- ADDR00 = 0x04 (read only)
//!  - 1- ADDR01 = 0x04
//!  - ....
//!  - 10- ADDR0A = 0x00
//!  - 11- ADDR0B = 0x40 (read only)
//!  - 0 - Return to Main Menu
//!  - Enter a command (1-10 to modify register, or '0' to return to Main Menu):
//! @return void
void menu_2_RW_to_reg_addresss()
{
  uint8_t i, regval, user_regval, num_reg;
  uint16_t user_address;          // User input command

  num_reg = get_LTC6945_REGSIZE();
  user_address=1;
// Read/Write loop, can exit loop by choosing '0'
  while  (user_address != 0)
  {
    Serial.println(F("\n*****************************************************************"));
    // Read All Registers and display results
    for (i=0; i<num_reg; i++)
    {
      regval = LTC6945_read(LTC6945_CS,i);
      Serial.print(i);
      if (i<16)
        Serial.print(F("- ADDR0"));
      else
        Serial.print(F("- ADDR"));
      Serial.print(i, HEX);
      Serial.print(F(" = 0x"));
      if (regval<16) Serial.print(F("0"));
      Serial.print(regval, HEX);
      if ((i==0)||(i==(num_reg-1))) Serial.print(" (read only) ");
      if (i==2) Serial.print(" (warning: if D0=1 it resets all registers. Power On Reset Bit)");
      Serial.println("");
    }  // end for loop
    Serial.print("0 - Return to Main Menu\n\n");
    // User input: Select which register to modify, or return to main menu
    Serial.print("Enter a command (1-10 to modify register, or '0' to return to Main Menu): ");
    user_address = read_int();  //! Reads the user command
    Serial.println(user_address);

    // User input: enter new setting for selected register
    if (user_address >0 && user_address<(num_reg-1))
    {
      Serial.print("What value should ADDR");
      Serial.print(user_address);
      Serial.print(" be set to (ex: HEX format 0xff): ");
      user_regval = read_int();  //! Reads the user command
      Serial.println(user_regval);

      // writes new setting to part
      LTC6945_write(LTC6945_CS, (uint8_t)user_address, user_regval);
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

  if (get_LTC6945_SPI_FIELD_RW(f)==0)
  {
    field_size=get_LTC6945_SPI_FIELD_NUMBITS(f);
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
//!  - 1-BST       12-LKCT      23-PDREFO *
//!  - 2-CP        13-LKEN      24-POR
//!  - 3-CPCHI     14-LKWIN     25-RD
//!  - 4-CPCLO     15-LOCK *    26-REV *
//!  - 5-CPDN      16-ND        27-RFO
//!  - 6-CPINV     17-OD        28-THI *
//!  - 7-CPMID     18-OMUTE     29-TLO *
//!  - 8-CPRST     19-PART *    30-UNLOCK *
//!  - 9-CPUP      20-PDALL     31-x
//!  - 10-CPWIDE   21-PDOUT
//!  - 11-FILT     22-PDPLL
//!  - 0 - Return to Main Menu
//!  - * = READ ONLY FIELD
//!  - Enter a command (1-31 to modify register, or '0' to return to Main Menu):
//! @return void
void menu_3_RW_to_reg_field()
{
  uint8_t  field_num;
  long field_val;

  field_num=1;
// Read/Write loop, can exit loop by choosing 'm'
  while  (field_num != 0)
  {
    Serial.println(F("\n*****************************************************************"));
    // Select Fields to read and write to
    Serial.print(F("1-BST       12-LKCT      23-PDREFO\n"));
    Serial.print(F("2-CP        13-LKEN      24-POR\n"));
    Serial.print(F("3-CPCHI     14-LKWIN     25-RD\n"));
    Serial.print(F("4-CPCLO     15-LOCK *    26-REV *\n"));
    Serial.print(F("5-CPDN      16-ND        27-RFO\n"));
    Serial.print(F("6-CPINV     17-OD        28-THI *\n"));
    Serial.print(F("7-CPMID     18-OMUTE     29-TLO *\n"));
    Serial.print(F("8-CPRST     19-PART *    30-UNLOCK *\n"));
    Serial.print(F("9-CPUP      20-PDALL     31-x\n"));
    Serial.print(F("10-CPWIDE   21-PDOUT        \n"));
    Serial.print(F("11-FILT     22-PDPLL        \n"));
    Serial.print("0 - Return to Main Menu\n");
    Serial.print("* = READ ONLY FIELD\n\n");
    Serial.print("Enter a command (1-31 to modify register, or '0' to return to Main Menu): ");
    field_num = read_int();  //! Reads the user command
    Serial.println(field_num);

    // User input: enter new setting for selected register
    if (field_num != 0)
    {
      switch (field_num)        //! Prints the appropriate submenu
      {
        case LTC6945_BST:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_BST);    // reads selected field
          field_val=field_menu_RW(field_val,"BST",LTC6945_BST);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_BST, field_val); // updates selected field
          }
          break;

        case LTC6945_CP:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_CP);    // reads selected field
          field_val=field_menu_RW(field_val,"CP",LTC6945_CP);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_CP, field_val); // updates selected field
          }
          break;

        case LTC6945_CPCHI:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_CPCHI);    // reads selected field
          field_val=field_menu_RW(field_val,"CPCHI",LTC6945_CPCHI);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_CPCHI, field_val); // updates selected field
          }
          break;

        case LTC6945_CPCLO:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_CPCLO);    // reads selected field
          field_val=field_menu_RW(field_val,"CPCLO",LTC6945_CPCLO);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_CPCLO, field_val); // updates selected field
          }
          break;

        case LTC6945_CPDN:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_CPDN);    // reads selected field
          field_val=field_menu_RW(field_val,"CPDN",LTC6945_CPDN);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_CPDN, field_val); // updates selected field
          }
          break;

        case LTC6945_CPINV:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_CPINV);    // reads selected field
          field_val=field_menu_RW(field_val,"CPINV",LTC6945_CPINV);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_CPINV, field_val); // updates selected field
          }
          break;

        case LTC6945_CPMID:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_CPMID);    // reads selected field
          field_val=field_menu_RW(field_val,"CPMID",LTC6945_CPMID);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_CPMID, field_val); // updates selected field
          }
          break;

        case LTC6945_CPRST:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_CPRST);    // reads selected field
          field_val=field_menu_RW(field_val,"CPRST",LTC6945_CPRST);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_CPRST, field_val); // updates selected field
          }
          break;

        case LTC6945_CPUP:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_CPUP);    // reads selected field
          field_val=field_menu_RW(field_val,"CPUP",LTC6945_CPUP);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_CPUP, field_val); // updates selected field
          }
          break;

        case LTC6945_CPWIDE:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_CPWIDE);    // reads selected field
          field_val=field_menu_RW(field_val,"CPWIDE",LTC6945_CPWIDE);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_CPWIDE, field_val); // updates selected field
          }
          break;

        case LTC6945_FILT:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_FILT);    // reads selected field
          field_val=field_menu_RW(field_val,"FILT",LTC6945_FILT);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_FILT, field_val); // updates selected field
          }
          break;

        case LTC6945_LKCT:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_LKCT);    // reads selected field
          field_val=field_menu_RW(field_val,"LKCT",LTC6945_LKCT);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_LKCT, field_val); // updates selected field
          }
          break;

        case LTC6945_LKEN:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_LKEN);    // reads selected field
          field_val=field_menu_RW(field_val,"LKEN",LTC6945_LKEN);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_LKEN, field_val); // updates selected field
          }
          break;

        case LTC6945_LKWIN:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_LKWIN);    // reads selected field
          field_val=field_menu_RW(field_val,"LKWIN",LTC6945_LKWIN);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_LKWIN, field_val); // updates selected field
          }
          break;

        case LTC6945_LOCK:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_LOCK);    // reads selected field
          field_val=field_menu_RW(field_val,"LOCK",LTC6945_LOCK);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_LOCK, field_val); // updates selected field
          }
          break;

        case LTC6945_ND:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_ND);    // reads selected field
          field_val=field_menu_RW(field_val,"ND",LTC6945_ND);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_ND, field_val); // updates selected field
          }
          break;

        case LTC6945_OD:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_OD);    // reads selected field
          field_val=field_menu_RW(field_val,"OD",LTC6945_OD);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_OD, field_val); // updates selected field
          }
          break;

        case LTC6945_OMUTE:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_OMUTE);    // reads selected field
          field_val=field_menu_RW(field_val,"OMUTE",LTC6945_OMUTE);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_OMUTE, field_val); // updates selected field
          }
          break;

        case LTC6945_PART:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_PART);    // reads selected field
          field_val=field_menu_RW(field_val,"PART",LTC6945_PART);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_PART, field_val); // updates selected field
          }
          break;

        case LTC6945_PDALL:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_PDALL);    // reads selected field
          field_val=field_menu_RW(field_val,"PDALL",LTC6945_PDALL);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_PDALL, field_val); // updates selected field
          }
          break;

        case LTC6945_PDOUT:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_PDOUT);    // reads selected field
          field_val=field_menu_RW(field_val,"PDOUT",LTC6945_PDOUT);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_PDOUT, field_val); // updates selected field
          }
          break;

        case LTC6945_PDPLL:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_PDPLL);    // reads selected field
          field_val=field_menu_RW(field_val,"PDPLL",LTC6945_PDPLL);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_PDPLL, field_val); // updates selected field
          }
          break;

        case LTC6945_PDREFO:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_PDREFO);    // reads selected field
          field_val=field_menu_RW(field_val,"PDREFO",LTC6945_PDREFO);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_PDREFO, field_val); // updates selected field
          }
          break;

        case LTC6945_POR:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_POR);    // reads selected field
          field_val=field_menu_RW(field_val,"POR",LTC6945_POR);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_POR, field_val); // updates selected field
          }
          break;

        case LTC6945_RD:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_RD);    // reads selected field
          field_val=field_menu_RW(field_val,"RD",LTC6945_RD);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_RD, field_val); // updates selected field
          }
          break;

        case LTC6945_REV:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_REV);    // reads selected field
          field_val=field_menu_RW(field_val,"REV",LTC6945_REV);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_REV, field_val); // updates selected field
          }
          break;

        case LTC6945_RFO:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_RFO);    // reads selected field
          field_val=field_menu_RW(field_val,"RFO",LTC6945_RFO);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_RFO, field_val); // updates selected field
          }
          break;

        case LTC6945_THI:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_THI);    // reads selected field
          field_val=field_menu_RW(field_val,"THI",LTC6945_THI);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_THI, field_val); // updates selected field
          }
          break;

        case LTC6945_TLO:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_TLO);    // reads selected field
          field_val=field_menu_RW(field_val,"TLO",LTC6945_TLO);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_TLO, field_val); // updates selected field
          }
          break;

        case LTC6945_UNLOCK:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_UNLOCK);    // reads selected field
          field_val=field_menu_RW(field_val,"UNLOCK",LTC6945_UNLOCK);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_UNLOCK, field_val); // updates selected field
          }
          break;

        case LTC6945_x:
          field_val=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_x);    // reads selected field
          field_val=field_menu_RW(field_val,"x",LTC6945_x);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6945_SPI_FIELD(LTC6945_CS, LTC6945_x, field_val); // updates selected field
          }
          break;
      }  // end of switch statement
    } // end if user_command != 0 statement
  } // end while loop
}  // end menu_3_RW_to_reg_field function


/* ------------------------------------------------------------------------- */
//! verifies VCO frequency is within datasheet specifications
void LTC6945_VCO_Freq_Verification()
{
  unsigned long temp_val, temp_max_VCO_MHz, temp_min_VCO_MHz, temp_max_VCO_Hz, temp_min_VCO_Hz;
  unsigned long fvco_max[2], fvco_min[2], fvco_max_lim[2], fvco_min_lim[2];
  boolean valid_input=false;

// USER INPUT
  valid_input=false;


  while (valid_input==false)
  {
    temp_max_VCO_MHz=get_LTC6945_global_VCO_MAX_MHz();
    temp_min_VCO_MHz=get_LTC6945_global_VCO_MIN_MHz();
    temp_max_VCO_Hz=get_LTC6945_global_VCO_MAX_Hz();
    temp_min_VCO_Hz=get_LTC6945_global_VCO_MIN_Hz();

    Serial.print(F("What is the upper frequency limit of the VCO (MHZ portion)? ["));
    Serial.print(temp_max_VCO_MHz);
    Serial.print(F("]: "));
    temp_val = read_float();  //! Reads the user command
    // if user selects enter, keep same Fref.  Otherwise set Fref and verify
    if (temp_val!=0) temp_max_VCO_MHz = abs(temp_val);
    Serial.println(temp_max_VCO_MHz);

    Serial.print(F("What is the upper VCO frequency limit (HZ portion)? "));
    temp_max_VCO_Hz = read_float();  //! Reads the user command
    // if user selects enter, keep same Fref.  Otherwise set Fref and verify
    Serial.println(temp_max_VCO_Hz);

    Serial.print(F("\nWhat is the lower frequency limit of the VCO (MHZ portion)? ["));
    Serial.print(temp_min_VCO_MHz);
    Serial.print(F("]: "));
    temp_val = read_float();  //! Reads the user command
    // if user selects enter, keep same Fref.  Otherwise set Fref and verify
    if (temp_val!=0) temp_min_VCO_MHz = abs(temp_val);
    Serial.println(temp_min_VCO_MHz);

    Serial.print(F("What is the lower frequency limit of the VCO (HZ portion)? "));
    temp_min_VCO_Hz = read_float();  //! Reads the user command
    Serial.println(temp_min_VCO_Hz);

    HZto64(fvco_max,temp_max_VCO_MHz,temp_max_VCO_Hz);  // convert to 64 bit integer
    HZto64(fvco_min,temp_min_VCO_MHz,temp_min_VCO_Hz);  // convert to 64 bit integer
    HZto64(fvco_max_lim,LTC6945_MAXFREQ,0);  // convert to 64 bit integer
    HZto64(fvco_min_lim,LTC6945_MINFREQ,0);  // convert to 64 bit integer


    // if valid input print the following to the screen
    if (lt64(fvco_min_lim,fvco_min) && lt64(fvco_max,fvco_max_lim) &&
        lt64(fvco_min_lim,fvco_max) && lt64(fvco_min,fvco_max_lim) &&
        (eq64(fvco_min, fvco_max) || lt64(fvco_min, fvco_max)) )
    {
      set_LTC6945_global_vcolim(temp_max_VCO_MHz, temp_max_VCO_Hz, temp_min_VCO_MHz, temp_min_VCO_Hz);
      Serial.print(F("VCO Frequencies entered are valid\n"));
      valid_input=true;
    }
    else
    {
      Serial.print(F("LTC6945 VCO Frequency datasheet limits are 350MHz to 6000MHz\n"));
    }  // end of if-else
  } // end of while
}  // end of LTC6945_VCO_Freq_Verification


/* ------------------------------------------------------------------------- */
//! verifies reference frequency is within datasheet specifications

void LTC6945_Ref_Freq_Verification()
{
  unsigned long temp_val, temp_fref_MHz, temp_fref_Hz;
  boolean valid_input=false;

// USER INPUT
  valid_input=false;


  while (valid_input==false)
  {
    temp_fref_MHz=get_LTC6945_global_fref_MHz();
    temp_fref_Hz=get_LTC6945_global_fref_Hz();
    Serial.print(F("\nWhat is the MHz portion of the Reference Input Frequency(MHZ)? ["));
    Serial.print(temp_fref_MHz);
    Serial.print(F("]: "));
    temp_val = read_float();  //! Reads the user command
    // if user selects enter, keep same Fref.  Otherwise set Fref and verify
    if (temp_val!=0) temp_fref_MHz = abs(temp_val);
    Serial.println(temp_fref_MHz);

    Serial.print(F("What is the sub-MHz portion of the Reference Input Frequency(HZ)? "));
    temp_val = read_float();  //! Reads the user command
    temp_fref_Hz = abs(temp_val);
    Serial.println(temp_fref_Hz);

    // if valid input print the following to the screen
    if (temp_fref_MHz >=LTC6945_MIN_REF_FREQ & temp_fref_MHz <= LTC6945_MAX_REF_FREQ)
    {
      set_LTC6945_global_fref(temp_fref_MHz,temp_fref_Hz);
      temp_val= temp_fref_MHz*OneMHz + temp_fref_Hz;
      Serial.print(F("Reference Frequency set to "));
      Serial.print(temp_val);
      Serial.println(F("Hz"));
      valid_input=true;
    }
    else
    {
      Serial.print(F("Reference Frequency must be between 10MHz and 250MHz\n"));
    }  // end of if-else
  } // end of while
}  // end of LTC6945_Ref_Freq_Verification


/* ------------------------------------------------------------------------- */
//! verifies frf frequency is within datasheet specifications
void LTC6945_Fout_Freq_Verification()
{
  unsigned long odiv, temp_fout_MHz, temp_fout_Hz, temp_val;
  unsigned long frf[2];
  boolean valid_input=false;

// USER INPUT
  temp_fout_MHz=get_LTC6945_global_frf_MHz();
  temp_fout_Hz=get_LTC6945_global_frf_Hz();

  while (valid_input==false)
  {
    Serial.print(F("\nWhat is the MHz portion of the Output Frequency(MHZ)? ["));
    Serial.print(temp_fout_MHz);
    Serial.print(F("]: "));
    temp_val = read_int();  //! Reads the user command
    // if user selects enter, keep same Fout.  Otherwise set Fout and verify
    if (temp_val!=0) temp_fout_MHz = abs(temp_val);
    Serial.println(temp_fout_MHz);

    Serial.print(F("What is the Hz portion of the Output Frequency(HZ)? "));
    temp_val = read_int();  //! Reads the user command
    temp_fout_Hz = abs(temp_val);
    Serial.println(temp_fout_Hz);

    HZto64(frf,temp_fout_MHz,temp_fout_Hz);  // convert to 64 bit integer

    // verify desired frequency falls within a divider range (1-6)
    odiv = LTC6945_calc_odiv(frf);
    valid_input=false;
    if ((odiv>=1) && (odiv<=6)) valid_input=true;

    // if valid input print the following to the screen
    if (valid_input==true)
    {
      set_LTC6945_global_frf(temp_fout_MHz,temp_fout_Hz);
      if (temp_fout_MHz < 4294)
      {
        temp_val= temp_fout_MHz*OneMHz + temp_fout_Hz;
        Serial.print(F("Desired Output Frequency is "));
        Serial.print(temp_val);
        Serial.println(F("Hz"));
      }
      else    // over flow condition
      {
        Serial.print(F("Desired Output Frequency is "));
        Serial.print(temp_fout_MHz);
        Serial.print(F("MHz + "));
        Serial.print(temp_fout_Hz);
        Serial.println(F("Hz"));
      }
    }
    // if invalid input print the following to the screen
    else
    {
      Serial.println(F("Invalid Fout frequency chosen"));
      Serial.println(F("Fout must be between the VCO freq range entered above"));
    } // end of if/else (valid_input==true)
  } // end of while(valid_input=false)
} // end of Fout_Freq_Verification


/* ------------------------------------------------------------------------- */
//! Menu 4: Calculates and programs OD and ND based on desired Frf
//!  This function calculates and programs OD and ND based on desired Frf,
//!  the reference frequency, and current RD value.
//!  Linduino One (Arduino Uno) are limited to 32 bit floats, int and doubles.
//!  Significant rounding errors are created with this 32 bit limitation.  Therefore,
//!  This function uses 64bit math functions specifically created to overcome this limitation.
//!  If RD needs to change see menu 2 or menu 3
//! @return void
void menu_4_set_frf()
{
  Serial.print(F("\nThis function calculates and programs OD and ND\n"));
  Serial.print(F("based on the value input for Fvco, Frf and Fref.\n"));
  Serial.print(F("This function assumes all other register settings are correct.\n"));
  Serial.print(F("The PLLWizard tool can verify the correctness of the other register settings.\n"));

  Serial.print(F("\nThe following frequencies will be entered with 2 integers\n"));
  Serial.print(F("1st number is the MHZ portion, the 2nd number is Hz portion\n"));
  Serial.print(F(" - Example: A. 100\n"));
  Serial.print(F("            B. 25\n"));
  Serial.print(F("   equates to 100.000025MHZ\n\n"));
  LTC6945_VCO_Freq_Verification();
  LTC6945_Ref_Freq_Verification();
  LTC6945_Fout_Freq_Verification();
  LTC6945_set_frf();
}


/* ------------------------------------------------------------------------- */
//! Store PLL settings to nonvolatile EEPROM on demo board
//! @return void
void menu_5_store_settings()
{
// Store the PLL Settings to the EEPROM
  uint8_t regval;

  uint8_t addr_offset;
  uint8_t num_reg;

  addr_offset=2;
  num_reg = get_LTC6945_REGSIZE();

  eeprom_write_int16(EEPROM_I2C_ADDRESS, EEPROM_CAL_KEY, EEPROM_CAL_STATUS_ADDRESS);         // Cal key

  for (uint8_t i = 0; i <= num_reg ; i++)
  {
    regval = LTC6945_read(LTC6945_CS,i);
    eeprom_write_byte(EEPROM_I2C_ADDRESS,(char) regval, EEPROM_CAL_STATUS_ADDRESS+ i+addr_offset);
  }
  Serial.println(F("PLL Settings Stored to EEPROM"));

}


/* ------------------------------------------------------------------------- */
//! Read stored PLL settings from nonvolatile EEPROM on demo board
//! @return void
void menu_6_restore_settings()
{
// Read the PLL settings from EEPROM
  int16_t cal_key;
  uint8_t regval;
  uint8_t user_address;

  uint8_t addr_offset;
  uint8_t num_reg;

  addr_offset=2;
  num_reg = get_LTC6945_REGSIZE();

// read the cal key from the EEPROM
  eeprom_read_int16(EEPROM_I2C_ADDRESS, &cal_key, EEPROM_CAL_STATUS_ADDRESS);
  if (cal_key == EEPROM_CAL_KEY)
  {
    // PLL Settings has been stored, read PLL Settings
    user_address=2;
    for (uint8_t i = 0; i <= num_reg ; i++)
    {
      eeprom_read_byte(EEPROM_I2C_ADDRESS,(char *) &regval, EEPROM_CAL_STATUS_ADDRESS + i+addr_offset);
      LTC6945_write(LTC6945_CS, (uint8_t)i, regval);
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
  Serial.println(F("* DC1649 Demonstration Program                                  *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to send data to the LTC6945     *"));
  Serial.println(F("* Ultra Low Noise & Spurious Integer-N Synthesizer.             *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* For loop filter design please use the PLL Wizard software.    *"));
  Serial.println(F("* - It is recommended to use Pll Wizard to determine the        *"));
  Serial.println(F("* correct SPI register values for the initial setup.  These     *"));
  Serial.println(F("* values can be entered into this program via menu option 2     *"));
  Serial.println(F("* below.  These values can then be stored and recalled from the *"));
  Serial.println(F("* DC1649 EEPROM using options 5 and 6 below.                    *"));
  Serial.println(F("*****************************************************************"));
  Serial.println();
} // end of print_title


/* ------------------------------------------------------------------------- */
//!    Prints main menu.
void print_prompt()
{

  Serial.println(F("\nCommand Summary:"));
  Serial.println(F("  1-Load Default Settings (same as the PLLWizard's DC1649_100MHz.pllset settings)"));
  Serial.println(F("  2-READ/WRITE to Registers Addresses"));
  Serial.println(F("  3-READ/WRITE to Registers Fields"));
  Serial.println(F("  4-Set Output Frequency (Frf)"));
  Serial.println(F("  5-Store LTC6945 SPI settings to the DC1649's EEPROM"));
  Serial.println(F("  6-Restore LTC6945 SPI settings from the DC1649's EEPROM"));
  Serial.println("");
  Serial.print(F("Enter a command: "));
} // end of print_prompt

