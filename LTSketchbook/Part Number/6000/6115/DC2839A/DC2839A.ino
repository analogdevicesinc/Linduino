/*!
Analog Devices DC2839A Demonstration Board.
LTC6115: High Voltage High Side Current and Voltage Sense.

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. Equipment
   required is a voltage source (preferably low-noise) and a precision voltmeter.
   Ensure all jumpers on the demo board are installed in their default positions
   from the factory. Refer to Demo Manual for LTC6115.

  How to test:
   The voltage source should be connected to input AIN+/AIN-. Ensure the
   input is within its specified absolute input voltage range. (It is easiest
   to tie the voltage source negative terminal to COM.) Ensure the voltage
   source is set within the range of 0V to +5.00V.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

https://www.analog.com/en/products/ltc6115.html

Copyright (c) 2018, Analog Devices Inc. (ADI)
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
either expressed or implied, of Analog Devices Inc.

The Analog Devices Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/

/*! @file
    @ingroup LTC6115
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC6115.h"
#include <SPI.h>
#include <Wire.h>


#ifndef LTC6115_GPIO
#define LTC6115_GPIO QUIKEVAL_GPIO  //!< Linduino QuikEval GPIO pin (QuikEval connector pin 14) connects to Arduino pin 9
#endif


// Function Declaration
void print_title();  // Print the title block
void print_prompt(); // Prompt the user for an input command


// Global variables
volatile float start_millis;   //!< Start time when menu 4 function is called
volatile float current_millis; //!< Number of elapsed milliseconds since the program started running
volatile float globaltime_ms;  //!< Number of elapsed milliseconds since menu 4 function is called
volatile int ISR_flag;         //!< Set to 1 when interrupt service routine is running
volatile int samples_counter;  //!< Counts the number of samples taken
volatile int num_samples;      //!< Total number of samples that the user desires
int update_interval;           //!< Sampling interval(ms) to update to
int compare_reg;               //!<  Value to set compare match register to get desired interrupt frequency (Hz)
volatile int print_flag;       //!< Set to 1 to print sampling data for menu option 4
volatile int ISR_prompt_flag;  //!< Set to 1 to print prompt after interrupt service completes
volatile uint32_t readV;       //!< Stores voltage reading
volatile uint32_t readI;       //!< Stores current reading

//! Initialize Linduino
//! @return void
void setup()
{
  char demo_name[]="DC2839A";     // Demo Board Name stored in QuikEval EEPROM
  int8_t demo_board_connected;    // Set to 1 if the board is connected
  
  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100kHz
  Serial.begin(115200);         // Initialize the serial port to the PC
  
  print_title();
  demo_board_connected = discover_demo_board(demo_name);

  if (!demo_board_connected)
  {
    Serial.println(F("EEPROM not detected, will attempt to proceed"));
    demo_board_connected = 1;
  }

  LTC6115_initialize();         // Initialize the 6115 variables
  print_prompt();

  pinMode (LTC6115_VIN, INPUT);   // Analog input: voltage sense
  pinMode (LTC6115_IIN, INPUT);   // Analog input: current sense
  pinMode (LTC6115_GPIO, OUTPUT); // Chip select needs to be an output

  // Initialize global variables
  start_millis = 0;
  current_millis = 0;
  globaltime_ms = 0;
  ISR_flag = 0;
  samples_counter = 0;
  num_samples = 0;
  update_interval = 1000;
  compare_reg = 15624;
  print_flag = 0;
  ISR_prompt_flag = 0;
  readV = 0;
  readI = 0;

  // Initialize timer1

  noInterrupts(); // Disable all interrupts

  // Set timer1 interrupt at 1Hz
  TCCR1A = 0; // Set entire TCCR1A register to 0
  TCCR1B = 0; // Same for TCCR1B
  TCNT1  = 0; // Initialize counter value to 0

  // Set compare match register for 1hz increments
  // OCR1A = compare_reg;
  // compare_reg = (16*10^6) / (1*1024) - 1 (must be <65536)
  update_compare_reg(compare_reg);

  TCCR1B |= (1 << WGM12);               // Turn on CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10);  // Set CS12 and CS10 bits for 1024 prescaler
  TIMSK1 |= (1 << OCIE1A);              // Enable timer compare interrupt

  interrupts(); // Enable all interrupts
}


//! Repeats Linduino loop
//! @return void
void loop()
{
  if (print_flag == 1) // Prints data for menu option 4
  {
    float VinVolts;
    float IinAmps;
    VinVolts = LTC6115ToVolts(readV);
    IinAmps = LTC6115ToAmps(readI);
    Serial.println();
    Serial.print(globaltime_ms);
    Serial.print(", ");
    Serial.print(VinVolts,3);
    Serial.print(", ");
    Serial.print(IinAmps,3);
    print_flag = 0;
  }

  if (ISR_prompt_flag == 1) // Prints prompt after completion of menu 4 function
  {
    Serial.println();
    print_prompt();
    Serial.println();
    ISR_prompt_flag = 0;
  }

  if (ISR_flag == 0) // If Service Routine is not running
  {
    uint16_t user_command;
    if (Serial.available())
    {
      user_command = read_int();      // Read the user command

      Serial.println(user_command);   // Prints the user command to com port
      switch (user_command)
      {
        case 1:
          menu_1_convert_read();
          break;
        case 2:
          menu_2_changeVGain();
          break;
        case 3:
          menu_3_changeIGain();
          break;
        case 4:
          menu_4_changeSampleFreq();
          break;
        case 5:
          menu_5_pipeToFile();
          break;
        case 6:
          menu_6_stopSampling();
          break;
        default:
          Serial.println("Invalid Option");
          break;
      }

      if (ISR_flag == 0)
      {
        print_prompt();
        Serial.println();
      }
    }
  }
  else // If Service Routine is running, stop sampling if user enters 6
  {
    if (Serial.available())
    {
      if (read_int() == 6)
      {
        ISR_flag = 0;
        ISR_prompt_flag = 1;
        samples_counter = num_samples;
      }
    }
  }

}

// Function Definitions

//! Read channel
//! @return void
float menu_1_convert_read()
{
  uint32_t readV;
  uint32_t readI;

  float VinVolts;
  float IinAmps;

  pinMode(LTC6115_VIN, INPUT); // Make sure pins configured as inputs
  pinMode(LTC6115_IIN, INPUT);

  readV = analogRead(LTC6115_VIN);
  readI = analogRead(LTC6115_IIN);

//  // Debug
//  Serial.print("Raw Voltage Code:  ");
//  Serial.println(readV);
//  Serial.print("Raw Current Code:  ");
//  Serial.println(readI);
//  Serial.println();

  VinVolts = LTC6115ConvertToVolts(readV);
  IinAmps = LTC6115ConvertToAmps(readI);

  // Print the ADC result
  Serial.println();
  Serial.print("VIN Input Voltage, Isense Current:  ");
  Serial.print(VinVolts, 4);
  Serial.print("V , ");
  Serial.print(IinAmps, 4);
  Serial.print("A");
  Serial.println();
}


//! @return void
void menu_2_changeVGain()
{
  float user_command;

  Serial.println();
  Serial.print(F("  Enter Voltage Gain (V/V): "));

  user_command = read_float();
  Serial.println(user_command);                // Prints the user command to com port
  if (user_command < 0 || user_command > 199)  // ?????????????????????????????????????????????????????
    Serial.println(F("  Input should be from 0 to 199."));
  else
  {
    LTC6115ChangeVGain(user_command);
    Serial.print(F("  Voltage gain has been set to "));
    Serial.println(user_command);
  }
}


//! Select current channel transimpedance gain (V/A)
//! @return void
void menu_3_changeIGain()
{
  float user_command;

  Serial.println();
  Serial.print(F("  Enter Transimpedance Gain (V/A): "));

  user_command = read_float();
  Serial.println(user_command);                  // Prints the user command to com port
  if (user_command < 0 || user_command > 20000)  // Adjust max allowable gain  ???????????????????????????????????????????/
    Serial.println(F("  Input should be from 0 to 20000."));
  else
  {
    LTC6115ChangeIGain(user_command);
    Serial.print(F("  Transimpedance has been set to "));
    Serial.println(user_command);
  }
}


//! Changes the sampling frequency (ms)
//! @return void
void menu_4_changeSampleFreq()
{
  // Get and store user input for sampling interval
  Serial.print("Enter desired sample interval between 10 and 1000(ms): ");
  update_interval = read_int();
  Serial.println(update_interval);
  Serial.println();

  while (update_interval < 10 || update_interval > 1000) // Prompt user until valid number is entered
  {
    Serial.println("Invalid sampling interval.");
    Serial.print("Enter desired sample interval between 10 and 1000(ms): ");
    update_interval = read_int();
    Serial.println(update_interval);
    Serial.println();
  }

  // Convert milliseconds to Hz
  int frequency;
  frequency = millsToHz(update_interval);

  // Calculate value to set the compare match register
  compare_reg = (int) ((16000000.0/((float)frequency * 1024.0)) - 1.0);
  update_compare_reg(compare_reg);
}

//! Updates the compare match register
//! @return void
void update_compare_reg(uint32_t regVal)
{
  noInterrupts();
  OCR1A = regVal;
  interrupts();
}

//! Pipes voltage and current reading results to a file
//! @return void
void menu_5_pipeToFile()
{
  samples_counter = 0; // Reset the counter to 0
  num_samples = 0;     // Reset the sample size to 0

  // Get and store user input for # of samples
  Serial.println();
  Serial.print("Enter desired number of samples: ");
  num_samples = read_int();
  Serial.println(num_samples);
  Serial.println(); 
  Serial.println();
  Serial.println("Elapsed ms, V, I");

  ISR_flag = 1; //Set flag to 1 to allow interrupts to start service
}


//! Stops the sampling from menu 5
//! @return void
void menu_6_stopSampling()
{
  samples_counter = num_samples;
}

//! Timer Compare Interrupt Service Routine
ISR(TIMER1_COMPA_vect)
{
  if (ISR_flag == 1)
  {
    // Update milliseconds elapsed since menu 5 is called
    if (samples_counter == 0) // Reset time variable to 0 at the start of each sample session
    {
      start_millis = millis();
      current_millis = millis();
      globaltime_ms = current_millis - start_millis;
    }
    else // Calculate elapsed time between current sample and start time
    {
      current_millis = millis();
      globaltime_ms = current_millis - start_millis;
    }


    if (samples_counter < num_samples) // Continue sampling if sampling is incomplete
    {
      readV = analogRead(LTC6115_VIN);
      readI = analogRead(LTC6115_IIN);
      print_flag = 1;
      samples_counter += 1;
    }
    else if (samples_counter >= num_samples && samples_counter != 0) // Sampling is complete, end ISR and print prompt
    {
      ISR_flag = 0;
      ISR_prompt_flag = 1;
    }
  }
}


//! Prints the title block when program first starts
//! @return void
void print_title()
{
  Serial.println();
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* LTC6115                                                       *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to work with the LTC6115.       *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************"));
}


//! Prints main menu
//! @return void
void print_prompt()
{
  Serial.println(F("\n\n"));
  Serial.println(F("*************************"));
  Serial.println(F("1  - Convert and Read V and I (Single Sample)"));
  Serial.print(F("2  - Change V Gain (Current setting: "));
  Serial.print(getVGain());
  Serial.println(F(" V/V)"));
  Serial.print(F("3  - Change I Gain (Current setting: "));
  Serial.print(getIGain());
  Serial.println(F(" V/A, divide by Rsense for V/V)"));
  Serial.print(F("4  - Change Sampling Period (Current setting: "));
  Serial.print(update_interval);
  Serial.println(F("ms)"));
  Serial.println(F("5  - Begin Continuous V and I Sampling Conversion Readings (For Specified # of Samples)"));
  Serial.println(F("6  - Stop Sampling"));
  Serial.print(F("\nEnter a command: "));
}


