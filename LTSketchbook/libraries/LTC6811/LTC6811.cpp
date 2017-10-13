/*!
LTC6811-1: Multicell Battery Monitor
@verbatim
  The LTC6811 is a 3rd generation multicell battery stack
  monitor that measures up to 12 series connected battery
  cells with a total measurement error of less than 1.2mV. The
  cell measurement range of 0V to 5V makes the LTC6811
  suitable for most battery chemistries. All 12 cell voltages
  can be captured in 290uS, and lower data acquisition rates
  can be selected for high noise reduction.

  Using the LTC6811-1, multiple devices are connected in
  a daisy-chain with one host processor connection for all
  devices.
@endverbatim

http://www.linear.com/product/LTC6811-1

http://www.linear.com/product/LTC6811-1#demoboards

REVISION HISTORY
$Revision: 7139 $
$Date: 2017-06-01 13:55:14 -0700 (Thu, 01 Jun 2017) $

Copyright (c) 2017, Linear Technology Corp.(LTC)
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

//! @ingroup BMS
//! @{
//! @defgroup LTC6811-1 LTC6811-1: Multicell Battery Monitor
//! @}

/*! @file
    @ingroup LTC6811-1
    Library for LTC6811-1 Multicell Battery Monitor
*/

#include "stdint.h"
#include "LTC681x.h"
#include "LTC6811.h"


void LTC6811_init_reg_limits(uint8_t total_ic, cell_asic ic[])
{
  for (uint8_t cic=0; cic<total_ic; cic++)
  {
    ic[cic].ic_reg.cell_channels=12;
    ic[cic].ic_reg.stat_channels=4;
    ic[cic].ic_reg.aux_channels=6;
    ic[cic].ic_reg.num_cv_reg=4;
    ic[cic].ic_reg.num_gpio_reg=2;
    ic[cic].ic_reg.num_stat_reg=3;
  }
}

/*
Starts cell voltage conversion
*/
void LTC6811_adcv(
  uint8_t MD, //ADC Mode
  uint8_t DCP, //Discharge Permit
  uint8_t CH //Cell Channels to be measured
)
{
  LTC681x_adcv(MD,DCP,CH);
}

//Starts cell voltage and SOC conversion
void LTC6811_adcvsc(
  uint8_t MD, //ADC Mode
  uint8_t DCP //Discharge Permit
)
{
  LTC681x_adcvsc(MD,DCP);
}

// Starts cell voltage  and GPIO 1&2 conversion
void LTC6811_adcvax(
  uint8_t MD, //ADC Mode
  uint8_t DCP //Discharge Permit
)
{
  LTC681x_adcvax(MD,DCP);
}

//Starts cell voltage overlap conversion
void LTC6811_adol(
  uint8_t MD, //ADC Mode
  uint8_t DCP //Discharge Permit
)
{
  LTC681x_adol(MD,DCP);
}

//Starts cell voltage self test conversion
void LTC6811_cvst(
  uint8_t MD, //ADC Mode
  uint8_t ST //Self Test
)
{
  LTC681x_cvst(MD,ST);
}

//Start an Auxiliary Register Self Test Conversion
void LTC6811_axst(
  uint8_t MD, //ADC Mode
  uint8_t ST //Self Test
)
{
  LTC681x_axst(MD,ST);
}

//Start a Status Register Self Test Conversion
void LTC6811_statst(
  uint8_t MD, //ADC Mode
  uint8_t ST //Self Test
)
{
  LTC681x_statst(MD,ST);
}

//Sends the poll adc command
uint8_t LTC6811_pladc()
{
  return(LTC681x_pladc());
}

//This function will block operation until the ADC has finished it's conversion
uint32_t LTC6811_pollAdc()
{
  return(LTC681x_pollAdc());
}

//Start a GPIO and Vref2 Conversion
void LTC6811_adax(
  uint8_t MD, //ADC Mode
  uint8_t CHG //GPIO Channels to be measured)
)
{
  LTC681x_adax(MD,CHG);
}

//Start an GPIO Redundancy test
void LTC6811_adaxd(
  uint8_t MD, //ADC Mode
  uint8_t CHG //GPIO Channels to be measured)
)
{
  LTC681x_adaxd(MD,CHG);
}

//Start a Status ADC Conversion
void LTC6811_adstat(
  uint8_t MD, //ADC Mode
  uint8_t CHST //GPIO Channels to be measured
)
{
  LTC681x_adstat(MD,CHST);
}

// Start a Status register redundancy test Conversion
void LTC6811_adstatd(
  uint8_t MD, //ADC Mode
  uint8_t CHST //GPIO Channels to be measured
)
{
  LTC681x_adstatd(MD,CHST);
}


// Start an open wire Conversion
void LTC6811_adow(
  uint8_t MD, //ADC Mode
  uint8_t PUP //Discharge Permit
)
{
  LTC681x_adow(MD,PUP);
}

// Reads and parses the LTC6811 cell voltage registers.
uint8_t LTC6811_rdcv(uint8_t reg, // Controls which cell voltage register is read back.
                     uint8_t total_ic, // the number of ICs in the system
                     cell_asic ic[] // Array of the parsed cell codes
                    )
{

  int8_t pec_error = 0;
  pec_error = LTC681x_rdcv(reg,total_ic,ic);
  return(pec_error);
}

/*
 The function is used
 to read the  parsed GPIO codes of the LTC6811. This function will send the requested
 read commands parse the data and store the gpio voltages in aux_codes variable
*/
int8_t LTC6811_rdaux(uint8_t reg, //Determines which GPIO voltage register is read back.
                     uint8_t total_ic,//the number of ICs in the system
                     cell_asic ic[]//A two dimensional array of the gpio voltage codes.
                    )
{
  int8_t pec_error = 0;
  LTC681x_rdaux(reg,total_ic,ic);
  return (pec_error);
}

/*
 Reads and parses the LTC6811 stat registers.
 The function is used
 to read the  parsed stat codes of the LTC6811. This function will send the requested
 read commands parse the data and store the stat voltages in stat_codes variable
*/
int8_t LTC6811_rdstat(uint8_t reg, //Determines which Stat  register is read back.
                      uint8_t total_ic,//the number of ICs in the system
                      cell_asic ic[]
                     )
{
  int8_t pec_error = 0;
  pec_error = LTC681x_rdstat(reg,total_ic,ic);
  return (pec_error);
}

/*
 The command clears the cell voltage registers and intiallizes
 all values to 1. The register will read back hexadecimal 0xFF
 after the command is sent.
*/
void LTC6811_clrcell()
{
  LTC681x_clrcell();
}

/*
 The command clears the Auxiliary registers and initializes
 all values to 1. The register will read back hexadecimal 0xFF
 after the command is sent.
*/
void LTC6811_clraux()
{
  LTC681x_clraux();
}

/*
 The command clears the Stat registers and intiallizes
 all values to 1. The register will read back hexadecimal 0xFF
 after the command is sent.

*/
void LTC6811_clrstat()
{
  LTC681x_clrstat();
}

/*
 The command clears the Sctrl registers and initializes
 all values to 0. The register will read back hexadecimal 0x00
 after the command is sent.
 */
void LTC6811_clrsctrl()
{
  LTC681x_clrsctrl();
}

//Starts the Mux Decoder diagnostic self test
void LTC6811_diagn()
{
  LTC681x_diagn();
}

/*
 This command will write the configuration registers of the LTC6811-1s
 connected in a daisy chain stack. The configuration is written in descending
 order so the last device's configuration is written first.
*/
void LTC6811_wrcfg(uint8_t total_ic, //The number of ICs being written to
                   cell_asic ic[] //A two dimensional array of the configuration data that will be written
                  )
{
  LTC681x_wrcfg(total_ic,ic);
}


/*
Reads configuration registers of a LTC6811 daisy chain
*/
int8_t LTC6811_rdcfg(uint8_t total_ic, //Number of ICs in the system
                     cell_asic ic[] //A two dimensional array that the function stores the read configuration data.
                    )
{
  int8_t pec_error = 0;
  pec_error = LTC681x_rdcfg(total_ic,ic);
  return(pec_error);
}

/*
Writes the pwm registers of a LTC6811 daisy chain
*/
void LTC6811_wrpwm(uint8_t total_ic,
                   uint8_t pwmReg,  //The number of ICs being written to
                   cell_asic ic[] //A two dimensional array of the configuration data that will be written
                  )
{
  LTC681x_wrpwm(total_ic,pwmReg,ic);
}


/*
Reads pwm registers of a LTC6811 daisy chain
*/
int8_t LTC6811_rdpwm(uint8_t total_ic, //Number of ICs in the system
                     uint8_t pwmReg,
                     cell_asic ic[] //A two dimensional array that the function stores the read configuration data.
                    )
{
  int8_t pec_error =0;
  pec_error = LTC681x_rdpwm(total_ic,pwmReg,ic);
  return(pec_error);
}

/*
Writes the COMM registers of a LTC6811 daisy chain
*/
void LTC6811_wrcomm(uint8_t total_ic, //The number of ICs being written to
                    cell_asic ic[] //A two dimensional array of the comm data that will be written
                   )
{
  LTC681x_wrcomm(total_ic,ic);
}

/*
Reads COMM registers of a LTC6811 daisy chain
*/
int8_t LTC6811_rdcomm(uint8_t total_ic, //Number of ICs in the system
                      cell_asic ic[] //A two dimensional array that the function stores the read configuration data.
                     )
{
  int8_t pec_error = 0;
  LTC681x_rdcomm(total_ic, ic);
  return(pec_error);
}

/*
Shifts data in COMM register out over LTC6811 SPI/I2C port
*/
void LTC6811_stcomm()
{
  LTC681x_stcomm();
}

//Helper function to set discharge bit in CFG register
void LTC6811_set_discharge(int Cell, uint8_t total_ic, cell_asic ic[])
{
  for (int i=0; i<total_ic; i++)
  {
    if (Cell<9)
    {
      ic[i].config.tx_data[4] = ic[i].config.tx_data[4] | (1<<(Cell-1));
    }
    else if (Cell < 13)
    {
      ic[i].config.tx_data[5] = ic[i].config.tx_data[5] | (1<<(Cell-9));
    }
  }
}

// Runs the Digital Filter Self Test
int16_t LTC6811_run_cell_adc_st(uint8_t adc_reg,uint8_t total_ic, cell_asic ic[])
{
  int16_t error = 0;
  error = LTC681x_run_cell_adc_st(adc_reg,total_ic,ic);
  return(error);
}

//runs the redundancy self test
int16_t LTC6811_run_adc_redundancy_st(uint8_t adc_mode, uint8_t adc_reg, uint8_t total_ic, cell_asic ic[])
{
  int16_t error = 0;
  LTC681x_run_adc_redundancy_st(adc_mode,adc_reg,total_ic,ic);
  return(error);
}
//Runs the datasheet algorithm for open wire
void LTC6811_run_openwire(uint8_t total_ic, cell_asic ic[])
{
  LTC681x_run_openwire(total_ic,ic);
}
// Runs the ADC overlap test for the IC
uint16_t LTC6811_run_adc_overlap(uint8_t total_ic, cell_asic ic[])
{
  uint16_t error = 0;
  LTC681x_run_adc_overlap(total_ic, ic);
  return(error);
}

void LTC6811_max_min(uint8_t total_ic, cell_asic ic_cells[],
                     cell_asic ic_min[],
                     cell_asic ic_max[],
                     cell_asic ic_delta[])
{
  for (int j=0; j < total_ic; j++)
  {
    for (int i = 0; i< 12; i++)
    {
      if (ic_cells[j].cells.c_codes[i]>ic_max[j].cells.c_codes[i])ic_max[j].cells.c_codes[i]=ic_cells[j].cells.c_codes[i];
      else if (ic_cells[j].cells.c_codes[i]<ic_min[j].cells.c_codes[i])ic_min[j].cells.c_codes[i]=ic_cells[j].cells.c_codes[i];
      ic_delta[j].cells.c_codes[i] = ic_max[j].cells.c_codes[i] - ic_min[j].cells.c_codes[i];
    }
  }




}

void LTC6811_init_max_min(uint8_t total_ic, cell_asic ic[],cell_asic ic_max[],cell_asic ic_min[])
{
  for (int j=0; j < total_ic; j++)
  {
    for (int i = 0; i< ic[j].ic_reg.cell_channels; i++)
    {
      ic_max[j].cells.c_codes[i]=0;
      ic_min[j].cells.c_codes[i]=0xFFFF;
    }
  }

}

//Helper function that increments PEC counters
void LTC6811_check_pec(uint8_t total_ic,uint8_t reg, cell_asic ic[])
{
  LTC681x_check_pec(total_ic,reg,ic);
}

//Helper Function to reset PEC counters
void LTC6811_reset_crc_count(uint8_t total_ic, cell_asic ic[])
{
  LTC681x_reset_crc_count(total_ic,ic);
}

//Helper function to intialize CFG variables.
void LTC6811_init_cfg(uint8_t total_ic, cell_asic ic[])
{
  LTC681x_init_cfg(total_ic,ic);
}
//Helper function to set CFGR variable
void LTC6811_set_cfgr(uint8_t nIC, cell_asic ic[], bool refon, bool adcopt, bool gpio[5],bool dcc[12])
{
  LTC681x_set_cfgr_refon(nIC,ic,refon);
  LTC681x_set_cfgr_adcopt(nIC,ic,adcopt);
  LTC681x_set_cfgr_gpio(nIC,ic,gpio);
  LTC681x_set_cfgr_dis(nIC,ic,dcc);
}
//Helper function to set the REFON bit
void LTC6811_set_cfgr_refon(uint8_t nIC, cell_asic ic[], bool refon)
{
  LTC681x_set_cfgr_refon(nIC,ic,refon);
}
//Helper function to set the adcopt bit
void LTC6811_set_cfgr_adcopt(uint8_t nIC, cell_asic ic[], bool adcopt)
{
  LTC681x_set_cfgr_adcopt(nIC,ic,adcopt);
}
//Helper function to set GPIO bits
void LTC6811_set_cfgr_gpio(uint8_t nIC, cell_asic ic[],bool gpio[5])
{
  LTC681x_set_cfgr_gpio(nIC,ic,gpio);
}
//Helper function to control discharge
void LTC6811_set_cfgr_dis(uint8_t nIC, cell_asic ic[],bool dcc[12])
{
  LTC681x_set_cfgr_dis(nIC,ic,dcc);
}
//Helper Function to set uv value in CFG register
void LTC6811_set_cfgr_uv(uint8_t nIC, cell_asic ic[],uint16_t uv)
{
  LTC681x_set_cfgr_uv(nIC, ic, uv);
}
//helper function to set OV value in CFG register
void LTC6811_set_cfgr_ov(uint8_t nIC, cell_asic ic[],uint16_t ov)
{
  LTC681x_set_cfgr_ov( nIC, ic, ov);
}