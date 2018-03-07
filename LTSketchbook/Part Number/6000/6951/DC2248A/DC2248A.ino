/*!
DC2248A
LTC6951: Ultra-Low Jitter 2MHz to 2.7GHz Multi-Output Clock Synthesizer with Integrated VCO

@verbatim

  Setup:
    Set the terminal baud rate to 115200 and select the newline terminator.
    Refer to Demo Manual DC2248A.
    Ensure all jumpers are installed in the factory default positions.
    One Power Supply is needed for this demo board: 6V supply.


Command Description:
    1-  Load Default Settings from a Look-up Table- Loads the SPI map from one of 10 options in a look-up table.
        The look-up table allows the user to select different frequencies and synchronization option.  All options
        assume a 100MHz reference input and a default DC2248A BOM.

        ** If you want to use a different loop filter, reference frequency or different
        register settings.  Please use LTC6951Wizard for the loop filter design and initial
        device setup.  The register settings from LTC6951Wizard can be entered into menu option 2
        or you can change the default registers in the LTC6951_lkup_tbl array.  After the
        LTC6951Wizard settings are created, export the register settings by selecting
        File --> Export Register Settings from the TLC6951Wizard Menu.

    2-  READ/WRITE to Registers Addresses- Selecting this option will cause all the registers to
        be read, stored to variables, and displayed.  The user will then have the option
        to write to one register address at a time.

    3-  READ/WRITE to Registers Fields- Selecting this option will allow the user
        to read or write to one register field name at a time.

    4-  This function stores the current SPI settings in the demo boards EEPROM

    5-  This function loads SPI settings from the demo boards EEPROM to the device

    6-  Frequency chnage Timing test (SPI write + Calibration + Settling Time):
        Provides two frequency settings and allows user to toggle between the two frequencies by
        changing the N-divider(ND) value.  The user can monitor the STATUS pin.  The user
        can trigger off the STATUS or CS pins. In most cases the user will want to set the SPI
        frequency to 8MHz (see menu option 7)

     7- SPI Frequency:  Allows user to control Linduino's SPI frequency.  By default this is set to 8MHz
        when connecting Linduino directly to the DC2248.  If Linduino is connected to the DC2430 the default
        SPI rate is 4MHz

     8- If using a DC2430 (Linduino Expander), this allows the user to select 1 of 8 sites to talk to.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC6951

http://www.linear.com/product/LTC6951#demoboards


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
    @ingroup LTC6951
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC6951.h"
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
void menu_6_multibytewrite();
void menu_7_SPI_speed();
void menu_8_DC2430_site_select();
//void DUTsync();

// Global Variables
static int8_t demo_board_connected = 0;    //!< Demo Board Name stored in QuikEval EEPROM
static int8_t dc2248_board_connected = 0;  //!< Demo Board Name stored in QuikEval EEPROM
uint8_t First_Run=0;                       //!< if first time through loop = 0, otherwise=1
boolean Using_DC2430= false;               //!< Indicator if DC2430 (Linduino Expander) is connected

// Analog Pins used on DC2430 (Linduino Expander)
int dc2430_site_A0_APin =0;                //!< DC2430 resistor tree, if it reads 3.3V then DC2430 connected
int dc2430_site_A1_APin =1;                //!< DC2430 resistor tree, if it reads 1.66V then DC2430 connected
int dc2430_site_A2_APin =2;                //!< DC2430 INFO ONLY, measures DC2226 (Linduino) VCCIO voltage

// Digital Pins used on DC2430 (Linduino Expander)
int dc2430_site_DA0_Pin =2;                //!< DC2430 digital pin allows Linduino to control which DC2430 site is selected
int dc2430_site_DA1_Pin =3;                //!< DC2430 digital pin allows Linduino to control which DC2430 site is selected
int dc2430_site_DA2_Pin =4;                //!< DC2430 digital pin allows Linduino to control which DC2430 site is selected
int dc2430_en_Pin =5;                      //!< DC2430 digital pin enables Linduino to control which DC2430 site is selected & turns on DC2430 LED

/* ------------------------------------------------------------------------- */
//! Initialize Linduino
//! @return void
void setup()
{
  char demo_name[] = "DC2248";    // Demo Board Name stored in QuikEval EEPROM
  uint8_t data;
  int site_select=0;
  int dc2430_A0_val=0;
  int dc2430_A1_val=0;
  int dc2430_A2_val=0;
  float dc2430_A0_fval=0;
  float dc2430_A1_fval=0;
  float dc2430_A2_fval=0;

  Using_DC2430= false;

  quikeval_SPI_init();      //! Configure the spi port for 500kHz SCK
  quikeval_SPI_connect();   //! Connect SPI to main data port
  quikeval_I2C_init();      //! Configure the EEPROM I2C port for 100kHz
  Serial.begin(115200);     //! Initialize the serial port to the PC
  LTC6951_init();
  print_title();

//! **************** DETECT IF USING DC2430 SECTION ******************
//! this checks to see if a DC2026 (linduino) to DC2430 (linduino expander)
//! by measuring 3 analog voltages
//! if a DC2430 is connect then read A0, A1 , A2
  dc2430_A0_val = analogRead(dc2430_site_A0_APin);
  dc2430_A0_fval = (5.0*dc2430_A0_val)/1023;

  dc2430_A1_val = analogRead(dc2430_site_A1_APin);
  dc2430_A1_fval = (5.0*dc2430_A1_val)/1023;


//! If DC2430 located, auto search site 0 to 7 until a DC2248 is found
  if ((dc2430_A0_fval>3.0) && (dc2430_A0_fval<3.6) && (dc2430_A1_fval>1.5) && (dc2430_A1_fval<1.8))
  {
    Serial.println(F("--- DC2430 Connected ---"));
    Using_DC2430= true;
    pinMode(dc2430_site_DA0_Pin, OUTPUT);
    pinMode(dc2430_site_DA1_Pin, OUTPUT);
    pinMode(dc2430_site_DA2_Pin, OUTPUT);
    pinMode(dc2430_en_Pin, OUTPUT);      // sets the digital pin as output
    dc2248_board_connected =0;
    demo_board_connected = 0;

    digitalWrite(dc2430_en_Pin, HIGH);   // sets the LED on
    site_select=0;
    do
    {
      dc2430_site_select(site_select);
      delay(150);  //! probably could be shorter, but this allows the eyes to track which site is being tested
      demo_board_connected = discover_demo_board_local(demo_name);  //! Checks if any demo board is connected.
      if (demo_board_connected)
      {
        Serial.println(F("\n********************************************************"));
        dc2248_board_connected = discover_demo_board(demo_name);  //! Checks if correct demo board is connected.

        if (dc2248_board_connected)
        {
          dc2430_A2_val = analogRead(dc2430_site_A2_APin);
          dc2430_A2_fval = (5.0*dc2430_A2_val)/1023;
          Serial.print(F("Linduino's JP3 VCCIO voltage = "));
          Serial.println(dc2430_A2_fval);     //! if DC2430 connect, VCCIO voltage set by JP3 on DC2026
          spi_enable(SPI_CLOCK_DIV4);  //! 1) Configure the spi port for 4MHz SCK
          Serial.println(F("Linduino's SPI Frequency = 4MHz, max DC2430 SPI speed"));
          Serial.print(F("\nDC2248 FOUND ON DC2430 SITE "));
          Serial.println(site_select);
          site_select=15;
        }
      }

      if (!dc2248_board_connected)
      {
        Serial.print(F("\nDC2248 NOT FOUND ON DC2430 SITE "));
        Serial.print(site_select);
        site_select++;
      }
    }
    while (site_select<8);

    //! if no DC2248A was found.  Return dc2430_en_Pin to default state (LOW)
    if (!dc2248_board_connected)
    {
      digitalWrite(dc2430_en_Pin, LOW);   // sets the LED on
      Serial.println(F("\n\nConnect DC2248 demo board to DC2430, then press the reset button."));
    }
  }
  else
  {
    //! If DC2430 is not located, check to see if Linduino is connected directly to the DC2248
    //! most common use case
    Serial.println(F("DC2248A CONNECTED TO LINDUINO"));
    dc2248_board_connected = discover_demo_board(demo_name);  //! Checks if correct demo board is connected.
    if (dc2248_board_connected)
    {
      Serial.println(F("Linduino's SPI Frequency = 8MHz, max speed"));
      spi_enable(SPI_CLOCK_DIV2);  //! 1) Configure the spi port for 8MHz SCK
    }
  }

  if (dc2248_board_connected)
  {
    print_prompt();
  }
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

      case 6:
        menu_6_multibytewrite();
        break;

      case 7:
        menu_7_SPI_speed();
        break;

      case 8:
        menu_8_DC2430_site_select();
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
//!  This function loads the register settings from a look up table.
//!  The setting loaded with this function assume the LTC6951's
//!  reference is set to 100MHz and the DC2248A's BOM has not been modified.
//!  The settings in the look up table were created using the LTC6951Wizard.
//!  It is recommended to use the LTC6951Wizard to create the register settings for
//!  all desired frequency plans and enter these frequency plans into a look-up table.
//!  The LTC6951Wizard generates register values for optimal LTC6951 performance.
//! @return void
void menu_1_load_default_settings()
{

  uint8_t  field_num;
  long field_val;

  field_val=999L;
  field_num=1;
// Read/Write loop, can exit loop by choosing 'm'
  while  (field_num != 0)
  {
    Serial.print(F("\n*****************************************************************"));
    Serial.print(F("\n*************    Select LTC6951 Configuration   *****************"));
    Serial.println(F("\n*****************************************************************"));

    // Select Fields to read and write to
    Serial.print(F("1-  OUT[4:0] = 600MHZ, EZSync\n"));
    Serial.print(F("2-  OUT[4:0] = 600MHZ, EZSync Ref Aligned\n"));
    Serial.print(F("3-  OUT[4:0] = 600MHZ, EZParallelSync\n"));
    Serial.print(F("4-  OUT[4:0] = 600MHZ, ParallelSync\n"));
    Serial.print(F("5-  OUT[3:0] = 2500MHZ, OUT4:625MHz, EZSync\n"));
    Serial.print(F("6-  OUT[3:0] = 2500MHZ, OUT4:625MHz, EZSync Ref Aligned\n"));
    Serial.print(F("7-  OUT[3:1] = 2400MHZ, OUT0&OUT4 = 600MHz, EZParallelSync\n"));
    Serial.print(F("8-  OUT[3:0] = 2500MHZ, OUT4 = 625MHz, ParallelSync\n"));
    Serial.print(F("9-  OUT[4:0] = Same frequencies as DC2226 U10, JESD204B SC1, ParallelSync\n"));
    Serial.print(F("10- OUT[4:0] = Same frequencies as DC2226 U13, JESD204B SC1, ParallelSync\n"));
    Serial.println(F("0 - Return to Main Menu\n"));

    Serial.println(F("Load LTC6951 register settings from a look up table (Settings created from LTC6951Wizard)"));
    Serial.println(F("Enter a command (1-10), or '0' to return to Main Menu): "));
    field_num = read_int();  //! Reads the user command
    Serial.println(field_num);

    // User input: enter new setting for selected register
    if (field_num != 0)
    {
      field_num=field_num-1;
      set_LTC6951_REGS_lkup_tbl(field_num);
      field_num=field_num+1;
    } // end if user_command != 0 statement
  } // end while loop
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
//!  - 21- ADDR15 = 0x04
//!  - 22- ADDR16 = 0x65 (read only)
//!  - 0 - Return to Main Menu
//!  - Enter a command (1-21 to modify register, or '0' to return to Main Menu):
//! @return void
void menu_2_RW_to_reg_addresss()
{
  uint8_t i, regval, user_regval, num_reg;
  uint16_t user_address;          // User input command

  num_reg = get_LTC6951_REGSIZE();
  user_address=1;
// Read/Write loop, can exit loop by choosing '0'
  while  (user_address != 0)
  {
    Serial.println(F("\n*****************************************************************"));
    // Read All Registers and display results
    for (i=0; i<num_reg; i++)
    {
      regval = LTC6951_read(LTC6951_CS,i);
      Serial.print(i);
      if (i<16)
        Serial.print(F("- ADDR0"));
      else
        Serial.print(F("- ADDR"));
      Serial.print(i, HEX);
      Serial.print(F(" = 0x"));
      if (regval<16) Serial.print(F("0"));
      Serial.print(regval, HEX);
      if (i==3) Serial.print(F(" (warning: if D2=1 it resets all registers. RES6951 Bit)"));
      if ((i==0)||(i==(num_reg-1))) Serial.print(F(" (read only) "));
      Serial.println("");
    }  // end for loop
    Serial.print(F("0 - Return to Main Menu\n\n"));
    // User input: Select which register to modify, or return to main menu
    Serial.print(F("Enter a command (1-19 to modify register, or '0' to return to Main Menu): "));
    user_address = read_int();  //! Reads the user command
    Serial.println(user_address);

    // User input: enter new setting for selected register
    if (user_address >0 && user_address<(num_reg-1))
    {
      Serial.print(F("What value should ADDR"));
      Serial.print(user_address);
      Serial.print(F(" be set to (ex: HEX format 0xff): "));
      user_regval = read_int();  //! Reads the user command
      Serial.println(user_regval);

      // writes new setting to part
      LTC6951_write(LTC6951_CS, (uint8_t)user_address, user_regval);
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
                   uint8_t f            //!< SPI field identifier identifies selected fields information in SPI MAP arrays
                  )
{
  long usr_field_val;
  uint8_t field_size, i;
  long max_num=1, pow2=1;

  Serial.print("CURRENT STATE (HEX): ");
  Serial.print(field_name);
  Serial.print("= 0x");
  Serial.println(field_val, HEX);

  if (get_LTC6951_SPI_FIELD_RW(f)==0)
  {
    field_size=get_LTC6951_SPI_FIELD_NUMBITS(f);
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
//!  - 1-ALCCAL     25-LKWIN      49-PART *
//!  - 2-ALCEN      26-LOCK *     50-PD
//!  - ....
//!  - 21-DLY4      45-OINV1      69-SYNCEN4
//!  - 23-INVSTAT   47-OINV3      71-x
//!  - 24-LKCT      48-OINV4
//!  - 0 - Return to Main Menu
//!  - * = READ ONLY FIELD
//!  - Enter a command (1-71 to modify register, or '0' to return to Main Menu):
//! @return void
void menu_3_RW_to_reg_field()
{
  uint8_t  field_num;
  long field_val;

  field_val=999L;
  field_num=1;
// Read/Write loop, can exit loop by choosing 'm'
  while  (field_num != 0)
  {
    Serial.println(F("\n*****************************************************************"));
    // Select Fields to read and write to
    Serial.print(F("1-ALCCAL     25-LKWIN      49-PART *\n"));
    Serial.print(F("2-ALCEN      26-LOCK *     50-PD\n"));
    Serial.print(F("3-ALCHI *    27-!LOCK *    51-PDALL\n"));
    Serial.print(F("4-ALCLO *    28-MCO        52-PDOUT\n"));
    Serial.print(F("5-ALCMON     29-MC1        53-PDPLL\n"));
    Serial.print(F("6-ALCULOK    30-MC2        54-PDREFPK\n"));
    Serial.print(F("7-AUTOCAL    31-MC3        55-PDVCO\n"));
    Serial.print(F("8-BD         32-MC4        56-POR\n"));
    Serial.print(F("9-BST        33-MD0        57-RAO\n"));
    Serial.print(F("10-CAL       34-MD1        58-RD\n"));
    Serial.print(F("11-CP        35-MD2        59-REFOK *\n"));
    Serial.print(F("12-CPDN      36-MD3        60-!REFOK *\n"));
    Serial.print(F("13-CPMID     37-MD4        61-REV *\n"));
    Serial.print(F("14-CPRST     38-MUTE0      62-SN\n"));
    Serial.print(F("15-CPUP      39-MUTE1      63-SR\n"));
    Serial.print(F("16-CPWIDE    40-MUTE2      64-SSYNC\n"));
    Serial.print(F("17-DLY0      41-MUTE3      65-SYNCEN0\n"));
    Serial.print(F("18-DLY1      42-MUTE4      66-SYNCEN1\n"));
    Serial.print(F("19-DLY2      43-ND         67-SYNCEN2\n"));
    Serial.print(F("20-DLY3      44-OINV0      68-SYNCEN3\n"));
    Serial.print(F("21-DLY4      45-OINV1      69-SYNCEN4\n"));
    Serial.print(F("22-FILT      46-OINV2      70-UNLOCK *\n"));
    Serial.print(F("23-INVSTAT   47-OINV3      71-x\n"));
    Serial.print(F("24-LKCT      48-OINV4    \n"));

    Serial.print(F("0 - Return to Main Menu\n"));
    Serial.print(F("* = READ ONLY FIELD\n\n"));

    Serial.print(F("Enter a command (1-71 to modify register, or '0' to return to Main Menu): "));
    field_num = read_int();  //! Reads the user command
    Serial.println(field_num);

    // User input: enter new setting for selected register
    if (field_num != 0)
    {
      switch (field_num)        //! Prints the appropriate submenu
      {
        default:
          field_val=get_LTC6951_SPI_FIELD(LTC6951_CS,field_num);    // reads selected field
          field_val=field_menu_RW(field_val," ",field_num);      // user interface control and printout
          if (field_val>-1)
          {
            set_LTC6951_SPI_FIELD(LTC6951_CS, field_num, field_val); // updates selected field
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
  num_reg = get_LTC6951_REGSIZE();

  eeprom_write_int16(EEPROM_I2C_ADDRESS, EEPROM_CAL_KEY, EEPROM_CAL_STATUS_ADDRESS);         // Cal key

  for (uint8_t i = 0; i <= num_reg ; i++)
  {
    regval = LTC6951_read(LTC6951_CS,i);
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
  long field_val;
  uint8_t  field_num;

  addr_offset=2;
  num_reg = get_LTC6951_REGSIZE();

// read the cal key from the EEPROM
  eeprom_read_int16(EEPROM_I2C_ADDRESS, &cal_key, EEPROM_CAL_STATUS_ADDRESS);
  if (cal_key == EEPROM_CAL_KEY)
  {
    // PLL Settings has been stored, read PLL Settings
    user_address=2;
    for (uint8_t i = 0; i <= num_reg ; i++)
    {
      eeprom_read_byte(EEPROM_I2C_ADDRESS,(char *) &regval, EEPROM_CAL_STATUS_ADDRESS + i+addr_offset);
      LTC6951_write(LTC6951_CS, (uint8_t)i, regval);
      user_address++;
    }
    Serial.println(F("LTC6951 Settings Restored"));

    Serial.println(F("Calibrate LTC6951 VCO"));
    set_LTC6951_SPI_FIELD(LTC6951_CS, LTC6951_CAL, 1L);
    delay(1); // Calibrations typically ~50us long, CAL bit clears automatically

    Serial.println(F("Syncs LTC6951 OUTPUTS"));
    DUTsync(1);

  }
  else
  {
    Serial.println(F("PLL Settings not found"));
  }

}

/* ------------------------------------------------------------------------- */
//! Syncs LTC6951 Outputs using SSYNC bit
//! @return void
void DUTsync(uint16_t delay_num)
{
  set_LTC6951_SPI_FIELD(LTC6951_CS, LTC6951_SSYNC, 1L);
  delay(1);
  set_LTC6951_SPI_FIELD(LTC6951_CS, LTC6951_SSYNC, 0L);
  delay(delay_num);
}

/* ------------------------------------------------------------------------- */
//! Frequency change Timing test (SPI write + Calibration + Settling Time):
//! Provides two frequency settings and allows user to toggle between the two frequencies by
//! changing the N-divider(ND) value.  The user can monitor the STATUS pin.  The user
//! can trigger off the STATUS or CS pins. In most cases the user will want to set the SPI
//! frequency to 8MHz (see menu option 7)
//! @return void
void menu_6_multibytewrite()
{
  uint8_t init_setup;
  uint8_t lkup_tbl_row;          // User input command

  lkup_tbl_row=1;
  init_setup=1;

// Read/Write loop, can exit loop by choosing '0'
  while  (lkup_tbl_row != 0)
  {
    Serial.println(F("\n*****************************************************************"));
    Serial.println(F("This option is good for measuring calibration and settling time"));
    Serial.println(F("It allows one to hop back and forth between 2 frequencies"));
    Serial.println(F("For different frequencies - modify the lookup table in function"));
    Serial.println(F("Fset_LTC6951_REGS_freq_jump.  LTC6951Wizard exports register settings"));
    Serial.println(F("in the correct format for an easy modification"));
    Serial.println(F("For best timing results, set the SPI clock frequency to 8MHz (main menu: option 7)"));

    // Read All Registers and display results
    Serial.println(F("1 - Frequency 1: 600MHz"));
    Serial.println(F("2 - Frequency 2: 550MHz\n"));
    Serial.print(F("0 - Return to Main Menu\n\n"));
    Serial.print(F("Select an Option (0,1, or 2) : "));
    lkup_tbl_row = read_int();  //! Reads the user command
    Serial.println(lkup_tbl_row);

    if ((lkup_tbl_row >2)|| (lkup_tbl_row <0))
    {
      Serial.println(F("Not an option - try again"));
    }
    else if (lkup_tbl_row == 0)
    {
      // do nothing
    }
    else
    {
      set_LTC6951_REGS_freq_jump((lkup_tbl_row-1), init_setup);
      init_setup=0;
    }
  } // end while loop
}


/* ------------------------------------------------------------------------- */
//! Read stored PLL settings from nonvolatile EEPROM on demo board
//! @return void
void menu_7_SPI_speed()
{
  int temp_val;

  Serial.println(F("\nCommand Summary:"));
  if (Using_DC2430)
  {
    Serial.println(F("  1-8MHz NOT Available with DC2430"));
  }
  else
  {
    Serial.println(F("  1-SPI Clock = 8MHz"));
  }
  Serial.println(F("  2-SPI Clock = 4MHz"));
  Serial.println(F("  3-SPI Clock = 2MHz"));
  Serial.println(F("  4-SPI Clock = 1MHz"));
  Serial.println(F("  5-SPI Clock = 500kHz"));
  Serial.println("");
  Serial.print(F("Enter a command: "));
  temp_val = read_int();  //! Reads the user command

  switch (temp_val)        //! Prints the appropriate submenu
  {
    case 1:
      if (Using_DC2430)
      {
        Serial.println(F("incorrect option"));
      }
      else
      {
        spi_enable(SPI_CLOCK_DIV2);  //! 1) Configure the spi port for 8MHz SCK
      }
      break;

    case 2:
      spi_enable(SPI_CLOCK_DIV4);  //! 1) Configure the spi port for 4MHz SCK
      break;

    case 3:
      spi_enable(SPI_CLOCK_DIV8);  //! 1) Configure the spi port for 2MHz SCK
      break;

    case 4:
      spi_enable(SPI_CLOCK_DIV16);  //! 1) Configure the spi port for 1MHz SCK
      break;

    case 5:
      spi_enable(SPI_CLOCK_DIV32);  //! 1) Configure the spi port for 500kHz SCK
      break;

    default:
      Serial.println(F("Incorrect Option"));
      break;
  }
}


/* ------------------------------------------------------------------------- */
//! Read stored PLL settings from nonvolatile EEPROM on demo board
//! @return void
void menu_8_DC2430_site_select()
{
  int temp_val;
  char demo_name[] = "DC2248";    // Demo Board Name stored in QuikEval EEPROM


  Serial.println(F("\nCommand Summary:"));
  Serial.println(F("  0-Site 0"));
  Serial.println(F("  1-Site 1"));
  Serial.println(F("  2-Site 2"));
  Serial.println(F("  3-Site 3"));
  Serial.println(F("  4-Site 4"));
  Serial.println(F("  5-Site 5"));
  Serial.println(F("  6-Site 6"));
  Serial.println(F("  7-Site 7"));

  Serial.println("");
  Serial.print(F("Enter a command: "));
  temp_val = read_int();  //! Reads the user command

  dc2430_site_select(temp_val);

  demo_board_connected=0;
  dc2248_board_connected=0;
  demo_board_connected = discover_demo_board_local(demo_name);  //! Checks if any demo board is connected.

  if (demo_board_connected)
  {
    Serial.println(F("\n********************************************************"));
    dc2248_board_connected = discover_demo_board(demo_name);  //! Checks if correct demo board is connected.
  }
  if (!dc2248_board_connected)
  {
    Serial.print(F("\nDC2248 NOT FOUND ON DC2430 SITE "));
    Serial.print(temp_val);
  }

}

/* ------------------------------------------------------------------------- */
//! Called from function menu_8_DC2430_site_select(), programs DC2430 to site selected
//! @return void
void dc2430_site_select(int site_select)
{

  switch (site_select)        //! Prints the appropriate submenu
  {
    case 0:
      digitalWrite(dc2430_site_DA0_Pin, LOW);
      digitalWrite(dc2430_site_DA1_Pin, LOW);
      digitalWrite(dc2430_site_DA2_Pin, LOW);
      break;

    case 1:
      digitalWrite(dc2430_site_DA0_Pin, HIGH);
      digitalWrite(dc2430_site_DA1_Pin, LOW);
      digitalWrite(dc2430_site_DA2_Pin, LOW);
      break;

    case 2:
      digitalWrite(dc2430_site_DA0_Pin, LOW);
      digitalWrite(dc2430_site_DA1_Pin, HIGH);
      digitalWrite(dc2430_site_DA2_Pin, LOW);
      break;

    case 3:
      digitalWrite(dc2430_site_DA0_Pin, HIGH);
      digitalWrite(dc2430_site_DA1_Pin, HIGH);
      digitalWrite(dc2430_site_DA2_Pin, LOW);
      break;

    case 4:
      digitalWrite(dc2430_site_DA0_Pin, LOW);
      digitalWrite(dc2430_site_DA1_Pin, LOW);
      digitalWrite(dc2430_site_DA2_Pin, HIGH);
      break;

    case 5:
      digitalWrite(dc2430_site_DA0_Pin, HIGH);
      digitalWrite(dc2430_site_DA1_Pin, LOW);
      digitalWrite(dc2430_site_DA2_Pin, HIGH);
      break;

    case 6:
      digitalWrite(dc2430_site_DA0_Pin, LOW);
      digitalWrite(dc2430_site_DA1_Pin, HIGH);
      digitalWrite(dc2430_site_DA2_Pin, HIGH);
      break;

    case 7:
      digitalWrite(dc2430_site_DA0_Pin, HIGH);
      digitalWrite(dc2430_site_DA1_Pin, HIGH);
      digitalWrite(dc2430_site_DA2_Pin, HIGH);
      break;

    default:
      Serial.println(F("Incorrect Option"));
      break;
  }

}

/* ------------------------------------------------------------------------- */
//!    Prints the title block when program first starts.
void print_title()
{

  Serial.println(F("\n****************************************************************************************************"));
  Serial.println(F("* DC2248 Demonstration Program                                                                     *"));
  Serial.println(F("*   - Input Voltage 6V-20V (J15/J16)                                                               *"));
  Serial.println(F("*                                                                                                  *"));
  Serial.println(F("* This program demonstrates how to send data to the LTC6951 Ultralow Jitter                        *"));
  Serial.println(F("*  Multi-Output Clock Synthesizer with Integrated VCO.                                             *"));
  Serial.println(F("*                                                                                                  *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.                                   *"));
  Serial.println(F("*                                                                                                  *"));
  Serial.println(F("* Recommendations on how to best use LTC6951Wizard and DC2248 Linduino code together               *"));
  Serial.println(F("* Use LTC6951Wizard to                                                                             *"));
  Serial.println(F("*  - determine best SPI register values for specific frequenices and Sync Methods for all settings *"));
  Serial.println(F("*  - determine loop filter value                                                                   *"));
  Serial.println(F("*  - Simulate LTC6951 Phase noise and Time Domain response                                         *"));
  Serial.println(F("*  - initial engineering evaluation                                                                *"));
  Serial.println(F("* Use DC2248 Linduino code                                                                         *"));
  Serial.println(F("*  - Create a look up table of the SPI register values created from LTC6951Wizard (option 1)       *"));
  Serial.println(F("*  - Software development for reading and writing to specific register/address (option 2 & 3)      *"));
  Serial.println(F("*  - Storing demo board settings on the demo board EEPROM                                          *"));
  Serial.println(F("*  - test that require a fast SPI clocks up to 8MHz, (i.e. settling time)                          *"));
  Serial.println(F("*  - starting place to use the DC2248 with the DC2430                                              *"));
  Serial.println(F("****************************************************************************************************"));
  Serial.println();
} // end of print_title


/* ------------------------------------------------------------------------- */
//!    Prints main menu.
void print_prompt()
{

  Serial.println(F("\nCommand Summary:"));
  Serial.println(F("  1-Load Default Settings from a Look-up Table"));
  Serial.println(F("  2-READ/WRITE to Registers Addresses"));
  Serial.println(F("  3-READ/WRITE to Registers Fields"));
  Serial.println(F("  4-Store LTC6951 SPI settings to the DC2248's EEPROM"));
  Serial.println(F("  5-Restore LTC6951 SPI settings from the DC2248's EEPROM"));
  Serial.println(F("  6-Timing Measurement Aid: SPI WRITE + CALIBRAION + SETTLING TIME"));
  Serial.println(F("  7-Adjust SPI frequency"));
  if (Using_DC2430)
  {
    Serial.println(F("  8-DC2430 Site Selector"));
  }
  Serial.println("");
  Serial.print(F("Enter a command: "));
} // end of print_prompt


