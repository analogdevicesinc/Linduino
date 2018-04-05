/*!
Analog Devices EVAL-ADM1293EBZ Demonstration Board
ADM1293: Digital Power Minitor with PMBus Interface

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim


REVISION HISTORY
$Revision: 4080 $
$Date: 2018-03-19 16:36:34 -0600 (Mon, 19 Mar 2018) $

Copyright (c) 2018, Analog Devices
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Analog Devices.

The Analog Devices Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/

/*! @file
    @ingroup ADM1293
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_I2CBus.h"
#include "LT_SMBusNoPec.h"
#include "LT_SMBusPec.h"
//#include "LT_PMBusMath.h"
//#include "LT_PMBus.h"
#include "ADM1293.h"

#define ADM1293_I2C_ADDRESS 0x30

// Global variables
static uint8_t adm1293_i2c_address;
// static LT_PMBusMath *math = new LT_PMBusMath();
static LT_SMBus *smbus = new LT_SMBusNoPec();
//static LT_PMBus *pmbus = new LT_PMBus(smbus);

//! properties of the demo board
float r_sense; // sense resistor value in milliohms
uint16_t vin_range, i_range; // range settings
float vin_lsb, i_lsb, p_lsb, p_lsb_16b; // calculated ADC LSB sizes based on ranges


//! Initialize Linduino
//! @return void
void setup()
{
  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  adm1293_i2c_address = ADM1293_I2C_ADDRESS;
  print_prompt();

  // the ADM1293 doesn't know the value of the current sense resistor
  // there are also V and I full-scale range settings to consider
  // the user must convert direct register readings to voltage/current/energy/power
  //********CHANGE THESE VALUES FOR THE APPLICATION*************
  r_sense = 0.05; // sense resistor value in ohms; the demo board ships with 0.05
  vin_range = 3; // valid values are 0,1,2,3 from the VIN_SEL setting in the PMON_CONFIG reg
  i_range = 0; // valid values are 0,1,2,3 from the IRANGE setting in the PMON_CONFIG reg
  // NOTE: on the EVAL board the IC draws its power from the input
  //************************************************************

  switch(vin_range) {
  case 0:
    vin_lsb = 0.0; // vin sampling is disabled
    break;
  case 1:
    vin_lsb = 0.000292;
    break;
  case 2:
    vin_lsb = 0.001801;
    break;
  case 3:
    vin_lsb = 0.005101;
    break;
  default:
    // bad choice
    Serial.println(F("\nERROR, bad vin_range selection"));
    vin_lsb = 0.0; // vin sampling is disabled
    break;
  }
  switch(i_range) {
  case 0:
    i_lsb = 0.0000125 / r_sense;
    break;
  case 1:
    i_lsb = 0.000025 / r_sense;
    break;
  case 2:
    i_lsb = 0.00005 / r_sense;
    break;
  case 3:
    i_lsb = 0.0001 / r_sense;
    break;
  default:
    // bad choice
    Serial.println(F("\nERROR, bad i_range selection"));
    i_lsb = 0.0;
    break;
  }
  p_lsb = i_lsb * vin_lsb; // 24-bit LSB value (READ_PIN_EXT)
  p_lsb_16b = p_lsb * 256; // 16-bit LSB (READ_PIN), which is 2^8 bigger than the 24-b LSB
  configure_adm1293();


  //    Serial.print(F("VIN_LSB = "));
  //    Serial.println(vin_lsb, 6);
  //    Serial.print(F("I_LSB = "));
  //    Serial.println(i_lsb, 6);
  //    Serial.print(F("PIN_LSB = "));
  //    Serial.println(p_lsb_16b, 10);

}

//! Repeats Linduino loop
//! @return void
void loop()
{
  uint8_t user_command;
  uint8_t *addresses = NULL;

  if (Serial.available()) {                         //! Checks for user input
    user_command = read_int();                     //! Reads the user command
    Serial.println(user_command);
    
    switch (user_command)  {                         //! Prints the appropriate submenu
      
    case 1:
      print_v_i_p();
      break;
    case 2:
      print_all_status();
      break;
    case 3:
      print_peak_v_i_p();
      break;
    case 4:
      clear_peak_v_i_p();
      break;
    case 5:
      configure_adm1293();
      clear_peak_v_i_p();
      clear_energy();
      clear_faults();
      break;
    case 6:

      break;
    default:
      Serial.println(F("Invalid Selection"));
      break;
    }
    print_prompt();
  }
} // loop

//
// Function Definitions
//

//! Prints the title block when program first starts.
//! @return void
void print_title()
{
  Serial.print(F("\n*******************************************************************\n"));
  Serial.print(F("* EVAL-ADM1293EBZ Hello World Demonstration Program               *\n"));
  Serial.print(F("*                                                                 *\n"));
  Serial.print(F("* This program demonstrates how to send and receive data from     *\n"));
  Serial.print(F("* the EVAL-ADM1293EBZ demo board.                                 *\n"));
  Serial.print(F("*                                                                 *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.  *\n"));
  Serial.print(F("*                                                                 *\n"));
  Serial.print(F("*******************************************************************\n"));
}


 void print_prompt()
 {
   //! Displays the Read/Write menu
   Serial.print(F("\n"));
   Serial.print(F("  1-Read Voltage, Current, Power\n"));
   Serial.print(F("  2-Read All Status\n"));
   Serial.print(F("  3-Read Peak Registers\n"));
   Serial.print(F("  4-Clear Peak Registers\n"));
   Serial.print(F("  5-Re-Start; Clear All Registers\n"));
   //   Serial.print(F("  6- \n"));
   Serial.print(F("\nEnter a command: "));
 }

void configure_adm1293()
{
  uint16_t pmon_cfg = 0x0000;
  uint16_t iout_lmt = 0x07FF;
  
  // configure the part on the demo board
  smbus->writeByte(adm1293_i2c_address, ADM1293_PMON_CONTROL, 0x00); // stop the pwr monitor

  smbus->writeWord(adm1293_i2c_address, ADM1293_IOUT_OC_WARN_LIMIT, 0x0190);
  smbus->writeWord(adm1293_i2c_address, ADM1293_VIN_OV_WARN_LIMIT, 0x09FF);
  smbus->writeWord(adm1293_i2c_address, ADM1293_VIN_UV_WARN_LIMIT, 0x0780);
  smbus->writeWord(adm1293_i2c_address, ADM1293_PIN_OP_WARN_LIMIT, 0x7FFF);
  smbus->writeWord(adm1293_i2c_address, ADM1293_ALERT1_CONFIG, 0x0180); // alert on VIN OV/UV
  smbus->writeWord(adm1293_i2c_address, ADM1293_ALERT2_CONFIG, 0x0600); // alert on IOUT current (hysteresis values)
  smbus->writeWord(adm1293_i2c_address, ADM1293_VAUX_OV_WARN_LIMIT, 0x0FFF);
  smbus->writeWord(adm1293_i2c_address, ADM1293_VAUX_UV_WARN_LIMIT, 0x0000);
  smbus->writeWord(adm1293_i2c_address, ADM1293_DEVICE_CONFIG, 0x03F0);
  smbus->writeWord(adm1293_i2c_address, ADM1293_HYSTERESIS_LOW, 0x0180);
  smbus->writeWord(adm1293_i2c_address, ADM1293_HYSTERESIS_HIGH, 0x0190);

  // make some decisions based on selected values
  pmon_cfg = 0x2D10; // 2D1C
  switch(vin_range) {
  case 0:
    pmon_cfg = (pmon_cfg | 0x0000) ;
    break;
  case 1:
    pmon_cfg = (pmon_cfg | 0x0004) ;
    break;
  case 2:
    pmon_cfg = (pmon_cfg | 0x0008) ;
    break;
  case 3:
    pmon_cfg = (pmon_cfg | 0x000C) ;
    break;
  default:
    // bad choice
    pmon_cfg = (pmon_cfg | 0x0000) ;
    break;
  }
  switch(i_range) {
  case 0:
    pmon_cfg = (pmon_cfg | 0x0000) ;
    iout_lmt = 0x3FF;
  break;
  case 1:
    pmon_cfg = (pmon_cfg | 0x0040) ;
    iout_lmt = 0x3FF;
    break;
  case 2:
    pmon_cfg = (pmon_cfg | 0x0080) ;
    iout_lmt = 0x3FF;
    break;
  case 3:
    pmon_cfg = (pmon_cfg | 0x00C0) ;
    iout_lmt = 0x3FF;
    break;
  default:
    // bad choice
    pmon_cfg = (pmon_cfg | 0x0000) ;
    iout_lmt = 0x7FF;
    break;
  }
  pmon_cfg = (pmon_cfg | 0x0010) ; // command continuous sampling
  smbus->writeWord(adm1293_i2c_address, ADM1293_IOUT_OC_WARN_LIMIT, iout_lmt);
  smbus->writeWord(adm1293_i2c_address, ADM1293_PMON_CONFIG, pmon_cfg); // configure the converter

  smbus->sendByte(adm1293_i2c_address, ADM1293_CLEAR_FAULTS); // clear faults
  smbus->writeByte(adm1293_i2c_address, ADM1293_PMON_CONTROL, 0x01); // start the pwr monitor
 }

void print_all_status()
{
  uint16_t return_val; // register value

  // status
  Serial.print(F("PMON_CONFIG: "));
  return_val = 0x0000;
  return_val = smbus->readWord(adm1293_i2c_address, ADM1293_PMON_CONFIG);
  Serial.println(return_val, HEX);

  Serial.print(F("STATUS_BYTE: "));
  return_val = 0x0000;
  return_val = smbus->readByte(adm1293_i2c_address, ADM1293_STATUS_BYTE);
  Serial.println(return_val, HEX);

  Serial.print(F("STATUS_WORD: "));
  return_val = 0x0000;
  return_val = smbus->readWord(adm1293_i2c_address, ADM1293_STATUS_WORD);
  Serial.println(return_val, HEX);

  Serial.print(F("STATUS_INPUT: "));
  return_val = 0x0000;
  return_val = smbus->readByte(adm1293_i2c_address, ADM1293_STATUS_INPUT);
  Serial.println(return_val, HEX);

  Serial.print(F("STATUS_IOUT: "));
  return_val = 0x0000;
  return_val = smbus->readByte(adm1293_i2c_address, ADM1293_STATUS_IOUT);
  Serial.println(return_val, HEX);

  Serial.print(F("STATUS_MFR_SPECIFIC: "));
  return_val = 0x0000;
  return_val = smbus->readByte(adm1293_i2c_address, ADM1293_STATUS_MFR_SPECIFIC);
  Serial.println(return_val, HEX);

  Serial.print(F("STATUS_HYSTERESIS: "));
  return_val = 0x0000;
  return_val = smbus->readByte(adm1293_i2c_address, ADM1293_STATUS_HYSTERESIS);
  Serial.println(return_val, HEX);
}


void print_v_i_p()
{
  uint16_t return_val; // read value
  uint8_t block[9]; // block for larger reads (energy and power)
  uint32_t pwr24;
  float v_in, // voltage
    i_out, // current
    p_in, // power
    e_in; // energy
  char minus = ' ';
  int i;
  
  // voltage
  return_val = smbus->readWord(adm1293_i2c_address, ADM1293_READ_VIN);
  v_in = vin_lsb * return_val;
  Serial.print(F("VIN: "));
  Serial.print(v_in,4);
  Serial.print(F(" V (0x"));
  Serial.print(return_val, HEX);
  Serial.println(F(")"));

  // current
  return_val = smbus->readWord(adm1293_i2c_address, ADM1293_READ_IOUT);
  // current can be negative, so handle 2s complement
  if(return_val < 0x8000) { // must be a positive number
    i_out = i_lsb * return_val;
    Serial.println(F("POSITIVE CURRENT"));
  }
  else {
    i_out = i_lsb * (~return_val + 1);
    Serial.println(F("NEGATIVE CURRENT"));
    minus = '-';
  }
  Serial.print(F("IOUT: "));
  Serial.print(minus);
  Serial.print(i_out,4);
  Serial.print(F(" A (0x"));
  Serial.print(return_val, HEX);
  Serial.println(F(")"));

  // power
  // note that the 16-bit value is the upper 16 of 24 bits
  //  obtained by multiplying I x V
  return_val = smbus->readWord(adm1293_i2c_address, ADM1293_READ_PIN);
  // power can be negative (with current flow), so handle 2s complement
  if(return_val < 0x8000) { // must be a positive number
    p_in = p_lsb_16b * return_val;
  }
  else {
    p_in = p_lsb_16b * (~return_val + 1);
  }  
  Serial.print(F("PIN: "));
  Serial.print(minus);
  Serial.print(p_in,4);
  Serial.print(F(" W (0x"));
  Serial.print(return_val, HEX);
  Serial.println(F(")"));

  // power 24-bit
  //  obtained by multiplying I x V
  return_val = smbus->readBlock(adm1293_i2c_address, ADM1293_READ_PIN_EXT, block, 3); // 3-byte block read
  if(return_val != 3) {
    Serial.print(F("BAD BLOCK READ "));
    Serial.println(return_val);
    return; // something went wrong
  }
  else {    // the bytes are reversed in the i2c packet
    pwr24 = 0;
    for(i = 0; i < 3; i++) { 
      pwr24 += block[i]*pow(2,(i*8));
      //      Serial.print(block[i], HEX);
      //      Serial.print(F(" : "));
      //      Serial.println(pow(2,(i*8)), HEX);
    }
    //pwr24 = (block[2]*65536 + block[1]*256 + block[0]);
  }
  //  Serial.print(F("PWR24 "));
  //  Serial.println(pwr24, HEX);

  // power can be negative (with current flow), so handle 2s complement  
  if(pwr24 < 0x800000) { // must be a positive number
    p_in = p_lsb * pwr24;
  }
  else {
    p_in = p_lsb * (~pwr24 + 1);
  }  
  Serial.print(F("PIN (24-bit): "));
  Serial.print(minus);
  Serial.print(p_in,6);
  Serial.print(F(" W (0x"));
  Serial.print(pwr24, HEX);
  Serial.println(F(")"));
  
}

void print_peak_v_i_p()
{
  uint16_t return_val; // register value
  uint64_t ein64;
  uint8_t block[9]; // for larger reads
  float v_in, // voltage
    i_out, // current
    p_in; // power
  char minus = ' ';
  int i;
  
  // voltage
  return_val = smbus->readWord(adm1293_i2c_address, ADM1293_PEAK_VIN);
  v_in = vin_lsb * return_val;
  Serial.print(F("PEAK VIN: "));
  Serial.print(v_in,4);
  Serial.print(F(" V (0x"));
  Serial.print(return_val, HEX);
  Serial.println(F(")"));

  // current
  return_val = smbus->readWord(adm1293_i2c_address, ADM1293_MAX_IOUT);
  // current can be negative, so handle 2s complement
  if(return_val < 0x8000) { // must be a positive number
    i_out = i_lsb * return_val;
    Serial.println(F("POSITIVE CURRENT"));
  }
  else {
    i_out = i_lsb * (~return_val + 1);
    Serial.println(F("NEGATIVE CURRENT"));
    minus = '-';
  }
  Serial.print(F("MAX IOUT: "));
  Serial.print(minus);
  Serial.print(i_out,4);
  Serial.print(F(" A (0x"));
  Serial.print(return_val, HEX);
  Serial.println(F(")"));

  // power 16-bit
  // note that the 16-bit value is the upper 16 of 24 bits
  //  obtained by multiplying I x V
  return_val = smbus->readWord(adm1293_i2c_address, ADM1293_MAX_PIN);
  // power can be negative (with current flow), so handle 2s complement
  if(return_val < 0x8000) { // must be a positive number
    p_in = p_lsb_16b * return_val;
  }
  else {
    p_in = p_lsb_16b * (~return_val + 1);
  }  
  Serial.print(F("MAX PIN (16-bit): "));
  Serial.print(minus);
  Serial.print(p_in,4);
  Serial.print(F(" W (0x"));
  Serial.print(return_val, HEX);
  Serial.println(F(")"));

  // NOTE: Arduino PRINT doesn't handle 64-bit numbers very well, so this section is commented out
  // energy 64-bit
  // obtained by accumulating power
  //  return_val = smbus->readBlock(adm1293_i2c_address, ADM1293_READ_EIN_EXT, block, 8); // 8-byte block read
  //  if(return_val != 8) {
  //    Serial.print(F("BAD BLOCK READ "));
  //    Serial.println(return_val);
  //    return; // something went wrong
  //  }
  //  else {    // the bytes are reversed in the i2c packet
  //    ein64 = 0;
  //    for(i = 0; i < 8; i++) {
  //      ein64 += block[i]*pow(2,(i*8));
  //      Serial.println(block[i], HEX);
  //    }
  //  }
  //  Serial.print(F("EIN64 "));
  //  Serial.println(e_in);

  //  // power can be negative (with current flow), so handle 2s complement  
  //  if(ein64 < 0x8000000000000000) { // must be a positive number
  //    e_in = p_lsb * ein64;
  //  }
  //  else {
  //    e_in = p_lsb * (~ein64 + 1);
  //  }  
  //  Serial.print(F("EIN (64-bit): "));
  //  Serial.print(minus);
  //  Serial.print(e_in,6);
  //  Serial.print(F(" J (0x"));
  //  Serial.print(e_in, HEX);
  //  Serial.println(F(")"));  

}


void clear_peak_v_i_p()
{
  smbus->writeWord(adm1293_i2c_address, ADM1293_PEAK_VIN, 0x0000);
  smbus->writeWord(adm1293_i2c_address, ADM1293_MAX_IOUT, 0x0000);
  smbus->writeWord(adm1293_i2c_address, ADM1293_MAX_PIN, 0x0000);
}

void clear_faults()
{
   smbus->sendByte(adm1293_i2c_address, ADM1293_CLEAR_FAULTS); // clear faults 
}

void clear_energy()
{
  uint8_t block[8];
  int i;

  for (i = 0; i < 8; i++) {
    block[i] = 0;
  }
  smbus->writeBlock(adm1293_i2c_address, ADM1293_READ_EIN, block, 6);
  smbus->writeBlock(adm1293_i2c_address, ADM1293_READ_EIN_EXT, block, 8);
}
