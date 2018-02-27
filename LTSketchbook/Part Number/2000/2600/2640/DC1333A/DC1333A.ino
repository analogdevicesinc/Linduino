/*!
Linear Technology DC1333A Demonstration Board.
LTC2640: Single 12-/10-/8-Bit Rail-to-Rail DACs with 10ppm/C Reference
Demonstration Circuit DC1333 features the 12-bit versions of the LTC2640.

Linear Technology DC1074A Demonstration Board.
LTC2630: Single 12-/10-/8-Bit Rail-to-Rail DACs with Integrated Reference in SC70

@verbatim
NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. The
   program displays calculated voltages which are based on the voltage of the
   reference used, be it internal or external. A precision voltmeter is needed to
   verify the actual measured voltages against the calculated voltage displayed. If
   an external reference is used, a precision voltage source is required to apply
   the external reference voltage. A precision voltmeter is also required to measure
   the external reference voltage. No external power supply is required. All four
   demo board options may be used: DC1333A-A, DC1333A-B, DC1333A-C, or DC1333A-D.


  Explanation of Commands:


   1- Write to DAC input register: Value is stored in the DAC for updating later,
      through a software "Update" command. User will be prompted to enter either a
      code in hex or decimal, or a voltage. If a voltage is entered, a code will be
      calculated based on the active scaling and reference parameters - ideal values
      if no calibration was ever stored. (This is more often used with the
      multichannel DACs in the family, where all DACs can be updated at once in
      software or by asserting the LDAC# pin.)

   2- Write and Update: Similar to item 1, but DAC is updated immediately.

   3- Write to DAC: Sends the DAC code to the Input Register.

   4- Update DAC: Copies the value from the input register into the DAC Register.
      Note that a "write and update" command writes the code to BOTH the input
      register and DAC register, so subsequent "update" commands will simply re-copy
      the same data (no change in output.)

   5- Power Down DAC: Disable DAC output. Power supply current is reduced. DAC code
      present in DAC registers at time of shutdown are preserved.

   6- Set reference mode: Either internal or external. Selecting external mode
      prompts for the external refernce voltage, which is used directly if no
      individual DAC calibration is stored. The selection and entered volgage are
      stored to EEPROM so it is persisent across reset / power cycles.

   7- Calibrate DAC: Use a precision voltmeter to obtain and enter VOUT readings
      taken with different DAC codes. Set reference mode FIRST, as values are stored
      separately for internal and external reference mode. Entries are used to
      calculate the closest code to send to the DAC to achieve an entered voltage.

   8- Enable / Disable Calibration: Switch between stored calibration values and
      defaults. Calibration parameters are stored separately for internal and
      external reference modes. Ideal calibration will be used if the calibration
      parameter valid key is not set.


USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2640
http://www.linear.com/product/LTC2630

http://www.linear.com/product/LTC2640#demoboards
http://www.linear.com/product/LTC2630#demoboards


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
    @ingroup LTC2640
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LT_I2C.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC2640.h"
#include <SPI.h>
#include <Wire.h>

#define EEPROM_CAL_KEY_INT  0x5678          //!< Calibration associated with internal reference
#define EEPROM_CAL_KEY_EXT  0x9ABC          //!< Calibration associated with external reference

// DAC Reference State
// Could have been zero or 1, this allows you to use the
// variable "reference_mode" as the command argument to a write
#define REF_INTERNAL LTC2640_CMD_INTERNAL_REFERENCE   //!< Stored reference state is Internal
#define REF_EXTERNAL LTC2640_CMD_EXTERNAL_REFERENCE   //!< Stored reference state is External

// EEPROM memory locations
#define STORED_REF_STATE_BASE     EEPROM_CAL_STATUS_ADDRESS     //!< Base address of the stored reference state
#define INT_CAL_VALID_BASE        STORED_REF_STATE_BASE + 2     //!< Base address of the "internal ref calibration valid" flag
#define INT_CAL_PARAMS_BASE       INT_CAL_VALID_BASE + 2        //!< Base address of the internal ref calibration parameters
#define EXT_CAL_VALID_BASE        INT_CAL_PARAMS_BASE + 8       //!< Base address of the "external ref calibration valid" flag
#define EXT_CAL_PARAMS_BASE       EXT_CAL_VALID_BASE + 2        //!< Base address of the external ref calibration parameters
#define EXT_REF_V_BASE            EXT_CAL_PARAMS_BASE + 8       //!< Base address of the stored external reference voltage
#define LSB_PARAM_ADDR_OFFSET     4                             //!< Offset into XXX_CAL_PARAMS_BASE to locate stored LSB parameter

// Function Declaration
int8_t restore_calibration();               // Read the DAC calibration from EEPROM, Return 1 if successful, 0 if not
void store_calibration();                   // Store the DAC calibration to the EEPROM
void print_title();                         // Print the title block
void print_prompt();                        // Prompt the user for an input command
int16_t prompt_voltage_or_code();
uint16_t get_voltage(float LTC2640_lsb, int32_t LTC2640_offset);
uint16_t get_code();
void calibrate_dac();                       // Calibrate the selected DAC using a voltmeter. The routine does a linear curve fit given two data points.

void menu_1_write_to_input_register();
void menu_2_write_and_update_dac();
void menu_3_update_power_up_dac();
void menu_4_power_down_dac();
void menu_5_set_reference_mode();           // Int, ext, if ext, prompt for voltage
void menu_6_calibrate_dacs();
void menu_7_enable_calibration();

// Global variables
static uint8_t demo_board_connected;        //!< Set to 1 if the board is connected
static uint8_t shift_count = 4;             //!< The data align shift count. For 12-bits, shift_count = 4
static uint8_t reference_mode;              //!< Tells whether to set internal or external reference

// Global calibration variables
static float reference_voltage;         //!< Reference voltage, either internal or external
static float LTC2640_lsb = 1e-3;            //!< DAC lsb. Default to 12-bits
static int16_t LTC2640_offset = 0;          //!< DAC offset

//! Used to keep track to print voltage or print code
enum
{
  PROMPT_VOLTAGE = 0, /**< 0 */
  PROMPT_CODE = 1     /**< 1  */
};

//! Initialize Linduino
void setup()
// Setup the program
{
  char demo_name[] = "DC1333";  // Demo Board Name stored in QuikEval EEPROM

  quikeval_SPI_init();          // Configure the spi port for 4Mhz SCK
  quikeval_SPI_connect();       // Connect SPI to main data port
  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100khz
  Serial.begin(115200);         // Initialize the serial port to the PC
  print_title();
  demo_board_connected = discover_demo_board(demo_name);
  if (demo_board_connected)
  {
    // The DC1333A does not have the option indicated in the demo board name
    // Uses product name for the option
    if (strlen(strstr(demo_board.product_name, "-LM")) > 1)
      demo_board.option = 'A';
    if (strlen(strstr(demo_board.product_name, "-LZ")) > 1)
      demo_board.option = 'B';
    if (strlen(strstr(demo_board.product_name, "-HM")) > 1)
      demo_board.option = 'C';
    if (strlen(strstr(demo_board.product_name, "-HZ")) > 1)
      demo_board.option = 'D';
    Serial.print("Demo Board Option: ");
    Serial.println(demo_board.option);
    restore_calibration();
    print_prompt();
  }
}

//! Repeats Linduino loop
void loop()
{
  int16_t user_command;
  // The main control loop
  if (demo_board_connected)             // Do nothing if the demo board is not connected
  {
    if (Serial.available())             // Check for user input
    {
      user_command = read_int();        // Read the user command
      Serial.println(user_command);
      Serial.flush();
      switch (user_command)
      {
        case 1:
          menu_1_write_to_input_register();
          break;
        case 2:
          menu_2_write_and_update_dac();
          break;
        case 3:
          menu_3_update_power_up_dac();
          break;
        case 4:
          menu_4_power_down_dac();
          break;
        case 5:
          menu_5_set_reference_mode(); // Int, ext, if ext, prompt for voltage
          restore_calibration();
          break;
        case 6:
          menu_6_calibrate_dacs();
          restore_calibration();
          break;
        case 7:
          menu_7_enable_calibration();
          restore_calibration();
          break;
        default:
          Serial.println("Incorrect Option");
          break;
      }
      Serial.println("\n*****************************************************************");
      print_prompt();
    }
  }
}

// Function Definitions

//! Write data to input register, but do not update DAC output
void menu_1_write_to_input_register()
{
  uint16_t dac_code;

  if (prompt_voltage_or_code() == PROMPT_VOLTAGE)
    dac_code = get_voltage(LTC2640_lsb, LTC2640_offset);
  else
    dac_code = get_code();

  LTC2640_write(LTC2640_CS, LTC2640_CMD_WRITE, dac_code << shift_count);
}

//!Write data to DAC register (which updates output immediately)
void menu_2_write_and_update_dac()
{
  uint16_t dac_code;

  if (prompt_voltage_or_code() == PROMPT_VOLTAGE)
    dac_code = get_voltage(LTC2640_lsb, LTC2640_offset);
  else
    dac_code = get_code();
  LTC2640_write(LTC2640_CS, LTC2640_CMD_WRITE_UPDATE, dac_code << shift_count);
}

//! Update DAC with data that is stored in input register, power up if sleeping
void menu_3_update_power_up_dac()
{
  // Update DAC
  LTC2640_write(LTC2640_CS, LTC2640_CMD_UPDATE, 0x0000);
}

//! Power down DAC
void menu_4_power_down_dac()
{
  // Power down DAC
  LTC2640_write(LTC2640_CS, LTC2640_CMD_POWER_DOWN, 0x0000);
}

//! Set reference mode and store to EEPROM
void menu_5_set_reference_mode() // Int, ext, if ext, prompt for voltage
{
  int16_t user_input;
  Serial.println("Select reference mode - 0 for Internal, 1 for External");
  user_input = read_int();
  if (user_input == 1)
  {
    reference_mode = REF_EXTERNAL;
    Serial.println("External reference mode; enter external reference voltage");
    reference_voltage = read_float();
    Serial.print(reference_voltage, 5);
    Serial.println("V");
    eeprom_write_float(EEPROM_I2C_ADDRESS, reference_voltage, EXT_REF_V_BASE);
  }
  else
  {
    reference_mode = REF_INTERNAL;
    Serial.println("Internal reference mode selected");
  }
  Serial.println("Writing reference mode to EEPROM\n\n");
  eeprom_write_byte(EEPROM_I2C_ADDRESS, reference_mode, STORED_REF_STATE_BASE);
}

//! Calibrate all DACs by measuring two known outputs
void menu_6_calibrate_dacs()
{
  // Calibrate the DACs using a multimeter
  calibrate_dac();     // Run calibration routine
  store_calibration();
}

//! Enable / Disable calibration. Use with caution - behavior is undefined if you enable calibration and an actual
//! calibration cycle has not been performed.
void menu_7_enable_calibration()
{
  int16_t user_input;
  Serial.println(F("\n\nSelect option -  0: Enable Internal, 1: Disable Internal, 2: Enable External, 3: Disable External"));
  user_input = read_int();
  switch (user_input)
  {
    case 0:
      Serial.println(F("Enabling Internal Cal Params"));
      eeprom_write_int16(EEPROM_I2C_ADDRESS, EEPROM_CAL_KEY, INT_CAL_VALID_BASE);
      break;
    case 1:
      Serial.println(F("Disabling Internal Cal Params"));
      eeprom_write_int16(EEPROM_I2C_ADDRESS, 0x0000, INT_CAL_VALID_BASE);
      break;
    case 2:
      Serial.println(F("Enabling External Cal Params"));
      eeprom_write_int16(EEPROM_I2C_ADDRESS, EEPROM_CAL_KEY, EXT_CAL_VALID_BASE);
      break;
    case 3:
      Serial.println(F("Disabling External Cal Params"));
      eeprom_write_int16(EEPROM_I2C_ADDRESS, 0x0000, EXT_CAL_VALID_BASE);
      break;
  }
}

//! Read stored calibration parameters from nonvolatile EEPROM on demo board
//! @return Return 1 if successful, 0 if not
int8_t restore_calibration()
// Read the DAC calibration from EEPROM
// Return 1 if successful, 0 if not
{
  int16_t intvalid, extvalid;
  uint8_t i, eeaddr;
  float dac_count;                                  // The number of codes, 4096 for 12 bits, 65536 for 16 bits

  Serial.println(F("\n\nReading Calibration parameters from EEPROM..."));
  float full_scale; // To avoid confusion - in internal ref mode, FS=Vref, in ext mode, FS=2*Vref
  // Read the cal keys from the EEPROM.
  eeprom_read_int16(EEPROM_I2C_ADDRESS, &intvalid, INT_CAL_VALID_BASE);
  eeprom_read_int16(EEPROM_I2C_ADDRESS, &extvalid, EXT_CAL_VALID_BASE);
  // Read the stored reference state
  eeprom_read_byte(EEPROM_I2C_ADDRESS, (char *) &reference_mode, STORED_REF_STATE_BASE);
  // Read external ref V unconditionally, overwrite with defaults if no cal found
  eeprom_read_float(EEPROM_I2C_ADDRESS, &reference_voltage, EXT_REF_V_BASE);

  if (reference_mode == REF_EXTERNAL)
  {
    Serial.print(F("Restored external ref. Voltage:"));
    Serial.println(reference_voltage, 5);
  }
  else  // EITHER reference is set to internal, OR not programmed in which case default to internal
  {
    reference_mode = REF_INTERNAL; // Redundant if already set
    Serial.println("Internal reference mode set");
  }

  // Write the reference mode to the DAC right away
  LTC2640_write(LTC2640_CS, reference_mode, 0x0000);

  // Set up default values, shift count, DAC count
  // Calibration parameters MAY be changed next, if match
  // between reference mode and stored calibration
  full_scale = reference_voltage; // If external ref mode, this applies.
  switch (demo_board.option)
  {
    case 'A':
      // LTC2640-LM, 12-bits, 2.5V full scale
      shift_count = 4;
      if (reference_mode == REF_INTERNAL) full_scale = 2.5;
      dac_count = 4096;
      break;
    case 'B':
      // LTC2640-LZ, 12-bits, 4.096V full scale
      shift_count = 4;
      if (reference_mode == REF_INTERNAL) full_scale = 2.5;
      dac_count = 4096;
      break;
    case 'C':
      // LTC2640-HM, 12-bits, 2.5V full scale
      shift_count = 4;
      if (reference_mode == REF_INTERNAL) full_scale = 4.096;
      dac_count = 4096;
      break;
    case 'D':
      // LTC2640-HZ, 12-bits, 4.096V full scale
      shift_count = 4;
      if (reference_mode == REF_INTERNAL) full_scale = 4.096;
      dac_count = 4096;
      break;
  }

  LTC2640_offset = 0;
  LTC2640_lsb = full_scale / dac_count;

  // Restore calibration IF reference mode matches stored calibraiton
  eeaddr = 0;   // Assume no calibration present or mismatch between cal and reference mode

  if ((intvalid == EEPROM_CAL_KEY) && (reference_mode == REF_INTERNAL))
  {
    eeaddr = INT_CAL_PARAMS_BASE;
    Serial.println(F("Found internal calibration, restoring...)"));
  }
  else if ((extvalid == EEPROM_CAL_KEY) && (reference_mode == REF_EXTERNAL))
  {
    eeaddr = EXT_CAL_PARAMS_BASE;
    Serial.println(F("Found external calibration, restoring...)"));
  }
  else Serial.println(F("Calibration not found for this\nreference mode, using ideal calibration"));

  if (eeaddr != 0) // If cal key was enabled and reference mode is correct, read offset and lsb
  {
    eeprom_read_int16(EEPROM_I2C_ADDRESS, &LTC2640_offset, eeaddr);
    eeprom_read_float(EEPROM_I2C_ADDRESS, &LTC2640_lsb, eeaddr + LSB_PARAM_ADDR_OFFSET);
    Serial.println("Calibration Restored");
  }

  Serial.print("offset: ");
  Serial.print(LTC2640_offset);
  Serial.print(" , lsb: ");
  Serial.print(LTC2640_lsb * 1000, 4);
  Serial.println(" mv");

  if (eeaddr != 0) return (1);
  return (0);
}

//! Store measured calibration parameters to nonvolatile EEPROM on demo board
void store_calibration()
// Store the DAC calibration to the EEPROM
{
  uint8_t eeaddr;
  if (reference_mode == REF_INTERNAL)
  {
    eeprom_write_int16(EEPROM_I2C_ADDRESS, EEPROM_CAL_KEY, INT_CAL_VALID_BASE);
    eeaddr = INT_CAL_PARAMS_BASE;
  }
  else
  {
    eeprom_write_int16(EEPROM_I2C_ADDRESS, EEPROM_CAL_KEY, EXT_CAL_VALID_BASE);
    eeaddr = EXT_CAL_PARAMS_BASE;
  }
  eeprom_write_int16(EEPROM_I2C_ADDRESS, LTC2640_offset, eeaddr);        // Offset
  eeprom_write_float(EEPROM_I2C_ADDRESS, LTC2640_lsb, eeaddr + LSB_PARAM_ADDR_OFFSET);      // lsb
  Serial.println(F("Calibration Stored to EEPROM"));
}

//! Prompt user to enter a voltage or digital code to send to DAC
//! @return Returns the prompt indicator
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
//! @return Returns the DAC code
uint16_t get_voltage(float LTC2640_lsb, int16_t LTC2640_offset)
{
  float dac_voltage;

  Serial.print(F("Enter Desired DAC output voltage: "));
  dac_voltage = read_float();
  Serial.print(dac_voltage);
  Serial.println(" V");
  Serial.flush();
  return(LTC2640_voltage_to_code(dac_voltage, LTC2640_lsb, LTC2640_offset));
}

//! Get code to send to DAC directly, in decimal, hex, or binary
//! @return Returns users input dac code
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
  Serial.println("");
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC1333 Demonstration Program                                  *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to send data to the LTC2640     *"));
  Serial.println(F("* Single 12-bit DAC found on the DC1333 demo board.             *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.println(F("\nCommand Summary:"));
  Serial.println(F("  1-Write to input register (no update)"));
  Serial.println(F("  2-Write and update DAC"));
  Serial.println(F("  3-Update / power up DAC"));
  Serial.println(F("  4-Power down DAC"));
  Serial.println(F("  5-Set reference mode"));
  Serial.println(F("  6-Calibrate DAC"));
  Serial.println(F("  7-Enable / Disable calibration"));

  Serial.println("\nPresent Values:");
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

//! Calibrate the selected DAC using a voltmeter. The routine
//! does a linear curve fit given two data points.
void calibrate_dac()
{
  uint16_t code1 = 0x0020;                            //! Calibration code 1
  uint16_t code2 = 0x0FFF;                            //! Calibration code 2
  float voltage1;                                     //! Calibration voltage 1
  float voltage2;                                     //! Calibration voltage 2
  Serial.println();
  Serial.print("Calibrating DAC ");

  // Left align 12-bit code1 to 16 bits & write to DAC
  LTC2640_write(LTC2640_CS,LTC2640_CMD_WRITE_UPDATE, code1 << shift_count);
  Serial.print("DAC code set to 0x");
  Serial.println(code1, HEX);
  Serial.print("Enter measured DAC voltage:");
  voltage1 = read_float();
  Serial.print(voltage1, 6);
  Serial.println(" V");
  // Left align 12-bit code2 to 16 bits & write to DAC
  LTC2640_write(LTC2640_CS, LTC2640_CMD_WRITE_UPDATE, code2 << shift_count);
  Serial.print("DAC code set to 0x");
  Serial.println(code2, HEX);
  Serial.print("Enter measured DAC voltage:");
  voltage2 = read_float();
  Serial.print(voltage2, 6);
  Serial.println(" V");
  LTC2640_calibrate(code1, code2, voltage1, voltage2, &LTC2640_lsb, &LTC2640_offset);
}