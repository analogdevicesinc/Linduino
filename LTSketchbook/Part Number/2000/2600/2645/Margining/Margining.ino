/*!
Code for datasheet application - Margining a power supply with the
LTC2645 Quad 12-/10-/8-Bit PWM to VOUT DACs with 10ppm/C Reference
@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.
   Equipment required is a precision voltage source (null box) and a precision voltmeter (to monitor voltage source).
   No external power supply is required.
   Ensure JP1 is installed in the default position from the factory.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2645

http://www.linear.com/product/LTC2645

REVISION HISTORY
$Revision: 3659 $
$Date: 2015-07-01 10:19:20 -0700 (Wed, 01 Jul 2015) $

Copyright (c) 2013, Linear Technology Corp.(LTC)
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
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/

/*! @file
    @ingroup LTC2645
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "math.h"

// Pin Map
const uint8_t DAC_A  = 9;   //! 16-bit PWM Pin
const uint8_t DAC_B  = 10;  //! 16-bit PWM Pin

// Function Decoration
void print_title();                             // Print the title block
void print_prompt();                            // Prompt the user for an input command
void init_16_bit_PWM();                         // Initializes Pin 9 and 10 for 16-bit PWM
void pwm_16_bit(uint8_t pin, uint16_t duty);    // Sets the 16-bit PWM

//! Initialize Linduino
void setup()
{
  Serial.begin(115200);   // Initialize the serial port to the PC
  init_16_bit_PWM();      // Initialize the 16-bit PWM


  // Initialize toggle pin
  pinMode(DAC_A ,OUTPUT);

  print_title();
  print_prompt(0);
}

//! Repeats Linduino loop
void loop()
{
  int16_t user_command;
  static  int8_t selected_dac = 0;      // The selected DAC to be updated (0=A, 1=B ... 3=D).  Initialized to "A".

  if (Serial.available())               // Check for user input
  {

    user_command = read_int();          // Read the user command
    Serial.println(user_command);
    Serial.flush();

    switch (user_command)
    {
      case 1:
        pwm_16_bit(DAC_A, 0);  // Sets the PWM to no Margin
        break;
      case 2:
        // +5 % Margin
        pwm_16_bit(DAC_A, 10480);  // Sets the PWM to +5 % Margin
        break;
      case 3:
        // -5 % Margin
        pwm_16_bit(DAC_A, 31450);  // Sets the PWM to -5 % Margin
        break;
      case 4:
        // +10 % Margin
        pwm_16_bit(DAC_A, 16);  // Sets the PWM to +10 % Margin
        break;
      case 5:
        // -10 % Margin
        pwm_16_bit(DAC_A, 41933);  // Sets the PWM to -10 % Margin
        break;
      default:
        Serial.println("Incorrect Option");
        break;
    }
    Serial.println("\n*****************************************");
    print_prompt(selected_dac);
  }
}

// Function Definition

//! Prints the title block when program first starts.
void print_title()
{
  Serial.println();
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DCxxxxx Demonstration Program                                 *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to implement a PWM signal       *"));
  Serial.println(F("* to set the LTC2645 DACs to a margining circuit .              *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************"));
}


//! Prints main menu.
void print_prompt(int16_t selected_dac)
{
  Serial.println(F("\nCommand Summary:"));
  Serial.println(F("  1-No Margining/High Impedance"));
  Serial.println(F("  2- +5 % Margin"));
  Serial.println(F("  3- -5 % Margin"));
  Serial.println(F("  4- +10 % Margin"));
  Serial.println(F("  5- -10 % Margin"));

  Serial.print(F("  Selected DAC: "));
  Serial.println((char) (selected_dac + 0x41));
  Serial.print(F("Enter a command:"));
  Serial.flush();
}

//! Initializes Pin 9 and 10 for 16-bit PWM
void init_16_bit_PWM()
{
  pinMode(DAC_A, OUTPUT); // Set Pin 9 as output
  pinMode(DAC_B, OUTPUT); // Set Pin 10 as output


  // Enable timer1 for PWM, Phase Correct mode for pin 9 and 10.
  // Use IRC1 as top, no prescalar (~122Hz),
  // Negative Edge Triggered
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(CS10);
  TCCR1C = 0x0;
  ICR1 = 0xFFFF; // Set Timer for 16-bit

  // The following code should be uncommented if Fast PWM Mode
  // is desired
  /*
  // Enable timer1 for Fast PWM mode for pin 9 and 10.
  // Use IRC1 as top, no prescalar (~244Hz),
  // Negative Edge Triggered
  TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(WGM11);
  TCCR1B = _BV(WGM13)| _BV(WGM12) | _BV(CS10);
  TCCR1C = 0x0;
  ICR1 = 0xFFFF; // Set Timer for 16-bit
  */

  // Set to 0% duty cycle
  OCR1A = 0x0;
  OCR1B = 0x0;
}

//! Sets duty cycle for 16-bit PWM.
void pwm_16_bit(uint8_t pin, uint16_t duty)
{
  if (pin == 9)
    OCR1A = duty;   // Pin 9
  else
    OCR1B = duty;   // Pin 10
}
