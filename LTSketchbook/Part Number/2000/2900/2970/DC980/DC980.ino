/*!
Linear Technology DC980A/B Demonstration Board Control

LTC2970: Dual I2C Power Supply Monitor and Margining Controller

@verbatim
http://www.linear.com/product/LTC2970

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim

http://www.linear.com/product/LTC2970

http://www.linear.com/demo/#demoboards


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
!*/

/*! @file
    @ingroup LTC2970
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_SMBusNoPec.h"
#include "LTC2970.h"

#define LTC2970_I2C_ADDRESS 0x5B //global 7-bit address
//#define LTC2970_I2C_ADDRESS 0x6F //SLAVE_HH 7-bit address


/****************************************************************************/
// Global variables
static uint8_t ltc2970_i2c_address;

static LT_SMBusNoPec *smbus = new LT_SMBusNoPec();

uint16_t servo0_value_marg;
uint16_t servo0_value_nom;
uint16_t servo1_value_marg;
uint16_t servo1_value_nom;


/****************************************************************************/
//! Initialize Linduino
//! @return void
void setup()
{
  uint16_t return_val;

  // initialize the i2c port
  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  print_prompt();

  ltc2970_i2c_address = LTC2970_I2C_ADDRESS;

  servo0_value_nom = 0x2733;
  servo1_value_nom = 0x1A24;
  servo0_value_marg = 0x2347; // 10% low
  servo1_value_marg = 0x1786; // 10% low

  //************************  init_voltage_transition();
}

//! Main Linduino loop
//! @return void
void loop()
{
  uint8_t user_command;
  uint16_t return_val;

  int i = 0;

  if (Serial.available())                //! Checks for user input
  {
    user_command = read_int();         //! Reads the user command
    if (user_command != 'm')
      Serial.println(user_command);

    switch (user_command)              //! Prints the appropriate submenu
    {

      case 1 :
        Serial.print(F("\n****INITIALIZING THE LTC2970****\n"));
        ltc2970_configure();
        break;

      case 2 :
        Serial.print(F("\n****ENABLE LTC2970 CHANNEL 0 AND CHANNEL 1****\n"));
        ltc2970_dac_disconnect(smbus, ltc2970_i2c_address, 0);
        ltc2970_gpio_up(smbus, ltc2970_i2c_address, 0);

        ltc2970_dac_disconnect(smbus, ltc2970_i2c_address, 1);
        ltc2970_gpio_up(smbus, ltc2970_i2c_address, 1);
        break;

      case 3 :
        Serial.print(F("\n****SOFT CONNECT LTC2970 DAC0 and DAC1****\n"));
        ltc2970_soft_connect_dac(smbus, ltc2970_i2c_address, 0);
        ltc2970_soft_connect_dac(smbus, ltc2970_i2c_address, 1);
        break;

      case 4 :
        Serial.print(F("\n****SERVO CHANNEL 0 and 1 VOLTAGES 10% LOW****\n"));
        ltc2970_servo_to_adc_val(smbus, ltc2970_i2c_address, 0, servo0_value_marg);
        ltc2970_servo_to_adc_val(smbus, ltc2970_i2c_address, 1, servo1_value_marg);
        break;

      case 5 :
        Serial.print(F("\n****SERVO CHANNEL 0 and 1 VOLTAGES TO NOMINAL****\n"));
        ltc2970_servo_to_adc_val(smbus, ltc2970_i2c_address, 0, servo0_value_nom);
        ltc2970_servo_to_adc_val(smbus, ltc2970_i2c_address, 1, servo1_value_nom);
        break;

      case 6 :
        Serial.print(F("\n****ADC CH_0 VOLTAGE =   (HEX VALUE)\n"));
        return_val = smbus->readWord(ltc2970_i2c_address, LTC2970_CH0_A_ADC);
        Serial.println(((return_val & 0x7FFF)*500e-6), DEC);
        Serial.println(return_val, HEX);

        Serial.print(F("\n****ADC CH_1 VOLTAGE =   (HEX VALUE)\n"));
        return_val = smbus->readWord(ltc2970_i2c_address, LTC2970_CH1_A_ADC);
        Serial.println(((return_val & 0x7FFF)*500e-6), DEC);
        Serial.println(return_val, HEX);
        break;

      case 7 :
        Serial.print(F("\n****ADC CH_0 CURRENT =   (HEX VALUE)\n"));
        return_val = smbus->readWord(ltc2970_i2c_address, LTC2970_CH0_B_ADC);
        Serial.println((((return_val & 0x7FFF)*500e-6)/0.007), DEC);
        Serial.println(return_val, HEX);

        Serial.print(F("\n****ADC CH_1 CURRENT =   (HEX VALUE)\n"));
        return_val = smbus->readWord(ltc2970_i2c_address, LTC2970_CH1_B_ADC);
        Serial.println((((return_val & 0x7FFF)*500e-6)/0.008), DEC);
        Serial.println(return_val, HEX);
        break;

      case 8 :
        Serial.print(F("\n****PRINT FAULTS, CLEAR LATCHED FAULTS \n"));
        ltc2970_read_faults(smbus, ltc2970_i2c_address);
        break;

      case 9 :
        Serial.print(F("\n****PRINT DIE TEMPERATURE \n"));
        ltc2970_print_die_temp (smbus, ltc2970_i2c_address);
        break;

      default:
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
  Serial.print(F("* DC980 Regulator Control Program                               *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program provides a simple interface to control the       *\n"));
  Serial.print(F("* the DC980 regulators through the LTC2970                      *\n"));
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
  Serial.print(F("  1  - Reset the LTC2970, Disable Regulators\n"));
  Serial.print(F("  2  - Enable Channel 0 and Channel 1; DACs disconnected\n"));
  Serial.print(F("  3  - Soft-Connect DAC0 and DAC1, and Confirm Connection\n"));
  Serial.print(F("  4  - Servo Channel 0 and Channel 1 Voltages 10% low\n"));
  Serial.print(F("  5  - Servo Channel 0 and Channel 1 Voltages to nominal\n"));
  Serial.print(F("  6  - Print Channel 0 & 1 Voltages\n"));
  Serial.print(F("  7  - Print Channel 0 & 1 Currents\n"));
  Serial.print(F("  8  - Print Fault Register Contents\n"));
  Serial.print(F("  9  - Print LTC2970 Temperature\n"));
  Serial.print(F("\nEnter a command number:"));
}


//! Writes configuration values to the LTC2970 registers
//! @return void
void ltc2970_configure()
{
  uint16_t return_val;
  //start the 2970 by configuring all of its registers for this application
  // use SMbus commands
  smbus->writeWord(ltc2970_i2c_address, LTC2970_FAULT_EN, 0x0168);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_IO, 0x000A);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_ADC_MON, 0x007F);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_VDD_OV, 0x2CEC);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_VDD_UV, 0x2328);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_V12_OV, 0x3FFF);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_V12_UV, 0x00000);

  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_A_OV, 0x2AF8);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_A_UV, 0x2328);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_A_SERVO, 0x0000);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_A_IDAC, 0x0084);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_B_OV, 0x3FFF);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_B_UV, 0x0000);

  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_A_OV, 0x1C5D);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_A_UV, 0x1770);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_A_SERVO, 0x0000);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_A_IDAC, 0x0084);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_B_OV, 0x3FFF);
  smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_B_UV, 0x0000);
}

