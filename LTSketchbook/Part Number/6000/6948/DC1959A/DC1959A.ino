/*!
DC1959A
LTC6948: Ultralow Noise and Spurious 0.37GHz to 6.39GHz FracN Synthesizer with Integrated VCO

@verbatim

  Setup:
    Set the terminal baud rate to 115200 and select the newline terminator.
    Refer to Demo Manual DC1959A.
    Ensure all jumpers are installed in the factory default positions.
    Two power supplies are needed for this demo board: a 5v and a 3.3v supply.
    A reference frequency is also needed for this demo board, refer to the
    DC1959 Demo Manual for details.


Command Description:

                             *****Main Menu*****
    1-  Load Default Settings- Loads the SPI map that is identical to file
        LTC6948-x_100MHz.fracnset that is supplied with the FracNWizard and mentioned
        in the DC1959A user's manual.  It assumes a 100MHz reference input and the
        default DC1959A loop filter.  It should output a 907MHz signal on RF+/-.

        ** If you want to use a different loop filter, reference frequency or different
        register settings.  Please use FracNWizard for the loop filter design and initial
        device setup.  The register settings from FracNWizard can be entered into menu option 2.

    2-  READ/WRITE to Registers Addresses- Selecting this option will cause all the registers to
        be read, stored to variables, and displayed.  The user will then have the option
        to write to one register address at a time.

    3-  READ/WRITE to Registers Fields- Selecting this option will allow the user
        to read or write to one register field name at a time.

    4-  This function calculates and programs OD, ND, NUM based on the desired Frf,
        the reference frequency, and the current RD value.  Linduino One (Arduino Uno) are
        limited to 32 bit floats, int and doubles.  Significant rounding errors are created
        with this 32 bit limitation.  Therefore, this function uses 64bit math functions
        specifically created to overcome this limitation.  After OD, ND, and NUM are programmed,
        the program calibrates the LTC6948.  If other register need change see menu 2 or menu 3.

    5-  This function stores the current SPI settings in the demo boards EEPROM

    6-  This function loads SPI settings from the demo boards EEPROM to the device

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC6948

http://www.linear.com/product/LTC6948#demoboards


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
    @ingroup LTC6948
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC6948.h"
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
  char demo_name[] = "DC1959";    // Demo Board Name stored in QuikEval EEPROM
  uint8_t data;

  quikeval_SPI_init();      //! Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();   //! Connect SPI to main data port
  quikeval_I2C_init();      //! Configure the EEPROM I2C port for 100kHz
  Serial.begin(115200);     //! Initialize the serial port to the PC
  LTC6948_init();
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
//!  This function identifies which of the 4 LTC6948 frequency versions are connected.
//!  Based on the version connected, this function loads the register settings referenced
//!  in the DC1959A demo manual's quick start section.
//!  The register settings loaded are the same as FRACN WIZARDS fracnset files
//!  LTC6948-x_100MHz.fracnset (where x=1,2,3,or 4)
//!  The setting loaded with this function assume the LTC6948's reference is set to 100MHz and
//!  the DC1959A's loop filter has not been modified.
//! @return void
void menu_1_load_default_settings()
{

// select which default register setting to load based on part number
  if (demo_board.product_name[8]=='1')   // if this is a LTC6948-1
    set_LTC6948_ALLREGS(LTC6948_CS,0x04,0x04,0x3E,0x6E,0x11,0x10,0x48,0x23,0xD7,0x10,0x1C,0x0D,0x00);
  else if (demo_board.product_name[8]=='2')  // if this is a LTC6948-2
    set_LTC6948_ALLREGS(LTC6948_CS, 0x04,0x04,0x3E,0x5E,0x11,0x10,0x5A,0x2C,0xCC,0xD0,0x1D,0x0D,0xC0);
  else if (demo_board.product_name[8]=='3')  // if this is a LTC6948-3
    set_LTC6948_ALLREGS(LTC6948_CS,0x04,0x04,0x3E,0x4E,0x11,0x10,0x6C,0x35,0xC2,0x90,0x1E,0x0D,0xC0);
  else if (demo_board.product_name[8]=='4')  // if this is a LTC6948-4
    set_LTC6948_ALLREGS(LTC6948_CS,0x04,0x04,0x3E,0x4E,0x11,0x10,0x6C,0x35,0xC2,0x90,0x1E,0x0D,0xC0);
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
//!  - 0- ADDR00 = 0x04 (read only)
//!  - 1- ADDR01 = 0x04
//!  - ....
//!  - 13- ADDR0D = 0x00
//!  - 14- ADDR0E = 0x11 (read only)
//!  - 0 - Return to Main Menu
//!  - Enter a command (1-13 to modify register, or '0' to return to Main Menu):
//! @return void
void menu_2_RW_to_reg_addresss()
{
  uint8_t i, regval, user_regval,num_reg;
  uint16_t user_address;          // User input command

  num_reg = get_LTC6948_REGSIZE();
  user_address=1;
// Read/Write loop, can exit loop by choosing '0'
  while  (user_address != 0)
  {
    Serial.println(F("\n*****************************************************************"));
    // Read All Registers and display results
    for (i=0; i<num_reg; i++)
    {
      regval = LTC6948_read(LTC6948_CS,i);
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
    Serial.print("Enter a command (1-13 to modify register, or '0' to return to Main Menu): ");
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
      LTC6948_write(LTC6948_CS, (uint8_t)user_address, user_regval);
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

  if (get_LTC6948_SPI_FIELD_RW(f)==0)
  {
    field_size=get_LTC6948_SPI_FIELD_NUMBITS(f);
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
//!  - 1-ALCCAL     18-CPMID     35-PART *
//!  - 2-ALCEN      19-CPRST     36-PDALL
//!  - ....
//!  - 16-CPINV     33-OD        50-x
//!  - 17-CPLE      34-OMUTE
//!  - 0 - Return to Main Menu
//!  - * = READ ONLY FIELD
//!  - Enter a command (1-50 to modify register, or '0' to return to Main Menu):
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
    Serial.print(F("1-ALCCAL     18-CPMID     35-PART *\n"));
    Serial.print(F("2-ALCEN      19-CPRST     36-PDALL\n"));
    Serial.print(F("3-ALCHI *    20-CPUP      37-PDFN\n"));
    Serial.print(F("4-ALCLO *    21-CPWIDE    38-PDOUT\n"));
    Serial.print(F("5-ALCMON     22-DITHEN    39-PDPLL\n"));
    Serial.print(F("6-ALCULOK    23-FILT      40-PDVCO\n"));
    Serial.print(F("7-AUTOCAL    24-INTN      41-POR\n"));
    Serial.print(F("8-AUTORST    25-LDOEN     42-RD\n"));
    Serial.print(F("9-BD         26-LDOV      43-REV *\n"));
    Serial.print(F("10-BST       27-LKCT      44-RFO\n"));
    Serial.print(F("11-CAL       28-LKWIN     45-RSTFN\n"));
    Serial.print(F("12-CP        29-LOCK *    46-SEED\n"));
    Serial.print(F("13-CPCHI     30-MTCAL     47-THI *\n"));
    Serial.print(F("14-CPCLO     31-ND        48-TLO *\n"));
    Serial.print(F("15-CPDN      32-NUM       49-UNLOK *\n"));
    Serial.print(F("16-CPINV     33-OD        50-x\n"));
    Serial.print(F("17-CPLE      34-OMUTE\n"));
    Serial.print("0 - Return to Main Menu\n");
    Serial.print("* = READ ONLY FIELD\n\n");

    Serial.print("Enter a command (1-50 to modify register, or '0' to return to Main Menu): ");
    field_num = read_int();  //! Reads the user command
    Serial.println(field_num);

    // User input: enter new setting for selected register
    if (field_num != 0)
    {
      switch (field_num)        //! Prints the appropriate submenu
      {
        case LTC6948_ALCCAL:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_ALCCAL);    // reads selected field
          field_val=field_menu_RW(field_val,"ALCCAL",LTC6948_ALCCAL);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_ALCCAL, field_val); // updates selected field
          }
          break;

        case LTC6948_ALCEN:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_ALCEN);    // reads selected field
          field_val=field_menu_RW(field_val,"ALCEN",LTC6948_ALCEN);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_ALCEN, field_val); // updates selected field
          }
          break;

        case LTC6948_ALCHI:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_ALCHI);    // reads selected field
          field_val=field_menu_RW(field_val,"ALCHI",LTC6948_ALCHI);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_ALCHI, field_val); // updates selected field
          }
          break;

        case LTC6948_ALCLO:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_ALCLO);    // reads selected field
          field_val=field_menu_RW(field_val,"ALCLO",LTC6948_ALCLO);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_ALCLO, field_val); // updates selected field
          }
          break;

        case LTC6948_ALCMON:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_ALCMON);    // reads selected field
          field_val=field_menu_RW(field_val,"ALCMON",LTC6948_ALCMON);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_ALCMON, field_val); // updates selected field
          }
          break;

        case LTC6948_ALCULOK:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_ALCULOK);    // reads selected field
          field_val=field_menu_RW(field_val,"ALCULOK",LTC6948_ALCULOK);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_ALCULOK, field_val); // updates selected field
          }
          break;

        case LTC6948_AUTOCAL:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_AUTOCAL);    // reads selected field
          field_val=field_menu_RW(field_val,"AUTOCAL",LTC6948_AUTOCAL);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_AUTOCAL, field_val); // updates selected field
          }
          break;

        case LTC6948_AUTORST:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_AUTORST);    // reads selected field
          field_val=field_menu_RW(field_val,"AUTORST",LTC6948_AUTORST);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_AUTORST, field_val); // updates selected field
          }
          break;

        case LTC6948_BD:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_BD);    // reads selected field
          field_val=field_menu_RW(field_val,"BD",LTC6948_BD);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_BD, field_val); // updates selected field
          }
          break;

        case LTC6948_BST:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_BST);    // reads selected field
          field_val=field_menu_RW(field_val,"BST",LTC6948_BST);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_BST, field_val); // updates selected field
          }
          break;

        case LTC6948_CAL:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_CAL);    // reads selected field
          field_val=field_menu_RW(field_val,"CAL",LTC6948_CAL);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_CAL, field_val); // updates selected field
          }
          break;

        case LTC6948_CP:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_CP);    // reads selected field
          field_val=field_menu_RW(field_val,"CP",LTC6948_CP);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_CP, field_val); // updates selected field
          }
          break;

        case LTC6948_CPCHI:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_CPCHI);    // reads selected field
          field_val=field_menu_RW(field_val,"CPCHI",LTC6948_CPCHI);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_CPCHI, field_val); // updates selected field
          }
          break;

        case LTC6948_CPCLO:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_CPCLO);    // reads selected field
          field_val=field_menu_RW(field_val,"CPCLO",LTC6948_CPCLO);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_CPCLO, field_val); // updates selected field
          }
          break;

        case LTC6948_CPDN:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_CPDN);    // reads selected field
          field_val=field_menu_RW(field_val,"CPDN",LTC6948_CPDN);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_CPDN, field_val); // updates selected field
          }
          break;

        case LTC6948_CPINV:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_CPINV);    // reads selected field
          field_val=field_menu_RW(field_val,"CPINV",LTC6948_CPINV);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_CPINV, field_val); // updates selected field
          }
          break;

        case LTC6948_CPLE:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_CPLE);    // reads selected field
          field_val=field_menu_RW(field_val,"CPLE",LTC6948_CPLE);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_CPLE, field_val); // updates selected field
          }
          break;

        case LTC6948_CPMID:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_CPMID);    // reads selected field
          field_val=field_menu_RW(field_val,"CPMID",LTC6948_CPMID);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_CPMID, field_val); // updates selected field
          }
          break;

        case LTC6948_CPRST:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_CPRST);    // reads selected field
          field_val=field_menu_RW(field_val,"CPRST",LTC6948_CPRST);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_CPRST, field_val); // updates selected field
          }
          break;

        case LTC6948_CPUP:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_CPUP);    // reads selected field
          field_val=field_menu_RW(field_val,"CPUP",LTC6948_CPUP);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_CPUP, field_val); // updates selected field
          }
          break;

        case LTC6948_CPWIDE:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_CPWIDE);    // reads selected field
          field_val=field_menu_RW(field_val,"CPWIDE",LTC6948_CPWIDE);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_CPWIDE, field_val); // updates selected field
          }
          break;

        case LTC6948_DITHEN:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_DITHEN);    // reads selected field
          field_val=field_menu_RW(field_val,"DITHEN",LTC6948_DITHEN);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_DITHEN, field_val); // updates selected field
          }
          break;

        case LTC6948_FILT:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_FILT);    // reads selected field
          field_val=field_menu_RW(field_val,"FILT",LTC6948_FILT);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_FILT, field_val); // updates selected field
          }
          break;

        case LTC6948_INTN:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_INTN);    // reads selected field
          field_val=field_menu_RW(field_val,"INTN",LTC6948_INTN);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_INTN, field_val); // updates selected field
          }
          break;

        case LTC6948_LDOEN:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_LDOEN);    // reads selected field
          field_val=field_menu_RW(field_val,"LDOEN",LTC6948_LDOEN);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_LDOEN, field_val); // updates selected field
          }
          break;

        case LTC6948_LDOV:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_LDOV);    // reads selected field
          field_val=field_menu_RW(field_val,"LDOV",LTC6948_LDOV);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_LDOV, field_val); // updates selected field
          }
          break;

        case LTC6948_LKCT:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_LKCT);    // reads selected field
          field_val=field_menu_RW(field_val,"LKCT",LTC6948_LKCT);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_LKCT, field_val); // updates selected field
          }
          break;

        case LTC6948_LKWIN:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_LKWIN);    // reads selected field
          field_val=field_menu_RW(field_val,"LKWIN",LTC6948_LKWIN);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_LKWIN, field_val); // updates selected field
          }
          break;

        case LTC6948_LOCK:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_LOCK);    // reads selected field
          field_val=field_menu_RW(field_val,"LOCK",LTC6948_LOCK);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_LOCK, field_val); // updates selected field
          }
          break;

        case LTC6948_MTCAL:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_MTCAL);    // reads selected field
          field_val=field_menu_RW(field_val,"MTCAL",LTC6948_MTCAL);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_MTCAL, field_val); // updates selected field
          }
          break;

        case LTC6948_ND:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_ND);    // reads selected field
          field_val=field_menu_RW(field_val,"ND",LTC6948_ND);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_ND, field_val); // updates selected field
          }
          break;

        case LTC6948_NUM:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_NUM);    // reads selected field
          field_val=field_menu_RW(field_val,"NUM",LTC6948_NUM);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_NUM, field_val); // updates selected field
          }
          break;

        case LTC6948_OD:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_OD);    // reads selected field
          field_val=field_menu_RW(field_val,"OD",LTC6948_OD);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_OD, field_val); // updates selected field
          }
          break;

        case LTC6948_OMUTE:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_OMUTE);    // reads selected field
          field_val=field_menu_RW(field_val,"OMUTE",LTC6948_OMUTE);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_OMUTE, field_val); // updates selected field
          }
          break;

        case LTC6948_PART:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_PART);    // reads selected field
          field_val=field_menu_RW(field_val,"PART",LTC6948_PART);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_PART, field_val); // updates selected field
          }
          break;

        case LTC6948_PDALL:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_PDALL);    // reads selected field
          field_val=field_menu_RW(field_val,"PDALL",LTC6948_PDALL);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_PDALL, field_val); // updates selected field
          }
          break;

        case LTC6948_PDFN:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_PDFN);    // reads selected field
          field_val=field_menu_RW(field_val,"PDFN",LTC6948_PDFN);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_PDFN, field_val); // updates selected field
          }
          break;

        case LTC6948_PDOUT:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_PDOUT);    // reads selected field
          field_val=field_menu_RW(field_val,"PDOUT",LTC6948_PDOUT);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_PDOUT, field_val); // updates selected field
          }
          break;

        case LTC6948_PDPLL:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_PDPLL);    // reads selected field
          field_val=field_menu_RW(field_val,"PDPLL",LTC6948_PDPLL);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_PDPLL, field_val); // updates selected field
          }
          break;

        case LTC6948_PDVCO:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_PDVCO);    // reads selected field
          field_val=field_menu_RW(field_val,"PDVCO",LTC6948_PDVCO);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_PDVCO, field_val); // updates selected field
          }
          break;

        case LTC6948_POR:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_POR);    // reads selected field
          field_val=field_menu_RW(field_val,"POR",LTC6948_POR);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_POR, field_val); // updates selected field
          }
          break;

        case LTC6948_RD:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_RD);    // reads selected field
          field_val=field_menu_RW(field_val,"RD",LTC6948_RD);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_RD, field_val); // updates selected field
          }
          break;

        case LTC6948_REV:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_REV);    // reads selected field
          field_val=field_menu_RW(field_val,"REV",LTC6948_REV);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_REV, field_val); // updates selected field
          }
          break;

        case LTC6948_RFO:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_RFO);    // reads selected field
          field_val=field_menu_RW(field_val,"RFO",LTC6948_RFO);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_RFO, field_val); // updates selected field
          }
          break;

        case LTC6948_RSTFN:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_RSTFN);    // reads selected field
          field_val=field_menu_RW(field_val,"RSTFN",LTC6948_RSTFN);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_RSTFN, field_val); // updates selected field
          }
          break;

        case LTC6948_SEED:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_SEED);    // reads selected field
          field_val=field_menu_RW(field_val,"SEED",LTC6948_SEED);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_SEED, field_val); // updates selected field
          }
          break;

        case LTC6948_THI:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_THI);    // reads selected field
          field_val=field_menu_RW(field_val,"THI",LTC6948_THI);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_THI, field_val); // updates selected field
          }
          break;

        case LTC6948_TLO:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_TLO);    // reads selected field
          field_val=field_menu_RW(field_val,"TLO",LTC6948_TLO);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_TLO, field_val); // updates selected field
          }
          break;

        case LTC6948_UNLOK:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_UNLOK);    // reads selected field
          field_val=field_menu_RW(field_val,"UNLOK",LTC6948_UNLOK);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_UNLOK, field_val); // updates selected field
          }
          break;

        case LTC6948_x:
          field_val=get_LTC6948_SPI_FIELD(LTC6948_CS,LTC6948_x);    // reads selected field
          field_val=field_menu_RW(field_val,"x",LTC6948_x);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6948_SPI_FIELD(LTC6948_CS, LTC6948_x, field_val); // updates selected field
          }
          break;
      }  // end of switch statement
    } // end if user_command != 0 statement
  } // end while loop
}  // end menu_3_RW_to_reg_field function

/* ------------------------------------------------------------------------- */
//! verifies reference frequency is within datasheet specifications
void LTC6948_Ref_Freq_Verification()
{
  unsigned long temp_val, temp_fref_MHz, temp_fref_Hz;
  boolean valid_input=false;

// USER INPUT
  valid_input=false;


  while (valid_input==false)
  {
    temp_fref_MHz=get_LTC6948_global_fref_MHz();
    temp_fref_Hz=get_LTC6948_global_fref_Hz();
    Serial.print(F("\nThe Reference frequency will be entered with 2 integers\n"));
    Serial.print(F("1st number is the MHZ portion, the 2nd number is Hz portion\n"));
    Serial.print(F(" - Example: A. 100\n"));
    Serial.print(F("            B. 25\n"));
    Serial.print(F("   equates to 100.000025MHZ\n\n"));
    Serial.print(F("A. What is the MHz portion of the Reference Input Frequency(MHZ)? ["));
    Serial.print(temp_fref_MHz);
    Serial.print(F("]: "));
    temp_val = read_float();  //! Reads the user command
    // if user selects enter, keep same Fref.  Otherwise set Fref and verify
    if (temp_val!=0) temp_fref_MHz = abs(temp_val);
    Serial.println(temp_fref_MHz);

    Serial.print(F("B. What is the sub-MHz portion of the Reference Input Frequency(HZ)? "));
    temp_val = read_float();  //! Reads the user command
    temp_fref_Hz = abs(temp_val);
    Serial.println(temp_fref_Hz);

    // if valid input print the following to the screen
    if (temp_fref_MHz >=LTC6948_MIN_REF_FREQ & temp_fref_MHz <= LTC6948_MAX_REF_FREQ)
    {
      set_LTC6948_global_fref(temp_fref_MHz,temp_fref_Hz);
      temp_val= temp_fref_MHz*OneMHz + temp_fref_Hz;
      Serial.print(F("Reference Frequency set to "));
      Serial.print(temp_val);
      Serial.println(F("Hz"));
      valid_input=true;
    }
    else
    {
      Serial.print(F("Reference Frequency must be between 10MHz and 425MHz\n"));
    }  // end of if-else
  } // end of while
}  // end of LTC6948_Ref_Freq_Verification


/* ------------------------------------------------------------------------- */
//! verifies frf frequency is within datasheet specifications
void LTC6948_Fout_Freq_Verification(char part_version[])
{
  unsigned long odiv, temp_fout_MHz, temp_fout_Hz, temp_val;
  unsigned long frf[2];
  boolean valid_input=false;

// USER INPUT
  temp_fout_MHz=get_LTC6948_global_frf_MHz();
  temp_fout_Hz=get_LTC6948_global_frf_Hz();

  while (valid_input==false)
  {
    Serial.print(F("\nThe Output Frequency (Frf) will be entered with 2 integers\n"));
    Serial.print(F("1st number is the MHZ portion, the 2nd number is Hz portion\n"));
    Serial.print(F("C. What is the MHz portion of the Output Frequency(MHZ)? ["));
    Serial.print(temp_fout_MHz);
    Serial.print(F("]: "));
    temp_val = read_int();  //! Reads the user command
    // if user selects enter, keep same Fout.  Otherwise set Fout and verify
    if (temp_val!=0) temp_fout_MHz = abs(temp_val);
    Serial.println(temp_fout_MHz);

    Serial.print(F("D. What is the Hz portion of the Output Frequency(HZ)? "));
    temp_val = read_int();  //! Reads the user command
    temp_fout_Hz = abs(temp_val);
    Serial.println(temp_fout_Hz);

    HZto64(frf,temp_fout_MHz,temp_fout_Hz);  // convert to 64 bit integer

    // verify desired frequency falls within a divider range (1-6)
    odiv = LTC6948_calc_odiv(part_version, frf);
    valid_input=false;
    if ((odiv>=1) && (odiv<=6)) valid_input=true;

    // if valid input print the following to the screen
    if (valid_input==true)
    {
      set_LTC6948_global_frf(temp_fout_MHz,temp_fout_Hz);
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
    } // end of if/else (valid_input==true)
  } // end of while(valid_input=false)
} // end of Fout_Freq_Verification


/* ------------------------------------------------------------------------- */
//! Menu 4: Calculates and programs OD, ND, NUM based on desired Frf
//!  This function calculates and programs OD, ND, NUM based on desired Frf,
//!  the reference frequency, and current RD value.
//!  Linduino One (Arduino Uno) are limited to 32 bit floats, int and doubles.
//!  Significant rounding errors are created with this 32 bit limitation.  Therefore,
//!  This function uses 64bit math functions specifically created to overcome this limitation.
//!  After OD, ND, and NUM are programmed, the program calibrates the LTC6948
//!  If RD needs to change see menu 2 or menu 3
//! @return void
void menu_4_set_frf()
{
  Serial.print(F("\nThis function calculates and programs OD, ND, NUM\n"));
  Serial.print(F("based on the value input for Frf and Fref.  It then calibrates the part.\n"));
  Serial.print(F("It assumes all other register settings are correct\n"));
  Serial.print(F("The FracNWizard tool can verify the correctness of the other register settings.\n"));

  LTC6948_Ref_Freq_Verification();
  LTC6948_Fout_Freq_Verification(demo_board.product_name);
  LTC6948_set_frf(demo_board.product_name);
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
  num_reg = get_LTC6948_REGSIZE();

  eeprom_write_int16(EEPROM_I2C_ADDRESS, EEPROM_CAL_KEY, EEPROM_CAL_STATUS_ADDRESS);         // Cal key

  for (uint8_t i = 0; i <= num_reg ; i++)
  {
    regval = LTC6948_read(LTC6948_CS,i);
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
  num_reg = get_LTC6948_REGSIZE();

// read the cal key from the EEPROM
  eeprom_read_int16(EEPROM_I2C_ADDRESS, &cal_key, EEPROM_CAL_STATUS_ADDRESS);
  if (cal_key == EEPROM_CAL_KEY)
  {
    // PLL Settings has been stored, read PLL Settings
    user_address=2;
    for (uint8_t i = 0; i <= num_reg ; i++)
    {
      eeprom_read_byte(EEPROM_I2C_ADDRESS,(char *) &regval, EEPROM_CAL_STATUS_ADDRESS + i+addr_offset);
      LTC6948_write(LTC6948_CS, (uint8_t)i, regval);
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
  Serial.println(F("* DC1959 Demonstration Program                                  *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to send data to the LTC6948     *"));
  Serial.println(F("* Ultra Low Noise & Spurious FracN Synthesizer with             *"));
  Serial.println(F("* Integrated VCO.                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* For loop filter design please use the FracN Wizard software.  *"));
  Serial.println(F("* - It is recommended to use FracNWizard to determine the       *"));
  Serial.println(F("* correct SPI register values for the initial setup.  These     *"));
  Serial.println(F("* values can be entered into this program via menu option 2     *"));
  Serial.println(F("* below.  These values can then be stored and recalled from the *"));
  Serial.println(F("* DC1959 EEPROM using options 5 and 6 below.                    *"));
  Serial.println(F("*****************************************************************"));
  Serial.println();
} // end of print_title


/* ------------------------------------------------------------------------- */
//!    Prints main menu.
void print_prompt()
{

  Serial.println(F("\nCommand Summary:"));
  Serial.println(F("  1-Load Default Settings (same as the FracN Wizard's LTC6948-x_100MHz.fracnset settings)"));
  Serial.println(F("  2-READ/WRITE to Registers Addresses"));
  Serial.println(F("  3-READ/WRITE to Registers Fields"));
  Serial.println(F("  4-Set Output Frequency (Frf)"));
  Serial.println(F("  5-Store LTC6948 SPI settings to the DC1959's EEPROM"));
  Serial.println(F("  6-Restore LTC6948 SPI settings from the DC1959's EEPROM"));
  Serial.println("");
  Serial.print(F("Enter a command: "));
} // end of print_prompt


