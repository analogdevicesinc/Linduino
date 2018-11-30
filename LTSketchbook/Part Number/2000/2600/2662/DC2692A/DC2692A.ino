/*!
Linear Technology DC2692A Demonstration Board.
LTC2662: 5-Channel 16-/12-Bit 300mA SoftSpan Current Source DACs

@verbatim
NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

   An external +7V - 5V power supply is required to power the circuit.

   The program displays calculated currents which are based on the voltage
   of the reference used and Radj resistor, be it internal or external. A
   precision voltmeter is needed to verify the actual measured voltages
   against the calculated current displayed.

   If an external reference is used, a precision voltage
   source is required to apply the external reference voltage. A
   precision voltmeter is also required to measure the external reference
   voltage.

  Explanation of Commands:
   1- Select DAC: Select one of five DACs to test : 0 to 4

   2- Write to DAC input register: Value is stored in the DAC for updating
      later, allowing multiple channels to be updated at once, either
      through a software "Update All" command or by asserting the LDAC# pin.
      User will be prompted to enter either a code in hex or decimal, or a
      current. If a current is entered, a code will be calculated based on
      the active scaling and reference parameters - ideal values if no
      calibration was ever stored.

   3- Write and Update: Similar to item 1, but DAC is updated immediately.

   4- Update DAC: Copies the value from the input register into the DAC
      Register. Note that a "write and update" command writes the code to
      BOTH the input register and DAC register, so subsequent "update"
      commands will simply re-copy the same data (no change in output.)

   5- Power Down DAC: Disable DAC output. Power supply current is reduced.
      DAC code present in DAC registers at time of shutdown are preserved.

   6- Set reference mode, either internal or external: Selecting external
      mode powers down the internal reference voltage. It is the users
      responsibility to apply a 1.25v reference.

   7- Set SoftSpan: There are ten options.

   8- Toggle Selected Word: Switch between register A or B for DAC code.

   9- Set MUX: Sets the channel.

   10- Global Toggle Bit Settings - Enabling this feature sets the DAC to
       toggle from register A(when TGL pin is LOW) and register b(when TGL pin
       is HIGH). TGL pin is set HIGH with an internal pull up when the global toggle bit
       is set, and TGL pin is set LOW with an internal pull down when the global toggle bit
       is not set.

   11- Enable, disable, or store to EEPROM: To store to EEROM ensure all
       registers are set to their desired settings. Next, go to th store
       setting EEPROM menu and select it. Upon Linduinos power up, the
       previously stored settings will be restored.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2662

http://www.linear.com/product/LTC2662#demoboards


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
    @ingroup LTC2662
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC2662.h"
#include <SPI.h>
#include <Wire.h>

// Macros
#define REF_EXTERNAL    LTC2662_REF_DISABLE     //!< External mode 
#define REF_INTERNAL    0                       //!< Internal mode

// Function Declaration

void restore_dac_from_eeprom();                         // Restores DAC settings from EEPROM
void print_title();                                     // Print the title block
void print_prompt(int8_t selected_dac);                 // Prompt the user for an input command
int16_t prompt_current_or_code(uint8_t selected_dac);   // Prompt the user for current or code
uint16_t get_current(uint8_t selected_dac);             // Obtains current from user
uint16_t get_code();                                    // Obtains code from user
void display_fault(uint8_t fault);                      // Displays the fault

void menu_1_select_dac(uint8_t *selected_dac);                 // Menu  1: prompts user for DAC selection
uint8_t menu_2_write_to_input_register(uint8_t selected_dac);  // Menu  2: sends data to selected DAC register with no update
uint8_t menu_3_write_and_update_dac(uint8_t selected_dac);     // Menu  3: sends data to selected DAC register with update
uint8_t menu_4_update_power_up_dac(uint8_t selected_dac);      // Menu  4: updates and powers up selected  DAC
uint8_t menu_5_power_down_dac(uint8_t selected_dac);           // Menu  5: powers down selected DAC
uint8_t menu_6_set_reference_mode();                           // Menu  6: prompts user to enable/disable internal reference
uint8_t menu_7_set_softspan_range(uint8_t selected_dac);       // Menu  7: prompts user to sets the SoftSpan range
uint8_t menu_8_toggle_select_word();                           // Menu  8: prompts user to enter the toggle select word
uint8_t menu_9_set_mux();                                      // Menu  9: sets MEX
uint8_t menu_10_global_toggle_settings();                      // Menu 10: sets the global toggle bit
void menu_11_enable_disable_eeprom_restore();                  // Menu 11: EEPOM restore settings
void menu_13_settling_test();                               // Menu 13: Settling time test
void menu_14_demo_board_test();                                // Menu 14: Demo Board functional test


// Global variable
static uint8_t demo_board_connected;   //!< Set to 1 if the board is connected

//! Used to manipulate EEPROM data.
union eeprom_data_union
{
  struct data_struct_type               //! EEPROM data structure
  {
    int16_t cal_key;                    //!< The key that keeps track of the calibration
    uint8_t soft_span_range[5];         //!< SoftSpan range
    uint16_t dac_code_a[16];            //!< DAC Register A
    uint16_t dac_code_b[16];            //!< DAC Register B
    uint16_t toggle_word;               //!< Toggle control word
    uint8_t global_toggle_bit;          //!< Global toggle bit
    uint8_t mux_state;                  //!< Multiplexer address AND enable bit
    uint8_t reference_mode;             //!< Int. reference may be disabled in external reference mode (not required)
  } data_struct;                        //!< Name of structure

  char byte_array[sizeof(data_struct_type)]; //!< Array used to store the structure
};

eeprom_data_union eeprom; // Create union

// Constants

//! Used to keep track to print current or print code
enum
{
  PROMPT_CURRENT = 0, /**< 0 */
  PROMPT_CODE = 1     /**< 1 */
};

//! Initialize Linduino
void setup()
// Setup the program
{
  char demo_name[] = "DC2692";  // Demo Board Name stored in QuikEval EEPROM

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
  uint8_t fault_reg = 0;
  static  uint8_t selected_dac = 0;     // The selected DAC to be updated (0=CH0, 1=CH1 ... 5=All).  Initialized to CH0.

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
        fault_reg |= menu_3_write_and_update_dac(selected_dac);
        break;
      case 4:
        fault_reg |= menu_4_update_power_up_dac(selected_dac);
        break;
      case 5:
        fault_reg |= menu_5_power_down_dac(selected_dac);
        break;
      case 6:
        fault_reg |= menu_6_set_reference_mode();
        break;
      case 7:
        fault_reg |= menu_7_set_softspan_range(selected_dac);
        break;
      case 8:
        fault_reg |= menu_8_toggle_select_word();
        break;
      case 9:
        fault_reg |= menu_9_set_mux();
        break;
      case 10:
        fault_reg |= menu_10_global_toggle_settings();
        break;
      case 11:
        menu_11_enable_disable_eeprom_restore();
		break;
      case 13:
        menu_13_settling_test();
        break;
      case 14:
        menu_14_demo_board_test();
        break;
      default:
        Serial.println(F("Incorrect Option"));
        break;
    }
    display_fault(fault_reg);
    Serial.println(F("\n************************************************************"));
    print_prompt(selected_dac);
  }

}

// Function Definitions

//! Select which DAC to operate on
void menu_1_select_dac(uint8_t *selected_dac)
{
  // Select a DAC to operate on
  Serial.print(F("Select DAC to operate on (0 to 4, 5 for ALL DACs)"));
  *selected_dac = read_int();
  if (*selected_dac == 5)
    Serial.println(F("All"));
  else
    Serial.println(*selected_dac);
}

//! Write data to input register, but do not update DAC output
//! @return fault code
uint8_t menu_2_write_to_input_register(uint8_t selected_dac)
{
  uint16_t dac_code;
  uint8_t fault_reg = 0;

  if (prompt_current_or_code(selected_dac) == PROMPT_CURRENT)
    dac_code = get_current(selected_dac);
  else
    dac_code = get_code();

  if (selected_dac <= 4)
  {
    // Store dac_code to register variable a or b
    if (eeprom.data_struct.toggle_word & (0x01 << selected_dac))
    {
      eeprom.data_struct.dac_code_b[selected_dac] = dac_code;
    }
    else
      eeprom.data_struct.dac_code_a[selected_dac] = dac_code;
    fault_reg |= LTC2662_write(LTC2662_CS, LTC2662_CMD_WRITE_N, selected_dac, dac_code);
  }
  else
  {
    // Store dac_code to register variable a or b
    for (uint8_t i = 0; i <= 4 ; i++)
    {
      if (eeprom.data_struct.toggle_word & (0x01 << i))
      {
        eeprom.data_struct.dac_code_b[i] = dac_code;
      }
      else
        eeprom.data_struct.dac_code_a[i] = dac_code;
    }
    fault_reg |= LTC2662_write(LTC2662_CS, LTC2662_CMD_WRITE_ALL, selected_dac, dac_code);
  }
  return fault_reg;
}

//! Write data to DAC register (which updates output immediately)
//! @return fault code
uint8_t menu_3_write_and_update_dac(uint8_t selected_dac)
{
  uint16_t dac_code;
  uint8_t fault_reg = 0;

  if (prompt_current_or_code(selected_dac) == PROMPT_CURRENT)
    dac_code = get_current(selected_dac);
  else
    dac_code = get_code();

  if (selected_dac <= 4)
  {
    // Store dac_code to register variable a or b
    if (eeprom.data_struct.toggle_word & (0x01 << selected_dac))
    {
      eeprom.data_struct.dac_code_b[selected_dac] = dac_code;
    }
    else
      eeprom.data_struct.dac_code_a[selected_dac] = dac_code;

    fault_reg |= LTC2662_write(LTC2662_CS,LTC2662_CMD_WRITE_N_UPDATE_N, selected_dac, dac_code); // Send dac_code
  }
  else  // All DACs
  {
    // Store dac_code to register variable a or b
    for (uint8_t i = 0; i <= 4 ; i++)
    {
      if (eeprom.data_struct.toggle_word & (0x01 << i))
      {
        eeprom.data_struct.dac_code_b[i] = dac_code;
      }
      else
        eeprom.data_struct.dac_code_a[i] = dac_code;
    }
    fault_reg |= LTC2662_write(LTC2662_CS,LTC2662_CMD_WRITE_ALL_UPDATE_ALL, 0, dac_code);    // Send dac_code
  }
  return fault_reg;
}

//! Update DAC with data that is stored in input register, power up if sleeping
//! @return fault code
uint8_t menu_4_update_power_up_dac(uint8_t selected_dac)
{
  uint8_t fault_reg = 0;
  // Update DAC
  if (selected_dac <= 4)
    fault_reg |= LTC2662_write(LTC2662_CS,LTC2662_CMD_UPDATE_N, selected_dac, 0);
  else
    fault_reg |= LTC2662_write(LTC2662_CS,LTC2662_CMD_UPDATE_ALL, 0, 0);
  return fault_reg;
}

//! Power down DAC
//! @return fault code
uint8_t menu_5_power_down_dac(uint8_t selected_dac)
{
  uint8_t fault_reg = 0;
  // Power down DAC
  if (selected_dac <= 4)
    fault_reg |= LTC2662_write(LTC2662_CS, LTC2662_CMD_POWER_DOWN_N, selected_dac, 0);
  else
    fault_reg |= LTC2662_write(LTC2662_CS, LTC2662_CMD_POWER_DOWN_ALL, 0, 0);
  return fault_reg;
}

//! Set reference mode
//! @return fault code
uint8_t menu_6_set_reference_mode(void)
{
  uint8_t fault_reg = 0;
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
    fault_reg |= LTC2662_write(LTC2662_CS, LTC2662_CMD_CONFIG, 0, eeprom.data_struct.reference_mode);
    Serial.println(F("Internal reference has been powered up"));
  }
  else
  {
    eeprom.data_struct.reference_mode = REF_EXTERNAL;
    fault_reg |= LTC2662_write(LTC2662_CS, LTC2662_CMD_CONFIG, 0, eeprom.data_struct.reference_mode);
    Serial.println(F("Internal reference has been powered down"));
  }
  return fault_reg;
}

//! Set SoftSpan Range
//! @return fault code
uint8_t menu_7_set_softspan_range(uint8_t selected_dac)
{
  int16_t user_command;
  uint8_t fault_reg = 0;

  Serial.println(F("0- Hi-Z"));
  Serial.println(F("1- 0mA to 3.125mA"));
  Serial.println(F("2- 0mA to 6.25mA"));
  Serial.println(F("3- 0mA to 12.5mA"));
  Serial.println(F("4- 0mA to 25mA"));
  Serial.println(F("5- 0mA to 50mA"));
  Serial.println(F("6- 0mA to 100mA"));
  Serial.println(F("7- 0mA to 200mA"));
  Serial.println(F("8- Switch to V-"));
  Serial.println(F("9- 0mA to 300mA"));
  Serial.print(F("Select a SoftSpan Range: "));
  user_command = read_int();

  if (user_command > 9)
    user_command = 9;
  if (user_command < 0)
    user_command = 0;
  Serial.println(user_command);

  // User command 9 maps to 0xF
  if (user_command == 9)
    user_command = 15;
  if (selected_dac <= 4)
  {
    eeprom.data_struct.soft_span_range[selected_dac] = user_command;
    fault_reg |= LTC2662_write(LTC2662_CS, LTC2662_CMD_SPAN, selected_dac, eeprom.data_struct.soft_span_range[selected_dac]);
  }
  else
  {
    for (uint8_t i = 0; i <= 4; i++)
      eeprom.data_struct.soft_span_range[i] = user_command;
    fault_reg |= LTC2662_write(LTC2662_CS, LTC2662_CMD_SPAN_ALL, 0, eeprom.data_struct.soft_span_range[0]);;
  }
  return fault_reg;
}

//! Enter toggle select word, which also sets the register that will be written
//! if bit is 0, register A is written, if 1, register B is written.
//! @return fault code
uint8_t menu_8_toggle_select_word()
{
  uint8_t fault_reg = 0;
  // Select a DAC to operate on
  Serial.println(F("Toggle Select bit sets the register to be written for the corresponding DAC."));
  Serial.println(F("0 for Register A or 1 for Register B."));
  Serial.println(F("Note: DAC Update from Register B requires TGB = 1"));
  Serial.println(F("      DAC Update from Register A requires TCB = 0"));
  Serial.print(F("Enter Toggle Select Byte as hex (0xMM) or binary (B10101010): "));
  eeprom.data_struct.toggle_word = 0x1F & read_int();

  fault_reg |= LTC2662_write(LTC2662_CS, LTC2662_CMD_TOGGLE_SEL, 0, eeprom.data_struct.toggle_word);
  Serial.println(eeprom.data_struct.toggle_word, BIN);
  return fault_reg;
}

//! Sets the channel for the MUX
//! @return fault code
uint8_t menu_9_set_mux()
{
  int16_t user_command;
  uint8_t fault_reg = 0;

  Serial.println(F("MUX Control:"));
  Serial.println(F("0- Disable(Hi-Z)"));
  Serial.println(F("1- OUT0 Current Mesurement"));
  Serial.println(F("2- OUT1 Current Mesurement"));
  Serial.println(F("3- OUT2 Current Mesurement"));
  Serial.println(F("4- OUT3 Current Mesurement"));
  Serial.println(F("5- OUT4 Current Mesurement"));
  Serial.println(F("6- VCC"));
  Serial.println(F("7- IOVCC"));
  Serial.println(F("8- VREF"));
  Serial.println(F("9- VREFLO"));
  Serial.println(F("10- Die Temperatur"));
  Serial.println(F("16- VDD0"));
  Serial.println(F("17- VDD1"));
  Serial.println(F("18- VDD2"));
  Serial.println(F("19- VDD3"));
  Serial.println(F("20- VDD4"));
  Serial.println(F("21- V+"));
  Serial.println(F("22- V-"));
  Serial.println(F("23- GND"));
  Serial.println(F("24- OUT0 Pin Voltage"));
  Serial.println(F("25- OUT1 Pin Voltage"));
  Serial.println(F("26- OUT2 Pin Voltage"));
  Serial.println(F("27- OUT3 Pin Voltage"));
  Serial.println(F("28- OUT4 Pin Voltage"));
  Serial.print(F("Enter a command: "));
  user_command = read_int();


  if (user_command < 0)
    user_command = 0;
  user_command = user_command & 0x1F;
  Serial.println(user_command);
  eeprom.data_struct.mux_state = user_command;
  fault_reg |= LTC2662_write(LTC2662_CS,LTC2662_CMD_MUX, 0, eeprom.data_struct.mux_state);
  return fault_reg;
}
//! Enable / Disable the global toggle bit
//! @return fault code
uint8_t menu_10_global_toggle_settings()
{
  int16_t user_command;
  uint8_t fault_reg = 0;
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
  fault_reg |= LTC2662_write(LTC2662_CS, LTC2662_CMD_GLOBAL_TOGGLE, 0, eeprom.data_struct.global_toggle_bit);
  return fault_reg;
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


// Functional test for demo board, tests continuity, EEPROM contents,
// And correct resolution DAC installed.
void menu_14_demo_board_test()
{

  uint8_t i, errorcount = 0;

  Serial.println(F("Welcome to the LTC2662 Demo Board Test Program."));
  Serial.print(F("Found board assembly type "));
  Serial.println(demo_board.option);

  Serial.println(F("Checking EEPROM contents..."));

  read_quikeval_id_string(&ui_buffer[0]);
  ui_buffer[48] = 0;
  Serial.println(ui_buffer);
  switch (demo_board.option)
  {
    case 'A':
      if (strcmp("LTC2662-16,Cls,D2662,01,01,DC,DC2692A-A,--------", ui_buffer))
      {
        Serial.println(F("FAILED EEPROM Contents"));
        errorcount += 1;
      }
      break;
    case 'B':
      if (strcmp("LTC2662-12,Cls,D2662,01,01,DC,DC2692A-B,--------", ui_buffer))
      {
        Serial.println(F("FAILED EEPROM Contents"));
        errorcount += 1;
      }
      break;
  }
  if (!errorcount)
    Serial.println(F("EEPROM good!"));

  Serial.println(F("Ensure all jumpers are in the Upper position (INT ref, VDDx=V+"));
  Serial.println(F("Writing ramp of currents to DAC outputs. Verify DAC0 = 10mA, DAC1 = 20mA,"));
  Serial.println(F("DAC2 = 30mA ... DAC4 = 50MAV. Type 1, enter when done."));

  LTC2662_write(LTC2662_CS, LTC2662_CMD_CONFIG, 0, REF_INTERNAL);

  for (i=0; i <= 4; ++i)
  {
    LTC2662_write(LTC2662_CS,LTC2662_CMD_SPAN, i, LTC2662_SPAN_100MA);
    LTC2662_write(LTC2662_CS,LTC2662_CMD_WRITE_N_UPDATE_N, i, (i + 1) * 6554);
  }

  read_int();        // Read the user command

  Serial.println(F("Setting MUX to VREF."));
  Serial.println(F("Verify MUX output is between 1.24V and 1.26V"));

  LTC2662_write(LTC2662_CS,LTC2662_CMD_MUX, 0, 8);
  LTC2662_write(LTC2662_CS,LTC2662_CMD_UPDATE_ALL, 0, 0);

  Serial.println(F("Type 1, enter when done."));
  read_int();

  Serial.println(F("Remove the load from OUT0"));
  Serial.println(F("The Fault LED should light up"));
  Serial.println(F("Type 1, enter when done."));
  read_int();

}

//! Prompt user to enter a current or digital code to send to DAC
//! @returns user input
int16_t prompt_current_or_code(uint8_t selected_dac )
{
  int16_t user_input;

  Serial.print(F("Type 1 to enter current, 2 to enter code: "));
  Serial.flush();
  user_input = read_int();
  Serial.println(user_input);

  if (user_input != 2)
  {
    if (selected_dac >= 5)
    {
      Serial.println(F("\nCaution! current SoftSpan could be different for different DACs"));
      Serial.println(F("Ensure All DACs can be set to desired currents"));
      Serial.println(F("DAC 0 is used for SoftSpan values for all DACs."));
      Serial.print(F("Toggle Register Select Bits 0=A, 1 = B: 0b"));
      Serial.println(eeprom.data_struct.toggle_word, BIN);
    }
    return(PROMPT_CURRENT);
  }
  else
  {
    if (selected_dac >= 5)
    {
      Serial.println(F("DAC 0 is used for SoftSpan values for all DACs."));
    }
  }
  return(PROMPT_CODE);
}

//! Get current from user input, calculate DAC code based on lsb, offset
//! @returns the DAC code
uint16_t get_current(uint8_t selected_dac)
{
  float dac_current;

  Serial.print(F("Enter Desired DAC output current: "));
  dac_current = read_float();
  Serial.print(dac_current);
  Serial.println(F(" mA"));
  Serial.flush();

  if (selected_dac <= 4)
  {
    if (eeprom.data_struct.soft_span_range[selected_dac] == 15)
      return(LTC2662_current_to_code(dac_current, LTC2662_MAX_OUTPUT[9]));
    else
      return(LTC2662_current_to_code(dac_current,
                                     LTC2662_MAX_OUTPUT[eeprom.data_struct.soft_span_range[selected_dac]]));
  }
  else
  {
    if (eeprom.data_struct.soft_span_range[0] == 15)
      return(LTC2662_current_to_code(dac_current, LTC2662_MAX_OUTPUT[9]));
    else
      return(LTC2662_current_to_code(dac_current,
                                     LTC2662_MAX_OUTPUT[eeprom.data_struct.soft_span_range[0]]));
  }
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
  Serial.println(F("* DC2692 Demonstration Program                                  *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to send data to the LTC2662     *"));
  Serial.println(F("* five Channel 16/12-bit DAC found on the DC2692 demo board.    *"));
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
  Serial.println(F("\nPresent Values:"));
  Serial.print(F("  Selected DAC: "));

  // Display current settings
  if (selected_dac >= 4 )
  {
    Serial.println(F("All"));
    Serial.print(F("  SoftSpan range: "));
    if (eeprom.data_struct.soft_span_range[0] == 0)
      Serial.println(F("Hi-Z"));
    else if (eeprom.data_struct.soft_span_range[0] == 8)
      Serial.println(F("Switch to V-"));
    else
    {
      Serial.print(F("0 mA to "));
      Serial.print(LTC2662_MAX_OUTPUT[eeprom.data_struct.soft_span_range[0]], 4);
      Serial.println(F(" mA"));
    }
  }
  else
  {
    Serial.println(selected_dac);
    Serial.print(F("  SoftSpan range: "));
    if (eeprom.data_struct.soft_span_range[selected_dac] == 0)
      Serial.println(F("Hi-Z"));
    else if (eeprom.data_struct.soft_span_range[selected_dac] == 8)
      Serial.println(F("Switch to V-"));
    else
    {
      Serial.print(F("0 mA to "));
      if (eeprom.data_struct.soft_span_range[selected_dac] == 15)
        Serial.print(LTC2662_MAX_OUTPUT[9], 4);
      else
        Serial.print(LTC2662_MAX_OUTPUT[eeprom.data_struct.soft_span_range[selected_dac]], 4);
      Serial.println(F(" mA"));
    }
  }
  Serial.print(F("  Toggle Register: "));
  for (uint8_t i = 0; i <= 4; i++)
  {
    if ((eeprom.data_struct.toggle_word &  (0x10 >> i)))
      Serial.print(F("B"));
    else
      Serial.print(F("A"));
  }
  Serial.println();

  Serial.print(F("  Global Toggle Bit: "));
  Serial.println(eeprom.data_struct.global_toggle_bit);

  Serial.print(F("  Reference: "));
  if (eeprom.data_struct.reference_mode == REF_INTERNAL)
    Serial.println(F("Internal"));
  else
    Serial.println(F("External"));

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
    for (uint8_t i = 0; i <= 15; i++)
      LTC2662_write(LTC2662_CS, LTC2662_CMD_SPAN, i, eeprom.data_struct.soft_span_range[i]);

    // Write stored data to DAC A registers
    for (uint8_t i = 0; i <= 15 ; i++)
      LTC2662_write(LTC2662_CS,LTC2662_CMD_WRITE_N, i, eeprom.data_struct.dac_code_a[i]);

    LTC2662_write(LTC2662_CS, LTC2662_CMD_TOGGLE_SEL, 0, 0xFFFF);   // Set to B registers

    // Write stored data to DAC B registers
    for (uint8_t i = 0; i <= 15 ; i++)
      LTC2662_write(LTC2662_CS,LTC2662_CMD_WRITE_N, i, eeprom.data_struct.dac_code_b[i]);

    LTC2662_write(LTC2662_CS, LTC2662_CMD_TOGGLE_SEL, 0, eeprom.data_struct.toggle_word);   // Set toggle word

    LTC2662_write(LTC2662_CS, LTC2662_CMD_GLOBAL_TOGGLE, 0, eeprom.data_struct.global_toggle_bit);  // Set global toggle Bit

    LTC2662_write(LTC2662_CS,LTC2662_CMD_MUX, 0, eeprom.data_struct.mux_state); // Set MUX

    LTC2662_write(LTC2662_CS, LTC2662_CMD_CONFIG, 0, eeprom.data_struct.reference_mode);    // Set reference

    LTC2662_write(LTC2662_CS,LTC2662_CMD_UPDATE_ALL, 0, 0); // Update ALL

    Serial.println(F("DAC settings loaded from EEPROM"));
  }
  else
    Serial.println(F("EEPROM data not found"));
}

//! Display fault conditions
void display_fault(uint8_t fault)
{
  if (fault & 0x01)
    Serial.println(F("********    Open-Circuit detected on OUT0    ********"));
  if (fault & 0x02)
    Serial.println(F("********    Open-Circuit detected on OUT1    ********"));
  if (fault & 0x04)
    Serial.println(F("********    Open-Circuit detected on OUT2    ********"));
  if (fault & 0x08)
    Serial.println(F("********    Open-Circuit detected on OUT3    ********"));
  if (fault & 0x10)
    Serial.println(F("********    Open-Circuit detected on OUT4    ********"));
  if (fault & 0x20)
    Serial.println(F("********    Over Temperature. Die > 160 C    ********"));
  if (fault & 0x40)
    Serial.println(F("********    10V Power Limit Exceeded on VDD to IOUT    ********"));
  if (fault & 0x80)
    Serial.println(F("********    Invalid SPI Sequence Length    ********"));
}



///////////////////////////////////////////////////////////////////////////
// The following functions are used internally for testing, and included //
// here for reference.                                                   //
///////////////////////////////////////////////////////////////////////////

// Settling time routine, used for datasheet pictures. Set scope to ARM trigger on SETTLE_TRIGGER
// Rising edge, trigger on CS pin for the actual edge that causes the update.
// You can test any other channel / starting / final values by setting up the DAC's
// register A to the initial (starting) voltage and register B to the final (settled) voltage.
// The channel's toggle bit must be set as well.

#define INTEGRATOR_CONTROL  2               // Pin to control integrator on settling time circuit
// Set low for 100ms after full settling has occurred,
// high to run test.
#define SETTLE_TRIGGER      3               // High to low edge close to DAC active edge. Can also be
// used to drive DAC simulator.

void menu_13_settling_test()
{
  int16_t user_input, initial, final;
  int8_t range;
  int8_t settle_channel;// Which DAC to test for settling (only one at a time to prevent
                        // crosstalk artefacts.)
  Serial.println(F("Select test to run:"));
  Serial.println(F("0: presently programmed DAC values"));
  Serial.println(F("1: 0-200mA rising, CH1"));
  Serial.println(F("2: 200mA-0 falling, CH1"));
  Serial.println(F("3: 145mA-155mA rising, CH1"));
  Serial.println(F("4: 155mA-145mA falling, CH1"));
  Serial.println(F("5: 0-3.125mA rising, CH3"));
  Serial.println(F("6: 3.125-0 falling, CH3"));

  Serial.flush();
  user_input = read_int();
  switch (user_input)
  {
    case 1:
	  settle_channel = 1;
      initial = 0x0000;
      final = 0xFFFF;
      range = LTC2662_SPAN_200MA;
      break;
    case 2:
	  settle_channel = 1;
      initial = 0xFFFF;
      final = 0x0000;
      range = LTC2662_SPAN_200MA;
      break;
    case 3:
	  settle_channel = 1;
      initial = 0xB99A;
      final = 0xC666;
      range = LTC2662_SPAN_200MA;
      break;
    case 4:
	  settle_channel = 1;
      initial = 0xC666;
      final = 0xB99A;
      range = LTC2662_SPAN_200MA;
      break;
    case 5:
	  settle_channel = 3;
      initial = 0x0000;
      final = 0xFFFF;
      range = LTC2662_SPAN_3_125MA;
      break;
    case 6:
	  settle_channel = 3;
      initial = 0xFFFF;
      final = 0x0000;
      range = LTC2662_SPAN_3_125MA;
      break;
    default:
      Serial.println("Using presently programmed DAC values");
  }
  if (user_input != 0) // Load DAC to be tested with range, initial, and final values.
  {
    LTC2662_write(LTC2662_CS, LTC2662_CMD_SPAN, settle_channel, range); // Set Span
    LTC2662_write(LTC2662_CS, LTC2662_CMD_TOGGLE_SEL, 0, 0x0000); // Point to register A
    LTC2662_write(LTC2662_CS, LTC2662_CMD_WRITE_N_UPDATE_N, settle_channel, initial); // Send dac_code
    LTC2662_write(LTC2662_CS, LTC2662_CMD_TOGGLE_SEL, 0, 0x0001 << settle_channel); // Point to register B
    LTC2662_write(LTC2662_CS, LTC2662_CMD_WRITE_N_UPDATE_N, settle_channel, final); // Send dac_code
  }
  Serial.println("Settling time test running, send any character to terminate.");
  pinMode(INTEGRATOR_CONTROL, OUTPUT);
  pinMode(SETTLE_TRIGGER, OUTPUT);
  while (!Serial.available())
  {
    PORTD |= 1 << INTEGRATOR_CONTROL; // Disable integrator
    PORTD |= 1 << SETTLE_TRIGGER;
    LTC2662_write(LTC2662_CS, LTC2662_CMD_GLOBAL_TOGGLE, 0, 0); // Set global toggle bit LOW, INITIAL value
    PORTD &= ~(1 << SETTLE_TRIGGER); // Reset settle trigger LOW
    delay(10); // Wait to get to initial value
    LTC2662_write(LTC2662_CS, LTC2662_CMD_GLOBAL_TOGGLE, 0, 1); // Set global toggle HIGH, FINAL value
    delay(10); // Wait to get to final value, here is where you look at the settling
    PORTD &= ~(1 << INTEGRATOR_CONTROL); // Enable integrator
    delay(100);
  }
  Serial.read();
  Serial.println("Exiting Settling time test.");
}