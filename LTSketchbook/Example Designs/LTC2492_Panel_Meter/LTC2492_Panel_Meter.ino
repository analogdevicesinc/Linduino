/*!
Two-Channel Panel Meter based on the LTC2492, using any 2x16 LCD display
compatible with the Arduino LiquidCrystal library.

(The LTC2492 is a code-compatible subset of the LTC2498.)

Initial scaling factors are for the DC2132A, Constant Voltage, Constant Current
Bench Supply, which uses two LT3018 regulators.

LTC2492 mapping:
CH0 - voltage measurement divider, define vdiv_gain accordingly
CH1 - current monitor output
CH2 - LT3080 junction temperature monitor output
(Ambient temperature read from LTC2498 internal sensor)

@verbatim

NOTES:

LCD pin mapping is designed to be "minimally obtrusive", not interfering
with an Uno's SPI, I2C or UART. Since we're using an external,
high-performance ADC, the "analog" pins can be re-used as digital outputs.

LiquidCrystal lcd(2, 3, 4, A0, A1, A2, A3); // RS, RW, EN, D4:D7.

For reference, the standard character LCD pinout is as follows:
1 GROUND
2 VDD (usually +5V)
3 Contrast
4 RS (register select)
5 RW (High = READ, low = WRITE)
6 EN (Enable)
7 D0 (ground in 4-bit mode)
8 D1 (ground in 4-bit mode)
9 D2 (ground in 4-bit mode)
10 D3 (ground in 4-bit mode)
11 D4
12 D5
13 D6
14 D7
15 - Pins 15, 16 are often used for backlight.
16 - Check datasheet for voltage / polarity.



  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. Equipment
   required is a precision voltage source and a precision voltmeter. Additionally,
   an external power supply is required to provide a negative voltage for Amp V-.
   Set it to anywhere from -1V to -5V. Set Amp V+ to Vcc. Ensure the COM and REF-
   pins are connected to ground. The REF+ pin should be connected to +5V.

  How to test Single-Ended mode:
   The voltage source should be connected to the ADC such that the negative lead is
   connected to the COM(common) pin. The positive lead may be connected to any
   channel input. Ensure voltage is within analog input voltage range -0.3 to 2.5V.

  How to test Differential Mode:
   The voltage source should be connected with positive and negative leads to paired
   channels. The voltage source negative output must also be connected to the COM
   pin in order to provide a ground-referenced voltage. Ensure voltage is within
   analog input voltage range -0.3V to +2.5V. Swapping input voltages results in a
   reversed polarity reading.


@endverbatim

http://www.linear.com/product/LTC2498

http://www.linear.com/product/LTC2498#demoboards


The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.



Copyright 2011(c) Analog Devices, Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
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

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED.

IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, INTELLECTUAL PROPERTY
RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <Arduino.h>
#include <LiquidCrystal.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include <SPI.h>
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC24XX_general.h"
#include "LTC2498.h"

// Function Declaration

// Global variables
static uint8_t demo_board_connected;                //!< Set to 1 if the board is connected
static uint8_t two_x_mode = LTC2498_SPEED_1X;       //!< The LTC2498 2X Mode settings
static uint8_t rejection_mode = LTC2498_R50_R60;    //!< The LTC2498 rejection mode settings
static float LTC2498_vref = 5.0;                    //!< The LTC2498 ideal reference voltage
static uint16_t eoc_timeout = 250;                  //!< timeout in ms
// Constants
#define vdiv_gain 10.75 // Measured from actual supply
#define imon_gain 10.0
#define tmon_gain 100.0


//! Lookup table to build the command for single-ended mode
const uint8_t BUILD_COMMAND_SINGLE_ENDED[16] = {LTC2498_CH0, LTC2498_CH1, LTC2498_CH2, LTC2498_CH3,
    LTC2498_CH4, LTC2498_CH5, LTC2498_CH6, LTC2498_CH7,
    LTC2498_CH8, LTC2498_CH9, LTC2498_CH10, LTC2498_CH11,
    LTC2498_CH12, LTC2498_CH13, LTC2498_CH14, LTC2498_CH15
                                               };    //!< Builds the command for single-ended mode

//! Lookup table to build the command for differential mode
const uint8_t BUILD_COMMAND_DIFF[16] = {LTC2498_P0_N1, LTC2498_P2_N3, LTC2498_P4_N5, LTC2498_P6_N7,
                                        LTC2498_P8_N9, LTC2498_P10_N11, LTC2498_P12_N13, LTC2498_P14_N15,
                                        LTC2498_P1_N0, LTC2498_P3_N2, LTC2498_P5_N4, LTC2498_P7_N6,
                                        LTC2498_P9_N8, LTC2498_P11_N10, LTC2498_P13_N12, LTC2498_P15_N14
                                       };      //!< Build the command for differential mode

//! Lookup table to build 1X / 2X bits
const uint8_t BUILD_1X_2X_COMMAND[2] = {LTC2498_SPEED_1X, LTC2498_SPEED_2X};   //!< Build the command for 1x or 2x mode

LiquidCrystal lcd(2, 3, 4, A0, A1, A2, A3); // RS, RW, EN, D4:D7. This pinout does not
// interfere with any Linduino functionality.

//! Initialize Linduino
void setup()
{

  lcd.begin(16,2);
  lcd.setCursor(0, 0);
  lcd.print("Hello, DC2132");
  lcd.setCursor(0, 1);
  lcd.print("Power Supply! :)");

  delay(2000);
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
  char demo_name[]="DC1011";    // Demo Board Name stored in QuikEval EEPROM
  quikeval_SPI_init();          // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();       // Connect SPI to main data port
  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100kHz
  Serial.begin(115200);         // Initialize the serial port to the PC

  demo_board_connected = discover_demo_board(demo_name);
  // if (demo_board_connected)
  // {
  // print_prompt();
  // }
  // else
  // {
  // Serial.println(F("EEPROM not detected, will attempt to proceed"));
  // demo_board_connected = 1;
  // print_prompt();
  // }
  quikeval_SPI_connect();       //Initialize for SPI
}

//! Repeats Linduino loop
void loop()
{
  uint8_t adc_command_high; // The LTC2498 command high byte
  uint8_t adc_command_low;  // The LTC2498 command low byte
  int16_t user_command;                 // The user input command
  uint8_t ack_EOC = 0;                  // Keeps track of the EOC timeout
  int32_t adc_code = 0;            // The LTC2498 code
  float temperature, tj, adc_voltage, vout_voltage, iout_current;

// Program channel 1, read channel 0
  adc_command_high = BUILD_COMMAND_SINGLE_ENDED[1];     // Build ADC command for channel 0
  adc_command_low = rejection_mode | two_x_mode;
  LTC2498_EOC_timeout(LTC2498_CS, eoc_timeout);
  LTC2498_read(LTC2498_CS, adc_command_high, adc_command_low, &adc_code);   // Throws out last reading
  adc_voltage = LTC2498_code_to_voltage(adc_code, LTC2498_vref);
  vout_voltage = adc_voltage * vdiv_gain;
  lcd.setCursor(0, 0);
  lcd.print("        ");
  lcd.setCursor(0, 0);
  lcd.print("V=");
  lcd.print(vout_voltage, 2);
// Program temperature, read channel 1
  adc_command_high = BUILD_COMMAND_SINGLE_ENDED[0]; // Any channel can be selected
  adc_command_low = LTC2498_INTERNAL_TEMP | rejection_mode;
  LTC2498_EOC_timeout(LTC2498_CS, eoc_timeout);
  LTC2498_read(LTC2498_CS, adc_command_high, adc_command_low, &adc_code);   // Throws out last reading
  adc_voltage = LTC2498_code_to_voltage(adc_code, LTC2498_vref);
  iout_current = adc_voltage * imon_gain;
  lcd.setCursor(8, 0);
  lcd.print("        ");
  lcd.setCursor(8, 0);
  lcd.print("I=");
  lcd.print(iout_current, 2);


// Program channel 2, read temperature


  adc_command_high = BUILD_COMMAND_SINGLE_ENDED[2]; // Any channel can be selected
  adc_command_low = rejection_mode | two_x_mode;
  LTC2498_EOC_timeout(LTC2498_CS, eoc_timeout);
  LTC2498_read(LTC2498_CS, adc_command_high, adc_command_low, &adc_code);   // Throws out last reading
  adc_voltage = LTC2498_code_to_voltage(adc_code, LTC2498_vref);
  temperature = 27.0 + ((adc_voltage-0.028) / 93.5e-6);
  lcd.setCursor(0, 1);
  lcd.print("        ");
  lcd.setCursor(0, 1);
  lcd.print("Ta=");
  lcd.print(temperature, 1);

// Program channel 0, read channel 2
  adc_command_high = BUILD_COMMAND_SINGLE_ENDED[0]; // Any channel can be selected
  adc_command_low = rejection_mode | two_x_mode;
  LTC2498_EOC_timeout(LTC2498_CS, eoc_timeout);
  LTC2498_read(LTC2498_CS, adc_command_high, adc_command_low, &adc_code);   // Throws out last reading
  adc_voltage = LTC2498_code_to_voltage(adc_code, LTC2498_vref);
  tj = (adc_voltage * tmon_gain);
  lcd.setCursor(8, 1);
  lcd.print("        ");
  lcd.setCursor(8, 1);
  lcd.print("Tj=");
  lcd.print(tj, 1);



}

