/*
makes a DC934 output 1 millivolt per minute plus 100 millivolts per hour

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.
   No external power supply is required.

@endverbatim

http://www.linear.com/product/LTC2607

http://www.linear.com/product/LTC2607#demoboards

REVISION HISTORY
$Revision: 1509 $
$Date: 2013-04-17 09:28:33 -0700 (Wed, 17 Apr 2013) $

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

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "LT_SPI.h"
#include "LTC2607.h"
#include "LTC2422.h"
#include "QuikEval_EEPROM.h"

// Globals

//! Demo Board Name stored in QuikEval EEPROM
char demo_name[] = "DC934";
//! Look-up table for DAC_A, DAC_B, or both command byte option
const uint8_t address_map[3] = {LTC2607_DAC_A, LTC2607_DAC_B, LTC2607_ALL_DACS};
//! Set to 1 if the board is connected
int8_t demo_board_connected;

uint32_t startMinutes = 10*60 + 24;

float MY_LSB = 7.65841584E-5;
float MY_OFF = 0.0012425743;

//! Initialize Linduino
void setup()
// Setup the Program
{
  output_high(LTC2422_CS);                //! Pulls LTC2442 Chip Select High
  quikeval_I2C_init();                    //! Initializes Linduino I2C port.
  quikeval_SPI_init();                    //! Configures the SPI port for 4MHz SCK
  quikeval_SPI_connect();                 //! Connects SPI to QuikEval port

  Serial.begin(115200);                   //! Initializes the serial port to the PC

  demo_board_connected = discover_demo_board(demo_name); //! Checks if correct demo board is connected.
}

// calculate current time and convert to millivolts for display in voltmeter
void loop()
{
  quikeval_I2C_connect();  //! Connects I2C port to the QuikEval connector

  uint32_t elapsedTime = millis();
  Serial.print("elapsedTime: ");
  Serial.println(elapsedTime);

  // convert milliseconds to minutes
  uint32_t elapsedMinutes = elapsedTime / (1000L*60L);
  // current TOTAL minutes (minutes in the hour plus 60 per hour)
  uint32_t minutes = startMinutes + elapsedMinutes;
  // get the current hour from the minutes, make it go from 1-12 (not 0-11)
  uint32_t hours = 1 + (((minutes / 60) - 1) % 12);
  // now minutes is just minutes in the current hour
  minutes %= 60;

  Serial.print("minutes: ");
  Serial.println(minutes);
  Serial.print("hours: ");
  Serial.println(hours);

  // make the hours in the 1000's and 100's column, minutes in 10's and 1's on a millivolt display
  uint32_t mV = hours * 100L + minutes;

  Serial.print("Outputting ");
  Serial.print(mV, 4);
  Serial.println(" mV on DAC 0");

  //! Write a code to the LTC2607 internal register and update the output voltage. (convert millivolts to volts)
  LTC2607_write(LTC2607_I2C_GLOBAL_ADDRESS, LTC2607_WRITE_UPDATE_COMMAND, address_map[0],
                LTC2607_voltage_to_code((float)mV/1000.0F, MY_LSB, MY_OFF));
}
