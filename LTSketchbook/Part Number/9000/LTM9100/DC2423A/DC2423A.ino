/*!
Linear Technology DC2423A Demonstration Board.
LTM9100: Anyside™ High Voltage Isolated Switch Controller with I²C Command and Telemetry

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTM9100

http://www.linear.com/product/LTC9100#demoboards


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
    @ingroup LTM9100
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTM9100.h"

float sense_resistor = 0.008;     //default resistance in ohms
float adin_gain = 0.035;          //default
float adin2_gain = 0.035;         //default
uint8_t i2c_address = 0x10;       //default

//! Initialize Linduino
void setup()
{
  //Port initialization
  DDRD = DDRD | B00111100;      // Sets data direction of Digital Pins 0-7 (Logical OR to not interfere with Rx and Tx)
  PORTD = B00000000;            // Initializes Digital Pins 0-7 to LOW
  DDRB = B00011111;             // Sets data direction of Digital Pins 8-13
  PORTB = B00000001;            // Initializes Digital Pin 8 HIGH and 9-13 to LOW, Pin 8 is I2C Linduino select bit
  i2c_enable();                 // Initializes Linduino I2C port.
  Serial.begin(115200);         // Initialize the serial port to the PC
  print_title();
  print_prompt();
}

//! Repeats Linduino loop
void loop()
{
  int8_t ack=0;
  uint8_t user_command;
  uint8_t data;
  uint8_t user_register;

  if (Serial.available())
  {
    user_command = read_int();                              //! Reads the user command
    Serial.println(user_command);                         // Print user command

    //! Reads or writes to a LTM9100 and prints result.
    switch (user_command)
    {
      case 1:
        // Read every register and print it's data.
        ack |= LTM9100_print_all_registers(i2c_address);
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
        ack |= LTM9100_register_read(i2c_address, user_register, &data);
        if (!ack)
        {
          Serial.print("Register data: 0x");
          Serial.println(data, HEX);
        }
        break;
      case 3:
        // Write a byte to a chosen register.
        Serial.print(F("\nAddress (in hex with '0x' prefix) of register to write: "));
        user_register = read_int();
        Serial.print("0x");
        Serial.println(user_register, HEX);
        if (!valid_register(user_register, reg_write_list, sizeof(reg_write_list)))
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.print(F("Data (in hex with '0x' prefix) to write: "));
        data = read_int();
        Serial.println(data, HEX);
        ack |= LTM9100_register_write(i2c_address, user_register, data);
        if (!ack)
        {
          Serial.print("0x");
          Serial.print(data, HEX);
          Serial.print(" written to register 0x");
          Serial.println(user_register, HEX);
        }
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
        ack |= LTM9100_bit_set(i2c_address, user_register, data);
        if (!ack)
        {
          Serial.print("Bit set. Register data is now 0x");
          ack |= LTM9100_register_read(i2c_address, user_register, &data);
          Serial.println(data, HEX);
        }
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
        ack |= LTM9100_bit_clear(i2c_address, user_register, data);
        if (!ack)
        {
          Serial.print("Bit cleared. Register data is now 0x");
          ack |= LTM9100_register_read(i2c_address, user_register, &data);
          Serial.println(data, HEX);
        }
        break;
      case 6:
        // Write a byte to a chosen register via the Mass Write I2C address.
        Serial.print(F("\nAddress (in hex with '0x' prefix) of register to write: "));
        user_register = read_int();
        Serial.print("0x");
        Serial.println(user_register, HEX);
        if (!valid_register(user_register, reg_write_list, sizeof(reg_write_list)))
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.print(F("Data (in hex with '0x' prefix) to write: "));
        data = read_int();
        Serial.println(data, HEX);
        ack |= LTM9100_register_write(0x1F, user_register, data);
        if (!ack)
        {
          Serial.print("0x");
          Serial.print(data, HEX);
          Serial.print(" written to register 0x");
          Serial.println(user_register, HEX);
        }
        break;
      case 7:
        ack |= menu_read_status();                      // Print status menu
        break;
      case 8:
        ack |= menu_read_write_faults_alerts(false);    // Print Faults menu
        break;
      case 9:
        ack |= menu_read_write_faults_alerts(true);     // Print Alert setting menu
        break;
      case 10:
        ack |= menu_read_write_controls();              // Print Controls menu
        break;
      case 11:
        ack |= menu_read_write_adc_params();            // Print ADC menu
        break;
      case 12:
        ack |= LTM9100_continuous_read_all_registers(i2c_address);
        break;
    }
    if (ack != 0)
    {
      Serial.print(F("Error: No Acknowledge. \n"));
    }
    print_prompt();
  }
}

// Function Definitions

//! Prints a warning if the demo board is not detected.
void print_warning_prompt()
{
  Serial.println(F("\nWarning: Demo board not detected. Linduino will attempt to proceed."));
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* LTM9100 Demonstration Program                                 *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates sending and receiving data from     *\n"));
  Serial.print(F("* the LTM9100 via I2C.                                          *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

// Prints main menu.
void print_prompt()
{
  //! Displays the Read/Write Registers menu
  Serial.print(F("\nRead/Write Registers\n\n"));
  Serial.print(F("  1-Read All Registers\n"));
  Serial.print(F("  2-Read Single Register\n"));
  Serial.print(F("  3-Write Single Register\n"));
  Serial.print(F("  4-Set Bit\n"));
  Serial.print(F("  5-Clear Bit\n"));
  Serial.print(F("  6-Write Register to Mass Write Address\n"));
  Serial.println(F("  7-Read Status"));
  Serial.println(F("  8-Read/Write Faults"));
  Serial.println(F("  9-Read/Write Alerts"));
  Serial.println(F("  10-Read/Write Controls"));
  Serial.println(F("  11-Read/Write ADC parameters"));
  Serial.println(F("  12-Continuously Read Registers"));
  Serial.print(F("\nEnter a command: "));
}

int8_t menu_read_status()
{
  int8_t ack=0;
  uint8_t user_command;
  uint8_t data;
  uint8_t user_register = LTM_9100_STATUS_A_REG;
  uint8_t user_bit;
  do
  {
    //! Displays the Read Status menu
    Serial.print(F("\nRead Status\n\n"));
    Serial.print(F("  1-Gate Status\n"));
    Serial.print(F("  2-PG Input Pin Status\n"));
    Serial.print(F("  3-FET Status\n"));
    Serial.print(F("  4-Overcurrent Condition\n"));
    Serial.print(F("  5-Undervoltage Condition\n"));
    Serial.print(F("  6-Overvoltage Condition\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
      Serial.print(F("m\n"));
    else
      Serial.println(user_command);                         // Print user command

    //! Reads LTM9100 and prints result.
    switch (user_command)
    {
      case 1:
        ack |= LTM9100_bit_read(i2c_address, user_register, LTM_9100_STATUS_GATE, &data);
        Serial.print(F("Gate State: "));
        (data)?Serial.println(F("On")):Serial.println(F("Off"));
        break;
      case 2:
        ack |= LTM9100_bit_read(i2c_address, user_register, LTM_9100_STATUS_PGI, &data);
        Serial.print(F("PG Input State: "));
        (data)?Serial.println(F("High")):Serial.println(F("Low"));
        break;
      case 3:
        ack |= LTM9100_bit_read(i2c_address, user_register, LTM_9100_STATUS_FET, &data);
        Serial.print(F("FET State: "));
        (data)?Serial.println(F("Short")):Serial.println(F("No Short"));
        break;
      case 4:
        ack |= LTM9100_bit_read(i2c_address, user_register, LTM_9100_STATUS_OC, &data);
        Serial.print(F("Overcurrent: "));
        (data)?Serial.println(F("Yes")):Serial.println(F("No"));
        break;
      case 5:
        ack |= LTM9100_bit_read(i2c_address, user_register, LTM_9100_STATUS_UV, &data);
        Serial.print(F("Undervoltage: "));
        (data)?Serial.println(F("Yes")):Serial.println(F("No"));
        break;
      case 6:
        ack |= LTM9100_bit_read(i2c_address, user_register, LTM_9100_STATUS_OV, &data);
        Serial.print(F("Overvoltage: "));
        (data)?Serial.println(F("Yes")):Serial.println(F("No"));
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

static int8_t ask_clear_set_bit(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number)
{
  int8_t ack = 0;
  uint8_t user_bit;
  Serial.println(F("0=Clear/Disable, 1=Set/Enable:"));
  user_bit = read_int();
  if (user_bit > 1)
    Serial.println(F("  Invalid input."));
  else
    ack |= (user_bit)?LTM9100_bit_set(i2c_address, register_address, bit_number):LTM9100_bit_clear(i2c_address, register_address, bit_number);

  return ack;
}

int8_t menu_read_write_faults_alerts(bool alerts)
{
  int8_t ack=0;
  uint8_t user_command;
  uint8_t data;
  uint8_t user_register = (alerts)?LTM_9100_ALERT_C_REG:LTM_9100_FAULT_B_REG;

  do
  {
    if (!alerts) //! Displays the Read/Write Faults menu
    {
      Serial.print(F("\nRead/Write Faults\n\n"));
      Serial.print(F("  1-Read PGIO_HIGH\n"));
      Serial.print(F("  2-Read FET_FAULT\n"));
      Serial.print(F("  3-Read OC_FAULT\n"));
      Serial.print(F("  4-Read UV_FAULT\n"));
      Serial.print(F("  5-Read OV_FAULT\n"));
      Serial.print(F("  6-Clear/Set PGIO_HIGH\n"));
      Serial.print(F("  7-Clear/Set FET_FAULT\n"));
      Serial.print(F("  8-Clear/Set OC_FAULT\n"));
      Serial.print(F("  9-Clear/Set UV_FAULT\n"));
      Serial.print(F("  10-Clear/Set OV_FAULT\n"));
    }
    else //! Displays the Read/Write Alerts menu
    {
      Serial.print(F("\nRead/Write Alerts\n\n"));
      Serial.print(F("  1-Read PGIO_OUT\n"));
      Serial.print(F("  2-Read FET_ALERT\n"));
      Serial.print(F("  3-Read OC_ALERT\n"));
      Serial.print(F("  4-Read UV_ALERT\n"));
      Serial.print(F("  5-Read OV_ALERT\n"));
      Serial.print(F("  6-Disable/Enable PGIO_OUT\n"));
      Serial.print(F("  7-Disable/Enable FET_ALERT\n"));
      Serial.print(F("  8-Disable/Enable OC_ALERT\n"));
      Serial.print(F("  9-Disable/Enable UV_ALERT\n"));
      Serial.print(F("  10-Disable/Enable OV_ALERT\n"));
    }
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
      Serial.print(F("m\n"));
    else
      Serial.println(user_command);                         // Print user command

    //! Reads or writes to a LTM9100 and prints result.
    switch (user_command)
    {
      case 1:
        ack |= LTM9100_bit_read(i2c_address, user_register, LTM_9100_FAULT_PGI, &data);
        Serial.print(F("PGIO: "));
        (data)?Serial.println(F("Yes")):Serial.println(F("No"));
        break;
      case 2:
        ack |= LTM9100_bit_read(i2c_address, user_register, LTM_9100_FAULT_FET, &data);
        Serial.print(F("FET Short Detected: "));
        (data)?Serial.println(F("Fault")):Serial.println(F("No Fault"));
        break;
      case 3:
        ack |= LTM9100_bit_read(i2c_address, user_register, LTM_9100_FAULT_OC, &data);
        Serial.print(F("Overcurrent: "));
        (data)?Serial.println(F("Fault")):Serial.println(F("No Fault"));
        break;
      case 4:
        ack |= LTM9100_bit_read(i2c_address, user_register, LTM_9100_FAULT_UV, &data);
        Serial.print(F("Undervoltage: "));
        (data)?Serial.println(F("Fault")):Serial.println(F("No Fault"));
        break;
      case 5:
        ack |= LTM9100_bit_read(i2c_address, user_register, LTM_9100_FAULT_OV, &data);
        Serial.print(F("Overvoltage: "));
        (data)?Serial.println(F("Fault")):Serial.println(F("No Fault"));
        break;
      case 6:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9100_FAULT_PGI);
        break;
      case 7:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9100_FAULT_FET);
        break;
      case 8:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9100_FAULT_OC);
        break;
      case 9:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9100_FAULT_UV);
        break;
      case 10:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9100_FAULT_OV);
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

int8_t menu_read_write_controls()
{
  int8_t ack=0;
  uint8_t user_command;
  uint8_t data;
  uint8_t user_register = LTM_9100_CTRL_D_REG;
  uint8_t user_bit;

  do
  {
    //! Displays the Read/Write Controls menu
    Serial.print(F("\nRead/Write Controls\n\n"));
    Serial.print(F("  1-Read PGIO_CONFIG\n"));
    Serial.print(F("  2-Read ADC_WRITE\n"));
    Serial.print(F("  3-Read GATE_CTRL\n"));
    Serial.print(F("  4-Read OC_AUTO\n"));
    Serial.print(F("  5-Read UV_AUTO\n"));
    Serial.print(F("  6-Read OV_AUTO\n"));
    Serial.print(F("  7-Disable/Enable PGIO_CONFIG\n"));
    Serial.print(F("  8-Disable/Enable ADC_WRITE\n"));
    Serial.print(F("  9-Disable/Enable GATE_CTRL\n"));
    Serial.print(F("  10-Disable/Enable OC_AUTO\n"));
    Serial.print(F("  11-Disable/Enable UV_AUTO\n"));
    Serial.print(F("  12-Disable/Enable OV_AUTO\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
      Serial.print(F("m\n"));
    else
      Serial.println(user_command);                         // Print user command

    //! Reads or writes to a LTM9100 and prints result.
    switch (user_command)
    {
      case 1:
        ack |= LTM9100_register_read(i2c_address, user_register, &data);
        data &= LTM_9100_CTRL_PGIO_CFG_MASK;
        Serial.print(F("PGIO Config: "));
        if (~data & 0x40)
          Serial.println(F("Power Good, Open Drain"));
        else if (data & 0x80)
          Serial.println(F("General Purpose Input"));
        else
          Serial.println(F("General Purpose Output"));
        break;
      case 2:
        ack |= LTM9100_bit_read(i2c_address, user_register, LTM_9100_CTRL_ADC_WRITE, &data);
        Serial.print(F("ADC write: "));
        (data)?Serial.println(F("Enabled")):Serial.println(F("Disabled"));
      case 3:
        ack |= LTM9100_bit_read(i2c_address, user_register, LTM_9100_CTRL_GATE_CTRL, &data);
        Serial.print(F("Gate State: "));
        (data)?Serial.println(F("Enabled")):Serial.println(F("Disabled"));
        break;
        break;
      case 4:
        ack |= LTM9100_bit_read(i2c_address, user_register, LTM_9100_CTRL_OC, &data);
        Serial.print(F("Overcurrent: "));
        (data)?Serial.println(F("Enabled")):Serial.println(F("Disabled"));
        break;
      case 5:
        ack |= LTM9100_bit_read(i2c_address, user_register, LTM_9100_CTRL_UV, &data);
        Serial.print(F("Undervoltage: "));
        (data)?Serial.println(F("Enabled")):Serial.println(F("Disabled"));
        break;
      case 6:
        ack |= LTM9100_bit_read(i2c_address, user_register, LTM_9100_CTRL_OV, &data);
        Serial.print(F("Overvoltage: "));
        (data)?Serial.println(F("Enabled")):Serial.println(F("Disabled"));
        break;
      case 7:
        Serial.println(F("0,2=Power Good, 1=General Purpose Output, 3=General Purpose Input:"));
        user_bit = read_int();
        if (user_bit > 3)
          Serial.println(F("  Invalid input."));
        else
        {
          ack |= LTM9100_register_read(i2c_address, user_register, &data);
          data &= ~LTM_9100_CTRL_PGIO_CFG_MASK;
          data |= user_bit<<LTM_9100_CTRL_PGIO_CFG;
          LTM9100_register_write(i2c_address, user_register, data);
        }
        break;
      case 8:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9100_CTRL_ADC_WRITE );
        break;
      case 9:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9100_CTRL_GATE_CTRL);
        break;
      case 10:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9100_CTRL_OC);
        break;
      case 11:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9100_CTRL_UV);
        break;
      case 12:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9100_CTRL_OV);
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

static void set_adc_scaling_factor(float *data)
{
  Serial.print(F("Current value: "));
  Serial.println(*data, 4);
  Serial.print(F("Enter new value: "));
  *data = read_float();
  Serial.println("");
}

int8_t menu_read_write_adc_params()
{
  int8_t ack=0;
  uint8_t user_command;
  float data;

  do
  {
    //! Displays the Read/Write ADC menu
    Serial.print(F("\nRead/Write ADCs \n\n"));
    Serial.print(F("  1-Read ADC Current Sense Voltage\n"));
    Serial.print(F("  2-Read ADIN Voltage\n"));
    Serial.print(F("  3-Read ADIN2 Voltage or Temperature\n"));
    Serial.print(F("  4-Configure Current Sense Resistor(ohms)\n"));
    Serial.print(F("  5-Configure ADIN Gain(defaults: 0.035[A], 0.006[B])\n"));
    Serial.print(F("  6-Configure ADIN2 Gain(defaults: 0.035[A], 0.006[B], or 0.004V/Kelvin[A/B])\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
      Serial.print(F("m\n"));
    else
    {
      Serial.println(user_command);                         // Print user command
    }

    //! Reads or writes to a LTM9100 and prints result.
    switch (user_command)
    {
      case 1:
        ack |= LTM9100_adc_read(i2c_address, LTM_9100_SENSE_E_REG, &data);
        break;
      case 2:
        ack |= LTM9100_adc_read(i2c_address, LTM_9100_ADIN_I_REG, &data);
        break;
      case 3:
        ack |= LTM9100_adc_read(i2c_address, LTM_9100_ADIN2_G_REG, &data);
        break;
      case 4:
        set_adc_scaling_factor(&sense_resistor);
        break;
      case 5:
        set_adc_scaling_factor(&adin_gain);
        break;
      case 6:
        set_adc_scaling_factor(&adin2_gain);
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

