#include <16F877a.h>
#device adc=16

#FUSES NOWDT                 	//No Watch Dog Timer
#FUSES NOBROWNOUT            	//No brownout reset
#FUSES NOLVP                 	//No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O

#use delay(clock=20000000)
#use I2C(master, sda=PIN_C5, scl=PIN_C3)

#type int=16, long=32

#define MISO PIN_C4
#define MOSI PIN_C5
#define CS   PIN_B5
#define SCK  PIN_C3
#define SCL  PIN_C3
#define SDAO PIN_C5
#define SDAI PIN_C4

#include  <math.h>

