


/*!
  Linear Technology DC393B Demonstration Board.
  DC393B: Digitaly Progaramable Lowpass Filter, 10kHz to 150kHz - LTC1564

  @verbatim
  393B
  NOTES
  Setup:
    Set the terminal baud rate to 115200 and select the newline terminator.
    Equipment required is a precision voltage source and a precision
    voltmeter. No external power supply is required. Ensure all jumpers on
    the demo board are installed in their default positions from the
    factory.

  USER INPUT DATA FORMAT:
  decimal : 1024
  hex     : 0x400
  octal   : 02000  (leading 0 "zero")
  binary  : B10000000000
  float   : 1024.0

  @endverbatim

  http://www.analog.com/en/products/amplifiers/adc-drivers/fully-differential-amplifiers/ltc6603.html

  http://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/dc1304a-b.html



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

  The views and conclusions contained in the software and documentation are those
  of the authors and should not be interpreted as representing official policies,
  either expressed or implied, of Linear Technology Corp.

  The Linear Technology Linduino is not affiliated with the official Arduino team.
  However, the Linduino is only possible because of the Arduino team's commitment
  to the open-source community.  Please, visit http://www.arduino.cc and
  http://store.arduino.cc , and consider a purchase that will help fund their
  ongoing work.
*/

/*! @file
    @ingroup DC393B
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LT_I2C.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include <SPI.h>
#include <Wire.h>


// Based on J8 of DC2026 mating up with J3 of DC393B.
//
// F0   {DC2026.J8.1, oops}    DC393B.J3.1     Arduinio pin 6, need to rework the board
// F1   {DC2026.J8.2, oops}    DC393B.J3.2     Arduinio pin 7, need to rework the board
// F2   DC2026.J8.3    DC393B.J3.3     Arduinio pin 2
// F3   DC2026.J8.4    DC393B.J3.4     Arduinio pin 3
// CS   DC2026.J8.5    DC393B.J3.5     Arduinio pin 4
// EN   DC2026.J8.6    DC393B.J3.6     Arduinio pin 5



#define DC393B_ENB 7     // Enable active low             works
#define DC393B_CS  6     // Chip select active low ; hold active high      works
#define DC393B_F0  2     // Parallel frequency bit 0    works
#define DC393B_F1  3     // Parallel frequency bit 1    works
#define DC393B_F2  4     // Parallel frequency bit 2   works
#define DC393B_F3  5     // Parallel frequency bit 3   works
#define DC393B_G0  8     // Parallel gain bit 0        works
#define DC393B_G1  9     // Parallel gain bit 1        works 
#define DC393B_G2  10    // Parallel gain bit 2        works
#define DC393B_G3  11    // Parallel gain bit 3        works
#define DC393B_RST 12    // Reset active low           works
#define DC393B_AREFEN 13 // Enable ADC Reference       works

#define DC393B_A0 A0     // Analog Input







// Function Declaration
void print_title();                         // Print the title block
void print_prompt();                        // Prompt the user for an input command
void print_user_command(uint8_t menu);      // Display selected differential channels
void menu_1_filter_settings();              // Sets the gain and cutoff frequencies
void menu_2_set_gpo();                      // Sets the GPO
void menu_3_shutdown();                     // Shuts down the LTC6603
void menu_4_poweron();                      // Power up the LTC6603

// Global variables
uint8_t spi_out;
uint8_t control_byte;
uint8_t enWord;
uint8_t refenWord;
uint8_t csWord;
uint8_t rstWord;
uint8_t frequencyWord;
uint8_t gainWord;



//! Initialize Linduino
void setup()
{
  //quikeval_SPI_init();  // Configure the spi port for 4MHz SCK
  Serial.begin(115200); // Initialize the serial port to the PC
  print_title();        // Displays the title
  
         // Display user options
  control_byte = 0x00;  // A global variable that contains the whole control byte.


  pinMode (DC393B_ENB, OUTPUT); // Digital output
  pinMode (DC393B_CS, OUTPUT); // Digital output
  pinMode (DC393B_F0, OUTPUT); // Digital output
  pinMode (DC393B_F1, OUTPUT); // Digital output
  pinMode (DC393B_F2, OUTPUT); // Digital output
  pinMode (DC393B_F3, OUTPUT); // Digital output
  pinMode (DC393B_G0, OUTPUT); // Digital output
  pinMode (DC393B_G1, OUTPUT); // Digital output
  pinMode (DC393B_G2, OUTPUT); // Digital output
  pinMode (DC393B_G3, OUTPUT); // Digital output
  pinMode (DC393B_RST, OUTPUT); // Digital output
  pinMode (DC393B_AREFEN, OUTPUT); // Digital output
  pinMode (DC393B_A0, INPUT);  // Analog input: voltage sense



  digitalWrite (DC393B_ENB, LOW); // Digital output
  digitalWrite (DC393B_CS, LOW); // Digital output
  digitalWrite (DC393B_F0, LOW); // Digital output
  digitalWrite (DC393B_F1, LOW); // Digital output
  digitalWrite (DC393B_F2, LOW); // Digital output
  digitalWrite (DC393B_F3, LOW); // Digital output
  digitalWrite (DC393B_G0, LOW); // Digital output
  digitalWrite (DC393B_G1, LOW); // Digital output
  digitalWrite (DC393B_G2, LOW); // Digital output
  digitalWrite (DC393B_G3, LOW); // Digital output
  digitalWrite (DC393B_RST, LOW); // Digital output
  digitalWrite (DC393B_AREFEN, LOW); // Digital output

  
  enWord = 0;
  
  analogReference(EXTERNAL);
  refenWord = 1;
  for (int i = 0; i <=3; i++) {
  analogRead(DC393B_A0);
  }
  
  csWord = 0;
  rstWord = 1;
  frequencyWord = 1;
  gainWord = 0;
  DC393B_UpdateParallelOutputs();
  print_status();
  print_prompt();

}

//! Repeats Linduino loop
void loop()
{
  int16_t user_command;         // The user input command

  //DC393B_UpdateParallelOutputs();

  if (Serial.available())       // Check for user input
  {
    user_command = read_int();        // Read the user command
    if (user_command != 'm')
      Serial.println(user_command);   // Prints the user command to com port

    switch (user_command)
    {
      case 1:
        menu_1_enable();
        break;
      case 2:
        menu_2_refenable();
        break;
      case 3:
        menu_3_frequencyChange();
        break;
      case 4:
        menu_4_gainChange();
        break;
      case 5:
        menu_5_cstoggle();
        break;
      case 6:
        menu_6_rsttoggle();
        break;
      default:
        Serial.println();
        break;
    }
    DC393B_UpdateParallelOutputs();
    print_status();
    print_prompt();
    
    
  }
}


//! Set enable/disable
//! @return void
void menu_1_enable()
{
  uint16_t user_command;
  Serial.println (F("Enter a Command: 0-Enable Filter, 1-Disable Filter, 2-Go back to Main Menu"));
  Serial.println();
  Serial.println(F("Note about Enable/Disable:"));
  Serial.println(F("When the LTC1564 is disabled (Logic 1), it is in a low power state and will not respond to any signals other than an Enable_Bar (Logic 0). "));
  Serial.println();
  Serial.println(F("When the LTC1564 is enabled (Logic 0), it is powered up and will respond to an analog or digital input. "));
  user_command = read_int();
  switch (user_command)
  {
    case 0:
      enWord = 0;
      break;
    case 1:
      enWord = 1;
      break;
    case 2:
      break;
    default:
      Serial.println (F("incorrect option"));
      break;
  }

}

//! Set reference enable/disable
//! @return void
void menu_2_refenable()
{
  uint16_t user_command;
  Serial.println (F("Enter a Command: 0-Disable Peak Detector, 1-Enable and Read Peak Detector, 2-Go Back to Main Menu"));
 
  user_command = read_int();
  switch (user_command)
  {
    case 0:
      refenWord = 0;
      analogReference(DEFAULT);
      break;
    case 1:
      analogReference(EXTERNAL);
      refenWord = 1;
      DC393B_UpdateParallelOutputs();
      uint16_t readVint;
      for (int i = 0; i <=20; i++) {
      readVint=analogRead(DC393B_A0);
      delay(20);
      }
      
      
      float readVpp;
      
      readVpp = ConvertToVolts(readVint);
      // print the ADC result
     Serial.println();
     Serial.print(readVpp, 4);
     //Serial.println(readVint);
     Serial.print(" Vpp");
     Serial.println();
     if (readVpp<1 or readVpp>9.8){
    Serial.println("Vpp peak detector error is greater than 10%");
  }
     
     Serial.println();
      break;

    case 2:
      break;
    default:
      Serial.println (F("incorrect option"));
      break;
  }
}


//! Set frequency
//! @return void
void menu_3_frequencyChange()
{
  uint16_t user_command;
  Serial.println(F("Enter Command: 1-Set Frequency(10kHz-150kHz), 2-Set Mute State, 3-Go Back to Main Menu"));
  Serial.println(F("Note: Mute State is Gain=0 (input signal is disconnected from filter output)."));
  Serial.println();
  user_command = read_int();
  switch(user_command){
    case 1:
  Serial.println (F("Enter frequency in kHz in 10k steps (ex: 10..20, etc.). Range 10 through 150."));
  user_command = read_int();
  if (user_command < 10 || user_command > 150)
    Serial.println(F("  Input should be from 10 to 150."));
   
  else
  {
    frequencyWord = user_command / 10;
    //Serial.print ("chosen frequency code is ");
    //Serial.println (frequencyWord );
  }
   break;
   
    case 2: 
    frequencyWord=0;
    //Serial.println("you selected option two");
    break;
    
    case 3:
    break;

    default:
    Serial.println (F("incorrect option"));
    break;

    
  }
}

//! Set gain
//! @return void
void menu_4_gainChange()
{
  uint16_t user_command;
  Serial.println(F("Enter Command: 1-Enter Gain, 2-Go Back to Main Menu"));
  user_command = read_int();
  switch(user_command){
    
  case 1:
  Serial.println (F("Enter gain in V/V.  Range 1 through 16."));
  
  user_command = read_int();
  if (user_command < 1 || user_command > 16)
    Serial.println(F("  Input should be from 1 to 16."));
  else
  {
    gainWord = user_command - 1;
    //Serial.print ("chosen gain code is ");
    //Serial.println (gainWord );
  }
  break;

  case 2:
  break;

  default:
  Serial.println (F("incorrect option"));
   break;
  }
}





//! set CS low / high
//! @return void
void menu_5_cstoggle()
{
  uint16_t user_command;
  Serial.println (F("Enter a Command: CS pin: 0-Default, 1-Hold, 2-Go Back to Main Menu"));
  Serial.println();
  Serial.println(F("Note about CS/Hold:"));
  Serial.println(F("A Hold (Logic 1) latches in the LTC1564 the last frequency and gain code (F0-F3 & G0-G3) before")); 
  Serial.println(F("CS was changed to 1. After CS is set to 1, filter will not respond to a new frequency or gain setting. ")); 
  Serial.println();
  Serial.println(F("A CS_Bar (Logic 0) allows the LTC1564 to respond to a frequency or gain setting on the F0-F3 and G0-G3 pins.")); 
    
     
  user_command = read_int();
  switch (user_command)
  {
    case 0:
      csWord = 0;
      break;
    case 1:
      csWord=1;
      break;
    case 2: 
       break;
    default:
      Serial.println (F("incorrect option"));
      break;
  }
}


//! set RST low / high
//! @return void
void menu_6_rsttoggle()
{
  uint16_t user_command;
  Serial.println (F("Enter 0 to set the LTC1564 to fc=10kHz and Gain=1(0dB)."));
  Serial.println(F("Enter 2 to go back to Main Menu."));
  user_command = read_int();
  switch (user_command)
  {
    case 0:
      rstWord = 0;
      delay(1);
      rstWord=1;
      gainWord=0;//because gainWord=userWord-1, sets gain to 1
      frequencyWord=1;//set default to 10kHz, because frequencyWord=userWord/10
      break;
    case 2:
      break;
    default:
      Serial.println (F("incorrect option"));
      break;
  }
}




//! Show analog input
//! @return void
/*void menu_7_showAnalogInput()
{

  uint16_t readVint;
  float readAnalog;

  readVint = analogRead(DC393B_A0);
  readAnalog = ConvertToVolts(readVint);
  // print the ADC result
  Serial.println();
  Serial.print(readAnalog, 4);
  Serial.println();

}*/










//! Prints the title block when program first starts.
//! @return void
void print_title()
{
  (F("\n*****************************************************************\n"));
  Serial.print(F("* DC393BA Demonstration Program                               *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to exercise the DC393B.    *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}


//! Prints main menu.
//! @return void
void print_prompt()
{
  //Serial.print(F("*************************\n\n"));
  Serial.println (F("1-LPF Shutdown Enable/Disable"));
  Serial.println (F("2-Peak Detection Enable/Disable"));
  Serial.println (F("3-Frequency Control and Mute State(Gain=0)"));
  Serial.println (F("4-Gain Control"));
  Serial.println (F("5-Hold Last Setting(CS_bar=1)"));
  Serial.println (F("6-Reset Pulse (default to F=10k,G=1)"));
  //Serial.print   (F("m-Main Menu\n"));
  Serial.println();
  Serial.println(F("\nEnter a Command in the Serial Monitor: ")); 
  
}

void print_status()
{
  int enable =digitalRead(DC393B_ENB);     // Enable active low             works
  int cshold=digitalRead(DC393B_CS);    // Chip select active low ; hold active high      works
  int f0=digitalRead(DC393B_F0);     // Parallel frequency bit 0    works
  int f1=digitalRead(DC393B_F1);     // Parallel frequency bit 1    works
  int f2= digitalRead(DC393B_F2);// Parallel frequency bit 2   works
  int f3= digitalRead(DC393B_F3);// Parallel frequency bit 3   works
  int g0= digitalRead(DC393B_G0);    // Parallel gain bit 0        works
  int g1= digitalRead(DC393B_G1);   // Parallel gain bit 1        works 
  int g2= digitalRead(DC393B_G2);   // Parallel gain bit 2        works
  int g3= digitalRead(DC393B_G3);    // Parallel gain bit 3        works
  int rst= digitalRead(DC393B_RST);    // Reset active low           works
  int aref=digitalRead(DC393B_AREFEN);
  Serial.print(F("\nCurrent Pin Status:"));
  Serial.print(F("\nEnable Status:"));
  Serial.println(enable);
  Serial.print("CS/Hold Status:");
  Serial.println(cshold);

  Serial.print("Reset Status:");
  Serial.println(rst);
  Serial.print("Aref Status:");
  Serial.println(aref);
  
  Serial.print("F3");
  Serial.print("  F2");
  Serial.print("  F1");
  Serial.print("  F0");
  Serial.print("  G3");
  Serial.print("  G2");
  Serial.print("  G1");
  Serial.println("  G0");
  
  Serial.print(f3);
  Serial.print("   ");
  Serial.print(f2);
  Serial.print("   ");
  Serial.print(f1);
  Serial.print("   ");
  Serial.print(f0);
  Serial.print("   ");
  Serial.print(g3);
  Serial.print("   ");
  Serial.print(g2);
  Serial.print("   ");
  Serial.print(g1);
  Serial.print("   ");
  Serial.println(g0);
  Serial.println("-------------------------------------------------------------------------------");
 //

  
}


//! Takes the control fields and outputs to the pins.
//! @return void
void DC393B_UpdateParallelOutputs()
{



  // Enable
  if ((enWord & 0x01) == 0x01)
    digitalWrite(DC393B_ENB, HIGH);
  else
    digitalWrite(DC393B_ENB, LOW);

  // External Reference Enable
  if ((refenWord) & 0x01 == 0x01)
    digitalWrite(DC393B_AREFEN, HIGH);
  else
    digitalWrite(DC393B_AREFEN, LOW);

  // CS
  if ((csWord & 0x01) == 0x01)
    digitalWrite(DC393B_CS, HIGH);
  else
    digitalWrite(DC393B_CS, LOW);

  // RST
  if ((rstWord & 0x01) == 0x01)
    digitalWrite(DC393B_RST, HIGH);
  else
    digitalWrite(DC393B_RST, LOW);


  // Frequency
  if ((frequencyWord & 0x01) == 0x01)
    digitalWrite(DC393B_F0, HIGH);
  else
    digitalWrite(DC393B_F0, LOW);
  if ((frequencyWord & 0x02) == 0x02)
    digitalWrite(DC393B_F1, HIGH);
  else
    digitalWrite(DC393B_F1, LOW);
  if ((frequencyWord & 0x4) == 0x04)
    digitalWrite(DC393B_F2, HIGH);
  else
    digitalWrite(DC393B_F2, LOW);
  if ((frequencyWord & 0x08) == 0x08)
    digitalWrite(DC393B_F3, HIGH);
  else
    digitalWrite(DC393B_F3, LOW);


  // Gain
  if ((gainWord & 0x01) == 0x01)
    digitalWrite(DC393B_G0, HIGH);
  else
    digitalWrite(DC393B_G0, LOW);
  if ((gainWord & 0x02) == 0x02)
    digitalWrite(DC393B_G1, HIGH);
  else
    digitalWrite(DC393B_G1, LOW);
  if ((gainWord & 0x4) == 0x04)
    digitalWrite(DC393B_G2, HIGH);
  else
    digitalWrite(DC393B_G2, LOW);
  if ((gainWord & 0x08) == 0x08)
    digitalWrite(DC393B_G3, HIGH);
  else
    digitalWrite(DC393B_G3, LOW);




}





/*!
  \brief This function converts ADC to Voltage.
*/
float ConvertToVolts(uint16_t val)
{
  float temp;
  float slopecalc;
  float slopeinter;
  
  temp = (float)val;
  //Serial.println(val);
  
  float tempvolt=val+1;
  //Serial.println(tempvolt);
  float middle=tempvolt/1023;
  float dcvolt= middle * 4.098;
  //Serial.println(dcvolt);
  if(temp==0){
  slopecalc=0;
  slopeinter=slopecalc;
  } 
  else{
  slopecalc= dcvolt*(3.2514);
  slopeinter=slopecalc+.53505;
  
  }
  

  return slopeinter;
  
}
