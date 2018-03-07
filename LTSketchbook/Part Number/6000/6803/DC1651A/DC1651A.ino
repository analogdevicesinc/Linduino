/*!
Linear Technology DC1651A Demonstration Board.
LTC6803-1: Battery stack monitor

@verbatim

NOTES
 Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.
   Ensure all jumpers on the demo board are installed in their default positions from the factory.
   Refer to Demo Manual DC1651.


 Menu Entry 1: Write Configuration
   Writes the configuration register of the LTC6803s on the stack. This command can be used to turn on
   the reference and shorten ADC conversion Times.

 Menu Entry 2: Read Configuration
   Reads the configuration register of the LTC6803, the read configuration can differ from the written configuration.
   The GPIO pins will reflect the state of the pin

 Menu Entry 3: Start Cell voltage conversion
    Starts a LTC6803 cell channel adc conversion.

 Menu Entry 4: Read cell voltages
    Reads the LTC6803 cell voltage registers and prints the results to the serial port.

 Menu Entry 5: Start Auxiliary voltage conversion
    Starts a LTC6803 Temp channel adc conversion.

 Menu Entry 6: Read Auxiliary voltages
    Reads the LTC6803 Temp registers and prints the Temp voltages to the serial port.

 Menu Entry 7: Start cell voltage measurement loop
    The command will continuously measure the LTC6803 cell voltages and print the results to the serial port.
    The loop can be exited by sending the MCU a 'm' character over the serial link.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0)
 binary  : B10000000000
 float   : 1024.0
@endverbatim

http://www.linear.com/product/LTC6803-1

http://www.linear.com/product/LTC6803-1#demoboards


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

Copyright 2015 Linear Technology Corp. (LTC)
 */


/*! @file
    @ingroup LTC68031
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LTC68031.h"
#include <SPI.h>

const uint8_t TOTAL_IC = 1;//!<number of ICs in the daisy chain

/******************************************************
 *** Global Battery Variables received from 6803 commands
 These variables store the results from the LTC6803
 register reads and the array lengths must be based
 on the number of ICs on the stack
 ******************************************************/
uint16_t cell_codes[TOTAL_IC][12];
/*!<
  The cell codes will be stored in the cell_codes[][12] array in the following format:

  |  cell_codes[0][0]| cell_codes[0][1] |  cell_codes[0][2]|    .....     |  cell_codes[0][11]|  cell_codes[1][0] | cell_codes[1][1]|  .....   |
  |------------------|------------------|------------------|--------------|-------------------|-------------------|-----------------|----------|
  |IC1 Cell 1        |IC1 Cell 2        |IC1 Cell 3        |    .....     |  IC1 Cell 12      |IC2 Cell 1         |IC2 Cell 2       | .....    |
****/

uint16_t temp_codes[TOTAL_IC][3];
/*!<
 The Temp codes will be stored in the temp_codes[][3] array in the following format:

 |  temp_codes[0][0]| temp_codes[0][1]|temp_codes[0][2] | temp_codes[1][0]| temp_codes[1][1]|   .....   |
 |------------------|-----------------|-----------------|-----------------|-----------------|-----------|
 |IC1 Temp1         |IC1 Temp2        |IC1 ITemp        |IC2 Temp1        |IC2 Temp2        |  .....    |
*/

uint8_t tx_cfg[TOTAL_IC][6];
/*!<
  The tx_cfg[][6] stores the LTC6803 configuration data that is going to be written
  to the LTC6803 ICs on the daisy chain. The LTC6803 configuration data that will be
  written should be stored in blocks of 6 bytes. The array should have the following format:

 |  tx_cfg[0][0]| tx_cfg[0][1] |  tx_cfg[0][2]|  tx_cfg[0][3]|  tx_cfg[0][4]|  tx_cfg[0][5]| tx_cfg[1][0] |  tx_cfg[1][1]|  tx_cfg[1][2]|  .....    |
 |--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|-----------|
 |IC1 CFGR0     |IC1 CFGR1     |IC1 CFGR2     |IC1 CFGR3     |IC1 CFGR4     |IC1 CFGR5     |IC2 CFGR0     |IC2 CFGR1     | IC2 CFGR2    |  .....    |

*/

uint8_t rx_cfg[TOTAL_IC][7];
/*!<
  the rx_cfg[][8] array stores the data that is read back from a LTC6803-1 daisy chain.
  The configuration data for each IC  is stored in blocks of 7 bytes. Below is an table illustrating the array organization:

|rx_config[0][0]|rx_config[0][1]|rx_config[0][2]|rx_config[0][3]|rx_config[0][4]|rx_config[0][5]|rx_config[0][6]  |rx_config[1][0]|rx_config[1][1]|  .....    |
|---------------|---------------|---------------|---------------|---------------|---------------|-----------------|---------------|---------------|----------|
|IC1 CFGR0      |IC1 CFGR1      |IC1 CFGR2      |IC1 CFGR3      |IC1 CFGR4      |IC1 CFGR5      |IC1 PEC          |IC2 CFGR0      |IC2 CFGR1      |  .....    |
*/


//! Inititializes hardware and variables
void setup()
{
  Serial.begin(115200);
  LTC6803_initialize();  //Initialize LTC6803 hardware
  init_cfg();        //initialize the 6803 configuration array to be written
  print_menu();
}


//!main loop
void loop()
{

  if (Serial.available())           // Check for user input
  {
    uint32_t user_command;
    user_command = read_int();      // Read the user command
    Serial.println(user_command);
    run_command(user_command);
  }
}


/*!*****************************************
  \brief executes the user inputted command

  Menu Entry 1: Write Configuration \n
   Writes the configuration register of the LTC6803. This command can be used to turn on the reference
   and increase the speed of the ADC conversions.

 Menu Entry 2: Read Configuration \n
   Reads the configuration register of the LTC6803, the read configuration can differ from the written configuration.
   The GPIO pins will reflect the state of the pin

 Menu Entry 3: Start Cell voltage conversion \n
   Starts a LTC6803 cell channel adc conversion.

 Menu Entry 4: Read cell voltages \n
    Reads the LTC6803 cell voltage registers and prints the results to the serial port.

 Menu Entry 5: Start Temp voltage conversion \n
    Starts a LTC6803 Temp channel adc conversion.

 Menu Entry 6: Read Temp voltages \n
    Reads the LTC6803 axiliary registers and prints the GPIO voltages to the serial port.

 Menu Entry 7: Start cell voltage measurement loop \n
    The command will continuously measure the LTC6803 cell voltages and print the results to the serial port.
    The loop can be exited by sending the MCU a 'm' character over the serial link.

*******************************************/
void run_command(uint32_t cmd)
{
  int8_t error = 0;

  char input = 0;
  switch (cmd)
  {

    case 1:

      LTC6803_wrcfg(TOTAL_IC,tx_cfg);
      print_config();
      break;

    case 2:

      error = LTC6803_rdcfg(TOTAL_IC,rx_cfg);
      if (error == -1)
      {
        Serial.println(F("A PEC error was detected in the received data"));
      }
      print_rxconfig();
      break;

    case 3:

      LTC6803_stcvad();
      delay(3);
      Serial.println(F("cell conversion completed"));
      Serial.println();
      break;

    case 4:

      error = LTC6803_rdcv(TOTAL_IC,cell_codes); // Set to read back all cell voltage registers
      if (error == -1)
      {
        Serial.println(F("A PEC error was detected in the received data"));
      }
      print_cells();
      break;

    case 5:

      LTC6803_sttmpad();
      delay(3);
      Serial.println(F("aux conversion completed"));
      Serial.println();
      break;

    case 6:

      error = LTC6803_rdtmp(TOTAL_IC,temp_codes); // Set to read back all aux registers
      if (error == -1)
      {
        Serial.println(F("A PEC error was detected in the received data"));
      }
      print_temp();
      break;

    case 7:
      Serial.println(F("transmit 'm' to quit"));

      LTC6803_wrcfg(TOTAL_IC,tx_cfg);
      while (input != 'm')
      {
        if (Serial.available() > 0)
        {
          input = read_char();
        }


        LTC6803_stcvad();
        delay(10);

        error = LTC6803_rdcv(TOTAL_IC,cell_codes);
        if (error == -1)
        {
          Serial.println(F("A PEC error was detected in the received data"));
        }
        print_cells();


        if (error == -1)
        {
          Serial.println(F("A PEC error was detected in the received data"));
        }
        // print_rxconfig();

        delay(500);
      }
      print_menu();
      break;

    default:
      Serial.println(F("Incorrect Option"));
      break;
  }
}


//! Initializes the configuration array
void init_cfg()
{
  for (int i = 0; i<TOTAL_IC; i++)
  {
    tx_cfg[i][0] = 0xF1;
    tx_cfg[i][1] = 0x00 ;
    tx_cfg[i][2] = 0x00 ;
    tx_cfg[i][3] = 0x00 ;
    tx_cfg[i][4] = 0x00 ;
    tx_cfg[i][5] = 0x00 ;
  }

}


//! Prints the main menu
void print_menu()
{
  Serial.println(F("Please enter LTC6803 Command"));
  Serial.println(F("Write Configuration: 1"));
  Serial.println(F("Read Configuration: 2"));
  Serial.println(F("Start Cell Voltage Conversion: 3"));
  Serial.println(F("Read Cell Voltages: 4"));
  Serial.println(F("Start Aux Voltage Conversion: 5"));
  Serial.println(F("Read Aux Voltages: 6"));
  Serial.println(F("loop cell voltages: 7"));
  Serial.println(F("Please enter command: "));
  Serial.println();
}




//!Prints cell coltage codes to the serial port
void print_cells()
{


  for (int current_ic = 0 ; current_ic < TOTAL_IC; current_ic++)
  {
    Serial.print(" IC ");
    Serial.print(current_ic+1,DEC);
    for (int i=0; i<12; i++)
    {
      Serial.print(" C");
      Serial.print(i+1,DEC);
      Serial.print(":");
      Serial.print(cell_codes[current_ic][i]*0.0015,4);
      Serial.print(",");
    }
    Serial.println();
  }
  Serial.println();
}


//!Prints GPIO voltage codes and Vref2 voltage code onto the serial port
void print_temp()
{

  for (int current_ic =0 ; current_ic < TOTAL_IC; current_ic++)
  {
    Serial.print(" IC ");
    Serial.print(current_ic+1,DEC);
    for (int i=0; i < 2; i++)
    {
      Serial.print(" Temp-");
      Serial.print(i+1,DEC);
      Serial.print(":");
      Serial.print(temp_codes[current_ic][i]*0.0015,4);
      Serial.print(",");
    }
    Serial.print(" ITemp");
    Serial.print(":");
    Serial.print((temp_codes[current_ic][2]*0.1875)-274.15,4);
    Serial.println();
  }
  Serial.println();
}

//!Prints the configuration data that is going to be written to the LTC6803 to the serial port.
void print_config()
{
  int cfg_pec;

  Serial.println("Written Configuration: ");
  for (int current_ic = 0; current_ic<TOTAL_IC; current_ic++)
  {
    Serial.print(" IC ");
    Serial.print(current_ic+1,DEC);
    Serial.print(": ");
    Serial.print("0x");
    serial_print_hex(tx_cfg[current_ic][0]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][1]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][2]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][3]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][4]);
    Serial.print(", 0x");
    serial_print_hex(tx_cfg[current_ic][5]);
    Serial.print(", Calculated PEC: 0x");
    cfg_pec = pec8_calc(6,&tx_cfg[current_ic][0]);
    serial_print_hex((uint8_t)(cfg_pec>>8));
    Serial.print(", 0x");
    serial_print_hex((uint8_t)(cfg_pec));
    Serial.println();
  }
  Serial.println();
}


//! Prints the configuration data that was read back from the LTC6803 to the serial port.
void print_rxconfig()
{
  Serial.println("Received Configuration ");
  for (int current_ic=0; current_ic<TOTAL_IC; current_ic++)
  {
    Serial.print(" IC ");
    Serial.print(current_ic+1,DEC);
    Serial.print(": 0x");
    serial_print_hex(rx_cfg[current_ic][0]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][1]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][2]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][3]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][4]);
    Serial.print(", 0x");
    serial_print_hex(rx_cfg[current_ic][5]);
    Serial.print(", Received PEC: 0x");
    serial_print_hex(rx_cfg[current_ic][6]);

    Serial.println();
  }
  Serial.println();
}

//! Prints Data in a Hex Format
void serial_print_hex(uint8_t data)
{
  if (data < 16)
  {
    Serial.print("0");
    Serial.print((byte)data,HEX);
  }
  else
    Serial.print((byte)data,HEX);
}
