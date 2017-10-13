/***************************************************
  This is a library for the Adafruit Capacitive Touch Screens

  ----> http://www.adafruit.com/products/1947

  Check out the links above for our tutorials and wiring diagrams
  This chipset uses I2C to communicate

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

#include <Adafruit_FT6206.h>

#if defined(__SAM3X8E__)
#define Wire Wire1
#endif

/**************************************************************************/
/*!
    @brief  Instantiates a new FT6206 class
*/
/**************************************************************************/
// I2C, no address adjustments or pins
Adafruit_FT6206::Adafruit_FT6206()
{
}


/**************************************************************************/
/*!
    @brief  Setups the HW
*/
/**************************************************************************/
boolean Adafruit_FT6206::begin(uint8_t threshhold)
{
  Wire.begin();

  // change threshhold to be higher/lower
  writeRegister8(FT6206_REG_THRESHHOLD, threshhold);

  if ((readRegister8(FT6206_REG_VENDID) != 17) || (readRegister8(FT6206_REG_CHIPID) != 6)) return false;
  /*
  Serial.print("Vend ID: "); Serial.println(readRegister8(FT6206_REG_VENDID));
  Serial.print("Chip ID: "); Serial.println(readRegister8(FT6206_REG_CHIPID));
  Serial.print("Firm V: "); Serial.println(readRegister8(FT6206_REG_FIRMVERS));
  Serial.print("Point Rate Hz: "); Serial.println(readRegister8(FT6206_REG_POINTRATE));
  Serial.print("Thresh: "); Serial.println(readRegister8(FT6206_REG_THRESHHOLD));
  */
  // dump all registers
  /*
  for (int16_t i=0; i<0x20; i++) {
    Serial.print("I2C $"); Serial.print(i, HEX);
    Serial.print(" = 0x"); Serial.println(readRegister8(i), HEX);
  }
  */
  return true;
}

// DONT DO THIS - REALLY - IT DOESNT WORK
void Adafruit_FT6206::autoCalibrate(void)
{
  writeRegister8(FT6206_REG_MODE, FT6206_REG_FACTORYMODE);
  delay(100);
//Serial.println("Calibrating...");
  writeRegister8(FT6206_REG_CALIBRATE, 4);
  delay(300);
  for (uint8_t i = 0; i < 100; i++)
  {
    uint8_t temp;
    temp = readRegister8(FT6206_REG_MODE);
    Serial.println(temp, HEX);
    //return to normal mode, calibration finish
    if (0x0 == ((temp & 0x70) >> 4))
      break;
  }
  delay(200);
//Serial.println("Calibrated");
  delay(300);
  writeRegister8(FT6206_REG_MODE, FT6206_REG_FACTORYMODE);
  delay(100);
  writeRegister8(FT6206_REG_CALIBRATE, 5);
  delay(300);
  writeRegister8(FT6206_REG_MODE, FT6206_REG_WORKMODE);
  delay(300);
}


boolean Adafruit_FT6206::touched(void)
{

  uint8_t n = readRegister8(FT6206_REG_NUMTOUCHES);
  if ((n == 1) || (n == 2)) return true;
  return false;
}

/*****************************/

void Adafruit_FT6206::readData(uint16_t *x, uint16_t *y)
{

  uint8_t i2cdat[16];
  Wire.beginTransmission(FT6206_ADDR);
  Wire.write((byte)0);
  Wire.endTransmission();
  Wire.beginTransmission(FT6206_ADDR);
  Wire.requestFrom((byte)FT6206_ADDR, (byte)32);
  for (uint8_t i=0; i<16; i++)
    i2cdat[i] = Wire.read();
  Wire.endTransmission();

  /*
  for (int16_t i=0; i<0x20; i++) {
    Serial.print("I2C $"); Serial.print(i, HEX); Serial.print(" = 0x"); Serial.println(i2cdat[i], HEX);
  }
  */

  touches = i2cdat[0x02];

  //Serial.println(touches);
  if (touches > 2)
  {
    touches = 0;
    *x = *y = 0;
  }
  if (touches == 0)
  {
    *x = *y = 0;
    return;
  }

  /*
  if (touches == 2) Serial.print('2');
  for (uint8_t i=0; i<16; i++) {
   // Serial.print("0x"); Serial.print(i2cdat[i], HEX); Serial.print(" ");
  }
  */

  /*
  Serial.println();
  if (i2cdat[0x01] != 0x00) {
    Serial.print("Gesture #");
    Serial.println(i2cdat[0x01]);
  }
  */

  //Serial.print("# Touches: "); Serial.print(touches);
  for (uint8_t i=0; i<2; i++)
  {
    touchX[i] = i2cdat[0x03 + i*6] & 0x0F;
    touchX[i] <<= 8;
    touchX[i] |= i2cdat[0x04 + i*6];
    touchY[i] = i2cdat[0x05 + i*6] & 0x0F;
    touchY[i] <<= 8;
    touchY[i] |= i2cdat[0x06 + i*6];
    touchID[i] = i2cdat[0x05 + i*6] >> 4;
  }
  /*
  Serial.println();
  for (uint8_t i=0; i<touches; i++) {
    Serial.print("ID #"); Serial.print(touchID[i]); Serial.print("\t("); Serial.print(touchX[i]);
    Serial.print(", "); Serial.print(touchY[i]);
    Serial.print (") ");
  }
  Serial.println();
  */
  *x = touchX[0];
  *y = touchY[0];
}

TS_Point Adafruit_FT6206::getPoint(void)
{
  uint16_t x, y;
  uint8_t z;
  readData(&x, &y);
  return TS_Point(x, y, 1);
}


uint8_t Adafruit_FT6206::readRegister8(uint8_t reg)
{
  uint8_t x ;
  // use i2c
  Wire.beginTransmission(FT6206_ADDR);
  Wire.write((byte)reg);
  Wire.endTransmission();
  Wire.beginTransmission(FT6206_ADDR);
  Wire.requestFrom((byte)FT6206_ADDR, (byte)1);
  x = Wire.read();
  Wire.endTransmission();

  //  Serial.print("$"); Serial.print(reg, HEX);
  //  Serial.print(": 0x"); Serial.println(x, HEX);

  return x;
}

void Adafruit_FT6206::writeRegister8(uint8_t reg, uint8_t val)
{
  // use i2c
  Wire.beginTransmission(FT6206_ADDR);
  Wire.write((byte)reg);
  Wire.write((byte)val);
  Wire.endTransmission();
}

/****************/

TS_Point::TS_Point(void)
{
  x = y = 0;
}

TS_Point::TS_Point(int16_t x0, int16_t y0, int16_t z0)
{
  x = x0;
  y = y0;
  z = z0;
}

bool TS_Point::operator==(TS_Point p1)
{
  return  ((p1.x == x) && (p1.y == y) && (p1.z == z));
}

bool TS_Point::operator!=(TS_Point p1)
{
  return  ((p1.x != x) || (p1.y != y) || (p1.z != z));
}
