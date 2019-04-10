/*!
Linear Technology DC934 Demonstration Board.
LTC2607: 16-Bit, Dual Rail-to-Rail DACs with I2C Interface

Linear Technology DC936 Demonstration Board.
LTC2609: Quad 16-/14-/12-Bit Rail-to-Rail DACs with I²C Interface.

Linear Technology DC812 Demonstration Board.
LTC2606: 16-Bit Rail-to-Rail DACs with I²C Interface.

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.
   Calibration requires a precision voltmeter.
   No external power supply is required.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2607
http://www.linear.com/product/LTC2609
http://www.linear.com/product/LTC2606

http://www.linear.com/product/LTC2607#demoboards
http://www.linear.com/product/LTC2609#demoboards
http://www.linear.com/product/LTC2606#demoboards


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
    @ingroup LTC2607
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
//#include "LT_I2C.h"
#include "LT_SPI.h"
#include "LTC2607.h"
#include "LTC2422.h"
//#include "QuikEval_EEPROM.h"
#include <Wire.h>
#include <SPI.h>
#include "USE_WIRE.h"

#ifdef USEWIRE
#include "LT_I2C_Wire.h"
#include "QuikEval_EEPROM_Wire.h"
#else
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#endif

// Function Declaration
void print_title();
void print_prompt(int16_t selected_dac);
int16_t prompt_voltage_or_code();
uint16_t get_voltage(float LTC2607_lsb, int32_t LTC2607_offset);
uint16_t get_code();

uint8_t menu_1_select_dac(int16_t *selected_dac);
uint8_t menu_2_write_to_input_register(int16_t selected_dac);
uint8_t menu_3_write_and_update_dac(int16_t selected_dac);
uint8_t menu_4_update_power_up_dac(int16_t selected_dac);
uint8_t menu_5_power_down_dac(int16_t selected_dac);
uint8_t menu_6_read_adc();
uint8_t menu_7_sweep();
uint8_t menu_8_calibrate_all();

//! Used to keep track to print voltage or print code
enum prompt
{
  PROMPT_VOLTAGE = 0, /**< 0 */
  PROMPT_CODE = 1     /**< 1 */
};

// Global Constants
const uint16_t LTC2422_TIMEOUT= 1000;   //!< Configures the maximum timeout (ms) allowed for an LTC2607 read.

//! Look-up table for DAC_A, DAC_B, or both command byte option
const uint8_t address_map[3] = {LTC2607_DAC_A, LTC2607_DAC_B, LTC2607_ALL_DACS};  //!< Builds the command for dac address

const uint16_t MISO_TIMEOUT = 1000;     //!< The MISO timout in ms

// ***Global Variables***

//! The LTC2607 least significant bit value with 5V full-scale.
//! Initialized to typical value from datasheet.  Can be calibrated through option menu.
static float LTC2607_lsb[3] = {LTC2607_TYPICAL_lsb, LTC2607_TYPICAL_lsb, LTC2607_TYPICAL_lsb};  //!< Builds the lsb. Index 2 is used for All DACs

//! The LTC2422 least significant bit value with 5V full-scale.
//! Initialized to typical value from datasheet.  Can be calibrated through option menu.
static float LTC2422_lsb = LTC2422_TYPICAL_lsb;

//! The LTC2422 offset variable.
//! Initialized to typical value from datasheet.  Can be calibrated through option menu.
static int32_t LTC2607_offset[3] = {LTC2607_TYPICAL_OFFSET, LTC2607_TYPICAL_OFFSET, LTC2607_TYPICAL_OFFSET}; //!< Builds the offset. Index 2 is used for All DACs

static int8_t demo_board_connected; //!< Set to 1 if the board is connected

//! Initialize Linduino
void setup()
// Setup the Program
{
  char demo_name[] = "DC934";             // Demo Board Name stored in QuikEval EEPROM

  output_high(LTC2422_CS);                //! Pulls LTC2442 Chip Select High

  quikeval_I2C_init();                    //! Initializes Linduino I2C port.

  quikeval_SPI_init();                    //! Configures the SPI port for 4MHz SCK
  quikeval_SPI_connect();                 //! Connects SPI to QuikEval port

  Serial.begin(115200);                   //! Initializes the serial port to the PC
  print_title();                          //! Displays the title

  demo_board_connected = discover_demo_board(demo_name); //! Checks if correct demo board is connected.

  if (demo_board_connected)               //! Prints the prompt if the correct demo board is connected
  {
    print_prompt(0);                      // Prints prompt and indicates that "A" is selected.
  }
}

//! Repeats Linduino loop
void loop()
{
  uint8_t i2c_ack = 0;
  uint8_t spi_error_code = 0;
  static int16_t selected_dac = 0;  // The selected DAC to be updated (0=A, 1=B, 2=All).  Initialized to "A".

  // The main control loop
  if (demo_board_connected)         //! Does nothing if the demo board is not connected.
  {
    if (Serial.available())         //! Checks if user input is available.
    {
      int16_t user_command;         // User input command

      user_command = read_int();    //! Reads the user command (if available).
      Serial.println(user_command);
      Serial.flush();
      switch (user_command)
      {
        case 1:
          menu_1_select_dac(&selected_dac);  // Select a DAC to update
          break;

        case 2:
          i2c_ack |= menu_2_write_to_input_register(selected_dac);  // Write to input register only
          break;

        case 3:
          i2c_ack |= menu_3_write_and_update_dac(selected_dac);
          break;

        case 4:
          i2c_ack |= menu_4_update_power_up_dac(selected_dac);      // Update/Power up DAC
          break;

        case 5:
          i2c_ack |= menu_5_power_down_dac(selected_dac);           // Power down DAC
          break;

        case 6:
          spi_error_code = menu_6_read_adc();
          break;

        case 7:
          i2c_ack |= menu_7_sweep();
          spi_error_code |= i2c_ack;
          break;

        case 8:
          i2c_ack |= menu_8_calibrate_all();
          spi_error_code |= i2c_ack;
          break;

        default:
          Serial.println("Incorrect Option");
          break;
      }

      if (i2c_ack != 0) Serial.println(F("Error: No Acknowledge. Check I2C Address."));
      if (spi_error_code != 0) Serial.println(F("Error: SPI communication error."));

      Serial.println();
      Serial.println(F("*************************"));
      print_prompt(selected_dac);
    }
  }
}

// Function Definitions

//! Prompts user to find out if they want to enter DAC code as a "voltage" or a "code"
//! @return PROMPT_CODE or PROMPT_VOLTAGE that indicates whether the user requested for the next entry to be a CODE or VOLTAGE
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

//! Read desired DAC output voltage from user input.
//! @return the code that can be written to the LTC2607 DAC
uint16_t get_voltage(float LTC2607_lsb,         //!< lsb weight of the LTC2607 (possibly calibrated)
                     int32_t LTC2607_offset)    //!< offset of LTC2607 (possibly calibrated)
{
  float dac_voltage;

  Serial.print(F("Enter Desired DAC output voltage: ")); //! Prompt user to enter a voltage
  dac_voltage = read_float();   //! Read a float from the serial terminal
  Serial.print(dac_voltage);
  Serial.println(F(" V"));
  Serial.flush();
  return(LTC2607_voltage_to_code(dac_voltage, LTC2607_lsb, LTC2607_offset)); //! Return the DAC code that results in that voltage.
}

//! @todo Review this file.
//! Reads desired DAC code from user input.
//! @return the adc CODE entered by the user.  (Note, this is a CODE not a VOLTAGE.)
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

//! Prints the title block
void print_title()
{
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC934A Demonstration Program                                  *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates communication with the LTC2607      *"));
  Serial.println(F("* 16-Bit Dual Rail-to-Rail DAC with I2C Interface. This board   *"));
  Serial.println(F("* also features an LTC2422 2-Channel 20-Bit uPower No Latency   *"));
  Serial.println(F("* Delta Sigma ADC for readback.                                 *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 select the newline terminator.    *"));
  Serial.println(F("*****************************************************************"));
}

//! Prints the main menu, and prompts the user for an input command
void print_prompt(int16_t selected_dac) //!< this parameter is passed so that it can be printed at the bottom of the menu.
{
  Serial.println(F("\nCommand Summary:"));

  Serial.println(F("  1-Select DAC"));
  Serial.println(F("  2-Write to input register (no update)"));
  Serial.println(F("  3-Write and update DAC"));
  Serial.println(F("  4-Update/Power up DAC"));
  Serial.println(F("  5-Power Down DAC"));
  Serial.println(F("  6-Read ADC"));
  Serial.println(F("  7-Sweep"));
  Serial.println(F("  8-Calibrate ALL"));
  Serial.println();
  Serial.print(F("  Selected DAC: "));
  if (selected_dac != 2)
    Serial.println((char) (selected_dac + 0x41));
  else
    Serial.println(F("All"));
  Serial.println();
  Serial.print(F("Enter a command:"));
}

//! Menu 1: Select DAC to update.
//! Prompts user for DAC A, DAC B, or both.  This is only a user menu.  This function does not communicate with the LTC2607.
//! @return always returns 0. Fail return code is not implemented.
uint8_t menu_1_select_dac(int16_t *selected_dac) //!< Overwritten with 0, 1, or 2 to indicate whether the user wants commands to act upon DAC A, DAC B, or both DACs respectively.
{
  quikeval_I2C_connect();  //! Connects I2C port to the QuikEval connector
  Serial.print(F("Select DAC to be updated (0=A, 1=B, 2=All)"));
  *selected_dac = read_int();
  if (*selected_dac > 2) *selected_dac=2;  // If user enters and invalid option, default to ALL.
  if (*selected_dac == 2)
    Serial.println(F("All"));
  else
    Serial.println(*selected_dac);
  return(0);  // Always returns success, consider adding a fail code later.
}

//! Menu 2: Write to input register only.  Does not update the output voltage.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
uint8_t menu_2_write_to_input_register(int16_t selected_dac) //!< DAC to be updated. 0=A, 1=B, 2=All
{
  uint16_t dac_code;
  uint8_t ack = 0;
  quikeval_I2C_connect();             //! Connects I2C port to the QuikEval connector

  //! Read voltage or DAC code from user
  if (prompt_voltage_or_code() == PROMPT_VOLTAGE)
    dac_code = get_voltage(LTC2607_lsb[selected_dac], LTC2607_offset[selected_dac]);
  else
    dac_code = get_code();

  //! Write a code to the LTC2607 internal register.  Output voltage is not updated by the "write" command.
  ack = LTC2607_write(LTC2607_I2C_GLOBAL_ADDRESS, LTC2607_WRITE_COMMAND, address_map[selected_dac], dac_code);
  return(ack);
}

//! Menu 3: Write to input register and update output voltage.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
uint8_t menu_3_write_and_update_dac(int16_t selected_dac)  //!< DAC to be updated. 0=A, 1=B, 2=All
{
  uint8_t ack;
  uint16_t dac_code;
  quikeval_I2C_connect();             //! Connects I2C port to the QuikEval connector

  //! Read voltage or DAC code from user
  if (prompt_voltage_or_code() == PROMPT_VOLTAGE)
    dac_code = get_voltage(LTC2607_lsb[selected_dac], LTC2607_offset[selected_dac]);
  else
    dac_code = get_code();

  //! Write a code to the LTC2607 internal register and update the output voltage.
  ack = LTC2607_write(LTC2607_I2C_GLOBAL_ADDRESS, LTC2607_WRITE_UPDATE_COMMAND, address_map[selected_dac], dac_code);    // Write a code to the LTC2607
  return(ack);
}

//! Menu 4: Update/Power Up DAC
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
uint8_t menu_4_update_power_up_dac(int16_t selected_dac)  //!< DAC to be updated. 0=A, 1=B, 2=All
{
  uint8_t ack;

  quikeval_I2C_connect();           //! Connects I2C port to the QuikEval connector
  ack = LTC2607_write(LTC2607_I2C_GLOBAL_ADDRESS, LTC2607_UPDATE_COMMAND, address_map[selected_dac], 0);   //! Update output voltage from internal register.  Data is ignored.
  return(ack);
}

//! Menu 5: Power Down DAC
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
uint8_t menu_5_power_down_dac(int16_t selected_dac)
{
  uint8_t ack;
  quikeval_I2C_connect();     //! Connects I2C port to the QuikEval connector
  ack = LTC2607_write(LTC2607_I2C_GLOBAL_ADDRESS, LTC2607_POWER_DOWN_COMMAND, address_map[selected_dac], 0);    //! Power down selected DAC.  Data is ignored.
  return(ack);
}

//! Menu 6: Read voltage from the ADC
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
uint8_t menu_6_read_adc()
{
  float adc_voltage;
  int32_t adc_code;
  uint8_t adc_channel;
  uint32_t  adc_code_array[2];   // Array for ADC data. Useful because you don't know which channel until the LTC2422 tells you.
  uint8_t acknowledge;

  // Read ADC
  quikeval_SPI_connect();       //! Connect SPI to QuikEval connector

  LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);            //! Throw out the stale data

  if (acknowledge = LTC2422_EOC_timeout(LTC2422_CS, MISO_TIMEOUT))  // Check for EOC
    return(1);
  LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);            //! Read both channels from the ADC.  The ADC channel will toggle with each reading.
  adc_code_array[adc_channel] = adc_code;                           //! Note that we're not sure which channel is first prior to reading.

  if (acknowledge = LTC2422_EOC_timeout(LTC2422_CS, MISO_TIMEOUT))  // Check for EOC
    return(1);
  LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);            //! Therefore, we use adc_channel (reported by LTC2422 during read operation) as index into two element array.
  adc_code_array[adc_channel] = adc_code;                           //! One element holds channel A ADC code and the other holds the channel B ADC code.

  //! Print ADC A voltage.
  Serial.println();
  Serial.print(F("     ADC A : Code: 0x"));
  Serial.println(adc_code_array[1], HEX);
  Serial.print(F("             Voltage: "));
  adc_voltage = LTC2422_code_to_voltage(adc_code_array[1], LTC2422_lsb);
  Serial.println(adc_voltage, 6);

  //! Print ADC B voltage.
  Serial.println();
  Serial.print(F("     ADC B : Code: 0x"));
  Serial.println(adc_code_array[0], HEX);
  Serial.print(F("             Voltage: "));
  adc_voltage = LTC2422_code_to_voltage(adc_code_array[0], LTC2422_lsb);
  Serial.println(adc_voltage, 6);
  return(0);
}

//! Menu 7: Voltage Sweep
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
uint8_t menu_7_sweep()
{
  uint8_t ack = 0;
  Serial.print(F("Enter the desired number of sample points: "));

  //! Reads number of sample points from user.
  uint16_t sample;
  sample = read_int();
  if (sample == 0)
    sample = 1;
  Serial.println(sample);
  if (sample > 65535)
    sample = 65535;

  //! Calculates size of each step.
  uint16_t sample_code;
  sample_code = 65535/sample;

  int32_t adc_code;
  uint8_t adc_channel;
  quikeval_SPI_connect();                                   //! Connect SPI to QuikEval connector

  if (LTC2422_EOC_timeout(LTC2422_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);    //! Take one ADC reading. Throw away the data, but check the channel.
  //! If the data was from channel 1, take another reading so that the next reading will be channel 0.
  if (adc_channel == 1)
  {
    if (LTC2422_EOC_timeout(LTC2422_CS, MISO_TIMEOUT))      // Check for EOC
      return(1);
    LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);  // IF we just read channel 1 (DAC A),
    // a conversion on channel B has just started. We want the FIRST reading in the table to be DAC A, so flush
    // this conversion, starting another conversion on DAC A. The Autozero phase takes 4/60 of a second (66.7ms) so there
    // is some time to print the header and set the DAC outputs.
  }
  Serial.println(F("Code, DAC A,DAC B"));

  float adc_voltage;
  uint16_t dac_code;
  uint16_t i;
  //! For loop steps one voltage step at a time.
  for (i = 0; i <= sample; i++)
  {
    quikeval_I2C_connect();                                 //! Connects I2C port to the QuikEval connector
    dac_code = (sample_code*i);
    if (dac_code > 65535)
      dac_code = 65535;

    //! Write DAC code to both channels.
    ack |= LTC2607_write(LTC2607_I2C_GLOBAL_ADDRESS, LTC2607_WRITE_UPDATE_COMMAND, LTC2607_ALL_DACS, dac_code);

    quikeval_SPI_connect();                                 //! Connect SPI to QuikEval connector

    if (LTC2422_EOC_timeout(LTC2422_CS, MISO_TIMEOUT))      // Check for EOC
      return(1);
    //! Read ADC channel 0 (DAC A) voltage and print it.
    LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);

    Serial.print(dac_code);
    Serial.print(F(","));
    adc_voltage = LTC2422_code_to_voltage(adc_code, LTC2422_lsb);
    Serial.print(adc_voltage, 6);
    Serial.print(F(","));

    if (LTC2422_EOC_timeout(LTC2422_CS, MISO_TIMEOUT))      // Check for EOC
      return(1);
    //! Read ADC channel 1 (DAC B) voltage and print it.
    LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);
    adc_voltage = LTC2422_code_to_voltage(adc_code, LTC2422_lsb);
    Serial.println(adc_voltage, 6);
    Serial.flush();

    //! If they get out of sync, print "Out of sync!". This only happens if something bad occurs.
    if (adc_channel == 1)
    {
      Serial.println(F("Out of sync!!"));
      quikeval_I2C_connect();
      ack |= LTC2607_write(LTC2607_I2C_GLOBAL_ADDRESS, LTC2607_WRITE_UPDATE_COMMAND, LTC2607_ALL_DACS, 0x0000); //Set output to zero
      return(1);
    }
  }
  quikeval_I2C_connect();
  ack |= LTC2607_write(LTC2607_I2C_GLOBAL_ADDRESS, LTC2607_WRITE_UPDATE_COMMAND, LTC2607_ALL_DACS, 0x0000); //Set output to zero
  Serial.println();
  Serial.println(F("Copy and save data points to a .csv file"));
  Serial.println(F("and open in Excel to plot points."));
  return(ack);
}

//! Menu 8: Calibrate All
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
uint8_t menu_8_calibrate_all()
{
  uint8_t ack = 0;
  float voltage1[2];                                  // Calibration voltage 1
  float voltage2[2];                                  // Calibration voltage 2
  const uint16_t CAL_LOW_DAC_CODE = 0x00FF;           // LTC2607 calibration code used for low-side force/measure.
  const uint16_t CAL_HIGH_DAC_CODE = 0xFF00;          // LTC2607 calibration code used for high-side force/measure.

  // Calibrate All
  Serial.println(F("Measure and Enter Reference "));
  Serial.print(F("Voltage for The ADC: "));

  float ref_voltage;
  ref_voltage = read_float();
  Serial.println(ref_voltage, 6);
  LTC2422_calculate_lsb(ref_voltage, &LTC2422_lsb);

  quikeval_I2C_connect();                 //! Connects I2C port to the QuikEval connector

  ack |= LTC2607_write(LTC2607_I2C_GLOBAL_ADDRESS, LTC2607_WRITE_UPDATE_COMMAND, LTC2607_DAC_A, CAL_LOW_DAC_CODE);
  ack |= LTC2607_write(LTC2607_I2C_GLOBAL_ADDRESS, LTC2607_WRITE_UPDATE_COMMAND, LTC2607_DAC_B, CAL_LOW_DAC_CODE);
  Serial.println();
  Serial.print(F("Calibrating DACs ... "));
  delay(2000);

  quikeval_SPI_connect();                 //! Connect SPI to QuikEval connector

  delay(50);

  int32_t adc_code;
  uint8_t adc_channel;
  if (LTC2422_EOC_timeout(LTC2422_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);    // Throw away last reading
  if (LTC2422_EOC_timeout(LTC2422_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);
  if (adc_channel == 0)
    voltage1[1] = LTC2422_code_to_voltage(adc_code, LTC2422_lsb);
  if (adc_channel == 1)
    voltage1[0] = LTC2422_code_to_voltage(adc_code, LTC2422_lsb);
  if (LTC2422_EOC_timeout(LTC2422_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);
  if (adc_channel == 0)
    voltage1[1] = LTC2422_code_to_voltage(adc_code, LTC2422_lsb);
  if (adc_channel == 1)
    voltage1[0] = LTC2422_code_to_voltage(adc_code, LTC2422_lsb);

  quikeval_I2C_connect();                                   //! Connects I2C port to the QuikEval connector

  ack |= LTC2607_write(LTC2607_I2C_GLOBAL_ADDRESS, LTC2607_WRITE_UPDATE_COMMAND, LTC2607_DAC_A, CAL_HIGH_DAC_CODE);   // Set DAC to Full Scale
  ack |= LTC2607_write(LTC2607_I2C_GLOBAL_ADDRESS, LTC2607_WRITE_UPDATE_COMMAND, LTC2607_DAC_B, CAL_HIGH_DAC_CODE);   // Set DAC to Full Scale

  delay(2000);
  quikeval_SPI_connect();                                   //! Connect SPI to QuikEval connector

  delay(50);

  if (LTC2422_EOC_timeout(LTC2422_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);    // Throw away last reading
  if (LTC2422_EOC_timeout(LTC2422_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);
  if (adc_channel == 0)
    voltage2[1] = LTC2422_code_to_voltage(adc_code, LTC2422_lsb);
  if (adc_channel == 1)
    voltage2[0] = LTC2422_code_to_voltage(adc_code, LTC2422_lsb);
  if (LTC2422_EOC_timeout(LTC2422_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);
  if (adc_channel == 0)
    voltage2[1] = LTC2422_code_to_voltage(adc_code, LTC2422_lsb);
  if (adc_channel == 1)
    voltage2[0] = LTC2422_code_to_voltage(adc_code, LTC2422_lsb);

  LTC2607_calibrate(CAL_LOW_DAC_CODE, CAL_HIGH_DAC_CODE, voltage1[0], voltage2[0], &LTC2607_lsb[0], &LTC2607_offset[0]);
  LTC2607_calibrate(CAL_LOW_DAC_CODE, CAL_HIGH_DAC_CODE, voltage1[1], voltage2[1], &LTC2607_lsb[1], &LTC2607_offset[1]);
  // Store All DACs lsb and offset from LTC2607_lsb[0] and LTC2607_offset[0]
  LTC2607_lsb[2] = LTC2607_lsb[0];
  LTC2607_offset[2] = LTC2607_offset[0];
  Serial.println(F("Calibration Complete"));

  Serial.print(F("lsb DAC A: "));
  Serial.print(LTC2607_lsb[0], 6);
  Serial.print(F(" V   offset: "));
  Serial.println(LTC2607_offset[0]);

  Serial.print(F("lsb DAC B: "));
  Serial.print(LTC2607_lsb[1], 6);
  Serial.print(F(" V   offset: "));
  Serial.println(LTC2607_offset[1]);

  Serial.print(F("All DACs: "));
  Serial.print(LTC2607_lsb[2], 6);
  Serial.print(F(" V   offset: "));
  Serial.println(LTC2607_offset[2]);
  return(ack);
}