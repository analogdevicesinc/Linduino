/*!
Linear Technology DC2218A Demonstration Board.
LT3965 - 8-Switch Matrix LED Dimmer

@verbatim

  Setup:

   Follow the procedure below:
  1.  Set the PATTERN SELECT rotary switch S1 to any position between 0 and 6.
      Position 7 is reserved for use with the GUI (graphical user interface).
  2.  Connect 12V power to either the EMIVIN and GND banana jacks or to the
      J7 2.1mm (inside diameter) barrel jack.
  3.  Observe the red LED indicator light (D32). When it begins flashing,
      then the board is ready to start up.
  4.  When the red LED (D32) is flashing, push the Start button S3.
  5.  Observe the LED patterns and adjust the speed, brightness and pattern of
      the LEDs with the manual blue potentiometer R14.
  6.  Change the Pattern Select rotary switch to positions 0-6 to observe
      different patterns.
  7.  Press the Reset button S2 to start over again.



USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LT3965

http://www.linear.com/product/LT3965#demoboards


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
    @ingroup LT3965
*/


#include <stdio.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LT3965.h"


//******************Function Declarations******************
//******************Display Pattern Functions******************
void fadewave();
void analog();
void sidebend();
void sidebendAuto();
void dazzle();
void pulse();
void wave();
void steeringRightAndLeft();
void sectionalBlanking();
void orbit();
void GraphicUI();
int readpot();
void setThresh();


uint8_t updown;
int lastread;
int currentread;


//Setup message bytes
int16_t An2, An1, An0;
uint8_t inputByte_0;
uint8_t inputByte_1;
uint8_t inputByte_2;
uint8_t inputByte_3;
uint8_t inputByte_4;

uint8_t input;
uint8_t fade;
int set = 0;
uint8_t values[4];

bool startsequence = FALSE;

uint8_t startup = 0;
long interval = 250;
long previousTime = 0;
uint8_t ledState;


// Setup the hardware I2C interface.
// LT3965_i2c_enable or quikeval_I2C_init must be called before using any of the other I2C routines.
void LT3965_i2c_enable()
{
  TWSR = (HARDWARE_I2C_PRESCALER_1 & 0x03);  //! 1) set the prescaler bits
  TWBR = 12;            //! 2) set the bit rate
}


//! Initialize Linduino
void setup()
{
  int8_t ack = 0;                                // I2C acknowledge bit

  pinMode(REDLED, OUTPUT);
  pinMode(LOADPIN, OUTPUT);
  pinMode(BUTTONPIN, INPUT);
  pinMode(PWMPIN, OUTPUT);
  pinMode(ALERTPIN, INPUT);

  digitalWrite(REDLED, HIGH);
  digitalWrite(PWMPIN, LOW);    // Pull pwm pin on LT3797 low to turn on with LEDs in OFF state
  digitalWrite(LOADPIN, HIGH);

  analogReference(EXTERNAL);                     // Set analog reference voltage to voltage sensed on AREF pin
  currentread = analogRead(A3);
  digitalWrite(PWMPIN, LOW);
  lastread = currentread;
  updown = DOWN;

  delay(50);

  An0 = analogRead(A0);               // Read 8 position switch pin 1
  An1 = analogRead(A1);               // Read 8 position switch pin 2
  An2 = analogRead(A2);               // Read 8 position switch pin 3

  if (!(An0 > 120 && An1 > 120 && An2 > 120))
  {
    int alertStatus = digitalRead(ALERTPIN);
    while (alertStatus)
    {
      alertStatus = digitalRead(ALERTPIN);
    }
  }

  LT3965_i2c_enable();                       // Enable the I2C port
  quikeval_I2C_connect();                    // Connect I2C to main data port
  delay(50);

  // Change Open-Circuit Threshold to 4.5V and turn all LEDs ON; ADDRESS1
  ack |= i2c_scwriteshort(address1, CHANNEL0, 0b0000);
  ack |= i2c_scwriteshort(address1, CHANNEL1, 0b0000);
  ack |= i2c_scwriteshort(address1, CHANNEL2, 0b0000);
  ack |= i2c_scwriteshort(address1, CHANNEL3, 0b0000);
  ack |= i2c_scwriteshort(address1, CHANNEL4, 0b0000);
  ack |= i2c_scwriteshort(address1, CHANNEL5, 0b0000);
  ack |= i2c_scwriteshort(address1, CHANNEL6, 0b0000);
  ack |= i2c_scwriteshort(address1, CHANNEL7, 0b0000);
  // Change Open-Circuit Threshold to 4.5V and turn all LEDs ON; ADDRESS2
  ack |= i2c_scwriteshort(address2, CHANNEL0, 0b0000);
  ack |= i2c_scwriteshort(address2, CHANNEL1, 0b0000);
  ack |= i2c_scwriteshort(address2, CHANNEL2, 0b0000);
  ack |= i2c_scwriteshort(address2, CHANNEL3, 0b0000);
  ack |= i2c_scwriteshort(address2, CHANNEL4, 0b0000);
  ack |= i2c_scwriteshort(address2, CHANNEL5, 0b0000);
  ack |= i2c_scwriteshort(address2, CHANNEL6, 0b0000);
  ack |= i2c_scwriteshort(address2, CHANNEL7, 0b0000);
  // Change Short-Circuit Threshold to 1V and turn all LEDs ON; ADDRESS1
  ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, 1);
  ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, 1);
  ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, 1);
  ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, 1);
  ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, 1);
  ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, 1);
  ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, 1);
  ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, 1);
  // Change Short-Circuit Threshold to 1V and turn all LEDs ON; ADDRESS2
  ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, 1);
  ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, 1);
  ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, 1);
  ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, 1);
  ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, 1);
  ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, 1);
  ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, 1);
  ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, 1);

  delay(50);

  if (!(An0 > 120 && An1 > 120 && An2 > 120))
  {
    // require a push-button press before doing anything. LED will blink until button is pressed, then stay illuminated.
    while (digitalRead(BUTTONPIN))
    {
      unsigned long currentTime = millis();
      if (currentTime - previousTime > interval)
      {
        previousTime = currentTime;
        ledState = ! ledState;
        digitalWrite(REDLED, ledState);
      }
    }
  }
  digitalWrite(REDLED, LOW);

  An0 = analogRead(A0);           // Read 8 position switch pin 1
  An1 = analogRead(A1);                 // Read 8 position switch pin 2
  An2 = analogRead(A2);                 // Read 8 position switch pin 3
}

//! Repeats Linduino loop
void loop()
{
  int8_t ack = 0;                           // I2C acknowledge bit
  int switch_pos;                         // New switch possition
  int prev_switch_pos;            // Previous swicth position

  // require two consecutive switch readings, 50ms apart, before choosing a pattern.
  do
  {
    An0 = analogRead(A0);           // Read 8 position switch pin 1
    An1 = analogRead(A1);             // Read 8 position switch pin 2
    An2 = analogRead(A2);             // Read 8 position switch pin 3

    prev_switch_pos = switch_pos;

    //Interpret 3 bits from 8 position switch as decimal integer
    if (An0 > 120 && An1 < 120 && An2 < 120)
      switch_pos = 1;
    else if (An0 < 120 && An1 > 120 && An2 < 120)
      switch_pos = 2;
    else if (An0 > 120 && An1 > 120 && An2 < 120)
      switch_pos = 3;
    else if (An0 < 120 && An1 < 120 && An2 > 120)
      switch_pos = 4;
    else if (An0 > 120 && An1 < 120 && An2 > 120)
      switch_pos = 5;
    else if (An0 < 120 && An1 > 120 && An2 > 120)
      switch_pos = 6;
    else if (An0 > 120 && An1 > 120 && An2 > 120)
      switch_pos = 7;
    else
      switch_pos = 0;

    delay(50);
  }
  while (prev_switch_pos != switch_pos);

  // Call function associated with corresponding switch position value
  switch (switch_pos)
  {
    case 0:
      orbit();
      break;
    case 1:
      wave();  // Select a DAC to update
      break;
    case 2:
      steeringRightAndLeft();  // Select a DAC to update
      break;
    case 3:
      sectionalBlanking();  // Select a DAC to update
      break;
    case 4:
      sidebend();  // Select a DAC to update
      break;
    case 5:
      analog();  // Select a DAC to update
      break;
    case 6:
      fadewave();  // Select a DAC to update
      break;
    case 7:
      GraphicUI();  // Select a DAC to update
      break;
  }
}

//! Function to write I2C bytes which are received from GUI.
int8_t write_bytes(uint8_t size, uint8_t byte1, uint8_t byte2, uint8_t byte3)
{
  int8_t ret = 0 ;
  uint8_t data[3];
  if (i2c_start() != 0)                               //I2C START
    return (1);                                      //Stop and return 0 if START fail

  if (size == 2)
  {
    ret |= i2c_write(byte1);
    ret |= i2c_write(byte2);
  }
  else if (size == 3)
  {
    ret |= i2c_write(byte1);
    ret |= i2c_write(byte2);
    ret |= i2c_write(byte3);
  }
  else if (size == 1)
  {
    ret |= i2c_write(byte1);
  }

  i2c_stop();                                        //I2C STOP
  if (ret != 0)                                       //Returns 1 if failed
    return (1);
  return (0);                                        //Returns 0 if success
}

//! Function to read I2C bytes depending on the bytes received from GUI.
int8_t read_bytes(uint8_t size, uint8_t byte1, uint8_t byte2, uint8_t byte3)
{
  int8_t ret = 0 ;
  if (size == 250 && byte1 == 1 && byte2 == 1 && byte3 == 1)  // ALERT - ACMODE READ
  {
    uint8_t fault;
    fault = 1;
    fault = digitalRead(2);     // digital pin2 is connected to ALERT/
    if (!fault) // fault condition
      Serial.write(0);
    else
      Serial.write(1);
  }
  else
  {
    uint8_t x;
    if (i2c_start() != 0)                               //I2C START
      return (1);                                      //Stop and return 0 if START fail

    // write byte1 and read size number of bytes
    ret |= i2c_write(byte1);
    if (ret == 0)                                       // If response = ACK (slave responded)
    {
      for (x = 0; x < (size - 1); ++x)
      {
        Serial.write(i2c_read(WITH_ACK));
      }
      Serial.write(i2c_read(WITH_NACK));
    }
    else                        // If response = NACK (no slave responded
    {
      for (x = 0; x < size; ++x)
      {
        Serial.write(0);                // The GUI is waiting to read back bytes, sending dummy bytes
      }
    }
    i2c_stop();                                       //I2C STOP
  }
  if (ret != 0)                                     //Returns 1 if failed
    return (1);
  return (0);                                       //Returns 0 if success
}

//! Function to talk to GUI.
void GraphicUI()      // DO NOT EDIT THIS FUNCTION
{

  int8_t ack = 0;       // I2C acknowledge bit
  uint8_t data_SCMREG;
  uint8_t brightness;
  uint8_t channel;
  uint8_t x = 0;
  int toggle = 255;
  uint8_t values[3];
  uint8_t ARA;
  char read_id;
  setThresh();
  Serial.begin(115200);                    // Initialize the serial port to the PC at baud rate of 115200 bps

  An0 = analogRead(A0);
  An1 = analogRead(A1);
  An2 = analogRead(A2);

  while (An0 > 120 && An1 > 120 && An2 > 120)
  {
    if (Serial.available() > 0)
    {
      inputByte_0 = Serial.read();
      delay(10);

      if (inputByte_0 == 'i')   // Making Quikeval compatible
      {
        Serial.print("USBSPI,PIC,01,01,DC,DC590,----------------------\r\n");
      }
      else if (inputByte_0 == 'I')    // Making Quikeval compatible
      {
        Serial.print("LT3965,Cls,D3965,01,01,DC,DC2218A,--------------\r\n");
      }
      else if (inputByte_0 == 16)
      {
        inputByte_1 = Serial.read();
        delay(10);
        inputByte_2 = Serial.read();
        delay(10);
        inputByte_3 = Serial.read();
        delay(10);
        inputByte_4 = Serial.read();
        delay(10);

        //Detect Command type
        if (inputByte_1 == 128)
        {
          Serial.print("HELLO FROM ARDUINO");

          ack |= i2c_scwriteshort(address1, CHANNEL0, 0b0000);
          ack |= i2c_scwriteshort(address1, CHANNEL1, 0b0000);
          ack |= i2c_scwriteshort(address1, CHANNEL2, 0b0000);
          ack |= i2c_scwriteshort(address1, CHANNEL3, 0b0000);
          ack |= i2c_scwriteshort(address1, CHANNEL4, 0b0000);
          ack |= i2c_scwriteshort(address1, CHANNEL5, 0b0000);
          ack |= i2c_scwriteshort(address1, CHANNEL6, 0b0000);
          ack |= i2c_scwriteshort(address1, CHANNEL7, 0b0000);

          ack |= i2c_scwriteshort(address2, CHANNEL0, 0b0000);
          ack |= i2c_scwriteshort(address2, CHANNEL1, 0b0000);
          ack |= i2c_scwriteshort(address2, CHANNEL2, 0b0000);
          ack |= i2c_scwriteshort(address2, CHANNEL3, 0b0000);
          ack |= i2c_scwriteshort(address2, CHANNEL4, 0b0000);
          ack |= i2c_scwriteshort(address2, CHANNEL5, 0b0000);
          ack |= i2c_scwriteshort(address2, CHANNEL6, 0b0000);
          ack |= i2c_scwriteshort(address2, CHANNEL7, 0b0000);

          ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, 0);
          ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, 0);
          ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, 0);
          ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, 0);
          ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, 0);
          ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, 0);
          ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, 0);
          ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, 0);

          ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, 0);
          ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, 0);
          ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, 0);
          ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, 0);
          ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, 0);
          ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, 0);
          ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, 0);
          ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, 0);

          ack |= i2c_acwrite(address1, 0b00000000);
          ack |= i2c_acwrite(address2, 0b00000000);

          startsequence = TRUE;
          delay(50);
          digitalWrite(PWMPIN, HIGH);
          delay(50);
          digitalWrite(LOADPIN, LOW);
          delay(10);
        }
        else
        {
          write_bytes(inputByte_1, inputByte_2, inputByte_3, inputByte_4);
        }

      }
      else if (inputByte_0 == 17)
      {
        inputByte_1 = Serial.read();
        delay(10);
        inputByte_2 = Serial.read();
        delay(10);
        inputByte_3 = Serial.read();
        delay(10);
        inputByte_4 = Serial.read();
        delay(10);

        read_bytes(inputByte_1, inputByte_2, inputByte_3, inputByte_4);
      }
    }
    An0 = analogRead(A0);
    An1 = analogRead(A1);
    An2 = analogRead(A2);
  }
}

//! Func Desc: All LEDs illuminated to the same brightness level depending on potentiometer value.
void analogAuto()
{
  setThresh();

  int8_t ack = 0;                               // I2C acknowledge bit
  uint8_t i;
  int j;
  An0 = analogRead(A0);       // Read 8 position switch pin 1
  An1 = analogRead(A1);       // Read 8 position switch pin 2
  An2 = analogRead(A2);       // Read 8 position switch pin 3
  int analog_in = readpot();          // Read value of potentiometer

  while (An0 > 120 && An1 < 120 && An2 > 120)
  {
    if (!startsequence)
    {
      ack |= i2c_acwrite(address1, 0);
      ack |= i2c_acwrite(address2, 0);
      delay(10);
      digitalWrite(PWMPIN, HIGH);
      delay(10);
      startsequence = TRUE;
    }

    for (j = 0; j <= 511; j++)
    {
      An0 = analogRead(A0);
      An1 = analogRead(A1);
      An2 = analogRead(A2);
      analog_in = readpot();
      if (j > 255)
        i = 511 - j;
      else
        i = j;


      if (i < 1)
      {
        ack |= i2c_acwrite(address1, 0b00000000);
        ack |= i2c_acwrite(address2, 0b00000000);
      }
      else if (i > 254)
      {
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0001, i);
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0001, i);
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0001, i);
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0001, i);
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0001, i);
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0001, i);
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0001, i);
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0001, i);
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0001, i);
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0001, i);
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0001, i);
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0001, i);
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0001, i);
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0001, i);
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0001, i);
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0001, i);
      }
      else
      {
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, i);
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, i);
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, i);
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, i);
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, i);
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, i);
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, i);
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, i);
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, i);
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, i);
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, i);
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, i);
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, i);
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, i);
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, i);
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, i);
      }
      delay(5);
      if (i < 1)
        delay(500);
      if (i == 255)
        delay(500);
    }
  }
}

//! All sixteen LEDs adjust PWM dimming duty cycle and brightness based upon
//! the position of the steering wheel (interactive potentiometer). This pattern
//! represents a light that bends around the side of a car, projecting a
//! turning headlight pattern that is linked to the steering wheel.
void sidebendAuto()
{
  setThresh();

  int8_t ack = 0;       // I2C acknowledge bit
  int ch[8];
  int i;
  An0 = analogRead(A0);       // Read 8 position switch pin 1
  An1 = analogRead(A1);       // Read 8 position switch pin 2
  An2 = analogRead(A2);       // Read 8 position switch pin 3
  int analog_in;


  while (An0 < 120 && An1 < 120 && An2 > 120)
  {
    if (!startsequence)
    {
      ack |= i2c_acwrite(address1, 0);
      ack |= i2c_acwrite(address2, 0);
      delay(10);
      digitalWrite(PWMPIN, HIGH);
      delay(10);
      startsequence = TRUE;
    }

    for (int a = 0; a < 3; a++)
    {
      An0 = analogRead(A0);
      An1 = analogRead(A1);
      An2 = analogRead(A2);
      int increment = 5;
      int analogStart = 400;
      int analogMax = 530;

      if (a == 2)
      {
        analogMax = 650;
        increment = 8;
      }

      delay(500);

      for (analog_in = analogStart; analog_in > analogStart - 1; analog_in += increment)
      {

        delay(25);
        An0 = analogRead(A0);
        An1 = analogRead(A1);
        An2 = analogRead(A2);
        ch[0] = (analog_in) - 425;
        ch[1] = (analog_in) - 450;
        ch[2] = (analog_in) - 475;
        ch[3] = (analog_in) - 500;
        ch[4] = (analog_in) - 525;
        ch[5] = (analog_in) - 550;
        ch[6] = (analog_in) - 575;
        ch[7] = (analog_in) - 600;

        for (i = 0; i < 8; i++)
        {
          if (ch[i] < 0)
            ch[i] = 0;
          else if (ch[i] > 255)
            ch[i] = 255;
          An0 = analogRead(A0);
          An1 = analogRead(A1);
          An2 = analogRead(A2);

        }

        if (ch[0] == 0)
        {
          ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, ch[0]);
          ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, ch[0]);
        }
        else if (ch[0] == 255)
        {
          ack |= i2c_scwritelong(address1, CHANNEL0, 0b0001, ch[0]);
          ack |= i2c_scwritelong(address1, CHANNEL1, 0b0001, ch[0]);
        }
        else
        {
          ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, ch[0]);
          ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, ch[0]);
        }

        if (ch[1] == 0)
        {
          ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, ch[1]);
          ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, ch[1]);
        }
        else if (ch[1] == 255)
        {
          ack |= i2c_scwritelong(address1, CHANNEL2, 0b0001, ch[1]);
          ack |= i2c_scwritelong(address1, CHANNEL3, 0b0001, ch[1]);
        }
        else
        {
          ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, ch[1]);
          ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, ch[1]);
        }

        if (ch[2] == 0)
        {
          ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, ch[2]);
          ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, ch[2]);
        }
        else if (ch[2] == 255)
        {
          ack |= i2c_scwritelong(address1, CHANNEL4, 0b0001, ch[2]);
          ack |= i2c_scwritelong(address1, CHANNEL5, 0b0001, ch[2]);
        }
        else
        {
          ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, ch[2]);
          ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, ch[2]);
        }

        if (ch[3] == 0)
        {
          ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, ch[3]);
          ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, ch[3]);
        }
        else if (ch[3] == 255)
        {
          ack |= i2c_scwritelong(address1, CHANNEL6, 0b0001, ch[3]);
          ack |= i2c_scwritelong(address1, CHANNEL7, 0b0001, ch[3]);
        }
        else
        {
          ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, ch[3]);
          ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, ch[3]);
        }

        if (ch[4] == 0)
        {
          ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, ch[4]);
          ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, ch[4]);
        }
        else if (ch[4] == 255)
        {
          ack |= i2c_scwritelong(address2, CHANNEL0, 0b0001, ch[4]);
          ack |= i2c_scwritelong(address2, CHANNEL1, 0b0001, ch[4]);
        }
        else
        {
          ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, ch[4]);
          ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, ch[4]);
        }

        if (ch[5] == 0)
        {
          ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, ch[5]);
          ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, ch[5]);
        }
        else if (ch[5] == 255)
        {
          ack |= i2c_scwritelong(address2, CHANNEL2, 0b0001, ch[5]);
          ack |= i2c_scwritelong(address2, CHANNEL3, 0b0001, ch[5]);
        }
        else
        {
          ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, ch[5]);
          ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, ch[5]);
        }

        if (ch[6] == 0)
        {
          ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, ch[6]);
          ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, ch[6]);
        }
        else if (ch[6] == 255)
        {
          ack |= i2c_scwritelong(address2, CHANNEL4, 0b0001, ch[6]);
          ack |= i2c_scwritelong(address2, CHANNEL5, 0b0001, ch[6]);
        }
        else
        {
          ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, ch[6]);
          ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, ch[6]);
        }

        if (ch[7] == 0)
        {
          ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, ch[7]);
          ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, ch[7]);
        }
        else if (ch[7] == 255)
        {
          ack |= i2c_scwritelong(address2, CHANNEL6, 0b0001, ch[7]);
          ack |= i2c_scwritelong(address2, CHANNEL7, 0b0001, ch[7]);
        }
        else
        {
          ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, ch[7]);
          ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, ch[7]);
        }
        An0 = analogRead(A0);
        An1 = analogRead(A1);
        An2 = analogRead(A2);
        if (analog_in >= analogMax)
        {
          delay(250);
          increment *= -1;
        }
      }
    }
  }
}

//! Consecutive LEDs alternate fading up and down. Speed of fading controlled by
//! the position of the potentiometer.
void dazzle()
{
  setThresh();

  int8_t ack = 0;       // I2C acknowledge bit
  int8_t bright = -127;
  uint8_t brightness;
  An0 = analogRead(A0);       // Read 8 position switch pin 1
  An1 = analogRead(A1);       // Read 8 position switch pin 2
  An2 = analogRead(A2);       // Read 8 position switch pin 3
  int analog_in = analogRead(A3);         // Read value of potentiometer

  while ( An0 > 120 && An1 < 120 && An2 < 120)
  {
    An0 = analogRead(A0);     // Read 8 position switch pin 1
    An1 = analogRead(A1);     // Read 8 position switch pin 2
    An2 = analogRead(A2);     // Read 8 position switch pin 3
    analog_in = analogRead(A3);           // Read value of potentiometer

    brightness = abs(bright) * 2;

    ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, brightness);     // Single channel write command to control a single LED
    ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, brightness);
    ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, brightness);
    ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, brightness);
    ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, brightness);
    ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, brightness);
    ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, brightness);
    ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, brightness);

    ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, 255 - brightness);
    ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, 255 - brightness);
    ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, 255 - brightness);
    ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, 255 - brightness);
    ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, 255 - brightness);
    ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, 255 - brightness);
    ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, 255 - brightness);
    ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, 255 - brightness);

    delay(analog_in / 20);

    bright = bright + 2;
    if (bright >= 127)
      bright = -127;
  }
}

//! Center headlight LEDs stay on to face directly forward while the right and
//! left side LEDs turn on with brightness proportional to the amount that the
//! steering wheel (interactive potentiometer) is turned.
void steeringRightAndLeft()
{
  setThresh();

  int8_t ack = 0;             // I2C acknowledge bit
  int ch[6];                        // only need 8 channels - four right and four left
  int i;
  uint8_t j = 0;
  An0 = analogRead(A0);       // Read 8 position switch pin 1
  An1 = analogRead(A1);       // Read 8 position switch pin 2
  An2 = analogRead(A2);       // Read 8 position switch pin 3
  int analog_in = analogRead(A3);         // Read value of potentiometer



  while (An0 < 120 && An1 > 120 && An2 < 120)
  {
    An0 = analogRead(A0);           // Read 8 position switch pin 1
    An1 = analogRead(A1);     // Read 8 position switch pin 2
    An2 = analogRead(A2);     // Read 8 position switch pin 3
    analog_in = analogRead(A3);           // Read value of potentiometer

    ch[0] = 250 - (analog_in);
    ch[1] = 325 - (analog_in);
    ch[2] = 400 - (analog_in);
    ch[3] = (analog_in) - 650;
    ch[4] = (analog_in) - 725;
    ch[5] = (analog_in) - 800;

    // calculate right and left bending lights update
    for (i = 0; i < 6; i++)
    {
      if (ch[i] < 0)
        ch[i] = 0;
      else if (ch[i] > 255)
        ch[i] = 255;
    }

    if (!startsequence)
    {
      ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, 1);
      ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, 1);
      ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, 1);
      ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, 1);
      ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, 1);
      ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, 1);
      ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, 1);
      ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, 1);
      ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, 1);
      ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, 1);
      ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, 1);
      ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, 1);
      ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, 1);
      ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, 1);
      ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, 1);
      ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, 1);

      delay(500);
      digitalWrite(PWMPIN, HIGH);
      delay(10);
      digitalWrite(LOADPIN, LOW);
      delay(10);

      while (j < 255)
      {
        if (ch[0] >= j)
        {
          ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, ch[0]);
          ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, ch[0]);
        }
        if (ch[1] >= j)
        {
          ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, ch[1]);
          ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, ch[1]);
        }
        if (ch[2] >= j)
        {
          ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, ch[2]);
          ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, ch[2]);
        }

        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, j);
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, j);
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, j);
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, j);

        if (ch[3] >= j)
        {
          ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, ch[3]);
          ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, ch[3]);
        }
        if (ch[4] >= j)
        {
          ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, ch[4]);
          ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, ch[4]);
        }
        if (ch[5] >= j)
        {
          ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, ch[5]);
          ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, ch[5]);
        }
        j++;
        delay(8);
      }
      // delay(2500);
      startsequence = TRUE;
    }

    ack |= i2c_scwritelong(address1, CHANNEL6, 0b0001, 255);
    ack |= i2c_scwritelong(address1, CHANNEL7, 0b0001, 255);
    ack |= i2c_scwritelong(address2, CHANNEL0, 0b0001, 255);
    ack |= i2c_scwritelong(address2, CHANNEL1, 0b0001, 255);

    // write to right and left bending lights update
    ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, ch[0]);
    ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, ch[0]);
    ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, ch[1]);
    ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, ch[1]);
    ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, ch[2]);
    ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, ch[2]);

    ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, ch[3]);
    ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, ch[3]);
    ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, ch[4]);
    ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, ch[4]);
    ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, ch[5]);
    ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, ch[5]);
  }
}


//! All LEDs are illluminated to full brightness simulating a car
//! with its high-beams turned on. Two LEDs will turn off creating
//! a blank zone. This pattern shows how segments of a high-beam
//! LED cluster can be turned off to avoid blinding oncomming traffic.
void sectionalBlanking()
{
  setThresh();

  int8_t ack = 0;             // I2C acknowledge bit
  uint8_t j = 0;
  An0 = analogRead(A0);       // Read 8 position switch pin 1
  An1 = analogRead(A1);       // Read 8 position switch pin 2
  An2 = analogRead(A2);       // Read 8 position switch pin 3
  int analog_in = analogRead(A3);         // Read value of potentiometer

  if (!startsequence)         // Checks if start-up sequence has already been run to fade LEDs on
  {
    while (j < 255)
    {
      if (j < 1)
      {
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, j);
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, j);
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, j);
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, j);
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, j);
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, j);
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, j);
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, j);
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, j);
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, j);
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, j);
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, j);
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, j);
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, j);
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, j);
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, j);

        digitalWrite(PWMPIN, HIGH);
        delay(10);
        digitalWrite(LOADPIN, LOW);
        delay(10);
      }
      else if (j >= 254)
      {
        if (analog_in <= 146)
        {
          // Swtich OFF LED5 and LED8 of BOARD1
          ack |= i2c_acwrite(address1, 0b11110110);
          ack |= i2c_acwrite(address2, 0b11111111);
        }
        else if (analog_in > 146 && analog_in <= 292)
        {
          // Swtich OFF LED5 and LED4 of BOARD1
          ack |= i2c_acwrite(address1, 0b11100111);
          ack |= i2c_acwrite(address2, 0b11111111);
        }
        else if (analog_in > 292 && analog_in <= 438)
        {
          // Swtich OFF LED4 and LED1 of BOARD1
          ack |= i2c_acwrite(address1, 0b01101111);
          ack |= i2c_acwrite(address2, 0b11111111);
        }
        else if (analog_in > 438 && analog_in <= 584)
        {
          // Swtich OFF LED1 of BOARD1 and LED8 of BOARD2
          ack |= i2c_acwrite(address1, 0b01111111);
          ack |= i2c_acwrite(address2, 0b11111110);
        }
        else if (analog_in > 584 && analog_in <= 730)
        {
          // Swtich OFF LED8 and LED5 of BOARD2
          ack |= i2c_acwrite(address1, 0b11111111);
          ack |= i2c_acwrite(address2, 0b11110110);
        }
        else if (analog_in > 730 && analog_in <= 876)
        {
          // Swtich OFF LED5 and LED4 of BOARD2
          ack |= i2c_acwrite(address1, 0b11111111);
          ack |= i2c_acwrite(address2, 0b11100111);
        }
        else
        {
          // Swtich OFF LED4 and LED1 of BOARD2
          ack |= i2c_acwrite(address1, 0b11111111);
          ack |= i2c_acwrite(address2, 0b01101111);
        }
        startsequence = TRUE;
      }
      else
      {
        if (analog_in <= 146)
        {
          ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, j);
        }
        else if (analog_in > 146 && analog_in <= 292)
        {
          ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, j);
        }
        else if (analog_in > 292 && analog_in <= 438)
        {
          ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, j);
        }
        else if (analog_in > 438 && analog_in <= 584)
        {
          ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, j);
        }
        else if (analog_in > 584 && analog_in <= 730)
        {
          ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, j);
        }
        else if (analog_in > 730 && analog_in <= 876)
        {
          ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, j);
        }
        else
        {
          ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, j);
        }
      }
      delay(2);
      j++;
    }
  }

  while (An0 > 120 && An1 > 120 && An2 < 120)
  {
    An0 = analogRead(A0);       // Read 8 position switch pin 1
    An1 = analogRead(A1);       // Read 8 position switch pin 2
    An2 = analogRead(A2);       // Read 8 position switch pin 3
    analog_in = analogRead(A3);

    if (analog_in <= 146)
    {
      // Swtich OFF LED5 and LED8 of BOARD1
      ack |= i2c_acwrite(address1, 0b11110110);
      ack |= i2c_acwrite(address2, 0b11111111);
    }
    else if (analog_in > 146 && analog_in <= 292)
    {
      // Swtich OFF LED5 and LED4 of BOARD1
      ack |= i2c_acwrite(address1, 0b11100111);
      ack |= i2c_acwrite(address2, 0b11111111);
    }
    else if (analog_in > 292 && analog_in <= 438)
    {
      // Swtich OFF LED4 and LED1 of BOARD1
      ack |= i2c_acwrite(address1, 0b01101111);
      ack |= i2c_acwrite(address2, 0b11111111);
    }
    else if (analog_in > 438 && analog_in <= 584)
    {
      // Swtich OFF LED1 of BOARD1 and LED8 of BOARD2
      ack |= i2c_acwrite(address1, 0b01111111);
      ack |= i2c_acwrite(address2, 0b11111110);
    }
    else if (analog_in > 584 && analog_in <= 730)
    {
      // Swtich OFF LED8 and LED5 of BOARD2
      ack |= i2c_acwrite(address1, 0b11111111);
      ack |= i2c_acwrite(address2, 0b11110110);
    }
    else if (analog_in > 730 && analog_in <= 876)
    {
      // Swtich OFF LED5 and LED4 of BOARD2
      ack |= i2c_acwrite(address1, 0b11111111);
      ack |= i2c_acwrite(address2, 0b11100111);
    }
    else
    {
      // Swtich OFF LED4 and LED1 of BOARD2
      ack |= i2c_acwrite(address1, 0b11111111);
      ack |= i2c_acwrite(address2, 0b01101111);
    }
  }
}

//! Func Desc: Varying number of brightened LEDs depending on pot
void sidebend()
{
  setThresh();

  int8_t ack = 0;       // I2C acknowledge bit
  int ch[8];
  int i;
  uint8_t j = 0;
  An0 = analogRead(A0);       // Read 8 position switch pin 1
  An1 = analogRead(A1);       // Read 8 position switch pin 2
  An2 = analogRead(A2);       // Read 8 position switch pin 3
  int analog_in = readpot();          // Read value of potentiometer

  while (An0 < 120 && An1 < 120 && An2 > 120)
  {
    An0 = analogRead(A0);
    An1 = analogRead(A1);
    An2 = analogRead(A2);

    analog_in = 1023 - readpot();

    ch[0] = (analog_in) - 425;
    ch[1] = (analog_in) - 450;
    ch[2] = (analog_in) - 475;
    ch[3] = (analog_in) - 500;
    ch[4] = (analog_in) - 525;
    ch[5] = (analog_in) - 550;
    ch[6] = (analog_in) - 575;
    ch[7] = (analog_in) - 600;

    for (i = 0; i < 8; i++)
    {
      if (ch[i] < 0)
        ch[i] = 0;
      else if (ch[i] > 255)
        ch[i] = 255;
    }

    if (!startsequence)
    {
      if (ch[0] <= 0)
      {
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, 0);
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, 0);
      }
      else
      {
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, 1);
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, 1);
      }
      if (ch[1] <= 0)
      {
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, 0);
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, 0);
      }
      else
      {
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, 1);
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, 1);
      }
      if (ch[2] <= 0)
      {
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, 0);
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, 0);
      }
      else
      {
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, 1);
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, 1);
      }
      if (ch[3] <= 0)
      {
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, 0);
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, 0);
      }
      else
      {
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, 1);
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, 1);
      }
      if (ch[4] <= 0)
      {
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, 0);
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, 0);
      }
      else
      {
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, 1);
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, 1);
      }
      if (ch[5] <= 0)
      {
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, 0);
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, 0);
      }
      else
      {
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, 1);
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, 1);
      }
      if (ch[6] <= 0)
      {
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, 0);
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, 0);
      }
      else
      {
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, 1);
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, 1);
      }
      if (ch[7] <= 0)
      {
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, 0);
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, 0);
      }
      else
      {
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, 1);
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, 1);
      }

      delay(500);
      digitalWrite(PWMPIN, HIGH);
      delay(10);
      digitalWrite(LOADPIN, LOW);
      delay(10);

      while (j < 255)
      {
        if ((ch[0] >= j) && (ch[0] > 0))
        {
          ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, j);
        }
        if ((ch[1] >= j) && (ch[1] > 0))
        {
          ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, j);
        }
        if ((ch[2] >= j) && (ch[2] > 0))
        {
          ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, j);
        }
        if ((ch[3] >= j) && (ch[3] > 0))
        {
          ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, j);
          ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, j);
        }
        if ((ch[4] >= j) && (ch[4] > 0))
        {
          ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, j);
        }
        if ((ch[5] >= j) && (ch[5] > 0))
        {
          ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, j);
        }
        if ((ch[6] >= j) && (ch[6] > 0))
        {
          ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, j);
        }
        if ((ch[7] >= j) && (ch[7] > 0))
        {
          ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, j);
          ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, j);
        }
        j++;
        delay(8);
      }
      startsequence = TRUE;
    }

    if (ch[0] == 0)
    {
      ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, ch[0]);
      ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, ch[0]);
    }
    else if (ch[0] == 255)
    {
      ack |= i2c_scwritelong(address1, CHANNEL0, 0b0001, ch[0]);
      ack |= i2c_scwritelong(address1, CHANNEL1, 0b0001, ch[0]);
    }
    else
    {
      ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, ch[0]);
      ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, ch[0]);
    }

    if (ch[1] == 0)
    {
      ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, ch[1]);
      ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, ch[1]);
    }
    else if (ch[1] == 255)
    {
      ack |= i2c_scwritelong(address1, CHANNEL2, 0b0001, ch[1]);
      ack |= i2c_scwritelong(address1, CHANNEL3, 0b0001, ch[1]);
    }
    else
    {
      ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, ch[1]);
      ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, ch[1]);
    }

    if (ch[2] == 0)
    {
      ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, ch[2]);
      ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, ch[2]);
    }
    else if (ch[2] == 255)
    {
      ack |= i2c_scwritelong(address1, CHANNEL4, 0b0001, ch[2]);
      ack |= i2c_scwritelong(address1, CHANNEL5, 0b0001, ch[2]);
    }
    else
    {
      ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, ch[2]);
      ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, ch[2]);
    }

    if (ch[3] == 0)
    {
      ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, ch[3]);
      ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, ch[3]);
    }
    else if (ch[3] == 255)
    {
      ack |= i2c_scwritelong(address1, CHANNEL6, 0b0001, ch[3]);
      ack |= i2c_scwritelong(address1, CHANNEL7, 0b0001, ch[3]);
    }
    else
    {
      ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, ch[3]);
      ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, ch[3]);
    }

    if (ch[4] == 0)
    {
      ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, ch[4]);
      ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, ch[4]);
    }
    else if (ch[4] == 255)
    {
      ack |= i2c_scwritelong(address2, CHANNEL0, 0b0001, ch[4]);
      ack |= i2c_scwritelong(address2, CHANNEL1, 0b0001, ch[4]);
    }
    else
    {
      ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, ch[4]);
      ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, ch[4]);
    }

    if (ch[5] == 0)
    {
      ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, ch[5]);
      ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, ch[5]);
    }
    else if (ch[5] == 255)
    {
      ack |= i2c_scwritelong(address2, CHANNEL2, 0b0001, ch[5]);
      ack |= i2c_scwritelong(address2, CHANNEL3, 0b0001, ch[5]);
    }
    else
    {
      ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, ch[5]);
      ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, ch[5]);
    }

    if (ch[6] == 0)
    {
      ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, ch[6]);
      ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, ch[6]);
    }
    else if (ch[6] == 255)
    {
      ack |= i2c_scwritelong(address2, CHANNEL4, 0b0001, ch[6]);
      ack |= i2c_scwritelong(address2, CHANNEL5, 0b0001, ch[6]);
    }
    else
    {
      ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, ch[6]);
      ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, ch[6]);
    }

    if (ch[7] == 0)
    {
      ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, ch[7]);
      ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, ch[7]);
    }
    else if (ch[7] == 255)
    {
      ack |= i2c_scwritelong(address2, CHANNEL6, 0b0001, ch[7]);
      ack |= i2c_scwritelong(address2, CHANNEL7, 0b0001, ch[7]);
    }
    else
    {
      ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, ch[7]);
      ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, ch[7]);
    }
  }
}

//! Func Desc: All LEDs lighted up to the same level depending on pot value.
void analog()
{
  setThresh();

  int8_t ack = 0;                               // I2C acknowledge bit
  uint8_t i;
  uint8_t j = 0;
  An0 = analogRead(A0);       // Read 8 position switch pin 1
  An1 = analogRead(A1);       // Read 8 position switch pin 2
  An2 = analogRead(A2);       // Read 8 position switch pin 3
  int analog_in = readpot();          // Read value of potentiometer

  if (!startsequence)         // Checks if start-up sequence has already been run to fade LEDs on
  {
    analog_in = 1023 - readpot();
    i = analog_in / 4;

    while (j <= i)
    {
      if (j < 1)
      {
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, j);
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, j);
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, j);
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, j);
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, j);
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, j);
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, j);
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, j);
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, j);
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, j);
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, j);
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, j);
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, j);
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, j);
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, j);
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, j);
        digitalWrite(PWMPIN, HIGH);
        delay(10);
        digitalWrite(LOADPIN, LOW);
        delay(10);
      }
      else if (j > 254)
      {
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0001, j);
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0001, j);
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0001, j);
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0001, j);
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0001, j);
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0001, j);
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0001, j);
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0001, j);
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0001, j);
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0001, j);
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0001, j);
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0001, j);
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0001, j);
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0001, j);
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0001, j);
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0001, j);
        startsequence = TRUE;
        break;
      }
      else
      {
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, j);
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, j);
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, j);
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, j);
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, j);
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, j);
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, j);
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, j);
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, j);
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, j);
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, j);
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, j);
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, j);
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, j);
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, j);
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, j);
        if (j == i)
        {
          startsequence = TRUE;
          break;
        }
      }
      delay(2);
      j++;
    }
  }

  while (An0 > 120 && An1 < 120 && An2 > 120)
  {
    An0 = analogRead(A0);
    An1 = analogRead(A1);
    An2 = analogRead(A2);
    analog_in = 1023 - readpot();
    i = analog_in / 4;
    //    i = 128;

    if (i < 1)
    {
      ack |= i2c_acwrite(address1, 0b00000000);
      ack |= i2c_acwrite(address2, 0b00000000);
    }
    else if (i > 254)
    {
      ack |= i2c_scwritelong(address1, CHANNEL0, 0b0001, i);
      ack |= i2c_scwritelong(address1, CHANNEL1, 0b0001, i);
      ack |= i2c_scwritelong(address1, CHANNEL2, 0b0001, i);
      ack |= i2c_scwritelong(address1, CHANNEL3, 0b0001, i);
      ack |= i2c_scwritelong(address1, CHANNEL4, 0b0001, i);
      ack |= i2c_scwritelong(address1, CHANNEL5, 0b0001, i);
      ack |= i2c_scwritelong(address1, CHANNEL6, 0b0001, i);
      ack |= i2c_scwritelong(address1, CHANNEL7, 0b0001, i);
      ack |= i2c_scwritelong(address2, CHANNEL0, 0b0001, i);
      ack |= i2c_scwritelong(address2, CHANNEL1, 0b0001, i);
      ack |= i2c_scwritelong(address2, CHANNEL2, 0b0001, i);
      ack |= i2c_scwritelong(address2, CHANNEL3, 0b0001, i);
      ack |= i2c_scwritelong(address2, CHANNEL4, 0b0001, i);
      ack |= i2c_scwritelong(address2, CHANNEL5, 0b0001, i);
      ack |= i2c_scwritelong(address2, CHANNEL6, 0b0001, i);
      ack |= i2c_scwritelong(address2, CHANNEL7, 0b0001, i);
    }
    else
    {
      ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, i);
      ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, i);
      ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, i);
      ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, i);
      ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, i);
      ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, i);
      ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, i);
      ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, i);
      ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, i);
      ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, i);
      ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, i);
      ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, i);
      ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, i);
      ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, i);
      ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, i);
      ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, i);
    }
  }
}


//! LEDs illuminate around the outside of the board, giving the effect of
//! a comet with a tail traveling around the board. The traveling speed of
//! the comet is controlled by the posititon of the potentiometer. The
//! pattern will continue running after the pattern select switch is changed
//! from this position until the orbit() function has completed its pattern.
void orbit()
{
  setThresh();

  int8_t ack = 0;       // I2C acknowledge bit
  uint8_t i, j, k, l;
  int up_down = 1; //1 = up, 0 = down, 2 = startup wait, 3 = wait
  int delay_var = 5;
  uint8_t ch[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; ;  //channel count variables
  int ud[16] = {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};  //count up or down variables; 1 = up, 0 = down, 2 = wait
  int waitcount[16]; // wait count for unusued LEDs
  int analog_in = analogRead(A3);         // Read value of potentiometer
  An0 = analogRead(A0);       // Read 8 position switch pin 1
  An1 = analogRead(A1);       // Read 8 position switch pin 2
  An2 = analogRead(A2);       // Read 8 position switch pin 3

  waitcount[0] = 0;
  waitcount[1] = 15;
  waitcount[2] = 14;
  waitcount[3] = 13;
  waitcount[4] = 12;
  waitcount[5] = 11;
  waitcount[6] = 10;
  waitcount[7] = 9;
  waitcount[8] = 8;
  waitcount[9] = 7;
  waitcount[10] = 6;
  waitcount[11] = 5;
  waitcount[12] = 4;
  waitcount[13] = 3;
  waitcount[14] = 2;
  waitcount[15] = 1;

  // start with this brightness
  ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, 0);

  ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, 0);


  delay(50);
  if (!startsequence)
  {
    digitalWrite(PWMPIN, HIGH);
    delay(50);
    digitalWrite(LOADPIN, LOW);
    delay(10);
    startsequence = TRUE;
  }
  delay(500);

  for (j = 0; j < 64; j++)
  {
    for (i = 0; i <= 9 ; i++)
    {
      for (k = 0; k <= 15; k++)
      {
        if (ud[k] == 1)
          ch[k] = ch[k] + 25;
        else if (ud[k] == 0)
          ch[k] = ch[k] - 5;
        else
          ch[k] = ch[k];
      }

      if (ch[0] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, ch[0]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, ch[0]);

      if (ch[1] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, ch[1]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, ch[1]);

      if (ch[2] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, ch[2]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, ch[2]);

      if (ch[3] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, ch[3]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, ch[3]);

      if (ch[12] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, ch[12]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, ch[12]);

      if (ch[13] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, ch[13]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, ch[13]);

      if (ch[14] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, ch[14]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, ch[14]);

      if (ch[15] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, ch[15]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, ch[15]);

      if (ch[8] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, ch[8]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, ch[8]);

      if (ch[9] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, ch[9]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, ch[9]);

      if (ch[10] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, ch[10]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, ch[10]);

      if (ch[11] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, ch[11]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, ch[11]);

      if (ch[4] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, ch[4]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, ch[4]);

      if (ch[5] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, ch[5]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, ch[5]);

      if (ch[6] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, ch[6]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, ch[6]);

      if (ch[7] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, ch[7]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, ch[7]);


      analog_in = analogRead(A3);
      delay (analog_in / 24);
    }

    // check to see if up, down, or wait and increment or change
    for (l = 0; l <= 15; l++)
    {
      if (ud[l] == 1 && ch[l] >= 250)
      {
        ud[l] = 0;
        waitcount[l]++;
      }
      else if (ud[l] == 2)
      {
        if (waitcount[l] >= 15)
        {
          ud[l] = 1;
          waitcount[l] = 0;
        }
        else
          waitcount[l]++;
      }
      else if (ud[l] == 0 && ch[l] <= 5)
      {
        ud[l] = 2;
        waitcount[l]++;
      }
      else
      {
        ud[l] = ud[l];
        waitcount[l]++;
      }
    }
  }
  // run to finish then pause
  ud[0] = 2;
  ch[0] = 0;
  waitcount[0] = 1;
  for (j = 0; j < 6; j++)
  {
    for (i = 0; i <= 9 ; i++)
    {
      for (k = 0; k <= 15; k++)
      {
        if (ud[k] == 1)
          ch[k] = ch[k] + 25;
        else if (ud[k] == 0)
          ch[k] = ch[k] - 5;
        else
          ch[k] = ch[k];
      }

      if (ch[0] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, ch[0]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, ch[0]);

      if (ch[1] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, ch[1]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, ch[1]);

      if (ch[2] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, ch[2]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, ch[2]);

      if (ch[3] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, ch[3]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, ch[3]);

      if (ch[12] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, ch[12]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, ch[12]);

      if (ch[13] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, ch[13]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, ch[13]);

      if (ch[14] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, ch[14]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, ch[14]);

      if (ch[15] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, ch[15]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, ch[15]);

      if (ch[8] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, ch[8]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, ch[8]);

      if (ch[9] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, ch[9]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, ch[9]);

      if (ch[10] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, ch[10]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, ch[10]);

      if (ch[11] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, ch[11]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, ch[11]);

      if (ch[4] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, ch[4]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, ch[4]);

      if (ch[5] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, ch[5]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, ch[5]);

      if (ch[6] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, ch[6]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, ch[6]);

      if (ch[7] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, ch[7]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, ch[7]);

      analog_in = analogRead(A3);
      delay (analog_in / 24);
    }

    // check to see if up, down, or wait and increment or change
    for (l = 0; l <= 15; l++)
    {
      if (ud[l] == 1 && ch[l] >= 250)
      {
        ud[l] = 0;
        waitcount[l]++;
      }
      else if (ud[l] == 2)
      {
        if (waitcount[l] >= 15)
        {
          ud[l] = 2;
          waitcount[l] = 0;
        }
        else
          waitcount[l]++;
      }
      else if (ud[l] == 0 && ch[l] <= 5)
      {
        ud[l] = 2;
        waitcount[l]++;
      }
      else
      {
        ud[l] = ud[l];
        waitcount[l]++;
      }
    }
  }
  // restart in the other direction
  for (i = 0; i < 16; ++i)
    ch[i] = 0;


  for (i = 0; i < 15; ++i)
    ud[i] = 2;
  ud[i] = 1;

  waitcount[15] = 0;
  waitcount[14] = 15;
  waitcount[13] = 14;
  waitcount[12] = 13;
  waitcount[11] = 12;
  waitcount[10] = 11;
  waitcount[9] = 10;
  waitcount[8] = 9;
  waitcount[7] = 8;
  waitcount[6] = 7;
  waitcount[5] = 6;
  waitcount[4] = 5;
  waitcount[3] = 4;
  waitcount[2] = 3;
  waitcount[1] = 2;
  waitcount[0] = 1;

  // restart with this brightness for the other direction
  ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, 0);

  ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, 0);

  delay(500);
  for (j = 0; j < 64; j++)
  {
    for (i = 0; i <= 9 ; i++)
    {
      for (k = 0; k <= 15; k++)
      {
        if (ud[k] == 1)
          ch[k] = ch[k] + 25;
        else if (ud[k] == 0)
          ch[k] = ch[k] - 5;
        else ch[k] = ch[k];
      }

      if (ch[0] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, ch[0]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, ch[0]);

      if (ch[1] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, ch[1]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, ch[1]);

      if (ch[2] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, ch[2]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, ch[2]);

      if (ch[3] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, ch[3]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, ch[3]);

      if (ch[12] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, ch[12]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, ch[12]);

      if (ch[13] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, ch[13]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, ch[13]);

      if (ch[14] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, ch[14]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, ch[14]);

      if (ch[15] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, ch[15]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, ch[15]);

      if (ch[8] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, ch[8]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, ch[8]);

      if (ch[9] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, ch[9]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, ch[9]);

      if (ch[10] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, ch[10]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, ch[10]);

      if (ch[11] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, ch[11]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, ch[11]);

      if (ch[4] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, ch[4]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, ch[4]);

      if (ch[5] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, ch[5]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, ch[5]);

      if (ch[6] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, ch[6]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, ch[6]);

      if (ch[7] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, ch[7]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, ch[7]);

      analog_in = analogRead(A3);
      delay (analog_in / 24);
    }

    // check to see if up, down, or wait and increment or change
    for (l = 0; l <= 15; l++)
    {
      /* analog_in = analogRead(A3); */
      if (ud[l] == 1 && ch[l] >= 250)
      {
        ud[l] = 0;
        waitcount[l]++;
      }
      else if (ud[l] == 2)
      {
        if (waitcount[l] >= 15)
        {
          ud[l] = 1;
          waitcount[l] = 0;
        }
        else
          waitcount[l]++;
      }
      else if (ud[l] == 0 && ch[l] <= 5)
      {
        ud[l] = 2;
        waitcount[l]++;
      }
      else
      {
        ud[l] = ud[l];
        waitcount[l]++;
      }
    }
  }
  // run to finish then pause
  ud[15] = 2;
  ch[15] = 0;
  waitcount[15] = 1;
  for (j = 0; j < 6; j++)
  {
    for (i = 0; i <= 9 ; i++)
    {
      for (k = 0; k <= 15; k++)
      {
        if (ud[k] == 1)
          ch[k] = ch[k] + 25;
        else if (ud[k] == 0)
          ch[k] = ch[k] - 5;
        else
          ch[k] = ch[k];
      }

      if (ch[0] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, ch[0]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, ch[0]);

      if (ch[1] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, ch[1]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, ch[1]);

      if (ch[2] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, ch[2]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, ch[2]);

      if (ch[3] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, ch[3]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, ch[3]);

      if (ch[12] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, ch[12]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, ch[12]);

      if (ch[13] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, ch[13]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, ch[13]);

      if (ch[14] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, ch[14]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, ch[14]);

      if (ch[15] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, ch[15]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, ch[15]);

      if (ch[8] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, ch[8]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, ch[8]);

      if (ch[9] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, ch[9]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, ch[9]);

      if (ch[10] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, ch[10]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, ch[10]);

      if (ch[11] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, ch[11]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, ch[11]);

      if (ch[4] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, ch[4]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, ch[4]);

      if (ch[5] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, ch[5]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, ch[5]);

      if (ch[6] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, ch[6]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, ch[6]);

      if (ch[7] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, ch[7]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, ch[7]);

      analog_in = analogRead(A3);
      delay (analog_in / 24);
    }

    // check to see if up, down, or wait and increment or change
    for (l = 0; l <= 15; l++)
    {
      if (ud[l] == 1 && ch[l] >= 250)
      {
        ud[l] = 0;
        waitcount[l]++;
      }
      else if (ud[l] == 2)
      {
        if (waitcount[l] >= 15)
        {
          ud[l] = 2;
          waitcount[l] = 0;
        }
        else
          waitcount[l]++;
      }
      else if (ud[l] == 0 && ch[l] <= 5)
      {
        ud[l] = 2;
        waitcount[l]++;
      }
      else
      {
        ud[l] = ud[l];
        waitcount[l]++;
      }
    }
  }
}

//! Using the PWM dim WITH FADE commands, a wave of light is created by fading
//! the LED brightnesses up and down. WITH FADE option uses the logarithmic
//! fade function of the LT3965 and allows the brightness to fade from low to
//! high or from high to low with a single I2C bus command. This pattern has
//! low bus traffic.
void fadewave()
{
  setThresh();

  int8_t ack = 0;                               // I2C acknowledge bit
  An0 = analogRead(A0);       // Read 8 position switch pin 1
  An1 = analogRead(A1);       // Read 8 position switch pin 2
  An2 = analogRead(A2);       // Read 8 position switch pin 3

  ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, 1);
  ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, 1);
  ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, 1);
  ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, 1);
  ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, 1);
  ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, 1);
  ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, 1);
  ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, 1);
  ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, 1);
  ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, 1);
  ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, 1);
  ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, 1);
  ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, 1);
  ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, 1);
  ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, 1);
  ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, 1);
  delay(250);

  if (!startsequence)
  {
    digitalWrite(PWMPIN, HIGH);
    delay(30);
    digitalWrite(LOADPIN, LOW);
    delay(30);
    startsequence = TRUE;
    delay(150);
  }

  while (An0 < 120 && An1 > 120 && An2 > 120)
  {
    An0 = analogRead(A0);
    An1 = analogRead(A1);
    An2 = analogRead(A2);

    ack |= i2c_scwritelong(address1, CHANNEL0, 0b0011, 255);
    ack |= i2c_scwritelong(address1, CHANNEL1, 0b0011, 255);
    delay(200);
    ack |= i2c_scwritelong(address1, CHANNEL2, 0b0011, 255);
    ack |= i2c_scwritelong(address1, CHANNEL3, 0b0011, 255);
    delay(200);
    ack |= i2c_scwritelong(address1, CHANNEL4, 0b0011, 255);
    ack |= i2c_scwritelong(address1, CHANNEL5, 0b0011, 255);
    delay(200);
    ack |= i2c_scwritelong(address1, CHANNEL6, 0b0011, 255);
    ack |= i2c_scwritelong(address1, CHANNEL7, 0b0011, 255);
    delay(200);
    ack |= i2c_scwritelong(address2, CHANNEL0, 0b0011, 255);
    ack |= i2c_scwritelong(address2, CHANNEL1, 0b0011, 255);
    delay(200);
    ack |= i2c_scwritelong(address2, CHANNEL2, 0b0011, 255);
    ack |= i2c_scwritelong(address2, CHANNEL3, 0b0011, 255);
    delay(200);
    ack |= i2c_scwritelong(address2, CHANNEL4, 0b0011, 255);
    ack |= i2c_scwritelong(address2, CHANNEL5, 0b0011, 255);
    delay(200);
    ack |= i2c_scwritelong(address2, CHANNEL6, 0b0011, 255);
    ack |= i2c_scwritelong(address2, CHANNEL7, 0b0011, 255);
    delay(1000);

    An0 = analogRead(A0);
    An1 = analogRead(A1);
    An2 = analogRead(A2);

    ack |= i2c_scwritelong(address1, CHANNEL0, 0b0011, 1);
    ack |= i2c_scwritelong(address1, CHANNEL1, 0b0011, 1);
    delay(200);
    ack |= i2c_scwritelong(address1, CHANNEL2, 0b0011, 1);
    ack |= i2c_scwritelong(address1, CHANNEL3, 0b0011, 1);
    delay(200);
    ack |= i2c_scwritelong(address1, CHANNEL4, 0b0011, 1);
    ack |= i2c_scwritelong(address1, CHANNEL5, 0b0011, 1);
    delay(200);
    ack |= i2c_scwritelong(address1, CHANNEL6, 0b0011, 1);
    ack |= i2c_scwritelong(address1, CHANNEL7, 0b0011, 1);
    delay(200);
    ack |= i2c_scwritelong(address2, CHANNEL0, 0b0011, 1);
    ack |= i2c_scwritelong(address2, CHANNEL1, 0b0011, 1);
    delay(200);
    ack |= i2c_scwritelong(address2, CHANNEL2, 0b0011, 1);
    ack |= i2c_scwritelong(address2, CHANNEL3, 0b0011, 1);
    delay(200);
    ack |= i2c_scwritelong(address2, CHANNEL4, 0b0011, 1);
    ack |= i2c_scwritelong(address2, CHANNEL5, 0b0011, 1);
    delay(200);
    ack |= i2c_scwritelong(address2, CHANNEL6, 0b0011, 1);
    ack |= i2c_scwritelong(address2, CHANNEL7, 0b0011, 1);
    delay(1000);

    An0 = analogRead(A0);
    An1 = analogRead(A1);
    An2 = analogRead(A2);
  }
}

//! A wave of light from right to left and from left to right changing PWM
//! dimming brightness quickly increasing and decreasing at rates that look
//! like a wave running from one side of the LEDs to the other. The interactive
//! potentiometer controls the speed of the wave.
void wave()  // wave function
{
  setThresh();

  int8_t ack = 0;       // I2C acknowledge bit
  uint8_t i, j, k, l;
  int up_down = 1; //1 = up, 0 = down, 2 = startup wait, 3 = wait
  int delay_var = 20;
  uint8_t ch[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  ;  //channel count variables
  int ud[16] = {1, 2, 2, 2, 2, 2, 2, 2, 2,  2, 2, 2, 2, 2, 2, 1};  //count up or down variables; 1 = up, 0 = down, 2 = wait
  int waitcount[16]; // wait count for unusued LEDs
  uint16_t analog_in;  // potentiometer reading 0 - 255?

  waitcount[0] = -8;
  waitcount[1] = 15;
  waitcount[2] = 14;
  waitcount[3] = 13;
  waitcount[4] = 12;
  waitcount[5] = 11;
  waitcount[6] = 10;
  waitcount[7] = 9;
  waitcount[8] = 9;
  waitcount[9] = 10;
  waitcount[10] = 11;
  waitcount[11] = 12;
  waitcount[12] = 13;
  waitcount[13] = 14;
  waitcount[14] = 15;
  waitcount[15] = -8;

  /// start with this brightness
  // delay (1000);
  if (ch[0] == 0)
    ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, ch[0]);
  else
    ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, ch[0]);

  if (ch[1] == 0)
    ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, ch[1]);
  else
    ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, ch[1]);

  if (ch[2] == 0)
    ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, ch[2]);
  else
    ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, ch[2]);

  if (ch[3] == 0)
    ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, ch[3]);
  else
    ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, ch[3]);


  if (ch[4] == 0)
    ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, ch[4]);
  else
    ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, ch[4]);

  if (ch[5] == 0)
    ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, ch[5]);
  else
    ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, ch[5]);

  if (ch[6] == 0)
    ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, ch[6]);
  else
    ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, ch[6]);

  if (ch[7] == 0)
    ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, ch[7]);
  else
    ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, ch[7]);

  if (ch[8] == 0)
    ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, ch[8]);
  else
    ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, ch[8]);

  if (ch[9] == 0)
    ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, ch[9]);
  else
    ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, ch[9]);

  if (ch[10] == 0)
    ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, ch[10]);
  else
    ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, ch[10]);

  if (ch[11] == 0)
    ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, ch[11]);
  else
    ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, ch[11]);

  if (ch[12] == 0)
    ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, ch[12]);
  else
    ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, ch[12]);

  if (ch[13] == 0)
    ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, ch[13]);
  else
    ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, ch[13]);

  if (ch[14] == 0)
    ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, ch[14]);
  else
    ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, ch[14]);

  if (ch[15] == 0)
    ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, ch[15]);
  else
    ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, ch[15]);

  if (!startsequence)
  {
    digitalWrite(PWMPIN, HIGH);
    delay(1);
    digitalWrite(LOADPIN, LOW);
    delay(1);
    startsequence = TRUE;
  }
  for (j = 0; j < 21; j++)
  {
    for (i = 0; i <= 4 ; i++)
    {
      for (k = 0; k <= 15; k++)
      {
        if (ud[k] == 1)
          ch[k] = ch[k] + 50;
        else if (ud[k] == 0)
          ch[k] = ch[k] - 5;
        else
          ch[k] = ch[k];
      }

      if (ch[0] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, ch[0]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, ch[0]);

      if (ch[1] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, ch[1]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, ch[1]);

      if (ch[2] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, ch[2]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, ch[2]);

      if (ch[3] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, ch[3]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, ch[3]);


      if (ch[4] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, ch[4]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, ch[4]);

      if (ch[5] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, ch[5]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, ch[5]);

      if (ch[6] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, ch[6]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, ch[6]);

      if (ch[7] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, ch[7]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, ch[7]);

      if (ch[8] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, ch[8]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, ch[8]);

      if (ch[9] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, ch[9]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, ch[9]);

      if (ch[10] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, ch[10]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, ch[10]);

      if (ch[11] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, ch[11]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, ch[11]);

      if (ch[12] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, ch[12]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, ch[12]);

      if (ch[13] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, ch[13]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, ch[13]);

      if (ch[14] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, ch[14]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, ch[14]);

      if (ch[15] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, ch[15]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, ch[15]);

      analog_in = analogRead(A3);
      delay (analog_in / 100);
    }

    // check to see if up, down, or wait and increment or change
    for (l = 0; l <= 15; l++)
    {
      if (ud[l] == 1 && ch[l] >= 250)
      {
        ud[l] = 0;
        waitcount[l]++;
      }
      else if (ud[l] == 2)
      {
        if (waitcount[l] >= 15)
        {
          ud[l] = 1;
          waitcount[l] = -8;
        }
        else
          waitcount[l]++;
      }
      else if (ud[l] == 0 && ch[l] <= 5)
      {
        ud[l] = 2;
        waitcount[l]++;
      }
      else
      {
        ud[l] = ud[l];
        waitcount[l]++;
      }
    }
  }
  // delay (1000);
  delay(50);
  // Run Wave Backwards
  waitcount[0] = 9;
  waitcount[1] = 10;
  waitcount[2] = 11;
  waitcount[3] = 12;
  waitcount[4] = 13;
  waitcount[5] = 14;
  waitcount[6] = 15;
  waitcount[7] = -8;
  waitcount[8] = -8;
  waitcount[9] = 15;
  waitcount[10] = 14;
  waitcount[11] = 13;
  waitcount[12] = 12;
  waitcount[13] = 11;
  waitcount[14] = 10;
  waitcount[15] = 9;


  ud[0] = 2;
  ud[1] = 2;
  ud[2] = 2;
  ud[3] = 2;
  ud[4] = 2;
  ud[5] = 2;
  ud[6] = 2;
  ud[7] = 1;
  ud[8] = 1;
  ud[9] = 2;
  ud[10] = 2;
  ud[11] = 2;
  ud[12] = 2;
  ud[13] = 2;
  ud[14] = 2;
  ud[15] = 2;


  /// start with this brightness
  if (ch[0] == 0)
    ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, ch[0]);
  else
    ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, ch[0]);

  if (ch[1] == 0)
    ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, ch[1]);
  else
    ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, ch[1]);

  if (ch[2] == 0)
    ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, ch[2]);
  else
    ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, ch[2]);

  if (ch[3] == 0)
    ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, ch[3]);
  else
    ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, ch[3]);


  if (ch[4] == 0)
    ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, ch[4]);
  else
    ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, ch[4]);

  if (ch[5] == 0)
    ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, ch[5]);
  else
    ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, ch[5]);

  if (ch[6] == 0)
    ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, ch[6]);
  else
    ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, ch[6]);

  if (ch[7] == 0)
    ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, ch[7]);
  else
    ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, ch[7]);

  if (ch[8] == 0)
    ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, ch[8]);
  else
    ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, ch[8]);

  if (ch[9] == 0)
    ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, ch[9]);
  else
    ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, ch[9]);

  if (ch[10] == 0)
    ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, ch[10]);
  else
    ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, ch[10]);

  if (ch[11] == 0)
    ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, ch[11]);
  else
    ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, ch[11]);

  if (ch[12] == 0)
    ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, ch[12]);
  else
    ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, ch[12]);

  if (ch[13] == 0)
    ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, ch[13]);
  else
    ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, ch[13]);

  if (ch[14] == 0)
    ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, ch[14]);
  else
    ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, ch[14]);

  if (ch[15] == 0)
    ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, ch[15]);
  else
    ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, ch[15]);

  for (j = 0; j < 21; j++)
  {
    for (i = 0; i <= 4; i++)
    {
      for (k = 0; k <= 15; k++)
      {
        if (ud[k] == 1)
          ch[k] = ch[k] + 50;
        else if (ud[k] == 0)
          ch[k] = ch[k] - 5;
        else
          ch[k] = ch[k];
      }

      if (ch[0] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, ch[0]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL0, 0b0010, ch[0]);

      if (ch[1] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, ch[1]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL3, 0b0010, ch[1]);

      if (ch[2] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, ch[2]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL4, 0b0010, ch[2]);

      if (ch[3] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, ch[3]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL7, 0b0010, ch[3]);

      if (ch[4] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, ch[4]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL0, 0b0010, ch[4]);

      if (ch[5] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, ch[5]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL3, 0b0010, ch[5]);

      if (ch[6] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, ch[6]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL4, 0b0010, ch[6]);

      if (ch[7] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, ch[7]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL7, 0b0010, ch[7]);

      if (ch[8] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, ch[8]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL6, 0b0010, ch[8]);

      if (ch[9] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, ch[9]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL5, 0b0010, ch[9]);

      if (ch[10] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, ch[10]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL2, 0b0010, ch[10]);

      if (ch[11] == 0)
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, ch[11]);
      else
        ack |= i2c_scwritelong(address2, CHANNEL1, 0b0010, ch[11]);

      if (ch[12] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, ch[12]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL6, 0b0010, ch[12]);

      if (ch[13] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, ch[13]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL5, 0b0010, ch[13]);

      if (ch[14] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, ch[14]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL2, 0b0010, ch[14]);

      if (ch[15] == 0)
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, ch[15]);
      else
        ack |= i2c_scwritelong(address1, CHANNEL1, 0b0010, ch[15]);

      analog_in = analogRead(A3);
      delay (analog_in / 100);
    }

    // check to see if up, down, or wait and increment or change
    for (l = 0; l <= 15; l++)
    {
      if (ud[l] == 1 && ch[l] >= 250)
      {
        ud[l] = 0;
        waitcount[l]++;
      }
      else if (ud[l] == 2)
      {
        if (waitcount[l] >= 15)
        {
          ud[l] = 1;
          waitcount[l] = -8;
        }
        else
          waitcount[l]++;
      }
      else if (ud[l] == 0 && ch[l] <= 5)
      {
        ud[l] = 2;
        waitcount[l]++;
      }
      else
      {
        ud[l] = ud[l];
        waitcount[l]++;
      }
    }
  }

}


//! Performs an analog read on the potentiometer pin using hysteresis to
//! reduce variation in ADC values. This prevents the read value of the potentiomter
//! from bouncing between different values when the potentiometer is not
//! being moved.
int readpot(void)
{
  uint8_t tempupdown;
  currentread = analogRead(A3);

  if ((currentread -  lastread) > 0)
    tempupdown = UP;
  if ((currentread - lastread) < 0)
    tempupdown = DOWN;

  if (tempupdown  == updown)
  {
    lastread = currentread;
    return (lastread);
  }
  else
  {
    if (((abs(currentread -  lastread)) > 3))
    {
      if (updown == UP)
        updown = DOWN;
      else if (updown == DOWN)
        updown = UP;
      lastread = currentread;
      return (lastread);
    }
    else
    {
      return (lastread);
    }
  }
}

//! Sets the short circuit and open circuit thresholds defined for
//! DC2218A. This function performs a series of single channel and all
//! channel writes to ensure that threshold registers are set, as well
//! as ensuring all fault registers are cleared.
void setThresh()
{
  int8_t ack = 0;

  ack |= i2c_scwriteshort(address1, CHANNEL0, 0b0000);
  ack |= i2c_scwriteshort(address1, CHANNEL1, 0b0000);
  ack |= i2c_scwriteshort(address1, CHANNEL2, 0b0000);
  ack |= i2c_scwriteshort(address1, CHANNEL3, 0b0000);
  ack |= i2c_scwriteshort(address1, CHANNEL4, 0b0000);
  ack |= i2c_scwriteshort(address1, CHANNEL5, 0b0000);
  ack |= i2c_scwriteshort(address1, CHANNEL6, 0b0000);
  ack |= i2c_scwriteshort(address1, CHANNEL7, 0b0000);

  ack |= i2c_scwriteshort(address2, CHANNEL0, 0b0000);
  ack |= i2c_scwriteshort(address2, CHANNEL1, 0b0000);
  ack |= i2c_scwriteshort(address2, CHANNEL2, 0b0000);
  ack |= i2c_scwriteshort(address2, CHANNEL3, 0b0000);
  ack |= i2c_scwriteshort(address2, CHANNEL4, 0b0000);
  ack |= i2c_scwriteshort(address2, CHANNEL5, 0b0000);
  ack |= i2c_scwriteshort(address2, CHANNEL6, 0b0000);
  ack |= i2c_scwriteshort(address2, CHANNEL7, 0b0000);

  ack |= i2c_scwritelong(address1, CHANNEL0, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL1, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL2, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL3, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL4, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL5, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL6, 0b0000, 0);
  ack |= i2c_scwritelong(address1, CHANNEL7, 0b0000, 0);

  ack |= i2c_scwritelong(address2, CHANNEL0, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL1, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL2, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL3, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL4, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL5, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL6, 0b0000, 0);
  ack |= i2c_scwritelong(address2, CHANNEL7, 0b0000, 0);

  ack |= i2c_acwrite(address1, 0b00000000);
  ack |= i2c_acwrite(address2, 0b00000000);
}

