/*
Very simple RS485 full-duplex example for CN0416.
Incoming characters are echoed back, with lower-case
characters converted to upper case. This allows the observation
of simultaneous traffic being transmitted and recieved, ensuring
that the characters are different (no cheating!)
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