/*
  Control system for a simple voltage-mode buck converter based on
  the LT1054. ATMega-based Arduinos (such as the Linduino) only.

  Reads an analog input pin, compares to desired setpoint, integrates the error,
  maps the integral to a range from 0 to 255 and uses
  the result to set the pulse width modulation (PWM) of an output pin.
  Also prints the results to the Serial Monitor.

  The circuit:
  - LT1054 clock overdrive circuit, driven by Digital Pin 3 (PWM capable)
  - Analog Input 0 connected to buck converter output.


  Derived from AnalogInOutSerial example:
  http://www.arduino.cc/en/Tutorial/AnalogInOutSerial
  PWM frequency adjustment function from:
  https://playground.arduino.cc/Code/PwmFrequency

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

// These constants won't change. They're used to give names to the pins used:
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 3; // Analog output pin that the LED is attached to
#define fixed 0
#define closed_loop 1

float vout = 3.3; // Floating poiont output voltage.
int setpoint = int (vout * 1024.0 / 5.0);
//int setpoint = 512; // 1.25V
int feedback = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)
int error = 0;
int integral = 128;

#define verbose

void setup()
{
  // initialize serial communications at 115200 bps:
#ifdef verbose
  Serial.begin(115200);
#endif
  setPwmFrequency(analogOutPin, 1);
}

uint8_t state = closed_loop;
void loop()
{
  char ch;
  float fbv, dc;
  if (Serial.available())
  {
    ch = Serial.read();
    switch (ch)
    {
      case '0':
        state = fixed;
        outputValue = 0  ;
        break;
      case '1':
        state = fixed;
        outputValue = 26 ;
        break;
      case '2':
        state = fixed;
        outputValue = 51 ;
        break;
      case '3':
        state = fixed;
        outputValue = 77 ;
        break;
      case '4':
        state = fixed;
        outputValue = 102;
        break;
      case '5':
        state = fixed;
        outputValue = 128;
        break;
      case '6':
        state = fixed;
        outputValue = 153;
        break;
      case '7':
        state = fixed;
        outputValue = 179;
        break;
      case '8':
        state = fixed;
        outputValue = 204;
        break;
      case '9':
        state = fixed;
        outputValue = 230;
        break;
      case 'A':
        state = fixed;
        outputValue = 255;
        break;
      case '\r':
        break;
      case '\n':
        break;
      default:
        state = closed_loop;
    }
    if (ch != '\r' && ch != '\n')
    {
      Serial.print("Setting PWM to ");
      Serial.print(outputValue);
      Serial.println("/255");
    }
    analogWrite(analogOutPin, outputValue);
  }
  if (state == closed_loop)
  {
    // read the analog in value:
    feedback = analogRead(analogInPin);
    error = setpoint - feedback;
    integral = integral + error/4; //kI = 0.25
    if (integral > 1023) integral = 1023;
    if (integral < 0) integral = 0;

    // map it to the range of the analog out
    // (could probably just right-shift by two...)
    // outputValue = map(integral, 0, 1023, 0, 255);
    outputValue = integral >> 2; // Map 10 bit integral to 8 bit output, maintainting integral precision
    // change the analog out value:
    analogWrite(analogOutPin, outputValue);
#ifdef verbose
    // print the results to the Serial Monitor:
    Serial.print("feedback = ");
    fbv = (float) feedback * (5.0 / 1024.0);
    Serial.print(fbv, 2);
    Serial.print("\t PWM duty cycle = ");
    dc = (float) outputValue * (100.0 / 256.0);
    Serial.println(dc, 1);
#endif
    // wait some milliseconds before the next loop for the analog-to-digital
    // converter to settle after the last reading:
    delay(10);
  }
}


/**
 * Divides a given PWM pin frequency by a divisor.
 *
 * The resulting frequency is equal to the base frequency divided by
 * the given divisor:
 *   - Base frequencies:
 *      o The base frequency for pins 3, 9, 10, and 11 is 31250 Hz.
 *      o The base frequency for pins 5 and 6 is 62500 Hz.
 *   - Divisors:
 *      o The divisors available on pins 5, 6, 9 and 10 are: 1, 8, 64,
 *        256, and 1024.
 *      o The divisors available on pins 3 and 11 are: 1, 8, 32, 64,
 *        128, 256, and 1024.
 *
 * PWM frequencies are tied together in pairs of pins. If one in a
 * pair is changed, the other is also changed to match:
 *   - Pins 5 and 6 are paired on timer0
 *   - Pins 9 and 10 are paired on timer1
 *   - Pins 3 and 11 are paired on timer2
 *
 * Note that this function will have side effects on anything else
 * that uses timers:
 *   - Changes on pins 3, 5, 6, or 11 may cause the delay() and
 *     millis() functions to stop working. Other timing-related
 *     functions may also be affected.
 *   - Changes on pins 9 or 10 will cause the Servo library to function
 *     incorrectly.
 *
 * Thanks to macegr of the Arduino forums for his documentation of the
 * PWM frequency divisors. His post can be viewed at:
 *   http://forum.arduino.cc/index.php?topic=16612#msg121031
 */
void setPwmFrequency(int pin, int divisor)
{
  byte mode;
  if (pin == 5 || pin == 6 || pin == 9 || pin == 10)
  {
    switch (divisor)
    {
      case 1:
        mode = 0x01;
        break;
      case 8:
        mode = 0x02;
        break;
      case 64:
        mode = 0x03;
        break;
      case 256:
        mode = 0x04;
        break;
      case 1024:
        mode = 0x05;
        break;
      default:
        return;
    }
    if (pin == 5 || pin == 6)
    {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    }
    else
    {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  }
  else if (pin == 3 || pin == 11)
  {
    switch (divisor)
    {
      case 1:
        mode = 0x01;
        break;
      case 8:
        mode = 0x02;
        break;
      case 32:
        mode = 0x03;
        break;
      case 64:
        mode = 0x04;
        break;
      case 128:
        mode = 0x05;
        break;
      case 256:
        mode = 0x06;
        break;
      case 1024:
        mode = 0x07;
        break;
      default:
        return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}
