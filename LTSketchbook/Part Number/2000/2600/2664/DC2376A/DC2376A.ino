/*!
Linear Technology DC2376A Demonstration Board.
LTC2664: 4 Channel SPI 16-/12-Bit Rail-to-Rail DACs with 10ppm/C Max Reference.

@verbatim
NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

   An external +/- 15V power supply is required to power the circuit.

   The program displays calculated voltages which are based on the voltage
   of the reference used, be it internal or external. A precision voltmeter
   is needed to verify the actual measured voltages against the calculated
   voltage displayed.

   If an external reference is used, a precision voltage
   source is required to apply the external reference voltage. A
   precision voltmeter is also required to measure the external reference
   voltage.

  Explanation of Commands:
   1- Select DAC: Select one of four DACs to test : 0 to 3

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
      BOTH the input register and DAC register, so subsequent "update"
      commands will simply re-copy the same data (no change in output.)

   5- Power Down DAC: Disable DAC output. Power supply current is reduced.
      DAC code present in DAC registers at time of shut down are preserved.

   6- Set reference mode, either internal or external: Selecting external
      mode powers down the internal reference voltage. It is the users
      responsibility to apply a 2.5V reference.

   7- Set SoftSpan: There are four options. In external mode, it is the users
      responsibility to compensate for the desired voltage.

   8- Toggle Selected Word: Switch between register A or B for DAC code.

   9- Set MUX: Enables / disables the MUX and sets the channel.

   10- Global Toggle Bit Settings - Enabling this feature sets the DAC to
       toggle from register A(when TGL pin is LOW) and register b(when TGL pin
       is HIGH). TGL pin is set HIGH with an internal pull up when the global toggle bit
       is set, and TGL pin is set LOW with an internal pull down when the global toggle bit
       is not set.

   11- Enable, disable, or store to EEPROM: To store to EEROM ensure all
       registers are set to their desired settings. Next, go to the store
       setting EEPROM menu and select it. Upon Linduino power up, the
       previously stored settings will be restored.

   12- Voltage Ramp: Sets a voltage ramp to all the channels. CH0 = 0V, CH1 = 0.1V,
       CH2 = 0.2V, CH3 = 0.4V, ect.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2664

http://www.linear.com/product/LTC2664#demoboards


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
    @ingroup LTC2664
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC2664.h"
#include <SPI.h>
#include <Wire.h>

// Macros
#define REF_EXTERNAL    LTC2664_REF_DISABLE     //!< External mode 
#define REF_INTERNAL    0                       //!< Internal mode

// Function Declaration

void restore_dac_from_eeprom();                         // Restores DAC settings from EEPROM
void print_title();                                     // Print the title block
void print_prompt(int8_t selected_dac);                 // Prompt the user for an input command
int16_t prompt_voltage_or_code(uint8_t selected_dac);   // Prompt the user for voltage or code
uint16_t get_voltage(uint8_t selected_dac);             // Obtains voltage from user
uint16_t get_code();                                    // Obtains code from user

void menu_1_select_dac(uint8_t *selected_dac);              // Menu  1: prompts user for DAC selection
void menu_2_write_to_input_register(uint8_t selected_dac);  // Menu  2: sends data to selected DAC register with no update
void menu_3_write_and_update_dac(uint8_t selected_dac);     // Menu  3: sends data to selected DAC register with update
void menu_4_update_power_up_dac(uint8_t selected_dac);      // Menu  4: updates and powers up selected  DAC
void menu_5_power_down_dac(uint8_t selected_dac);           // Menu  5: powers down selected DAC
void menu_6_set_reference_mode();                           // Menu  6: prompts user to enable/disable internal reference
void menu_7_set_softspan_range(uint8_t selected_dac);       // Menu  7: prompts user to sets the SoftSpan range
void menu_8_toggle_select_word();                           // Menu  8: prompts user to enter the toggle select word
void menu_9_set_mux();                                      // Menu  9: sets MUX
void menu_10_global_toggle_settings();                      // Menu 10: sets the global toggle bit
void menu_11_enable_disable_eeprom_restore();               // Menu 11: EEPOM restore settings
void menu_12_voltage_ramp();                                // Menu 12: sets a voltage ramp for the LTC2664
void menu_13_demo_board_test();               // Menu 13: Demo Board functional test
void menu_14_loopback_test();
// Global variable
static uint8_t demo_board_connected;   //!< Set to 1 if the board is connected

//! Used to manipulate EEPROM data.
union eeprom_data_union
{
  struct data_struct_type               //! EEPROM data structure
  {
    int16_t cal_key;                    //!< The key that keeps track of the calibration
    uint8_t soft_span_range[4];        //!< SoftSpan range
    uint16_t dac_code_a[4];            //!< DAC Register A
    uint16_t dac_code_b[4];            //!< DAC Register B
    uint16_t toggle_word;               //!< Toggle control word
    uint8_t global_toggle_bit;          //!< Global toggle bit
    uint8_t mux_state;                  //!< Multiplexer address AND enable bit
    uint8_t reference_mode;             //!< Int. reference may be disabled in external reference mode (not required)
  } data_struct;                        //!< Name of structure

  char byte_array[sizeof(data_struct_type)]; //!< Array used to store the structure
};

eeprom_data_union eeprom; // Create union

// Constants

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
  char demo_name[] = "DC2376";  // Demo Board Name stored in QuikEval EEPROM

  quikeval_SPI_init();          // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();       // Connect SPI to main data port
  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100kHz

  Serial.begin(115200);         // Initialize the serial port to the PC
  print_title();
  demo_board_connected = discover_demo_board(demo_name);
  if (demo_board_connected)
  {
    restore_dac_from_eeprom();
    print_prompt(0);
  }
}

//! Repeats Linduino loop
void loop()
{

  int16_t user_command;
  static  uint8_t selected_dac = 0;     // The selected DAC to be updated (0=CH0, 1=CH1 ... 8=All).  Initialized to CH0.

  if (Serial.available())               // Check for user input
  {
    user_command = read_int();          // Read the user command
    Serial.println(user_command);
    Serial.print(F("\n"));
    Serial.flush();
    switch (user_command)
    {
      case 1:
        menu_1_select_dac(&selected_dac);
        break;
      case 2:
        menu_2_write_to_input_register(selected_dac);
        break;
      case 3:
        menu_3_write_and_update_dac(selected_dac);
        break;
      case 4:
        menu_4_update_power_up_dac(selected_dac);
        break;
      case 5:
        menu_5_power_down_dac(selected_dac);
        break;
      case 6:
        menu_6_set_reference_mode();
        break;
      case 7:
        menu_7_set_softspan_range(selected_dac);
        break;
      case 8:
        menu_8_toggle_select_word();
        break;
      case 9:
        menu_9_set_mux();
        break;
      case 10:
        menu_10_global_toggle_settings();
        break;
      case 11:
        menu_11_enable_disable_eeprom_restore();
        break;
      case 12:
        menu_12_voltage_ramp();
        break;
      case 13:
        menu_13_demo_board_test();
        break;
      case 14:
        menu_14_loopback_test();
        break;
      default:
        Serial.println(F("Incorrect Option"));
        break;
    }
    Serial.println(F("\n************************************************************"));
    print_prompt(selected_dac);
  }

}

// Function Definitions

//! Select which DAC to operate on
void menu_1_select_dac(uint8_t *selected_dac)
{
  // Select a DAC to operate on
  Serial.print(F("Select DAC to operate on (0 to 3, 4 for ALL DACs): "));
  *selected_dac = read_int();
  if (*selected_dac == 4)
    Serial.println(F("All"));
  else
    Serial.println(*selected_dac);
}

//! Write data to input register, but do not update DAC output
void menu_2_write_to_input_register(uint8_t selected_dac)
{
  uint16_t dac_code;

  if (prompt_voltage_or_code(selected_dac) == PROMPT_VOLTAGE)
    dac_code = get_voltage(selected_dac);
  else
    dac_code = get_code();

  if (selected_dac <= 3)
  {
    // Store dac_code to register variable a or b
    if (eeprom.data_struct.toggle_word & (0x01 << selected_dac))
    {
      eeprom.data_struct.dac_code_b[selected_dac] = dac_code;
    }
    else
      eeprom.data_struct.dac_code_a[selected_dac] = dac_code;
    LTC2664_write(LTC2664_CS, LTC2664_CMD_WRITE_N, selected_dac, dac_code);
  }
  else
  {
    // Store dac_code to register variable a or b
    for (uint8_t i = 0; i <= 3 ; i++)
    {
      if (eeprom.data_struct.toggle_word & (0x01 << i))
      {
        eeprom.data_struct.dac_code_b[i] = dac_code;
      }
      else
        eeprom.data_struct.dac_code_a[i] = dac_code;
    }
    LTC2664_write(LTC2664_CS, LTC2664_CMD_WRITE_ALL, selected_dac, dac_code);
  }
}

//! Write data to DAC register (which updates output immediately)
void menu_3_write_and_update_dac(uint8_t selected_dac)
{
  uint16_t dac_code;

  if (prompt_voltage_or_code(selected_dac) == PROMPT_VOLTAGE)
    dac_code = get_voltage(selected_dac);
  else
    dac_code = get_code();

  if (selected_dac <= 3)
  {
    // Store dac_code to register variable a or b
    if (eeprom.data_struct.toggle_word & (0x01 << selected_dac))
    {
      eeprom.data_struct.dac_code_b[selected_dac] = dac_code;
    }
    else
      eeprom.data_struct.dac_code_a[selected_dac] = dac_code;

    LTC2664_write(LTC2664_CS,LTC2664_CMD_WRITE_N_UPDATE_N, selected_dac, dac_code); // Send dac_code
  }
  else  // All DACs
  {
    // Store dac_code to register variable a or b
    for (uint8_t i = 0; i <= 3 ; i++)
    {
      if (eeprom.data_struct.toggle_word & (0x01 << i))
      {
        eeprom.data_struct.dac_code_b[i] = dac_code;
      }
      else
        eeprom.data_struct.dac_code_a[i] = dac_code;
    }
    LTC2664_write(LTC2664_CS,LTC2664_CMD_WRITE_ALL_UPDATE_ALL, 0, dac_code);    // Send dac_code
  }
}

//! Update DAC with data that is stored in input register, power up if sleeping
void menu_4_update_power_up_dac(uint8_t selected_dac)
{
  // Update DAC
  if (selected_dac <= 3)
    LTC2664_write(LTC2664_CS,LTC2664_CMD_UPDATE_N, selected_dac, 0);
  else
    LTC2664_write(LTC2664_CS,LTC2664_CMD_UPDATE_ALL, 0, 0);
}

//! Power down DAC
void menu_5_power_down_dac(uint8_t selected_dac)
{
  // Power down DAC
  if (selected_dac <= 3)
    LTC2664_write(LTC2664_CS, LTC2664_CMD_POWER_DOWN_N, selected_dac, 0);
  else
    LTC2664_write(LTC2664_CS, LTC2664_CMD_POWER_DOWN_ALL, 0, 0);
}

//! Set reference mode
void menu_6_set_reference_mode(void)
{
  int16_t user_command;
  Serial.println(F("0 - Power down the internal reference"));
  Serial.println(F("1 - Power up the internal reference"));
  user_command = read_int();
  if (user_command > 1)
    user_command = 1;
  if (user_command < 0)
    user_command = 0;

  if (user_command == 1)
  {
    eeprom.data_struct.reference_mode = REF_INTERNAL;
    LTC2664_write(LTC2664_CS, LTC2664_CMD_CONFIG, 0, eeprom.data_struct.reference_mode);
    Serial.println(F("Internal reference has been powered up"));
  }
  else
  {
    eeprom.data_struct.reference_mode = REF_EXTERNAL;
    LTC2664_write(LTC2664_CS, LTC2664_CMD_CONFIG, 0, eeprom.data_struct.reference_mode);
    Serial.println(F("Internal reference has been powered down"));
  }
}

//! Set SoftSpan Range
void menu_7_set_softspan_range(uint8_t selected_dac)
{
  int16_t user_command;

  Serial.println(F("0- 0V to 5V"));
  Serial.println(F("1- 0V to 10V"));
  Serial.println(F("2- -5V to 5V"));
  Serial.println(F("3- -10V to 10V"));
  Serial.println(F("4- -2.5V to 2.5V"));
  Serial.print(F("Select a SoftSpan Range: "));
  user_command = read_int();

  if (user_command > 4)
    user_command = 4;
  if (user_command < 0)
    user_command = 0;
  Serial.println(user_command);

  if (selected_dac <= 3)
  {
    eeprom.data_struct.soft_span_range[selected_dac] = user_command;
    LTC2664_write(LTC2664_CS, LTC2664_CMD_SPAN, selected_dac, eeprom.data_struct.soft_span_range[selected_dac]);
  }
  else
  {
    for (uint8_t i = 0; 0 <= 3; i++)
      eeprom.data_struct.soft_span_range[i] = user_command;

    LTC2664_write(LTC2664_CS, LTC2664_CMD_SPAN_ALL, 0, eeprom.data_struct.soft_span_range[0]);
  }
}

//! Enter toggle select word, which also sets the register that will be written
//! if bit is 0, register A is written, if 1, register B is written.
void menu_8_toggle_select_word()
{
  // Select a DAC to operate on
  Serial.println(F("Toggle Select bit sets the register to be written for the corresponding DAC."));
  Serial.println(F("0 for Register A or 1 for Register B."));
  Serial.println(F("Note: DAC Update from Register B requires TGB = 1"));
  Serial.println(F("      DAC Update from Register A requires TCB = 0"));
  Serial.print(F("Enter Toggle Select Byte as hex (0xM) or binary (B1010): "));


  eeprom.data_struct.toggle_word = read_int();
  // Toggle bits are stored as 16 bit word with 12 don't care bits

  LTC2664_write(LTC2664_CS, LTC2664_CMD_TOGGLE_SEL, 0, eeprom.data_struct.toggle_word);
  Serial.println(eeprom.data_struct.toggle_word, BIN);
}

//! Enable / Disable and sets the channel for the MUX
void menu_9_set_mux()
{
  int16_t user_command;
  Serial.println(F("0- Disable Mux"));
  Serial.println(F("1- Enable Mux"));
  Serial.print(F("Enter a command: "));
  user_command = read_int();

  if (user_command == 1)
  {
    Serial.print(F("Select MUX channel(0-MUXIN0, 1-VOUT0, 2-VOUT1, 3-MUXIN1, 4-MUXIN2, 5-VOUT2, 6-VOUT3, 7-MUXIN3, 8-REFLO, 9-REF, 10-TEMP Monitor, 11-V+, 12-V- : "));
    user_command = read_int();

    if (user_command > 12)
      user_command = 12;
    if (user_command < 0)
      user_command = 0;

    Serial.println(user_command);
    eeprom.data_struct.mux_state = LTC2664_MUX_ENABLE | user_command;
    LTC2664_write(LTC2664_CS,LTC2664_CMD_MUX, 0, eeprom.data_struct.mux_state);
  }
  else
  {
    eeprom.data_struct.mux_state = LTC2664_MUX_ENABLE | user_command;
    LTC2664_write(LTC2664_CS,LTC2664_CMD_MUX, 0, eeprom.data_struct.mux_state);
    Serial.println(F("The MUX has been disabled"));
  }
}
//! Enable / Disable the global toggle bit
void menu_10_global_toggle_settings()
{
  int16_t user_command;
  Serial.println(F("0- Disable Global Toggle"));
  Serial.println(F("1- Enable Global Toggle"));
  Serial.print(F("Enter a command: "));
  user_command = read_int();

  if (user_command > 1)
    user_command = 1;
  if (user_command < 0)
    user_command = 0;
  Serial.println(user_command);

  eeprom.data_struct.global_toggle_bit = user_command;
  LTC2664_write(LTC2664_CS, LTC2664_CMD_GLOBAL_TOGGLE, 0, eeprom.data_struct.global_toggle_bit);
}

//! Enable / Disable restoration of DAC values from EEPROM Use with caution - behaviour is undefined if you
//! enable restoration and data has NOT been previously stored from a known state.
void menu_11_enable_disable_eeprom_restore()
{
  int16_t user_input;
  Serial.println(F("\n0- Enable Restoration"));
  Serial.println(F("1- Disable Restoration"));
  Serial.println(F("2- Store DAC Settings"));
  user_input = read_int();
  switch (user_input)
  {
    case 0:
      Serial.println(F("Enabling EEPROM restoration"));
      eeprom.data_struct.cal_key = EEPROM_CAL_KEY;
      eeprom_write_int16(EEPROM_I2C_ADDRESS, eeprom.data_struct.cal_key, EEPROM_CAL_STATUS_ADDRESS);
      break;
    case 1:
      Serial.println(F("Disabling EEPROM restoration"));
      eeprom.data_struct.cal_key = 0;
      eeprom_write_int16(EEPROM_I2C_ADDRESS, eeprom.data_struct.cal_key, EEPROM_CAL_STATUS_ADDRESS);
      break;
    case 2:
      eeprom.data_struct.cal_key = EEPROM_CAL_KEY;
      eeprom_write_byte_array(EEPROM_I2C_ADDRESS, eeprom.byte_array, EEPROM_CAL_STATUS_ADDRESS, sizeof(eeprom_data_union));
      Serial.println(F("Stored Settings to EEPROM"));
  }
}

//! Sets a voltage ramp to all the channels
void menu_12_voltage_ramp()
{
  uint8_t i;
  for (i=0; i <= 3; ++i)
  {
    LTC2664_write(LTC2664_CS,LTC2664_CMD_WRITE_N_UPDATE_N, i, i * 1312);
  }
  Serial.println("A voltage ramp was set to the LTC2664");
}

// Functional test for demo board, tests continuity, EEPROM contents,
// And correct resolution DAC installed.
void menu_13_demo_board_test()
{
  uint8_t i, errorcount = 0;

  Serial.println(F("Welcome to the LTC2664 Demo Board Test Program."));
  Serial.print(F("Found board assembly type "));
  Serial.println(demo_board.option);

  Serial.println(F("Checking EEPROM contents..."));

  read_quikeval_id_string(&ui_buffer[0]);
  ui_buffer[48] = 0;
  Serial.println(ui_buffer);
  switch (demo_board.option)
  {
    case 'A':
      if (strcmp("LTC2664-16,Cls,D2668,01,01,DC,DC2376A-A,--------", ui_buffer))
      {
        Serial.println(F("FAILED EEPROM Contents"));
        errorcount += 1;
      }
      break;
    case 'B':
      if (strcmp("LTC2664-12,Cls,D2668,01,01,DC,DC2376A-B,--------", ui_buffer))
      {
        Serial.println(F("FAILED EEPROM Contents"));
        errorcount += 1;
      }
      break;
  }

  if (!errorcount)
    Serial.println(F("EEPROM good!"));

  Serial.println(F("Ensure all jumpers are in the LEFT position (INT ref, MSP0,1,2 = 1."));
  Serial.println(F("Writing ramp of voltages to DAC outputs. Verify DAC0 = 0.10V, DAC1 = 0.20V,"));
  Serial.println(F("DAC2 = 0.3V ... DAC7 = 0.8V. Type 1, enter when done."));

  LTC2664_write(LTC2664_CS, LTC2664_CMD_CONFIG, 0, REF_INTERNAL);

  for (i=0; i <= 3; ++i)
  {
    LTC2664_write(LTC2664_CS,LTC2664_CMD_SPAN, i, LTC2664_SPAN_0_TO_5V);
    LTC2664_write(LTC2664_CS,LTC2664_CMD_WRITE_N_UPDATE_N, i, (i + 1) * 1312);
  }

  read_int();        // Read the user command

  Serial.println(F("Setting SoftSpan range to +/-10V, DAC7 to -10V, and MUX to DAC7."));
  Serial.println(F("Verify MUX output is between -9.95V and -10.05V"));

  LTC2664_write(LTC2664_CS,LTC2664_CMD_SPAN, 3, LTC2664_SPAN_PLUS_MINUS_10V);
  LTC2664_write(LTC2664_CS,LTC2664_CMD_WRITE_N_UPDATE_N, 3, 0x0000);
  LTC2664_write(LTC2664_CS,LTC2664_CMD_MUX, 0, LTC2664_MUX_ENABLE | 6); // Note MUX mapping according to Datasheet Table 5

  Serial.println(F("Type 1, enter when done."));
  read_int();

  LTC2664_write(LTC2664_CS,LTC2664_CMD_SPAN, 3, LTC2664_SPAN_0_TO_10V);
  LTC2664_write(LTC2664_CS,LTC2664_CMD_WRITE_N_UPDATE_N, 3, 0x0000);
  Serial.println(F("Verifying correct resolution LTC2664 installed"));
  Serial.println(F("output set to 0V, press REL(delta) button on meter"));
  Serial.println(F("Type 1, enter when done."));
  read_int();
  LTC2664_write(LTC2664_CS,LTC2664_CMD_WRITE_N_UPDATE_N, 3, 0x000F);

  switch (demo_board.option)
  {
    case 'A':
      Serial.println(F("16 bit device - verify output voltage is GREATER than 1mV"));
      break;
    case 'B':
      Serial.println(F("12 bit device - verify output voltage is LESS than 1mV"));
      break;
  }

  Serial.println(F("Type 1, enter when done."));
  read_int();

  Serial.println(F("Connect MUXIN0 to REF, verify MUX output is between 2.4 and 2.6V."));
  LTC2664_write(LTC2664_CS,LTC2664_CMD_MUX, 0, LTC2664_MUX_ENABLE | 0);
  Serial.println(F("Type 1, enter when done."));
  read_int();

  Serial.println(F("Connect MUXIN1 to REF, verify MUX output is between 2.4 and 2.6V."));
  LTC2664_write(LTC2664_CS,LTC2664_CMD_MUX, 0, LTC2664_MUX_ENABLE | 3);
  Serial.println(F("Type 1, enter when done."));
  read_int();

  Serial.println(F("Connect MUXIN2 to REF, verify MUX output is between 2.4 and 2.6V."));
  LTC2664_write(LTC2664_CS,LTC2664_CMD_MUX, 0, LTC2664_MUX_ENABLE | 4);
  Serial.println(F("Type 1, enter when done."));
  read_int();

  Serial.println(F("Connect MUXIN3 to REF, verify MUX output is between 2.4 and 2.6V."));
  LTC2664_write(LTC2664_CS,LTC2664_CMD_MUX, 0, LTC2664_MUX_ENABLE | 7);
  Serial.println(F("Type 1, enter when done."));
  read_int();

  Serial.println(F("All Done! Returning to main menu..."));
}

// Loopback test to verify data integrity.
void menu_14_loopback_test()
{
  uint16_t i;
  int8_t ack = 0;
  Serial.print(F("Testing readback by writing 0-10,000, NOP commands\n"));
  LTC2664_write(LTC2664_CS, LTC2664_CMD_NO_OPERATION, 0, 0); // If this is the first time the read function is called, need to initialize
  for (i=0; i<=10000; ++i)
  {
    ack |= LTC2664_write(LTC2664_CS, LTC2664_CMD_NO_OPERATION, 0, i);
  }
  if (ack)
  {
    Serial.print(F("Oops, got an error somewhere!"));
  }
  else
  {
    Serial.print(F("No errors!!"));
  }
}


//! Prompt user to enter a voltage or digital code to send to DAC
//! @returns user input
int16_t prompt_voltage_or_code(uint8_t selected_dac )
{
  int16_t user_input;

  Serial.print(F("Type 1 to enter voltage, 2 to enter code: "));
  Serial.flush();
  user_input = read_int();
  Serial.println(user_input);

  if (user_input != 2)
  {
    if (selected_dac >= 8)
    {
      Serial.println(F("\nCaution! Voltage SoftSpan could be different for different DACs"));
      Serial.println(F("Ensure All DACs can be set to desired voltages"));
      Serial.println(F("DAC 0 is used for SoftSpan values for all DACs."));
      Serial.print(F("Toggle Register Select Bits 0=A, 1 = B: 0b"));
      Serial.println(eeprom.data_struct.toggle_word, BIN);
    }
    return(PROMPT_VOLTAGE);
  }
  else
  {
    if (selected_dac >= 8)
    {
      Serial.println(F("DAC 0 is used for SoftSpan values for all DACs."));
    }
  }
  return(PROMPT_CODE);
}

//! Get voltage from user input, calculate DAC code based on lsb, offset
//! @returns the DAC code
uint16_t get_voltage(uint8_t selected_dac)
{
  float dac_voltage;

  Serial.print(F("Enter Desired DAC output voltage: "));
  dac_voltage = read_float();
  Serial.print(dac_voltage);
  Serial.println(F(" V"));
  Serial.flush();

  if (selected_dac <= 3)
    return(LTC2664_voltage_to_code(dac_voltage,
                                   LTC2664_MIN_OUTPUT[eeprom.data_struct.soft_span_range[selected_dac]],
                                   LTC2664_MAX_OUTPUT[eeprom.data_struct.soft_span_range[selected_dac]]));
  else
    return(LTC2664_voltage_to_code(dac_voltage,
                                   LTC2664_MIN_OUTPUT[eeprom.data_struct.soft_span_range[0]],
                                   LTC2664_MAX_OUTPUT[eeprom.data_struct.soft_span_range[0]]));
}

//! Get code to send to DAC directly, in decimal, hex, or binary
//! @return code from user
uint16_t get_code()
{
  uint16_t returncode;
  Serial.println(F("Enter Desired DAC Code"));
  Serial.print(F("(Format 32768, 0x8000, 0100000, or B1000000000000000): "));
  returncode = (uint16_t) read_int();
  Serial.print(F("0x"));
  Serial.println(returncode, HEX);
  Serial.flush();
  return(returncode);
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.println();
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC2376 Demonstration Program                                  *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to send data to the LTC2664     *"));
  Serial.println(F("* Four Channel 16/12-bit DAC found on the DC2376 demo board.    *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************"));
}

//! Prints main menu.
void print_prompt(int8_t selected_dac)
{
  // Displays menu
  Serial.println(F("\nCommand Summary:"));
  Serial.println(F("  1-Select DAC"));
  Serial.println(F("  2-Write to input register (no update)"));
  Serial.println(F("  3-Write and update DAC"));
  Serial.println(F("  4-Update / power up DAC"));
  Serial.println(F("  5-Power down DAC"));
  Serial.println(F("  6-Set reference mode"));
  Serial.println(F("  7-Set SoftSpan"));
  Serial.println(F("  8-Toggle selected register"));
  Serial.println(F("  9-Set Mux"));
  Serial.println(F(" 10-Global toggle bit settings"));
  Serial.println(F(" 11-Enable, disable, or store to EEPROM"));
  Serial.println(F(" 12-Voltage Ramp"));
  Serial.println(F("\nPresent Values:"));
  Serial.print(F("  Selected DAC: "));

  // Display current settings
  if (selected_dac >= 3 )
  {
    Serial.println(F("All"));
    Serial.print(F("  SoftSpan range: "));
    Serial.print(LTC2664_MIN_OUTPUT[eeprom.data_struct.soft_span_range[0]], 4);
    Serial.print(F(" V to "));
    Serial.print(LTC2664_MAX_OUTPUT[eeprom.data_struct.soft_span_range[0]], 4);
    Serial.println(F(" V"));
  }
  else
  {
    Serial.println(selected_dac);
    Serial.print(F("  SoftSpan range: "));
    Serial.print(LTC2664_MIN_OUTPUT[eeprom.data_struct.soft_span_range[selected_dac]], 4);
    Serial.print(F(" V to "));
    Serial.print(LTC2664_MAX_OUTPUT[eeprom.data_struct.soft_span_range[selected_dac]], 4);
    Serial.println(F(" V"));
  }
  Serial.print(F("  Toggle Register: "));
  Serial.print(F("\nEnter a command: "));
  Serial.flush();
}

//! Read the alert settings from EEPROM
void restore_dac_from_eeprom()
{
  int16_t cal_key;
  int8_t i;

  // Read the cal key from the EEPROM
  eeprom_read_int16(EEPROM_I2C_ADDRESS, &cal_key, EEPROM_CAL_STATUS_ADDRESS);

  if (cal_key == EEPROM_CAL_KEY)
  {
    // Calibration has been stored, read thresholds
    eeprom_read_byte_array(EEPROM_I2C_ADDRESS, eeprom.byte_array, EEPROM_CAL_STATUS_ADDRESS, sizeof(eeprom_data_union));

    // Write SoftSpan ranges
    for (uint8_t i = 0; i <= 3; i++)
      LTC2664_write(LTC2664_CS, LTC2664_CMD_SPAN, i, eeprom.data_struct.soft_span_range[i]);

    // Write stored data to DAC A registers
    for (uint8_t i = 0; i <= 3 ; i++)
      LTC2664_write(LTC2664_CS,LTC2664_CMD_WRITE_N, i, eeprom.data_struct.dac_code_a[i]);

    LTC2664_write(LTC2664_CS, LTC2664_CMD_TOGGLE_SEL, 0, 0xFFFF);   // Set to B registers

    // Write stored data to DAC B registers
    for (uint8_t i = 0; i <= 3 ; i++)
      LTC2664_write(LTC2664_CS,LTC2664_CMD_WRITE_N, i, eeprom.data_struct.dac_code_b[i]);

    LTC2664_write(LTC2664_CS, LTC2664_CMD_TOGGLE_SEL, 0, eeprom.data_struct.toggle_word);   // Set toggle word

    LTC2664_write(LTC2664_CS, LTC2664_CMD_GLOBAL_TOGGLE, 0, eeprom.data_struct.global_toggle_bit);  // Set global toggle Bit

    LTC2664_write(LTC2664_CS,LTC2664_CMD_MUX, 0, eeprom.data_struct.mux_state); // Set MUX

    LTC2664_write(LTC2664_CS, LTC2664_CMD_CONFIG, 0, eeprom.data_struct.reference_mode);    // Set reference

    LTC2664_write(LTC2664_CS,LTC2664_CMD_UPDATE_ALL, 0, 0); // Update ALL

    Serial.println(F("DAC settings loaded from EEPROM"));
  }
  else
    Serial.println(F("EEPROM data not found"));
}