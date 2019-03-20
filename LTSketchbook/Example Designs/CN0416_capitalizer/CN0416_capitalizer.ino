/*
Very simple RS485 full-duplex example for CN0416.
Incoming characters are echoed back, with lower-case
characters converted to upper case. This allows the observation
of simultaneous traffic being transmitted and recieved, ensuring
that the characters are different (no cheating!)



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


#include <stdint.h>
#include <Arduino.h>

#define DE 2 // RS485 Driver Enable pin

const uint8_t addr2ch[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'}; // number to character

char chaddr;

void setup() {
  uint8_t addr = 0;
  pinMode(DE, OUTPUT);
  digitalWrite(DE, HIGH);
	
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  
  addr += digitalRead(A3);
  addr << 1;
  addr += digitalRead(A2);
  addr << 1;
  addr += digitalRead(A1);
  addr << 1;
  addr += digitalRead(A0);
  
  if (addr > 15) {addr = 15;} // just in case...
  chaddr = addr2ch[addr];
  Serial.begin(9600);
  delay(2);
  digitalWrite(DE, HIGH);
  Serial.print("Hello, I am node ");
  Serial.println(chaddr);
  Serial.flush();
  delay(2);
  digitalWrite(DE, LOW);
}


void loop() {
char ch;
  if (Serial.available()) {      // If anything comes in Serial (USB),
    ch = Serial.read();
    if(ch >= 'a' && ch <= 'z')
    {
      ch -= 32; // Convert lower-case to capital
    }
	Serial.write(ch); // Convert lower-case to capital
	}
}
