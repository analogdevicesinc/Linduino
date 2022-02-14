/************************************
REVISION HISTORY
$Revision: 3659 $
$Date: 2017-04-13

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

Copyright 2017 Linear Technology Corp. (LTC)
***********************************************************/

/*! @file
    @ingroup DC1304
    Header for DC1304 4-way wide CM current sense with AZ
*/

#include "LTC6603.h"
#include "LTC6602.h"
#include "LTC6903.h"
#include "LT_SPI.h"

#ifndef DC1304_H
#define DC1304_H

#define NUMBITS 10
#define MAXADCVAL ((2^NUMBITS) - 1)

#define LTC6903_CS QUIKEVAL_GPIO
#define LTC6603_CS QUIKEVAL_CS

class LTC6603
{
  uint8_t cs;
  uint8_t control_byte;	// Local copy of the LTC6603 control byte.
  uint16_t multiplier[3];
  uint8_t LPF_bits[3] = {LTC6603_LPF0, LTC6603_LPF1, LTC6603_LPF2};
  uint8_t HPF_bits[4] = {LTC6602_HPF0, LTC6602_HPF1, LTC6602_HPF2, LTC6602_HPF3};
  
  public:
    
	LTC6603(uint8_t device_cs);
	
	uint8_t set_control_byte(uint8_t mask, uint8_t mask_value);
	
	void set_multiplier(uint16_t m1, uint16_t m2, uint16_t m3);
	
	bool check_device_ready();
	
	void print_control_byte();
	
	uint16_t get_multiplier(uint8_t frange) { return multiplier[frange]; }
	
	uint8_t get_LPF_bits(uint8_t frange) { return LPF_bits[frange]; }
	
	uint8_t get_HPF_bits(uint8_t frange) { return HPF_bits[frange]; }
};

class LTC6903
{
  uint8_t clock_cs;
  public:
    
	LTC6903(uint8_t device_cs);
	
	void set_frequency(float freq);
	
};




/*!

 Pin Mapping
*/



#define DC1304_VIN 10  //!< Maps to DC2026 J1.6
#define DC1304_IIN 9   //PORTB1   //!< Maps to DC2026 J1.14




#endif
