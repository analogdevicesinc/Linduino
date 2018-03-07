/*!
Linear Technology DC2197 Demonstration Board.
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
    @ingroup LTC2645
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "math.h"

// Pin Map
const uint8_t DAC_A  = 9;   // 16-bit PWM Pin
const uint8_t DAC_B  = 10;  // 16-bit PWM Pin
const uint8_t DAC_C  = 5;   // 8-bit PWM Pin
const uint8_t DAC_D  = 6;   // 8-bit PWM Pin


// Function Decoration
void print_title();                             // Print the title block
void print_prompt();                            // Prompt the user for an input command
void init_16_bit_PWM();                         // Initializes Pin 9 and 10 for 16-bit PWM
void pwm_16_bit(uint8_t pin, uint16_t duty);    // Sets the 16-bit PWM
void menu_1_select_dac(int16_t *selected_dac);  // Selects DAC
void menu_2_set_duty(int8_t selected_dac);      // Sets the Duty Cycle
uint16_t LTC2645_voltage_to_pwm_code(float dac_voltage, float fullScaleVoltage, int8_t pwmBitResolution);   // Converts voltage to PWM code(DAC Code)
void python_program();                          // Used for the corresponding Python program
int16_t prompt_voltage_or_code();               // Prompts for voltage or code
uint16_t get_voltage(float fullScaleVoltage, int8_t pwmBitResolution);  // Obtains voltage from user
uint16_t get_code();                            // Obtains code from user
void test();

// Global Variable
float fullScale = 2.5;  //! Full Scale Voltage
float percentDutyCycle[4] = {0.0,0.0,0.0,0.0};

//! Used to keep track to print voltage or print code
enum
{
  PROMPT_VOLTAGE = 0, /**< 0 */
  PROMPT_CODE = 1     /**< 1 */
};


//! Initialize Linduino
void setup()
{

  init_16_bit_PWM();      // Initialize the 16-bit PWM
  Serial.begin(115200);   // Initialize the serial port to the PC

  // Goes into Python Mode if 'z' is sent by the Python program
  char python_char = 'x';
  for (int i = 0; i <= 50; i++)
  {
    if (Serial.available() > 0)
      python_char = Serial.read();
    if ((python_char == 'Z') || (python_char == 'z') )
      python_program();
    delay(30);
  }

  // Initialize toggle pin
  pinMode(4,OUTPUT);

  // Read the indicator pin for Reference Voltage
  pinMode(2, INPUT);
  if (digitalRead(2))
    fullScale = 4.096;

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
        menu_1_select_dac(&selected_dac);
        break;
      case 2:
        menu_2_set_duty(selected_dac);
        break;
      case 3:
        test();
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
  Serial.println(F("* to set the LTC2645 DACs.                                      "));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************"));
}


//! Prints main menu.
void print_prompt(int16_t selected_dac)
{
  Serial.println(F("\nCommand Summary:"));
  Serial.println(F("  1-Select DAC"));
  Serial.println(F("  2-Set Duty Cycle"));

  Serial.print(F("  Selected DAC: "));
  Serial.println((char) (selected_dac + 0x41));
  Serial.print(F("  Current Duty Cycle: "));
  Serial.print(percentDutyCycle[selected_dac], 2);
  Serial.println(F("%"));
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

//! Select which DAC to operate on
void menu_1_select_dac(int8_t *selected_dac)
{
  // Select a DAC to operate on
  Serial.print(F("Select DAC to operate on (0=A, 1=B, 2=C, 3=D)"));
  *selected_dac = read_int();
  if (*selected_dac >= 4)
    *selected_dac = 3;
  Serial.println(*selected_dac);
}

//! Sets the duty cycle for the DAC
void menu_2_set_duty(int8_t selected_dac)
{
  int32_t pwm_code;

  if ((selected_dac == 0) || (selected_dac == 1))
  {
    if (prompt_voltage_or_code() == PROMPT_VOLTAGE)
      pwm_code = get_voltage(fullScale, (int8_t)16);  // Converts voltage to 16 bit code
    else
      pwm_code = get_code();
    percentDutyCycle[selected_dac] = (pwm_code + 1)/ 65536.0*100;
    if (selected_dac == 0)
      pwm_16_bit(DAC_A, (uint16_t)pwm_code);
    else
      pwm_16_bit(DAC_B, (uint16_t)pwm_code);

  }
  else
  {
    if (prompt_voltage_or_code() == PROMPT_VOLTAGE)
      pwm_code = get_voltage(fullScale, (int8_t)8);
    else
      pwm_code = get_code();
    percentDutyCycle[selected_dac] = (pwm_code + 1) / 256.0*100;
    if (selected_dac == 2)
      analogWrite(DAC_C, pwm_code);
    else
      analogWrite(DAC_D, pwm_code);

  }

}

//! Used to interact with the corresponding python program.
//! Accepts Serial strings to control the DAC output
//! i.e.: Set DAC A (16 bit pwm) to 50% duty cycle -> 32767A
void python_program()
{
  uint16_t duty;
  char dac;
  Serial.println("Python Mode");
  while (1)
  {
    if (Serial.available() > 0)
    {
      duty = Serial.parseInt();     // Parse string to an integer
      dac = Serial.read();          // Read one byte

      switch (dac)
      {
        case 'a':
        case 'A':
          pwm_16_bit(DAC_A, (uint16_t)duty);
          break;
        case 'b':
        case 'B':
          pwm_16_bit(DAC_B, (uint16_t)duty);
          break;
        case 'c':
        case 'C':
          analogWrite(DAC_C, duty);
          break;
        case 'd':
        case 'D':
          analogWrite(DAC_D, duty);
          break;
      }
    }
  }
}

//! Calculate a LTC2645 duty code given the desired output voltage
//! @return PWM Code
uint16_t LTC2645_voltage_to_pwm_code(float dac_voltage, float fullScaleVoltage, int8_t pwmBitResolution)
{
  float float_code, max_code;
  max_code = pow(2, pwmBitResolution) - 1;                                                      // Calculate max code
  max_code = (max_code > (floor(max_code) + 0.5)) ? ceil(max_code) : floor(max_code);           // Round max code
  float_code = (dac_voltage / fullScaleVoltage) * max_code;                                     // Calculate the DAC code
  float_code = (float_code > (floor(float_code) + 0.5)) ? ceil(float_code) : floor(float_code); // Round
  if (float_code < 0)                                                                           // If DAC code < 0, Then DAC code = 0
    float_code = 0;
  if (float_code > max_code)                                                                    // If DAC code > max code, then DAC code = max code
    float_code = max_code;
  return ((uint16_t)float_code);                                                                // Cast DAC code as uint16_t
}

//! Prompt user to enter a voltage or digital code to send to DAC
//! @return prompt type
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

//! Get voltage from user input, calculate DAC code based
//! on full scale voltage and PWM bit resolution
//! @return voltage
uint16_t get_voltage(float fullScaleVoltage, int8_t pwmBitResolution)
{
  float dac_voltage;

  Serial.print(F("Enter Desired DAC output voltage: "));
  dac_voltage = read_float();
  Serial.print(dac_voltage);
  Serial.println(" V");
  Serial.flush();
  return(LTC2645_voltage_to_pwm_code(dac_voltage, fullScaleVoltage, pwmBitResolution));
}

//! Get code to send to DAC directly, in decimal, hex, or binary
//! @return DAC code
uint16_t get_code()
{
  uint16_t returncode;
  Serial.println(F("\nEnter Desired Duty Code"));
  Serial.println(F("Range: DAC A/B- 0-65535(16-bit PWM)\n       DAC C/D- 0-255(8-bit PWM)"));
  Serial.print(F("(Format 32768, 0x8000, 0100000, or B1000000000000000): "));
  returncode = (uint16_t) read_int();
  Serial.print(F("0x"));
  Serial.println(returncode, HEX);
  Serial.flush();
  return(returncode);
}

void test()
{
//    while(1)
//    {
  // PORTD = PORTD | 0xFF;
  // pwm_16_bit(DAC_B, 32751);
  // delay(10);
  // PORTD = (PORTD | 0xFF) & 0xEF;
  // pwm_16_bit(DAC_B, 32815);
  // delay(10);
  Serial.println(F("Writing 0.5V ladder to DACs..."));
  Serial.println(F("Verify VOUTA between 0.490 and 0.510"));
  Serial.println(F("Verify VOUTB between 0.990 and 1.010"));
  Serial.println(F("Verify VOUTC between 1.490 and 1.510"));
  Serial.println(F("Verify VOUTD between 1.990 and 2.010"));
  pwm_16_bit(DAC_A, 13107);
  pwm_16_bit(DAC_B, 26214);
  analogWrite(DAC_C, 153);
  analogWrite(DAC_D, 204);
//    }
}