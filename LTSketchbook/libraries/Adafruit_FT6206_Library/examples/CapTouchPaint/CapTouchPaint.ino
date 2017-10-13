/***************************************************
  This is our touchscreen painting example for the Adafruit ILI9341
  captouch shield
  ----> http://www.adafruit.com/products/1947

  Check out the links above for our tutorials and wiring diagrams

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>       // this is needed for display
#include <Adafruit_ILI9341.h>
#include <Wire.h>      // this is needed for FT6206
#include <Adafruit_FT6206.h>

// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ctp = Adafruit_FT6206();

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define PENRADIUS 3
int oldcolor, currentcolor;

void setup(void)
{
  while (!Serial);     // used for leonardo debugging

  Serial.begin(115200);
  Serial.println(F("Cap Touch Paint!"));

  tft.begin();

  if (! ctp.begin(40))    // pass in 'sensitivity' coefficient
  {
    Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1);
  }

  Serial.println("Capacitive touchscreen started");

  tft.fillScreen(ILI9341_BLACK);

  // make the color selection boxes
  tft.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
  tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
  tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_GREEN);
  tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_CYAN);
  tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_BLUE);
  tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);

  // select the current color 'red'
  tft.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
  currentcolor = ILI9341_RED;
}

void loop()
{
  // Wait for a touch
  if (! ctp.touched())
  {
    return;
  }

  // Retrieve a point
  TS_Point p = ctp.getPoint();

  /*
   // Print out raw data from screen touch controller
   Serial.print("X = "); Serial.print(p.x);
   Serial.print("\tY = "); Serial.print(p.y);
   Serial.print(" -> ");
  */

  // flip it around to match the screen.
  p.x = map(p.x, 0, 240, 240, 0);
  p.y = map(p.y, 0, 320, 320, 0);

  // Print out the remapped (rotated) coordinates
  Serial.print("(");
  Serial.print(p.x);
  Serial.print(", ");
  Serial.print(p.y);
  Serial.println(")");


  if (p.y < BOXSIZE)
  {
    oldcolor = currentcolor;

    if (p.x < BOXSIZE)
    {
      currentcolor = ILI9341_RED;
      tft.drawRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
    }
    else if (p.x < BOXSIZE*2)
    {
      currentcolor = ILI9341_YELLOW;
      tft.drawRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
    }
    else if (p.x < BOXSIZE*3)
    {
      currentcolor = ILI9341_GREEN;
      tft.drawRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
    }
    else if (p.x < BOXSIZE*4)
    {
      currentcolor = ILI9341_CYAN;
      tft.drawRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
    }
    else if (p.x < BOXSIZE*5)
    {
      currentcolor = ILI9341_BLUE;
      tft.drawRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
    }
    else if (p.x <= BOXSIZE*6)
    {
      currentcolor = ILI9341_MAGENTA;
      tft.drawRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_WHITE);
    }

    if (oldcolor != currentcolor)
    {
      if (oldcolor == ILI9341_RED)
        tft.fillRect(0, 0, BOXSIZE, BOXSIZE, ILI9341_RED);
      if (oldcolor == ILI9341_YELLOW)
        tft.fillRect(BOXSIZE, 0, BOXSIZE, BOXSIZE, ILI9341_YELLOW);
      if (oldcolor == ILI9341_GREEN)
        tft.fillRect(BOXSIZE*2, 0, BOXSIZE, BOXSIZE, ILI9341_GREEN);
      if (oldcolor == ILI9341_CYAN)
        tft.fillRect(BOXSIZE*3, 0, BOXSIZE, BOXSIZE, ILI9341_CYAN);
      if (oldcolor == ILI9341_BLUE)
        tft.fillRect(BOXSIZE*4, 0, BOXSIZE, BOXSIZE, ILI9341_BLUE);
      if (oldcolor == ILI9341_MAGENTA)
        tft.fillRect(BOXSIZE*5, 0, BOXSIZE, BOXSIZE, ILI9341_MAGENTA);
    }
  }
  if (((p.y-PENRADIUS) > BOXSIZE) && ((p.y+PENRADIUS) < tft.height()))
  {
    tft.fillCircle(p.x, p.y, PENRADIUS, currentcolor);
  }
}