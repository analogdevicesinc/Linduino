//! @todo Review this file.
/*
UserInterface.cpp

UserInterface routines are used to read and write user data through the Arduino's
serial interface.

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
#include "UserInterface.h"

char ui_buffer[UI_BUFFER_SIZE];

// Read data from the serial interface into the ui_buffer
uint8_t read_data()
{
  uint8_t index = 0; //index to hold current location in ui_buffer
  int c; // single character used to store incoming keystrokes
  while (index < UI_BUFFER_SIZE-1)
  {
    c = Serial.read(); //read one character
    if (((char) c == '\r') || ((char) c == '\n')) break; // if carriage return or linefeed, stop and return data
    if ( ((char) c == '\x7F') || ((char) c == '\x08') )   // remove previous character (decrement index) if Backspace/Delete key pressed      index--;
    {
      if (index > 0) index--;
    }
    else if (c >= 0)
    {
      ui_buffer[index++]=(char) c; // put character into ui_buffer
    }
  }
  ui_buffer[index]='\0';  // terminate string with NULL

  if ((char) c == '\r')    // if the last character was a carriage return, also clear linefeed if it is next character
  {
    delay(10);  // allow 10ms for linefeed to appear on serial pins
    if (Serial.peek() == '\n') Serial.read(); // if linefeed appears, read it and throw it away
  }

  return index; // return number of characters, not including null terminator
}

// Read a float value from the serial interface
float read_float()
{
  float data;
  read_data();
  data = atof(ui_buffer);
  return(data);
}

// Read an integer from the serial interface.
// The routine can recognize Hex, Decimal, Octal, or Binary
// Example:
// Hex:     0x11 (0x prefix)
// Decimal: 17
// Octal:   021 (leading zero prefix)
// Binary:  B10001 (leading B prefix)
int32_t read_int()
{
  int32_t data;
  read_data();
  if (ui_buffer[0] == 'm')
    return('m');
  if ((ui_buffer[0] == 'B') || (ui_buffer[0] == 'b'))
  {
    data = strtol(ui_buffer+1, NULL, 2);
  }
  else
    data = strtol(ui_buffer, NULL, 0);
  return(data);
}

// Read a string from the serial interface.  Returns a pointer to the ui_buffer.
char *read_string()
{
  read_data();
  return(ui_buffer);
}

// Read a character from the serial interface
int8_t read_char()
{
  read_data();
  return(ui_buffer[0]);
}
