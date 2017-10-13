/*
  LCDKeypad.cpp
  */



#include <Arduino.h>
#include <LiquidCrystal.h>
#include "LCDKeypad.h"

/** \brief  degree sign for LCD see https://omerk.github.io/lcdchargen/ */
uint8_t LCDKeypad::degree[8] =
{
  B01100,
  B10010,
  B10010,
  B01100,
  B00000,
  B00000,
  B00000,
  B00000
};

//template <typename T> inline void LCDKeypad::PRINTSCREEN(T const& A)
//{
//  if (foundLCD) this->print(A);
//  if (EnableSerialOutput)
//    Serial.print(A);
//}

/**  \fn  void searchForLCD()
\brief LCD keypad shield detection
poor man's way to detect LCD keypad shield:
Linduino's pin 10 is connected to the base of a NPN
on the LCD keypad. In normal operation this pin must
only be driven low or kept tristate to turn the LCD
backlight off or on.
to detect the base of the NPN we drive the pin high
and read back its value. After the read the pin will be
set tristate. The driving high phase will last only
a few microseconds, during that time the pin will be
forced < 0.8 V bei the PN-diode of the transistor.
As MCU's max. low level is 1.5V (for 5V supply) the
pin will be read as zero, which tells us the LCD is
connected.
*/
void LCDKeypad::searchForLCD()
{
  digitalWrite(10, HIGH);
  pinMode(10, OUTPUT);
  // pin 10 is connected to PN diode of transistor Q1 on LCD keypad
  // so it will be forced < 0.8V, max. low level is 1.5V
  foundLCD = (digitalRead(10) == 0);
  pinMode(10, INPUT); // go back to tristate
  digitalWrite(10, LOW);
  if (foundLCD)
  {
    createChar(0, degree);
    begin(16, 2);
  }
}

void LCDKeypad::Disable4BitLCD()
{
  foundLCD = false;
  // disable all drivers
  pinMode(_ledCtrlPin, INPUT);

  pinMode(8, INPUT);
  pinMode(9, INPUT);

  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
}

LCDKeypad::LCDKeypad(uint8_t ledCtrlPin) : LiquidCrystal(8, 9, 4, 5, 6, 7)
{
  foundLCD = false;
  _ledCtrlPin = ledCtrlPin;
  // output PIN state of LED control pin must always be low
  // this is due to poor design of LCD Keypad shield where arduino's pin 10
  // is directly connected to the base of a transistor: enabling the pins output
  // driver and setting the output level to high would draw a big current out
  // of the pin. thus unfortunately it is even not possible to dimm led
  // backlight via PWM signal!
  digitalWrite(_ledCtrlPin, LOW);
  pinMode(_ledCtrlPin, INPUT);    // set pinMode to INPUT to enable LED
}

int LCDKeypad::button()
{
  static int NUM_KEYS = 5;
  static int adc_key_val[5] = { 30, 150, 360, 535, 760 };
  int k, input;

  input = analogRead(0);

  for (k = 0; k < NUM_KEYS; k++)
    if (input < adc_key_val[k])
      return k;

  return -1; // No valid key pressed
}

void LCDKeypad::LEDon()
{
  digitalWrite(_ledCtrlPin, LOW);
  pinMode(_ledCtrlPin, INPUT);    // set pinMode to INPUT to enable LED
}

void LCDKeypad::LEDoff()
{
  digitalWrite(_ledCtrlPin, LOW);
  pinMode(_ledCtrlPin, OUTPUT);    // set pinMode to OUTPUT to disable LED
}


