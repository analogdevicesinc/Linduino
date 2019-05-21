/*! LTC6811: Multicell Battery Monitors
*
*@verbatim
*The LTC6811 is multi-cell battery stack monitor that measures up to 12 series 
*connected battery cells with a total measurement error of less than 1.2mV. 
*The cell measurement range of 0V to 5V makes the LTC6811 suitable for most 
*battery chemistries. All 12 cell voltages can be captured in 290uS, and lower 
*data acquisition rates can be selected for high noise reduction.
*Using the LTC6811-1, multiple devices are connected in a daisy-chain with one 
*host processor connection for all devices.
*@endverbatim
*
* https://www.analog.com/en/products/ltc6811-1.html
* https://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/dc2259a.html
*
********************************************************************************
* Copyright 2019(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

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

/* Initialize the Register limits */
void LTC6811_init_reg_limits(uint8_t total_ic, //The number of ICs in the system
							cell_asic *ic  //A two dimensional array where data will be written
							)
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
This command will write the configuration registers of the LTC6811-1s
connected. The configuration is written in descending
order so the last device's configuration is written first.
*/
void LTC6811_wrcfg(uint8_t total_ic, //The number of ICs being written to
                   cell_asic *ic //A two dimensional array of the configuration data that will be written
                  )
{
  LTC681x_wrcfg(total_ic,ic);
}

/* Reads configuration registers of a LTC6811 */
int8_t LTC6811_rdcfg(uint8_t total_ic, //Number of ICs in the system
                     cell_asic *ic //A two dimensional array that the function stores the read configuration data.
                    )
{
  int8_t pec_error = 0;
  pec_error = LTC681x_rdcfg(total_ic,ic);
  return(pec_error);
}

/* Starts cell voltage conversion */
void LTC6811_adcv(uint8_t MD, //ADC Mode
				  uint8_t DCP, //Discharge Permit
				  uint8_t CH //Cell Channels to be measured
				  )
{
  LTC681x_adcv(MD,DCP,CH);
}

/* Start a GPIO and Vref2 Conversion */
void LTC6811_adax(uint8_t MD, //ADC Mode
				  uint8_t CHG //GPIO Channels to be measured
				  )
{
  LTC681x_adax(MD,CHG);
}

/* Start a Status ADC Conversion */
void LTC6811_adstat(uint8_t MD, //ADC Mode
				    uint8_t CHST //Stat Channels to be measured
				   )
{
  LTC681x_adstat(MD,CHST);
}

/*  Starts cell voltage  and GPIO 1 &  2 conversion */
void LTC6811_adcvax(uint8_t MD, //ADC Mode
				    uint8_t DCP //Discharge Permit
				    )
{
  LTC681x_adcvax(MD,DCP);
}

/* Starts cell voltage and Sum of cells conversion */
void LTC6811_adcvsc(uint8_t MD, //ADC Mode
				    uint8_t DCP //Discharge Permit
				    )
{
  LTC681x_adcvsc(MD,DCP);
}

/*
The function is used to read the  parsed cell voltage codes of the LTC6811. 
This function will send the requested read commands parse the data and 
store the cell voltages in c_codes variable
*/
uint8_t LTC6811_rdcv(uint8_t reg, // Controls which cell voltage register is read back.
                     uint8_t total_ic, // The number of ICs in the system
                     cell_asic *ic // Array of the parsed cell codes
                    )
{
  int8_t pec_error = 0;
  pec_error = LTC681x_rdcv(reg,total_ic,ic);
  return(pec_error);
}

/*
The function is used to read the  parsed GPIO codes of the LTC6811. 
This function will send the requested read commands parse the data 
and store the gpio voltages in a_codes variable
*/
int8_t LTC6811_rdaux(uint8_t reg, //Determines which GPIO voltage register is read back.
                     uint8_t total_ic,//The number of ICs in the system
                     cell_asic *ic//A two dimensional array of the gpio voltage codes.
                     )
{
  int8_t pec_error = 0;
  LTC681x_rdaux(reg,total_ic,ic);
  return (pec_error);
}

/*
Reads and parses the LTC6811 stat registers.
The function is used to read the  parsed stat codes of the LTC6811. 
This function will send the requested read commands parse the data 
and store the stat voltages in stat_codes variable
*/
int8_t LTC6811_rdstat(uint8_t reg, //Determines which Stat register is read back.
                      uint8_t total_ic,//The number of ICs in the system
                      cell_asic *ic //Array of the parsed stat codes
                     )
{
  int8_t pec_error = 0;
  pec_error = LTC681x_rdstat(reg,total_ic,ic);
  return (pec_error);
}

/* Sends the poll ADC command */
uint8_t LTC6811_pladc()
{
  return(LTC681x_pladc());
}

//This function will block operation until the ADC has finished it's conversion */
uint32_t LTC6811_pollAdc()
{
  return(LTC681x_pollAdc());
}

/*
The command clears the cell voltage registers and initializes all values to 1. 
The register will read back hexadecimal 0xFF after the command is sent.
*/
void LTC6811_clrcell()
{
  LTC681x_clrcell();
}

/*
The command clears the Auxiliary registers and initializes all values to 1. 
The register will read back hexadecimal 0xFF after the command is sent.
*/
void LTC6811_clraux()
{
  LTC681x_clraux();
}

/*
The command clears the Stat registers and initializes all values to 1. 
The register will read back hexadecimal 0xFF after the command is sent.
*/
void LTC6811_clrstat()
{
  LTC681x_clrstat();
}

/* Starts the Mux Decoder diagnostic self test */
void LTC6811_diagn()
{
  LTC681x_diagn();
}

/* Starts cell voltage self test conversion */
void LTC6811_cvst(uint8_t MD, //ADC Mode
				  uint8_t ST //Self Test
				  )
{
  LTC681x_cvst(MD,ST);
}

/* Start an Auxiliary Register Self Test Conversion */
void LTC6811_axst(uint8_t MD, //ADC Mode
				  uint8_t ST //Self Test
				  )
{
  LTC681x_axst(MD,ST);
}

/* Start a Status Register Self Test Conversion */
void LTC6811_statst(uint8_t MD, //ADC Mode
				    uint8_t ST //Self Test
				    )
{
  LTC681x_statst(MD,ST);
}

/* Starts cell voltage overlap conversion */
void LTC6811_adol(uint8_t MD, //ADC Mode
				  uint8_t DCP //Discharge Permit
				  )
{
  LTC681x_adol(MD,DCP);
}

/* Start an GPIO Redundancy test */
void LTC6811_adaxd(uint8_t MD, //ADC Mode
				   uint8_t CHG //GPIO Channels to be measured
				   )
{
  LTC681x_adaxd(MD,CHG);
}

/* Start a Status register redundancy test Conversion */
void LTC6811_adstatd(uint8_t MD, //ADC Mode
				     uint8_t CHST //Stat Channels to be measured
				    )
{
  LTC681x_adstatd(MD,CHST);
}

/* Runs the Digital Filter Self Test */
int16_t LTC6811_run_cell_adc_st(uint8_t adc_reg, // Type of register
								uint8_t total_ic,//Number of ICs in the system 
								cell_asic *ic, //A two dimensional array that will store the data
								uint8_t md, //ADC Mode
								bool adcopt //The adcopt bit in the configuration register
								)
{
  int16_t error = 0;
  error = LTC681x_run_cell_adc_st(adc_reg,total_ic,ic,md,adcopt);
  return(error);
}

/* Runs the ADC overlap test for the IC */
uint16_t LTC6811_run_adc_overlap(uint8_t total_ic,//Number of ICs in the system  
								 cell_asic *ic //A two dimensional array that will store the data
								 )
{
  uint16_t error = 0;
  LTC681x_run_adc_overlap(total_ic, ic);
  return(error);
}

/* Runs the redundancy self test */
int16_t LTC6811_run_adc_redundancy_st(uint8_t adc_mode, //ADC Mode 
									  uint8_t adc_reg, // Type of register
									  uint8_t total_ic,//Number of ICs in the system  
									  cell_asic *ic //A two dimensional array that will store the data
									  )
{
  int16_t error = 0;
  LTC681x_run_adc_redundancy_st(adc_mode,adc_reg,total_ic,ic);
  return(error);
}

/* Start an open wire Conversion */
void LTC6811_adow(uint8_t MD, //ADC Mode
				  uint8_t PUP,//Pull up/ Pull down
				  uint8_t CH, //Cell selection 
				  uint8_t DCP //Discharge Permit 
				  )
{
  LTC681x_adow(MD,PUP,CH,DCP);
}

/* Runs the data sheet algorithm for open wire for single cell detection */
void LTC6811_run_openwire_single(uint8_t total_ic,//Number of ICs in the system  
						         cell_asic *ic //A two dimensional array that will store the data  
						        )
{
  LTC681x_run_openwire_single(total_ic,ic);
}

/* Runs the data sheet algorithm for open wire for multiple cell and two consecutive cells detection */
void LTC6811_run_openwire_multi(uint8_t total_ic,//Number of ICs in the system  
						         cell_asic *ic //A two dimensional array that will store the data  
						        )
{
  LTC681x_run_openwire_multi(total_ic,ic);
}

/* Helper function to set discharge bit in CFG register */
void LTC6811_set_discharge(int Cell, //The cell to be discharged
						   uint8_t total_ic, //Number of ICs in the system
						   cell_asic *ic //A two dimensional array that will store the data
						   )
{
  for (int i=0; i<total_ic; i++)
  {
    if ((Cell<9)&& (Cell!=0) )
    {
      ic[i].config.tx_data[4] = ic[i].config.tx_data[4] | (1<<(Cell-1));
    }
    else if (Cell < 13)
    {
      ic[i].config.tx_data[5] = ic[i].config.tx_data[5] | (1<<(Cell-9));
    }
	else
	{
		break;
	}
  }
}

/* Clears all of the DCC bits in the configuration registers */
void LTC6811_clear_discharge(uint8_t total_ic, cell_asic *ic)
{
  LTC681x_clear_discharge(total_ic,ic);
}

/* Writes the pwm registers of a LTC6811 */
void LTC6811_wrpwm(uint8_t total_ic, //Number of ICs in the system
                   uint8_t pwmReg,  //The number of registers being written to
                   cell_asic *ic //A two dimensional array that the function stores the data in.
                  )
{
  LTC681x_wrpwm(total_ic,pwmReg,ic);
}


/* Reads pwm registers of a LTC6811 */
int8_t LTC6811_rdpwm(uint8_t total_ic, //Number of ICs in the system
                     uint8_t pwmReg,  //The number of registers being written to
                     cell_asic *ic //A two dimensional array that the function stores the read pwm data.
                    )
{
  int8_t pec_error =0;
  pec_error = LTC681x_rdpwm(total_ic,pwmReg,ic);
  return(pec_error);
}

/* Writes data in S control register the LTC6811-1s connected */  
void LTC6811_wrsctrl(uint8_t total_ic, //Number of ICs in the system
                     uint8_t sctrl_reg, //The number of registers being written to
                     cell_asic *ic //A two dimensional array of the data that will be written
                    )
{
  LTC681x_wrsctrl(total_ic, sctrl_reg, ic);
}
  
/* Reads sctrl registers of a LTC6811 */  
int8_t LTC6811_rdsctrl(uint8_t total_ic, //Number of ICs in the system
                       uint8_t sctrl_reg,//The number of registers being written to
                       cell_asic *ic //A two dimensional array that the function stores the read data
                      )
{
  int8_t pec_error =0;
  pec_error =  LTC681x_rdsctrl(total_ic, sctrl_reg,ic );
  return(pec_error);	
}

/* 
Start Sctrl data communication              
This command will start the sctrl pulse communication over the spins
*/
void LTC6811_stsctrl()
{
  LTC681x_stsctrl();
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

/* Writes the COMM registers of a LTC6811 */ 
void LTC6811_wrcomm(uint8_t total_ic, //The number of ICs being written to
                    cell_asic *ic //A two dimensional array of the comm data that will be written
                   )
{
  LTC681x_wrcomm(total_ic,ic);
}

/* Reads COMM registers of a LTC6811 */
int8_t LTC6811_rdcomm(uint8_t total_ic, //Number of ICs in the system
                      cell_asic *ic //A two dimensional array that the function stores the read comm data.
                     )
{
  int8_t pec_error = 0;
  LTC681x_rdcomm(total_ic, ic);
  return(pec_error);
}

/* Shifts data in COMM register out over LTC6811 SPI/I2C port */
void LTC6811_stcomm(uint8_t len)
{
  LTC681x_stcomm(len); // length of data to be transmitted 
}

/* Helper function that increments PEC counters */
void LTC6811_check_pec(uint8_t total_ic,//Number of ICs in the system  
					   uint8_t reg, // Type of register
					   cell_asic *ic//A two dimensional array that will store the data
					   )
{
  LTC681x_check_pec(total_ic,reg,ic);
}

/* Helper Function to reset PEC counters */
void LTC6811_reset_crc_count(uint8_t total_ic, //Number of ICs in the system
							 cell_asic *ic //A two dimensional array that will store the data
							 )
{
  LTC681x_reset_crc_count(total_ic,ic);
}

/* Helper function to initialize CFG variables.*/
void LTC6811_init_cfg(uint8_t total_ic, //Number of ICs in the system
					  cell_asic *ic //A two dimensional array that will store the data
					  )
{
  LTC681x_init_cfg(total_ic,ic);
}

/* Helper function to set CFGR variable */
void LTC6811_set_cfgr(uint8_t nIC, //current IC
                      cell_asic *ic,//a two dimensional array that will store the data 
					  bool refon, // The REFON bit
					  bool adcopt, // The ADCOPT bit
					  bool gpio[5],// The GPIO bit
					  bool dcc[12],// The DCC bit 
					  bool dcto[4],// The Dcto bit 
					  uint16_t uv, // The UV bit
					  uint16_t  ov // The OV bit
					  )
{
  LTC681x_set_cfgr(nIC ,ic,refon,adcopt,gpio,dcc,dcto, uv, ov);
}

/* Helper function to set the REFON bit */
void LTC6811_set_cfgr_refon(uint8_t nIC, cell_asic *ic, bool refon)
{
  LTC681x_set_cfgr_refon(nIC,ic,refon);
}

/* Helper function to set the adcopt bit */
void LTC6811_set_cfgr_adcopt(uint8_t nIC, cell_asic *ic, bool adcopt)
{
  LTC681x_set_cfgr_adcopt(nIC,ic,adcopt);
}

/* Helper function to set GPIO bits */
void LTC6811_set_cfgr_gpio(uint8_t nIC, cell_asic *ic,bool gpio[5])
{
  LTC681x_set_cfgr_gpio(nIC,ic,gpio);
}

/* Helper function to control discharge */
void LTC6811_set_cfgr_dis(uint8_t nIC, cell_asic *ic,bool dcc[12])
{
  LTC681x_set_cfgr_dis(nIC,ic,dcc);
}

/* Helper function to control discharge */
void LTC6811_set_cfgr_dcto(uint8_t nIC, cell_asic *ic,bool dcto[4])
{
  LTC681x_set_cfgr_dcto(nIC, ic,dcto);
}

/* Helper Function to set uv value in CFG register */
void LTC6811_set_cfgr_uv(uint8_t nIC, cell_asic *ic,uint16_t uv)
{
  LTC681x_set_cfgr_uv(nIC, ic, uv);
}

/* Helper function to set OV value in CFG register */
void LTC6811_set_cfgr_ov(uint8_t nIC, cell_asic *ic,uint16_t ov)
{
  LTC681x_set_cfgr_ov( nIC, ic, ov);
}
