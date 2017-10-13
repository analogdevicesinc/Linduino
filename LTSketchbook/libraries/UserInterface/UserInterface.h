//! @todo Review this file.
/*
UserInterface.h

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

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <stdint.h>

#define UI_BUFFER_SIZE 64
#define SERIAL_TERMINATOR '\n'

// io buffer
extern char ui_buffer[UI_BUFFER_SIZE];

// Read data from the serial interface into the ui_buffer buffer
uint8_t read_data();

// Read a float value from the serial interface
float read_float();

// Read an integer from the serial interface.
// The routine can recognize Hex, Decimal, Octal, or Binary
// Example:
// Hex:     0x11 (0x prefix)
// Decimal: 17
// Octal:   O21 (leading letter O prefix)
// Binary:  B10001 (leading letter B prefix)
int32_t read_int();

// Read a string from the serial interface.  Returns a pointer to the ui_buffer.
char *read_string();

// Read a character from the serial interface
int8_t read_char();

#endif  // USERINTERFACE_H
