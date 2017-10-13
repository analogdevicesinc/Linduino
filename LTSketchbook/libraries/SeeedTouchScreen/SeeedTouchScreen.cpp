/*
  SeeedTouchScreen.cpp - Library for 4-line resistance touch screen.
  Modified by loovee Aug 12, 2012.
  (c) ladyada / adafruit
  Code under MIT License.
*/

#include "pins_arduino.h"
#include "wiring_private.h"
#include <avr/pgmspace.h>
#include "SeeedTouchScreen.h"

// increase or decrease the touchscreen oversampling. This is a little different than you make think:
// 1 is no oversampling, whatever data we get is immediately returned
// 2 is double-sampling and we only return valid data if both points are the same
// 3+ uses insert sort to get the median value.
// We found 2 is precise yet not too slow so we suggest sticking with it!

#define NUMSAMPLES 2    // sample number
#define COMP       2
#define AVERAGE    1
#define RXPLATE    300
#define TSDEBUG    0    // if print the debug information
Point::Point(void)
{
  x = y = 0;
}

Point::Point(int x0, int y0, int z0)
{
  x = x0;
  y = y0;
  z = z0;
}

bool Point::operator==(Point p1)
{
  return  ((p1.x == x) && (p1.y == y) && (p1.z == z));
}

bool Point::operator!=(Point p1)
{
  return  ((p1.x != x) || (p1.y != y) || (p1.z != z));
}

TouchScreen::TouchScreen(uint8_t xp, uint8_t yp, uint8_t xm, uint8_t ym)
{
  _yp = yp;
  _xm = xm;
  _ym = ym;
  _xp = xp;
}

#if AVERAGE
#define AVERAGETIME 4
int avr_analog(int adpin)
{
  int sum = 0;
  int max = 0;
  int min = 1024;
  for (int i = 0; i<AVERAGETIME; i++)
  {
    int tmp = analogRead(adpin);
    if (tmp > max)max = tmp;
    if (tmp < min)min = tmp;
    sum += tmp;
    //   sum+=analogRead(adpin);
  }
  return (sum-min-max)/(AVERAGETIME-2);

}
#endif

Point TouchScreen::getPoint(void)
{
  int x, y, z = 1;
  int samples[NUMSAMPLES];
#if TSDEBUG
  int xx[2] = {0, 0};
  int yy[2] = {0, 0};
#endif
  uint8_t i, valid;

  uint8_t xp_port = digitalPinToPort(_xp);
  unsigned char yp_port = digitalPinToPort(_yp);
  unsigned char xm_port = digitalPinToPort(_xm);
  unsigned char ym_port = digitalPinToPort(_ym);

  unsigned char xp_pin = digitalPinToBitMask(_xp);
  unsigned char yp_pin = digitalPinToBitMask(_yp);
  unsigned char xm_pin = digitalPinToBitMask(_xm);
  unsigned char ym_pin = digitalPinToBitMask(_ym);
  valid = 1;
  pinMode(_yp, INPUT);
  pinMode(_ym, INPUT);

  *portOutputRegister(yp_port) &= ~yp_pin;
  *portOutputRegister(ym_port) &= ~ym_pin;

  pinMode(_xp, OUTPUT);
  pinMode(_xm, OUTPUT);

  *portOutputRegister(xp_port) |= xp_pin;
  *portOutputRegister(xm_port) &= ~xm_pin;

  for (i=0; i<NUMSAMPLES; i++)
  {
#if AVERAGE
    samples[i] = avr_analog(_yp);
#else
    samples[i] = analogRead(_yp);
#endif

#if TSDEBUG
    xx[i] = samples[i];
#endif
  }

#if !COMP
  if (samples[0] != samples[1])
  {
    valid = 0;
  }
#else
  int icomp = samples[0]>samples[1]?samples[0]-samples[1]:samples[1] - samples[0];
  if (icomp > COMP)valid = 0;
#endif

  x = (samples[0] + samples[1]);

  pinMode(_xp, INPUT);
  pinMode(_xm, INPUT);
  *portOutputRegister(xp_port) &= ~xp_pin;

  pinMode(_yp, OUTPUT);
  *portOutputRegister(yp_port) |= yp_pin;
  pinMode(_ym, OUTPUT);

  for (i=0; i<NUMSAMPLES; i++)
  {
#if AVERAGE
    samples[i] = avr_analog(_xm);
#else
    samples[i] = analogRead(_xm);
#endif
#if TSDEBUG
    yy[i] = samples[i];
#endif
  }

#if !COMP
  if (samples[0] != samples[1])
  {
    valid = 0;
  }
#else
  icomp = samples[0]>samples[1]?samples[0]-samples[1]:samples[1] - samples[0];
  if (icomp>COMP)valid = 0;
#endif
  y = (samples[0]+samples[0]);

  pinMode(_xp, OUTPUT);
  *portOutputRegister(xp_port) &= ~xp_pin;            // Set X+ to ground
  *portOutputRegister(ym_port) |=  ym_pin;            // Set Y- to VCC
  *portOutputRegister(yp_port) &= ~yp_pin;            // Hi-Z X- and Y+
  pinMode(_yp, INPUT);

  int z1          = analogRead(_xm);
  int z2          = analogRead(_yp);
  float rtouch    = 0;

  rtouch  = z2;
  rtouch /= z1;
  rtouch -= 1;
  rtouch *= (2046-x)/2;
  rtouch *= RXPLATE;
  rtouch /= 1024;
  z = rtouch;
  if (! valid)
  {
    z = 0;
  }

#if TSDEBUG
  if (z > __PRESURE)
  {
    Serial.print("x1 = ");
    Serial.print(xx[0]);
    Serial.print("\tx2 = ");
    Serial.print(xx[1]);
    Serial.print("\ty2 = ");
    Serial.print(yy[0]);
    Serial.print("\ty2 = ");
    Serial.println(yy[1]);
  }
#endif

  return Point(x, y, z);
}

bool TouchScreen::isTouching(void)
{
  Point p = getPoint();
  if (p.z > __PRESURE)return 1;
  else return 0;
}
