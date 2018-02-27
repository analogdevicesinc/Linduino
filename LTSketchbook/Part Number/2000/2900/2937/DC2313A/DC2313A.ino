/*!
Linear Technology DC2313A Demonstration

LTC2937: Six Channel Sequencer and Voltage Supervisor with EEPROM

@verbatim
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim

http://www.linear.com/product/LTC2937

http://www.linear.com/demo/DC2313A


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
    @ingroup LTC2937
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
//#include "LT_I2CBus.h"
#include "LT_SMBusNoPec.h"
//#include "LT_SMBusPec.h"
//#include "LT_PMBUS.h"
//#include "LT_I2C.h"
#include "LTC2937.h"

#define LTC2937_I2C_ADDRESS 0x36 //global 7-bit address
//#define LTC2937_I2C_ADDRESS 0x50 //HHZ 7-bit address


/****************************************************************************/
// Global variables
static uint8_t ltc2937_i2c_address;

//static LT_I2CBus *i2cbus = new LT_I2CBus();
//static LT_I2CBus *i2cbus = new LT_I2CBus();
//static LT_SMBusNoPec *smbus = new LT_SMBusNoPec(i2cbus);
static LT_SMBusNoPec *smbus = new LT_SMBusNoPec();


// function return values
enum return_values {SUCCEED=0, // the function succeeded without errors
                    NOT_DOWN, // the LTC2937 is not sequenced-down
                    WRITE_PROTECTED, // the LTC2937 is write-protected
                    FAIL  // general failure to do the right thing
                   };

// delay times in milliseconds
#define LTC2937_RESTORE_DELAY   10
#define LTC2937_STORE_DELAY   120

/****************************************************************************/
//! Initialize Linduino
//! @return void
void setup()
{
  uint16_t return_val;

  ltc2937_i2c_address = LTC2937_I2C_ADDRESS;

  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  print_prompt();

  ltc2937_config0();
}

/****************************************************************************/
//! Main Linduino Loop
//! @return void
void loop()
{
  uint8_t user_command;

  if (Serial.available())                //! Checks for user input
  {
    user_command = read_int();         //! Reads the user command

    switch (user_command)
    {

      case 1 :
        Serial.print(F("1\n"));
        Serial.print(F("\n****INITIALIZING THE LTC2937****\n"));
        ltc2937_write_enable();
        ltc2937_restore();
        break;

      case 2:
        Serial.print(F("2\n"));
        Serial.print(F("\n****SEQUENCE-UP THE LTC2937****\n"));
        //    ltc2937_write_enable();
        ltc2937_sequence_up();
        break;

      case 3:
        Serial.print(F("3\n"));
        Serial.print(F("\n****SEQUENCE-DOWN THE LTC2937****\n"));
        //    ltc2937_write_enable();
        ltc2937_sequence_down();
        break;

      case 4:
        Serial.print(F("4\n"));
        Serial.print(F("\n****READ LTC2937 STATUS****\n"));
        ltc2937_read_all_status();
        break;

      case 5:
        Serial.print(F("5\n"));
        Serial.print(F("\n****SET LTC2937 BREAKPOINT AT STEP 1****\n"));
        ltc2937_write_enable();
        ltc2937_set_breakpoint(0x0001);
        ltc2937_print_seq_pos_count();
        break;

      case 6:
        Serial.print(F("6\n"));
        Serial.print(F("\n****INCREMENT LTC2937 BREAKPOINT****\n"));
        ltc2937_write_enable();
        ltc2937_inc_breakpoint();
        ltc2937_print_seq_pos_count();
        break;

      case 7:
        Serial.print(F("7\n"));
        Serial.print(F("\n****CLEAR LTC2937 BREAKPOINT****\n"));
        ltc2937_write_enable();
        ltc2937_clear_breakpoint();
        break;

      case 8:
        Serial.print(F("8\n"));
        Serial.print(F("\n****CONFIGURE SPECIAL LTC2937 SEQUENCING VALUES****\n"));
        ltc2937_write_seq_regs();
        break;

      case 9:
        Serial.print(F("9\n"));
        Serial.print(F("\n****STORE LTC2937 CONFIGURATION TO EEPROM****\n"));
        ltc2937_write_enable();
        ltc2937_store();
        break;

      case 10:
        Serial.print(F("10\n"));
        Serial.print(F("\n****WRITE DEMO BOARD DEFAULTS TO LTC2937 RAM****\n"));
        ltc2937_write_enable();
        ltc2937_write_all_regs_dc_default();
        break;

      case 11:
        Serial.print(F("11\n"));
        Serial.print(F("\n****READ LTC2937 FAULTS****\n"));
        ltc2937_pretty_print_monitor_status();
        Serial.print(F("\n*******************************\n"));
        ltc2937_pretty_print_monitor_status_history();
        Serial.print(F("\n*******************************\n"));
        ltc2937_pretty_print_status_information();
        Serial.print(F("\n*******************************\n"));
        break;
      case 12:
        Serial.print(F("12\n"));
        Serial.print(F("\n****CLEAR LTC2937 FAULT BACKUP WORD****\n"));
        //    ltc2937_write_enable();
        ltc2937_clear_fault_backup();
        break;

      case 13:
        Serial.print(F("13\n"));
        Serial.print(F("\n****CLEAR LTC2937 ALERT****\n"));
        //    ltc2937_write_enable();
        ltc2937_clear_alertb();
        break;

      case 14:
        Serial.print(F("14\n"));
        Serial.print(F("\n****WRITE-ENABLE THE LTC2937****\n"));
        ltc2937_write_enable();
        break;

      default:
        Serial.print(F("15\n"));
        Serial.println(F("Incorrect Option"));
        break;
    }
    print_prompt();
  }
}


/************************************************************************/
// Function Definitions

//! Prints the title block when program first starts.
//! @return void
void print_title()
{
  Serial.print(F("\n***************************************************************\n"));
  Serial.print(F("* DC2313A Control Program                                       *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program provides a simple interface to control the       *\n"));
  Serial.print(F("* the LTC2937 on the DC2313A Demo Board                         *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
//! @return void
void print_prompt()
{
  Serial.print(F("\n"));
  Serial.print(F("  1  - Restore the LTC2937 to EEPROM values\n"));
  Serial.print(F("  2  - Sequence-up supplies\n"));
  Serial.print(F("  3  - Sequence-down supplies\n"));
  Serial.print(F("  4  - Read all status registers\n"));
  Serial.print(F("  5  - Set breakpoint at step 1\n"));
  Serial.print(F("  6  - Increment breakpoint\n"));
  Serial.print(F("  7  - Clear breakpoint\n"));
  Serial.print(F("  8  - Specify a new sequence order\n"));
  Serial.print(F("  9  - Write configuration to EEPROM\n"));
  Serial.print(F("  10 - Write demo board defaults to RAM\n"));
  Serial.print(F("  11 - Read fault information registers\n"));
  Serial.print(F("  12 - Clear faults (including backup)\n"));
  Serial.print(F("  13 - Clear alert\n"));
  Serial.print(F("  14 - Release software write-protect bit\n"));
  Serial.print(F("\n Enter a command number: "));
}

//! make sure that the LTC2937 is configured for I2C bus control
//! @return int for success
int ltc2937_config0()
{
  uint16_t return_val, on_off_bits;
  //  ltc2937_write_enable();
  // ensure that the ON/OFF control is I2C, and ignores the ON pin
  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_ON_OFF_CONTROL);
  if (ltc2937_is_down() == SUCCEED)
  {
    on_off_bits = ((return_val & 0x00E3) + 0x0008); // mask the bits: b[15:8] and b[4:2]
    return_val = return_val | on_off_bits;  // set the ON/OFF control to I2C only
    smbus->writeWord(ltc2937_i2c_address, LTC2937_ON_OFF_CONTROL, return_val);
  }
  else
  {
    return NOT_DOWN;
  }
}

//! restore the LTC2937 settings from EEPROM values
//! @return int for success
int ltc2937_restore()
{
  uint16_t return_val;

  if (ltc2937_is_down() == SUCCEED)
  {
    // send the RESTORE command
    smbus->sendByte(ltc2937_i2c_address, LTC2937_RESTORE);
    //    return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_RESTORE);
    Serial.print (F("\nRESTORED LTC2937 CONFIGURATION FROM EEPROM.\n"));
    delay(LTC2937_RESTORE_DELAY);
    return SUCCEED;
  }
  else
  {
    Serial.print(F("\n IT IS A BAD IDEA TO RESTORE WHILE THE SUPPLIES ARE SEQUENCED-UP!\n"));
    Serial.print(F("   --SEQUENCE-DOWN FIRST. \n"));
    return NOT_DOWN;
  }
}

//! store the LTC2937 settings into EEPROM
//! @return int for success
int ltc2937_store()
{
  uint16_t return_val;

  if (ltc2937_is_down() == SUCCEED)
  {
    smbus->sendByte(ltc2937_i2c_address, LTC2937_STORE);
    //    smbus->writeWord(ltc2937_i2c_address, LTC2937_STORE, 0xFFFF);
    //    return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_STORE);
    Serial.print (F("STORED LTC2937 CONFIGURATION TO EEPROM.\n"));
    delay(LTC2937_STORE_DELAY);
    return SUCCEED;
  }
  else
  {
    Serial.print(F("\n IT IS A BAD IDEA TO STORE WHILE THE SUPPLIES ARE SEQUENCED-UP!\n"));
    Serial.print(F("   --SEQUENCE-DOWN FIRST. \n"));
    return NOT_DOWN;
  }
}

//! sequence-up the LTC2937
//! @return int for success
int ltc2937_sequence_up()
{
  uint16_t return_val;

  if ( ltc2937_is_write_protected() != WRITE_PROTECTED)
  {
    return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_ON_OFF_CONTROL);
    return_val = (return_val & 0xFFEB); // mask the bits of interest
    return_val = (return_val | 0x0018);  // set the ON/OFF control to ON
    smbus->writeWord(ltc2937_i2c_address, LTC2937_ON_OFF_CONTROL, return_val);
    return SUCCEED;
  }
  else
  {
    Serial.print (F("FAIL: WRITE_PROTECTED.\n"));
    return WRITE_PROTECTED;
  }
}

//! sequence-down the LTC2937
//! @return int for success
int ltc2937_sequence_down()
{
  uint16_t return_val;

  if ( ltc2937_is_write_protected() != WRITE_PROTECTED)
  {
    return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_ON_OFF_CONTROL);
    return_val = (return_val & 0xFFEF); // mask the bits
    smbus->writeWord(ltc2937_i2c_address, LTC2937_ON_OFF_CONTROL, return_val);
    return SUCCEED;
  }
  else
  {
    Serial.print (F("FAIL: WRITE_PROTECTED.\n"));
    return WRITE_PROTECTED;
  }
}

//! indicate if the LTC2937 is in the sequenced-down state
//! @return int for part staus
int ltc2937_is_down()
{
  uint16_t return_val,
           on_off,
           status;

  // This function indicates when it is safe to issue a CLEAR command

  // must meet the following conditions:
  // ON_OFF_CONTROL[7] == 1'b0 (commanded down)
  // STATUS_INFORMATION[11:10] == 2'b00 (local seq down complete)
  // STATUS_INFORMATION[9:8] == 2'b00 (global seq down complete)
  // STATUS_INFORMATION[2] == 1'b1 (supplies discharged)

  // may also want:
  // BREAK_POINT[10] == 1'b0 (not in BP mode)

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_STATUS_INFORMATION);
  status = (return_val & 0x0F04); // mask the bits
  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_ON_OFF_CONTROL);
  on_off = (return_val & 0x0080); // mask the bits
  if ((status == 0x0004) && (on_off == 0x0000))
  {
    return SUCCEED;
  }
  else
  {
    Serial.print(F("\n LTC2937 IS NOT DOWN \n"));
    return NOT_DOWN;
  }
}

//! read all status registers
//! @return int for success
int ltc2937_read_all_status()
{
  uint16_t return_val;

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_WRITE_PROTECTION);
  Serial.print(F("\n LTC2937_WRITE_PROTECTION: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_SPECIAL_LOT);
  Serial.print(F("\n LTC2937_SPECIAL_LOT: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_ON_OFF_CONTROL);
  Serial.print(F("\n LTC2937_ON_OFF_CONTROL: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_MONITOR_STATUS_HISTORY);
  Serial.print(F("\n LTC2937_MONITOR_STATUS_HISTORY: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_STATUS_INFORMATION);
  Serial.print(F("\n LTC2937_STATUS_INFORMATION: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_BREAK_POINT);
  Serial.print(F("\n LTC2937_BREAK_POINT: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_FAULT_RESPONSE);
  Serial.print(F("\n LTC2937_FAULT_RESPONSE: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_SEQ_POSITION_COUNT);
  Serial.print(F("\n LTC2937_SEQ_POSITION_COUNT: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_MONITOR_BACKUP);
  Serial.print(F("\n LTC2937_MONITOR_BACKUP: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_MONITOR_STATUS);
  Serial.print(F("\n LTC2937_MONITOR_STATUS: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_DEVICE_ID);
  Serial.print(F("\n LTC2937_DEVICE_ID: "));
  Serial.println(return_val, HEX);
  return SUCCEED;
}



//! read all registers
//! return int for success
int ltc2937_read_all_registers()
{
  uint16_t return_val;

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_WRITE_PROTECTION);
  Serial.print(F("\n LTC2937_WRITE_PROTECTION: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_SPECIAL_LOT);
  Serial.print(F("\n LTC2937_SPECIAL_LOT: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_ON_OFF_CONTROL);
  Serial.print(F("\n LTC2937_ON_OFF_CONTROL: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_V_RANGE);
  Serial.print(F("\n LTC2937_V_RANGE: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_V_THRESHOLD_1);
  Serial.print(F("\n LTC2937_V_THRESHOLD_1: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_V_THRESHOLD_2);
  Serial.print(F("\n LTC2937_V_THRESHOLD_2: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_V_THRESHOLD_3);
  Serial.print(F("\n LTC2937_V_THRESHOLD_3: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_V_THRESHOLD_4);
  Serial.print(F("\n LTC2937_V_THRESHOLD_4: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_V_THRESHOLD_5);
  Serial.print(F("\n LTC2937_V_THRESHOLD_5: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_V_THRESHOLD_6);
  Serial.print(F("\n LTC2937_V_THRESHOLD_6: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_TON_TIMERS_1);
  Serial.print(F("\n LTC2937_TON_TIMERS_1: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_TON_TIMERS_2);
  Serial.print(F("\n LTC2937_TON_TIMERS_2: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_TON_TIMERS_3);
  Serial.print(F("\n LTC2937_TON_TIMERS_3: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_TON_TIMERS_4);
  Serial.print(F("\n LTC2937_TON_TIMERS_4: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_TON_TIMERS_5);
  Serial.print(F("\n LTC2937_TON_TIMERS_5: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_TON_TIMERS_6);
  Serial.print(F("\n LTC2937_TON_TIMERS_6: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_TOFF_TIMERS_1);
  Serial.print(F("\n LTC2937_TOFF_TIMERS_1: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_TOFF_TIMERS_2);
  Serial.print(F("\n LTC2937_TOFF_TIMERS_2: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_TOFF_TIMERS_3);
  Serial.print(F("\n LTC2937_TOFF_TIMERS_3: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_TOFF_TIMERS_4);
  Serial.print(F("\n LTC2937_TOFF_TIMERS_4: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_TOFF_TIMERS_5);
  Serial.print(F("\n LTC2937_TOFF_TIMERS_5: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_TOFF_TIMERS_6);
  Serial.print(F("\n LTC2937_TOFF_TIMERS_6: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_SEQ_UP_POSITION_1);
  Serial.print(F("\n LTC2937_SEQ_UP_POSITION_1: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_SEQ_UP_POSITION_2);
  Serial.print(F("\n LTC2937_SEQ_UP_POSITION_2: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_SEQ_UP_POSITION_3);
  Serial.print(F("\n LTC2937_SEQ_UP_POSITION_3: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_SEQ_UP_POSITION_4);
  Serial.print(F("\n LTC2937_SEQ_UP_POSITION_4: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_SEQ_UP_POSITION_5);
  Serial.print(F("\n LTC2937_SEQ_UP_POSITION_5: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_SEQ_UP_POSITION_6);
  Serial.print(F("\n LTC2937_SEQ_UP_POSITION_6: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_SEQ_DOWN_POSITION_1);
  Serial.print(F("\n LTC2937_SEQ_DOWN_POSITION_1: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_SEQ_DOWN_POSITION_2);
  Serial.print(F("\n LTC2937_SEQ_DOWN_POSITION_2: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_SEQ_DOWN_POSITION_3);
  Serial.print(F("\n LTC2937_SEQ_DOWN_POSITION_3: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_SEQ_DOWN_POSITION_4);
  Serial.print(F("\n LTC2937_SEQ_DOWN_POSITION_4: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_SEQ_DOWN_POSITION_5);
  Serial.print(F("\n LTC2937_SEQ_DOWN_POSITION_5: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_SEQ_DOWN_POSITION_6);
  Serial.print(F("\n LTC2937_SEQ_DOWN_POSITION_6: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_RSTB_CONFIG);
  Serial.print(F("\n LTC2937_RSTB_CONFIG: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_FAULT_RESPONSE);
  Serial.print(F("\n LTC2937_FAULT_RESPONSE: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_MONITOR_STATUS_HISTORY);
  Serial.print(F("\n LTC2937_MONITOR_STATUS_HISTORY: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_STATUS_INFORMATION);
  Serial.print(F("\n LTC2937_STATUS_INFORMATION: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_BREAK_POINT);
  Serial.print(F("\n LTC2937_BREAK_POINT: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_SEQ_POSITION_COUNT);
  Serial.print(F("\n LTC2937_SEQ_POSITION_COUNT: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_MONITOR_BACKUP);
  Serial.print(F("\n LTC2937_MONITOR_BACKUP: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_MONITOR_STATUS);
  Serial.print(F("\n LTC2937_MONITOR_STATUS: "));
  Serial.println(return_val, HEX);

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_DEVICE_ID);
  Serial.print(F("\n LTC2937_DEVICE_ID: "));
  Serial.println(return_val, HEX);
  return SUCCEED;
}

//! write all registers to demo board defaults
//! @return int for failure mode
int ltc2937_write_all_regs_dc_default()
{
  if ( ltc2937_is_write_protected() != WRITE_PROTECTED)
  {
    if (ltc2937_is_down() == SUCCEED)
    {
      smbus->writeWord(ltc2937_i2c_address, LTC2937_WRITE_PROTECTION, 0x3AA);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_SPECIAL_LOT, 0x2313);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_ON_OFF_CONTROL, 0x0005);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_V_RANGE, 0x0554);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_V_THRESHOLD_1, 0x8970);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_V_THRESHOLD_2, 0xE6C1);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_V_THRESHOLD_3, 0xaF91);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_V_THRESHOLD_4, 0x997E);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_V_THRESHOLD_5, 0x7662);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_V_THRESHOLD_6, 0x5745);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_TON_TIMERS_1, 0xF000);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_TON_TIMERS_2, 0xF000);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_TON_TIMERS_3, 0xF000);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_TON_TIMERS_4, 0xF000);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_TON_TIMERS_5, 0xF000);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_TON_TIMERS_6, 0xF000);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_TOFF_TIMERS_1, 0x8000);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_TOFF_TIMERS_2, 0x8000);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_TOFF_TIMERS_3, 0x8000);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_TOFF_TIMERS_4, 0x8000);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_TOFF_TIMERS_5, 0x8000);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_TOFF_TIMERS_6, 0x8000);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_SEQ_UP_POSITION_1, 0x0001);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_SEQ_UP_POSITION_2, 0x0002);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_SEQ_UP_POSITION_3, 0x0003);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_SEQ_UP_POSITION_4, 0x0004);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_SEQ_UP_POSITION_5, 0x0005);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_SEQ_UP_POSITION_6, 0x0006);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_SEQ_DOWN_POSITION_1, 0x0006);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_SEQ_DOWN_POSITION_2, 0x0005);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_SEQ_DOWN_POSITION_3, 0x0004);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_SEQ_DOWN_POSITION_4, 0x0003);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_SEQ_DOWN_POSITION_5, 0x0002);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_SEQ_DOWN_POSITION_6, 0x0001);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_RSTB_CONFIG, 0xBFFF);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_FAULT_RESPONSE, 0x022F);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_BREAK_POINT, 0x0000);
      smbus->writeWord(ltc2937_i2c_address, LTC2937_DEVICE_ID, 0x2937);

      return SUCCEED;
    }
    else
    {
      return NOT_DOWN;
    }
  }
  else
  {
    Serial.print (F("FAIL: WRITE_PROTECTED.\n"));
    return WRITE_PROTECTED;
  }
}


//! write sequencing registers to a user-specified pattern
//! @return int for failure mode
int ltc2937_write_seq_regs()
{
  int i;
  uint16_t return_val;
  uint16_t user_command;
  uint8_t dummy_reg_address;


  if ( ltc2937_is_write_protected() != WRITE_PROTECTED)
  {
    if (ltc2937_is_down() == SUCCEED)
    {

      Serial.print(F("\nCHANGING SEQUENCE-UP ORDER\n"));
      dummy_reg_address = LTC2937_SEQ_UP_POSITION_1;
      for (i = 1; i <= 6; i++)
      {
        Serial.print(F("\nENTER SEQUENCE-UP POSITION FOR CHANNEL "));
        Serial.println(i, DEC);

        user_command = read_int();
        if (user_command > 1023)
        {
          user_command = 1023;
        }
        return_val = 0x0000 + user_command;
        smbus->writeWord(ltc2937_i2c_address, dummy_reg_address, return_val);
        dummy_reg_address++;
      }

      Serial.print(F("\nCHANGING SEQUENCE-DOWN ORDER\n"));
      dummy_reg_address = LTC2937_SEQ_DOWN_POSITION_1;
      for (i = 1; i <= 6; i++)
      {
        Serial.print(F("\nENTER SEQUENCE-DOWN POSITION FOR CHANNEL "));
        Serial.println(i, DEC);

        user_command = read_int();
        if (user_command > 1023)
        {
          user_command = 1023;
        }
        return_val = 0x0000 + user_command;
        smbus->writeWord(ltc2937_i2c_address, dummy_reg_address, return_val);
        dummy_reg_address++;
      }
      return SUCCEED;
    }
    else
    {
      return NOT_DOWN;
    }
  }
  else
  {
    Serial.print (F("FAIL: WRITE_PROTECTED.\n"));
    return WRITE_PROTECTED;
  }
}


//! Indicate if the part is write protected
//! return 0 if the part is write enabled (hardware and software), 1 if it is protected
int ltc2937_is_write_protected()
{
  uint16_t return_val;
  uint16_t wp_bits;

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_WRITE_PROTECTION);
  wp_bits = (return_val & 0x0003);

  if (wp_bits == 0x0000)
  {
    // neither hardware nor software lock bits set
    return SUCCEED;
  }
  else
  {
    Serial.print(F("\n LTC2937 IS WRITE-PROTECTED. WRITING WILL FAIL\n"));
    return WRITE_PROTECTED;
  }

}

//! remove software write-protection
//!  report if the part is hardware write-protected
//! @return int for failure mode
int ltc2937_write_enable()
{
  uint16_t return_val, write_val;
  uint16_t hardware_wp_bit;
  uint16_t software_wp_bit;

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_WRITE_PROTECTION);
  hardware_wp_bit = (return_val & 0x0002);
  software_wp_bit = (return_val & 0x0001);
  if (hardware_wp_bit == 0x0000)
  {
    // hardware is not write-protected, write the software unlock bit
    Serial.print(F("\n UNLOCKING LTC2937 SOFTWARE WRITE-PROTECT BIT."));
    //    Serial.println(return_val, HEX);
    write_val = (return_val & 0xFFFE);
    smbus->writeWord(ltc2937_i2c_address, LTC2937_WRITE_PROTECTION, write_val);
  }
  else
  {
    Serial.print(F("\n LTC2937 IS HARDWARE WRITE-PROTECTED. WRITING WILL FAIL\n"));
    return WRITE_PROTECTED;
  }
}

//! set the breakpoint to the given value
//! @return int for failure mode
int ltc2937_set_breakpoint(uint16_t set_val)
{
  uint16_t return_val;
  //  uint16_t bp_en_val;
  // read the existing breakpoint
  //  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_BREAK_POINT);
  //  bp_en_val = (return_val & 0x0400); // mask the bits of interest: b[10]
  //  return_val = (return_val & 0x03FF); // mask the bits of interest: b[9:0]
  if ( ltc2937_is_write_protected() != WRITE_PROTECTED)
  {
    if ((set_val >=0) && (set_val < 1024))
    {
      return_val = 0x0400 + set_val;  // set the enable bit and break_point value
      smbus->writeWord(ltc2937_i2c_address, LTC2937_BREAK_POINT, return_val);
    }
    else
    {
      Serial.print(F("\n ERROR! BREAKPOINT VALUE OUT OF RANGE.\n"));
    }
    return SUCCEED;
  }
  else
  {
    Serial.print (F("FAIL: WRITE_PROTECTED.\n"));
    return WRITE_PROTECTED;
  }
}

//! increment the breakpoint by 1
//! @return int for failure mode
int ltc2937_inc_breakpoint()
{
  uint16_t return_val;
  uint16_t bp_en_val;
  uint16_t bp_ct_val;

  //  read the existing breakpoint
  if ( ltc2937_is_write_protected() != WRITE_PROTECTED)
  {
    return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_BREAK_POINT);
    bp_en_val = (return_val & 0x0400); // mask the bits of interest: b[10]
    bp_ct_val = (return_val & 0x03FF); // mask the bits of interest: b[9:0]
    if (bp_ct_val < 1023)
    {
      return_val = (++bp_ct_val);
      return_val = (return_val | 0x0400); // ensure that the enable bit is set
      smbus->writeWord(ltc2937_i2c_address, LTC2937_BREAK_POINT, return_val);
      Serial.print(F("\n INCREMENTING BREAK_POINT TO VALUE : \n"));
      Serial.println(bp_ct_val);
      return SUCCEED;
    }
    else
    {
      Serial.print(F("\n ERROR! BREAKPOINT VALUE OUT OF RANGE.\n"));
      return FAIL;
    }
  }
  else
  {
    Serial.print (F("FAIL: WRITE_PROTECTED.\n"));
    return WRITE_PROTECTED;
  }
}


//! pretty-print the sequence_position_count register contents
//! @return int for success
int ltc2937_print_seq_pos_count()
{
  uint16_t return_val;
  uint16_t sp_bp_test;
  uint16_t sp_count;

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_SEQ_POSITION_COUNT);
  sp_bp_test = (return_val & 0x0400);
  sp_count   = (return_val & 0x3FF);

  Serial.println(return_val, HEX);
  Serial.print(F("\n SEQUENCE POSITION COUNT = "));
  Serial.println(sp_count, DEC);

  if (sp_bp_test == 0x0400)
  {
    return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_BREAK_POINT);
    if ((return_val & 0x0400) == 0x0400)
    {
      Serial.print(F("\n WAITING AT BREAKPOINT.\n"));
    }
  }
  else
  {
    Serial.print(F("\n NOT AT BREAKPOINT."));
  }
  return SUCCEED;
}


//! clear breakpoint register
//! @return int indicating failure mode
int ltc2937_clear_breakpoint()
{
  if ( ltc2937_is_write_protected() != WRITE_PROTECTED)
  {
    smbus->writeWord(ltc2937_i2c_address, LTC2937_BREAK_POINT, 0x0000);
    return SUCCEED;
  }
  else
  {
    return WRITE_PROTECTED;
  }
}

//! issue a clear command
//! @return int indicating failure mode
int ltc2937_clear()
{
  uint16_t return_val;
  uint8_t user_command;

  if (ltc2937_is_down() == NOT_DOWN)
  {
    Serial.print(F("\n IT IS A BAD IDEA TO CLEAR WHILE THE SUPPLIES ARE SEQUENCED-UP!\n"));
    Serial.print(F("     DO YOU WISH TO CLEAR? (y/n)"));
    user_command = read_char();         //! Reads the user command
    if ((user_command == 'y') || (user_command == 'Y'))
    {
      smbus->readWord(ltc2937_i2c_address, LTC2937_CLEAR);
      Serial.print(F("\n ****CLEARING**** \n"));
      return SUCCEED;
    }
    else
    {
      Serial.print(F("\n ****NOT CLEARING**** \n"));
      return NOT_DOWN;
    }
  }
  else
  {
    smbus->readWord(ltc2937_i2c_address, LTC2937_CLEAR);
    Serial.print(F("\n ****CLEARING**** \n"));
    return SUCCEED;
  }
}

//! handle the backup word fault log (affects NVM and RAM)
//! @return int indicating failure mode
int ltc2937_clear_fault_backup()
{
  uint8_t user_command;

  // RECIPE:
  //  check for sequenced-down and writable
  //  warn the user and ask for permission to overwrite config registers
  //  write-enable
  //  restore to retrieve the backup word
  //  pretty-print fault information
  //  clear to clear all faults
  //  store to clear the NVM backup word
  //  restore (again) to clear the RAM backup word flag

  if ( ltc2937_is_write_protected() != WRITE_PROTECTED)
  {
    if (ltc2937_is_down() == SUCCEED)
    {
      Serial.print(F("\n THIS OPERATION WILL AFFECT BOTH RAM AND EEPROM!\n"));
      Serial.print(F("     DO YOU WISH TO CLEAR RAM CONFIGURATION SETTINGS? (y/n)"));
      user_command = read_char();         //! Reads the user command
      if ((user_command == 'y') || (user_command == 'Y'))
      {
        if (ltc2937_restore() == SUCCEED)
        {
          ltc2937_pretty_print_monitor_backup();
          ltc2937_clear();
          ltc2937_store();
          delay(100);
          ltc2937_restore();
          Serial.print (F("\nTHE BACKUP WORD IS NOW CLEAR.\n"));
        }
        else
        {
          // fail due to unsuccessful restore
          return FAIL;
        }
      }
    }
    else
    {
      return NOT_DOWN;
    }
  }
  else
  {
    Serial.print (F("FAIL: CANNOT MANIPULATE FAULT BACKUP WHILE WRITE-PROTECTED.\n"));
    return WRITE_PROTECTED;
  }


}


//! clear ALERTB pin
//! @return int for success
int ltc2937_clear_alertb()
{
  smbus->readWord(ltc2937_i2c_address, LTC2937_CLEAR_ALERTB);
  return SUCCEED;
}

//! pretty-print MONITOR_BACKUP
//! @return int for success
int ltc2937_pretty_print_monitor_backup()
{
  uint16_t return_val,
           masked_val;

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_MONITOR_BACKUP);

  Serial.print(F("\n LTC2937 MONITOR_BACKUP CONTENTS: "));
  Serial.println(return_val, HEX);

  masked_val = (return_val & 0xE000);
  switch (masked_val)
  {
    case 0xE000 :
      Serial.print(F("\n**RESERVED SEQUENCE STATE (THIS IS BAD)"));
      break;
    case 0xC000 :
      Serial.print(F("\n**SEQUENCE FAULT ON CHANNEL 6"));
      break;
    case 0xA000 :
      Serial.print(F("\n**SEQUENCE FAULT ON CHANNEL 5"));
      break;
    case 0x8000 :
      Serial.print(F("\n**SEQUENCE FAULT ON CHANNEL 4"));
      break;
    case 0x6000 :
      Serial.print(F("\n**SEQUENCE FAULT ON CHANNEL 3"));
      break;
    case 0x4000 :
      Serial.print(F("\n**SEQUENCE FAULT ON CHANNEL 2"));
      break;
    case 0x2000 :
      Serial.print(F("\n**SEQUENCE FAULT ON CHANNEL 1"));
      break;
    case 0x0000 :
      Serial.print(F("\n  NO SEQUENCE FAULTS"));
      break;
    default :
      Serial.print(F("\n**UNDEFINED SEQUENCE FAULT STATE (THIS IS BAD)"));
      break;
  }
  masked_val = (return_val & 0x1000);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO RESET FAULT"));
  }
  masked_val = (return_val & 0x0800);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V6 OV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO V6 OV FAULT"));
  }
  masked_val = (return_val & 0x0400);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V6 UV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO V6 UV FAULT"));
  }
  masked_val = (return_val & 0x0200);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V5 OV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO V5 OV FAULT"));
  }
  masked_val = (return_val & 0x0100);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V5 UV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO V5 UV FAULT"));
  }
  masked_val = (return_val & 0x0080);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V4 OV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO V4 OV FAULT"));
  }
  masked_val = (return_val & 0x0040);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V4 UV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO V4 UV FAULT"));
  }
  masked_val = (return_val & 0x0020);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V3 OV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO V3 OV FAULT"));
  }
  masked_val = (return_val & 0x0010);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V3 UV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO V3 UV FAULT"));
  }
  masked_val = (return_val & 0x0008);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V2 OV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO V2 OV FAULT"));
  }
  masked_val = (return_val & 0x0004);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V2 UV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO V2 UV FAULT"));
  }
  masked_val = (return_val & 0x0002);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V1 OV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO V1 OV FAULT"));
  }
  masked_val = (return_val & 0x0001);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V1 UV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO V1 UV FAULT"));
  }
  return SUCCEED;
}

//! pretty-print MONITOR_STATUS_HISTORY
//! @return int for success
int ltc2937_pretty_print_monitor_status_history()
{
  uint16_t return_val,
           masked_val;

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_MONITOR_STATUS_HISTORY);

  Serial.print(F("\n LTC2937 MONITOR_STATUS_HISTORY CONTENTS: "));
  Serial.println(return_val, HEX);

  masked_val = (return_val & 0xE000);
  switch (masked_val)
  {
    case 0xE000 :
      Serial.print(F("\n RESERVED SEQUENCE STATE (THIS IS BAD)"));
      break;
    case 0xC000 :
      Serial.print(F("\n SEQUENCE FAULT ON CHANNEL 6"));
      break;
    case 0xA000 :
      Serial.print(F("\n SEQUENCE FAULT ON CHANNEL 5"));
      break;
    case 0x8000 :
      Serial.print(F("\n SEQUENCE FAULT ON CHANNEL 4"));
      break;
    case 0x6000 :
      Serial.print(F("\n SEQUENCE FAULT ON CHANNEL 3"));
      break;
    case 0x4000 :
      Serial.print(F("\n SEQUENCE FAULT ON CHANNEL 2"));
      break;
    case 0x2000 :
      Serial.print(F("\n SEQUENCE FAULT ON CHANNEL 1"));
      break;
    case 0x0000 :
      Serial.print(F("\n NO SEQUENCE FAULTS"));
      break;
    default :
      Serial.print(F("\n UNDEFINED SEQUENCE FAULT STATE (THIS IS BAD)"));
      break;
  }
  masked_val = (return_val & 0x1000);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n NO RESET FAULT"));
  }
  masked_val = (return_val & 0x0800);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n V6 OV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n NO V6 OV FAULT"));
  }
  masked_val = (return_val & 0x0400);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n V6 UV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n NO V6 UV FAULT"));
  }
  masked_val = (return_val & 0x0200);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n V5 OV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n NO V5 OV FAULT"));
  }
  masked_val = (return_val & 0x0100);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n V5 UV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n NO V5 UV FAULT"));
  }
  masked_val = (return_val & 0x0080);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n V4 OV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n NO V4 OV FAULT"));
  }
  masked_val = (return_val & 0x0040);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n V4 UV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n NO V4 UV FAULT"));
  }
  masked_val = (return_val & 0x0020);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n V3 OV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n NO V3 OV FAULT"));
  }
  masked_val = (return_val & 0x0010);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n V3 UV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n NO V3 UV FAULT"));
  }
  masked_val = (return_val & 0x0008);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n V2 OV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n NO V2 OV FAULT"));
  }
  masked_val = (return_val & 0x0004);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n V2 UV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n NO V2 UV FAULT"));
  }
  masked_val = (return_val & 0x0002);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n V1 OV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n NO V1 OV FAULT"));
  }
  masked_val = (return_val & 0x0001);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n V1 UV RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n NO V1 UV FAULT"));
  }
  return SUCCEED;
}

//! pretty-print MONITOR_STATUS
//! @return int for success
int ltc2937_pretty_print_monitor_status()
{
  uint16_t return_val,
           masked_val;

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_MONITOR_STATUS);

  Serial.print(F("\n LTC2937 MONITOR_STATUS CONTENTS: "));
  Serial.println(return_val, HEX);

  masked_val = (return_val & 0x2000);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n MARGIN IS ACTIVE"));
  }
  else
  {
    Serial.print(F("\n MARGIN IS NOT ACTIVE"));
  }
  masked_val = (return_val & 0x1000);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n RSTB PIN IS LOW"));
  }
  else
  {
    Serial.print(F("\n RSTB PIN IS HIGH"));
  }
  masked_val = (return_val & 0x0800);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V6 OV COMPARATOR ASSERTED"));
  }
  else
  {
    Serial.print(F("\n  V6 OV COMPARATOR NOT ASSERTED"));
  }
  masked_val = (return_val & 0x0400);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V6 UV COMPARATOR ASSERTED"));
  }
  else
  {
    Serial.print(F("\n  V6 UV COMPARATOR NOT ASSERTED"));
  }
  masked_val = (return_val & 0x0200);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V5 OV COMPARATOR ASSERTED"));
  }
  else
  {
    Serial.print(F("\n  V5 OV COMPARATOR NOT ASSERTED"));
  }
  masked_val = (return_val & 0x0100);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V5 UV COMPARATOR ASSERTED"));
  }
  else
  {
    Serial.print(F("\n  V5 UV COMPARATOR NOT ASSERTED"));
  }
  masked_val = (return_val & 0x0080);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V4 OV COMPARATOR ASSERTED"));
  }
  else
  {
    Serial.print(F("\n  V4 OV COMPARATOR NOT ASSERTED"));
  }
  masked_val = (return_val & 0x0040);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V4 UV COMPARATOR ASSERTED"));
  }
  else
  {
    Serial.print(F("\n  V4 UV COMPARATOR NOT ASSERTED"));
  }
  masked_val = (return_val & 0x0020);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V3 OV COMPARATOR ASSERTED"));
  }
  else
  {
    Serial.print(F("\n  V3 OV COMPARATOR NOT ASSERTED"));
  }
  masked_val = (return_val & 0x0010);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V3 UV COMPARATOR ASSERTED"));
  }
  else
  {
    Serial.print(F("\n  V3 UV COMPARATOR NOT ASSERTED"));
  }
  masked_val = (return_val & 0x0008);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V2 OV COMPARATOR ASSERTED"));
  }
  else
  {
    Serial.print(F("\n  V2 OV COMPARATOR NOT ASSERTED"));
  }
  masked_val = (return_val & 0x0004);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V2 UV COMPARATOR ASSERTED"));
  }
  else
  {
    Serial.print(F("\n  V2 UV COMPARATOR NOT ASSERTED"));
  }
  masked_val = (return_val & 0x0002);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V1 OV COMPARATOR ASSERTED"));
  }
  else
  {
    Serial.print(F("\n  V1 OV COMPARATOR NOT ASSERTED"));
  }
  masked_val = (return_val & 0x0001);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**V1 UV COMPARATOR ASSERTED"));
  }
  else
  {
    Serial.print(F("\n  V1 UV COMPARATOR NOT ASSERTED"));
  }
  return SUCCEED;
}


//! pretty-print STATUS_INFORMATION
//! @return int for success
int ltc2937_pretty_print_status_information()
{
  uint16_t return_val,
           masked_val;

  return_val = smbus->readWord(ltc2937_i2c_address, LTC2937_STATUS_INFORMATION);

  Serial.print(F("\n LTC2937 STATUS_INFORMATION CONTENTS: "));
  Serial.println(return_val, HEX);

  masked_val = (return_val & 0xE000);
  switch (masked_val)
  {
    case 0xE000 :
      Serial.print(F("\n***RESERVED SEQUENCE STATE (THIS IS BAD)"));
      break;
    case 0xC000 :
      Serial.print(F("\n**SEQUENCE FAULT ON CHANNEL 6"));
      break;
    case 0xA000 :
      Serial.print(F("\n**SEQUENCE FAULT ON CHANNEL 5"));
      break;
    case 0x8000 :
      Serial.print(F("\n**SEQUENCE FAULT ON CHANNEL 4"));
      break;
    case 0x6000 :
      Serial.print(F("\n**SEQUENCE FAULT ON CHANNEL 3"));
      break;
    case 0x4000 :
      Serial.print(F("\n**SEQUENCE FAULT ON CHANNEL 2"));
      break;
    case 0x2000 :
      Serial.print(F("\n**SEQUENCE FAULT ON CHANNEL 1"));
      break;
    case 0x0000 :
      Serial.print(F("\n  NO SEQUENCE FAULTS"));
      break;
    default :
      Serial.print(F("\n**UNDEFINED SEQUENCE FAULT STATE (THIS IS BAD)"));
      break;
  }
  masked_val = (return_val & 0x1000);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**MONITOR BACKUP IS STORED IN EEPROM"));
  }
  else
  {
    Serial.print(F("\n  NO MONITOR BACKUP IN EEPROM"));
  }

  masked_val = (return_val & 0x0C00);
  switch (masked_val)
  {
    case 0x0C00 :
      Serial.print(F("\n  PART SEQUENCE-UP COMPLETE"));
      break;
    case 0x0800 :
      Serial.print(F("\n  PART SEQUENCE-DOWN IN PROGRESS"));
      break;
    case 0x0400 :
      Serial.print(F("\n  PART SEQUENCE-UP IN PROGRESS"));
      break;
    case 0x0000 :
      Serial.print(F("\n  PART SEQUENCE-DOWN COMPLETE"));
      break;
    default :
      Serial.print(F("\n**UNDEFINED PART SEQUENCING STATE (THIS IS BAD)"));
      break;
  }

  masked_val = (return_val & 0x0300);
  switch (masked_val)
  {
    case 0x0300 :
      Serial.print(F("\n  GROUP SEQUENCE-UP COMPLETE"));
      break;
    case 0x0200 :
      Serial.print(F("\n  GROUP SEQUENCE-DOWN IN PROGRESS"));
      break;
    case 0x0100 :
      Serial.print(F("\n  GROUP SEQUENCE-UP IN PROGRESS"));
      break;
    case 0x0000 :
      Serial.print(F("\n  GROUP SEQUENCE-DOWN COMPLETE"));
      break;
    default :
      Serial.print(F("\n**UNDEFINED GROUP SEQUENCING STATE (THIS IS BAD)"));
      break;
  }

  masked_val = (return_val & 0x0080);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**SEQUENCE-UP FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO SEQUENCE-UP FAULT"));
  }
  masked_val = (return_val & 0x0040);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**SEQUENCE-DOWN FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO SEQUENCE-DOWN FAULT"));
  }
  masked_val = (return_val & 0x0200);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**OV FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO OV FAULT"));
  }
  masked_val = (return_val & 0x0010);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**UV FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO UV FAULT"));
  }
  masked_val = (return_val & 0x0008);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**RESET FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO RESET FAULT"));
  }
  masked_val = (return_val & 0x0004);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n  ALL SEQUENCED SUPPLIES ARE BELOW THEIR DISCHARGE THRESHOLDS"));
  }
  else
  {
    Serial.print(F("\n**NOT ALL SEQUENCED SUPPLIES ARE DOWN"));
  }
  masked_val = (return_val & 0x0002);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**CONTROL FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO CONTROL FAULT"));
  }
  masked_val = (return_val & 0x0001);
  if (masked_val != 0x0000)
  {
    Serial.print(F("\n**OTHER FAULT DETECTED"));
  }
  else
  {
    Serial.print(F("\n  NO OTHER FAULT"));
  }
  return SUCCEED;
}
