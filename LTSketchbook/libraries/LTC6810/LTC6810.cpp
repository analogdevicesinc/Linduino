/*! LTC6810: 6-Channel Battery Stack Monitors
*
*@verbatim
*The LTC6810 is multi-cell battery stack monitor that measures up to 6 series 
*connected battery cells with a total measurement error of less than 1.8mV. 
*The cell measurement range of 0V to 5V makes the LTC6810 suitable for most 
*battery chemistries. All 6 cell voltages can be captured in 290uS, and lower 
*data acquisition rates can be selected for high noise reduction.
*Using the LTC6810-1, multiple devices are connected in a daisy-chain with one 
*host processor connection for all devices, permitting simultaneous cell monitoring 
*of long, high voltage battery strings.
*@endverbatim
*
* https://www.analog.com/en/products/ltc6810-1.html
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
//! @defgroup LTC6810-1 LTC6810-1: Multicell Battery Monitor
//! @}

/*! @file
    @ingroup LTC6810-1
    Library for LTC6810-1 Multicell Battery Monitor
*/

#include "stdint.h"
#include "LTC681x.h"
#include "LTC6810.h"

/* Helper function to initialize register limits */
void LTC6810_init_reg_limits(uint8_t total_ic, // Number of ICs in the system
							cell_asic *ic // A two dimensional array that stores the data
							)
{
    for(uint8_t cic=0; cic<total_ic; cic++)
    {
        ic[cic].ic_reg.cell_channels=6; 
        ic[cic].ic_reg.stat_channels=4;
        ic[cic].ic_reg.aux_channels=6;
        ic[cic].ic_reg.num_cv_reg=2; 
        ic[cic].ic_reg.num_gpio_reg=2;
        ic[cic].ic_reg.num_stat_reg=3;
    } 
}

/*
This command will write into the configuration registers of the LTC6810-1s
connected in a daisy chain stack. The configuration is written in descending
order so the last device's configuration is written first.
*/
void LTC6810_wrcfg(uint8_t total_ic, //The number of ICs being written to
				 cell_asic *ic //A two dimensional array of the configuration data that will be written
				)
{
	LTC681x_wrcfg(total_ic,ic);
}

/* This command reads configuration registers of a LTC6810s connected in a daisy chain */
int8_t LTC6810_rdcfg(uint8_t total_ic, //Number of ICs in the system
				   cell_asic *ic //A two dimensional array that the function stores the read configuration data
				  )
{
   int8_t pec_error = 0;
   pec_error = LTC681x_rdcfg(total_ic,ic);
   return(pec_error);
}

/* Starts cell voltage conversion */ 
void LTC6810_adcv(uint8_t MD, //ADC Mode
				  uint8_t DCP, //Discharge Permit
				  uint8_t CH //Cell Channels to be measured
				 )
{
    LTC681x_adcv(MD,DCP,CH);
}

/* Start a GPIO and Vref2 Conversion */
void LTC6810_adax(uint8_t MD, //ADC Mode
				  uint8_t CHG //GPIO Channels to be measured
				 )
{
	LTC681x_adax(MD,CHG);
}

/* Start a Status ADC Conversion */
void LTC6810_adstat(uint8_t MD, //ADC Mode
					uint8_t CHST //Status Channels to be measured
					)
{
	LTC681x_adstat(MD,CHST);
}

/* Starts cell voltage and Sum of Cells conversion */
void LTC6810_adcvsc(uint8_t MD, //ADC Mode
					uint8_t DCP //Discharge Permit
					)
{
    LTC681x_adcvsc(MD,DCP);
}

/* Starts cell voltage, SV0 and GPIO1 conversion */
void LTC6810_adcvax(uint8_t MD, //ADC Mode
					uint8_t DCP //Discharge Permit
					)
{
    LTC681x_adcvax(MD,DCP);
}

/*
Reads and parses the LTC6810 cell voltage registers.
The function is used to read the parsed Cell Voltage of the LTC6810.
This function will send the requested read commands, parse the data 
and store the gpio voltages in c_codes variable.
*/
uint8_t LTC6810_rdcv(uint8_t reg, // Controls which cell voltage register is read back
                     uint8_t total_ic, // The number of ICs in the system
                     cell_asic *ic // Array of the parsed cell codes
                    )
{
  int8_t pec_error = 0;
  pec_error = LTC681x_rdcv(reg,total_ic,ic);
  return(pec_error);
}

/*
Reads and parses the LTC6810 Auxiliary registers.
The function is used to read the parsed GPIO codes of the LTC6810.
This function will send the requested read commands, parse the data 
and store the gpio voltages in a_codes variable.
*/
int8_t LTC6810_rdaux(uint8_t reg, //Determines which GPIO voltage register is read back
				   uint8_t total_ic,//The number of ICs in the system
				   cell_asic *ic//A two dimensional array of the gpio voltage codes
				  )
{
   int8_t pec_error = 0;
   LTC681x_rdaux(reg,total_ic,ic);
   return (pec_error);
}

/*
Reads and parses the LTC6810 stat registers.
The function is used to read the parsed stat codes of the LTC6810. 
This function will send the requested read commands, parse the data 
and store the stat voltages in stat_codes variable.
*/
int8_t LTC6810_rdstat(uint8_t reg, //Determines which Stat register is read back
					uint8_t total_ic,//The number of ICs in the system
					cell_asic *ic //A two dimensional array of the stat codes
				   )
{
   int8_t pec_error = 0;
   pec_error = LTC681x_rdstat(reg,total_ic,ic);
   return (pec_error);
}

/*
Reads and parses the LTC6810 S voltage registers.
The function is used to read the parsed s voltage codes of the LTC6810. 
This function will send the requested read commands, parse the data 
and store the stat voltages in c_codes variable
*/
uint8_t LTC6810_rds(uint8_t reg, //Controls which s voltage register is read back.
                     uint8_t total_ic, //Number of ICs in the daisy chain 
                     cell_asic *ic //Array of the parsed cell codes
                    )
{
    int8_t pec_error = 0;
    uint8_t *s_data;
    uint8_t c_ic = 0;
    s_data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));
    
    if (reg == 0)
    {
      for (uint8_t s_reg = 1; s_reg< 3; s_reg++) //executes once for each of the LTC6810 s voltage registers
      {
        LTC6810_rds_reg(s_reg, total_ic, s_data );
     
        for (int current_ic = 0; current_ic<total_ic; current_ic++)
        {
          if (ic->isospi_reverse == false)
          {
            c_ic = current_ic;
          }
          else
          {
            c_ic = total_ic - current_ic - 1;
          }
          pec_error = pec_error + parse_cells(current_ic, (2+ s_reg), s_data,
                                              &ic[c_ic].cells.c_codes[0],
                                              &ic[c_ic].cells.pec_match[0]);
        }
      }
     }
    
    else
    {
      LTC6810_rds_reg(reg, total_ic, s_data);
    
      for (int current_ic = 0; current_ic<total_ic; current_ic++)
      {
        if (ic->isospi_reverse == false)
        {
          c_ic = current_ic;
        }
        else
        {
          c_ic = total_ic - current_ic - 1;
        }
        pec_error = pec_error + parse_cells(current_ic, (2+ reg), &s_data[8*c_ic],
                                            &ic[c_ic].cells.c_codes[0],
                                            &ic[c_ic].cells.pec_match[0]);
      }
    }
    LTC6810_check_pec(total_ic,CELL,ic);
    free(s_data);
    return(pec_error);
}

/* Selects the S voltage register to read  */
void LTC6810_rds_reg(uint8_t reg, //Determines which S voltage register is read back
                      uint8_t total_ic, //The number of ICs in the daisy chain 
                      uint8_t *data //An array of the unparsed cell codes
                     )
{
	const uint8_t REG_LEN = 8; //Number of bytes in each ICs register + 2 bytes for the PEC
	uint8_t cmd[4];
	uint16_t cmd_pec;

	if (reg == 1)     // RDSA
	{
	cmd[1] = 0x08;
	cmd[0] = 0x00;
	}
	else if (reg == 2) // RDSB
	{
	cmd[1] = 0x0A;
	cmd[0] = 0x00;
	}
	
	read_68(total_ic, cmd, data);
}

/* Sends the poll ADC command */
uint8_t LTC6810_pladc()
{
	return(LTC681x_pladc());
}

/* This function will block operation until the ADC has finished it's conversion*/
uint32_t LTC6810_pollAdc()
{
	return(LTC681x_pollAdc());
}

/*
The command clears the Cell Voltage registers and initializes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.
*/
void LTC6810_clrcell()
{
	LTC681x_clrcell();
}

/*
The command clears the Auxiliary registers and initializes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.
*/
void LTC6810_clraux()
{
	LTC681x_clraux();
}

/*
The command clears the Stat registers and initializes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.
*/
void LTC6810_clrstat()
{
	LTC681x_clrstat();
}

/* Starts the Mux Decoder diagnostic self test */
void LTC6810_diagn()
{
	LTC681x_diagn();
}

/* Starts cell voltage self test conversion */
void LTC6810_cvst(uint8_t MD, //ADC Mode
				  uint8_t ST //Self Test
				 )
{
    LTC681x_cvst(MD,ST);
}

/* Start an Auxiliary Register Self Test Conversion */
void LTC6810_axst(uint8_t MD, //ADC Mode
				  uint8_t ST //Self Test
				 )
{
	LTC681x_axst(MD,ST);
}

/* Start a Status Register Self Test Conversion */
void LTC6810_statst(uint8_t MD, //ADC Mode
					uint8_t ST //Self Test
					)
{
    LTC681x_statst(MD,ST);
}

/* Start an GPIO Redundancy test */
void LTC6810_adaxd(uint8_t MD, //ADC Mode
				   uint8_t CHG //GPIO Channels to be measured
				  )
{
	LTC681x_adaxd(MD,CHG);
}

/* Start a Status register redundancy test */ 
void LTC6810_adstatd(uint8_t MD, //ADC Mode
					 uint8_t CHST //Stat Channels to be measured
					)
{
	LTC681x_adstatd(MD,CHST);
}

/* Runs the Digital Filter Self Test */
int16_t LTC6810_run_cell_adc_st(uint8_t adc_reg, //Type of register
								uint8_t total_ic, //Number of ICs in the daisy chain 
								cell_asic *ic, //A two dimensional array that will store the data
								uint8_t md, //ADC Mode
								bool adcopt //ADCOPT bit in the configuration register
								)
{
  int16_t error = 0;
  error = LTC681x_run_cell_adc_st(adc_reg,total_ic,ic, md, adcopt);
  return(error);
}

/* Runs the redundancy self test */
int16_t LTC6810_run_adc_redundancy_st(uint8_t adc_mode, //ADC Mode
									  uint8_t adc_reg,  //Type of register
									  uint8_t total_ic, //Number of ICs in the daisy chain 
									  cell_asic *ic  //A two dimensional array that will store the data
									  )
{
  int16_t error = 0;
  LTC681x_run_adc_redundancy_st(adc_mode,adc_reg,total_ic,ic);
  return(error);
}

/* Start an open wire Conversion */
void LTC6810_adow(uint8_t MD, //ADC Conversion Mode
				  uint8_t PUP, //Pull up/Pull down current
				  uint8_t CH, //Cell Channels
				  uint8_t DCP//Discharge Permit
				)
{
    LTC681x_adow(MD,PUP, CH, DCP);
}

/* Start GPIOs open wire ADC conversion */
void LTC6810_axow(uint8_t MD, //ADC Mode
				  uint8_t PUP //Pull up/Pull down current
				 )
{
    LTC681x_axow(MD, PUP);
}

/* Runs the data sheet algorithm for open wire for single cell open detection */
void LTC6810_run_openwire_single(uint8_t total_ic, //Number of ICs in the daisy chain 
								 cell_asic *ic //A two dimensional array that will store the data
								 )
{
	LTC681x_run_openwire_single(total_ic,ic);
}

/* Runs the data sheet algorithm for open wire for multiple cell and two consecutive cells open detection */
void LTC6810_run_openwire_multi(uint8_t total_ic,//Number of ICs in the system  
						         cell_asic *ic //A two dimensional array that will store the data
						        )
{
	LTC681x_run_openwire_multi(total_ic,ic);
}

/* Runs open wire for GPIOs */
void LTC6810_run_gpio_openwire(uint8_t total_ic, //Number of ICs in the daisy chain 
								cell_asic ic[] //A two dimensional array that will store the data
								)
{
	uint16_t OPENWIRE_THRESHOLD = 500;
	const uint8_t  N_CHANNELS =5;
	  
	uint16_t aux_val[total_ic][N_CHANNELS];
	uint16_t pDwn[total_ic][N_CHANNELS];
	uint16_t ow_delta[total_ic][N_CHANNELS];
	
	int8_t error;
	int8_t i;
	uint32_t conv_time=0;

	wakeup_sleep(total_ic); 
	LTC6810_clraux();
	 
	for (i = 0; i < 3; i++)
	{ 
		wakeup_idle(total_ic);
		LTC6810_adax(MD_7KHZ_3KHZ, AUX_CH_ALL);
		conv_time= LTC6810_pollAdc();
	}
	
	wakeup_idle(total_ic);
	error = LTC6810_rdaux(0, total_ic,ic);
	
	for (int cic=0; cic<total_ic; cic++)
	{
	  for (int channel=0; channel<N_CHANNELS; channel++)
		{
			aux_val[cic][channel]=ic[cic].aux.a_codes[channel];
		}	
	}	
	LTC6810_clraux();
	
	// pull ups
	for (i = 0; i < 3; i++)
	{ 
		wakeup_idle(total_ic);
		LTC6810_axow(MD_7KHZ_3KHZ,PULL_UP_CURRENT);
		conv_time =LTC6810_pollAdc();
	} 
	
	wakeup_idle(total_ic);
	error = LTC6810_rdaux(0, total_ic,ic);
	
	for (int cic=0; cic<total_ic; cic++)
	{
	   for (int channel=0; channel<N_CHANNELS; channel++)
		{
			pDwn[cic][channel]=ic[cic].aux.a_codes[channel] ;
		}	
	}
	  
	for (int cic=0; cic<total_ic; cic++)
	{  
		ic[cic].system_open_wire = 0xFFFF;
		
		for (int channel=0; channel<N_CHANNELS; channel++)
		{
			if (pDwn[cic][channel] > aux_val[cic][channel])                   
			{
				ow_delta[cic][channel] = (pDwn[cic][channel] - aux_val[cic][channel]);
			}
			else
			{
				ow_delta[cic][channel] = 0;                                             
			} 
			
			if(channel>0)
			{
				if (ow_delta[cic][channel] > OPENWIRE_THRESHOLD) 
				{
					ic[cic].system_open_wire = channel;
				}  
			}			
		}
	}		  
}

/* Helper function to set discharge bit in CFG register */
void LTC6810_set_discharge(int Cell, //The cell to be discharged
						   uint8_t total_ic, //Number of ICs in the daisy chain 
						   cell_asic *ic //A two dimensional array that will store the data
						   )
{
	for (int i=0; i<total_ic; i++)
	{
		if (Cell<7)
		{
		  ic[i].config.tx_data[4] = ic[i].config.tx_data[4] | (1<<(Cell-1));
		}
		else if (Cell ==0)
		{
		  ic[i].config.tx_data[4] = ic[i].config.tx_data[4] | (0x80);
		}
	}
}

/* Clears all of the DCC bits in the configuration registers */
void LTC6810_clear_discharge(uint8_t total_ic, //Number of ICs in the daisy chain 
                             cell_asic *ic //A two dimensional array that will store the data
							 )
{
	for (int i=0; i<total_ic; i++)
	{
		ic[i].config.tx_data[4] = ic[i].config.tx_data[4] & (0x40);
	}
}

/* Writes the pwm registers of a LTC6810 daisy chain */
void LTC6810_wrpwm(uint8_t total_ic, //Number of ICs in the daisy chain 
				   uint8_t pwmReg,  //Select register
				   cell_asic *ic //A two dimensional array that will store the data to be written
				  )
{
  LTC681x_wrpwm(total_ic,pwmReg,ic);
}

/* Reads pwm registers of a LTC6810 daisy chain */
int8_t LTC6810_rdpwm(uint8_t total_ic, //Number of ICs in the system
				   uint8_t pwmReg, // Select register
				   cell_asic *ic //A two dimensional array that the function stores the read data.
				  )
{
   int8_t pec_error =0;
   pec_error = LTC681x_rdpwm(total_ic,pwmReg,ic);
   return(pec_error);
}


/* Writes the COMM registers of a LTC6810 daisy chain */
void LTC6810_wrcomm(uint8_t total_ic, //The number of ICs being written to
				  cell_asic *ic //A two dimensional array of the comm data that will be written
				 )
{
	LTC681x_wrcomm(total_ic,ic);
}

/* Reads COMM registers of a LTC6810 daisy chain */
int8_t LTC6810_rdcomm(uint8_t total_ic, //Number of ICs in the system
					cell_asic *ic //A two dimensional array that the function stores the read data.
				   )
{
   int8_t pec_error = 0;
   LTC681x_rdcomm(total_ic, ic);
   return(pec_error);
}

/* Shifts data in COMM register out over LTC6810 SPI/I2C port */
void LTC6810_stcomm(uint8_t len) //Length of data to be transmitted
{
    LTC681x_stcomm(len);  
}

/* Reads Serial ID registers group. */
uint8_t LTC6810_rdsid(uint8_t total_ic, // The number of ICs in the system
                     cell_asic *ic //A two dimensional array that the function stores the read data.
                    )
{
    uint8_t cmd[4];
    uint8_t read_buffer[256];
    int8_t pec_error = 0;
    uint16_t data_pec;
    uint16_t calc_pec;
    uint8_t c_ic = 0;
	bool temp=ic->isospi_reverse;
    
    cmd[0] = 0x00;
    cmd[1] = 0x2C;
    
    pec_error = read_68(total_ic, cmd, read_buffer);

    for(uint8_t current_ic =0; current_ic<total_ic; current_ic++)
    {	
      if (temp== false)
      {
        c_ic = current_ic;
      }
      else
      {
        c_ic = total_ic - current_ic - 1;
      }
	
        for(int byte=0; byte<8;byte++)
        {
            ic[c_ic].sid[byte] = read_buffer[byte+(8*current_ic)];
        }
    }
    return(pec_error);
} 

/* Mutes the LTC6810 discharge transistors */
void LTC6810_mute()
{
	uint8_t cmd[2];

	cmd[0] = 0x00;
	cmd[1] = 0x28;

	cmd_68(cmd);
}

/* Clears the LTC6810 Mute Discharge */
void LTC6810_unmute()
{
	uint8_t cmd[2];

	cmd[0] = 0x00;
	cmd[1] = 0x29;
	cmd_68(cmd);
}

/* Helper function that increments PEC counters */
void LTC6810_check_pec(uint8_t total_ic, //The number of ICs in the system
					   uint8_t reg, //Type of register
					   cell_asic *ic //A two dimensional array that will store the data
					   )
{
    LTC681x_check_pec(total_ic,reg,ic);
}

/* Helper Function to reset PEC counters */
void LTC6810_reset_crc_count(uint8_t total_ic, //The number of ICs in the system
							 cell_asic *ic //A two dimensional array that will store the data
							 )
{
    LTC681x_reset_crc_count(total_ic,ic);
}

/* Helper function to initialize CFG variables. */
void LTC6810_init_cfg(uint8_t total_ic, cell_asic *ic)
{
    LTC681x_init_cfg(total_ic,ic);
}

/* Helper function to set CFGR variable */
void LTC6810_set_cfgr(uint8_t nIC, cell_asic *ic, bool refon, bool adcopt, bool gpio[4], bool dcc[6],  bool dcc_0,  bool mcal, bool en_dtmen, bool dis_red, bool fdrf, bool sconv, bool dcto[4], uint16_t uv, uint16_t  ov)
{
    LTC6810_set_cfgr_refon(nIC,ic,refon);
    LTC6810_set_cfgr_adcopt(nIC,ic,adcopt);
    LTC6810_set_cfgr_gpio(nIC,ic,gpio);
	LTC6810_set_cfgr_uv(nIC, ic, uv);
    LTC6810_set_cfgr_ov(nIC, ic, ov);
    LTC6810_set_cfgr_dis(nIC,ic,dcc);
	LTC6810_set_cfgr_mcal(nIC,ic,mcal);
	LTC6810_set_cfgr_dcc_0(nIC,ic,dcc_0);
	LTC6810_set_cfgr_en_dtmen(nIC,ic,en_dtmen);
	LTC6810_set_cfgr_dis_red(nIC,ic,dis_red);
	LTC6810_set_cfgr_fdrf(nIC,ic,fdrf);
	LTC6810_set_cfgr_sconv(nIC,ic,sconv);
	LTC6810_set_cfgr_dcto(nIC,ic,dcto);  
}

/* Helper function to set the REFON bit */
void LTC6810_set_cfgr_refon(uint8_t nIC, cell_asic *ic, bool refon) 
{
	LTC681x_set_cfgr_refon(nIC,ic,refon);
}

/* Helper function to set the ADCOPT bit */
void LTC6810_set_cfgr_adcopt(uint8_t nIC, cell_asic *ic, bool adcopt)
{
	LTC681x_set_cfgr_adcopt(nIC,ic,adcopt);
}

/* Helper function to set GPIO bits */
void LTC6810_set_cfgr_gpio(uint8_t nIC, cell_asic *ic,bool gpio[4])
{  
	for (int i =0; i<4; i++)
	{
		if (gpio[i])ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0]|(0x01<<(i+3));
		else ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0]&(~(0x01<<(i+3)));
	}
}

/* Helper function to control discharge */
void LTC6810_set_cfgr_dis(uint8_t nIC, cell_asic *ic,bool dcc[6])
{  
	for (int i =0; i<6; i++)
	{
		if (dcc[i])ic[nIC].config.tx_data[4] = ic[nIC].config.tx_data[4]|(0x01<<i);
		else ic[nIC].config.tx_data[4] = ic[nIC].config.tx_data[4]& (~(0x01<<i));
	}
}

/* Helper Function to set UV value in CFG register */
void LTC6810_set_cfgr_uv(uint8_t nIC, cell_asic *ic,uint16_t uv)
{
    LTC681x_set_cfgr_uv(nIC, ic, uv);
}

/* Helper function to set OV value in CFG register */
void LTC6810_set_cfgr_ov(uint8_t nIC, cell_asic *ic,uint16_t ov)
{
    LTC681x_set_cfgr_ov( nIC, ic, ov);
}

/* Helper function to set the MCAL bit */
void LTC6810_set_cfgr_mcal(uint8_t nIC, cell_asic *ic, bool mcal)
{
	if (mcal) ic[nIC].config.tx_data[4] = ic[nIC].config.tx_data[4]|0x40;
	else ic[nIC].config.tx_data[4] = ic[nIC].config.tx_data[4]&0xBF;
}

/* Helper function to set the DCC0 bit */
void LTC6810_set_cfgr_dcc_0(uint8_t nIC, cell_asic *ic, bool dcc_0)
{
	if (dcc_0) ic[nIC].config.tx_data[4] = ic[nIC].config.tx_data[4]|0x80;
	else ic[nIC].config.tx_data[4] = ic[nIC].config.tx_data[4]&0x7F;	
}

/* Helper function to set the EN_DTMEN bit */
void LTC6810_set_cfgr_en_dtmen(uint8_t nIC, cell_asic *ic, bool en_dtmen)
{
	if (en_dtmen) ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]|0x01;
	else ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]&0xFE;
}

/* Helper function to set the DIS_RED bit */
void LTC6810_set_cfgr_dis_red(uint8_t nIC, cell_asic *ic, bool dis_red)
{
	if (dis_red) ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]|0x02;
	else ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]&0xFD;
}

/* Helper function to set the FDRF bit */
void LTC6810_set_cfgr_fdrf(uint8_t nIC, cell_asic *ic, bool fdrf)
{
	if (fdrf) ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]|0x04;
	else ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]&0xFB;	
}

/* Helper function to set the SCONV bit */
void LTC6810_set_cfgr_sconv(uint8_t nIC, cell_asic *ic, bool sconv)
{
	if (sconv) ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]|0x08;
	else ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]&0xF7;	
}

/* Helper function to set the discharge timer bits */
void LTC6810_set_cfgr_dcto(uint8_t nIC, cell_asic *ic, bool dcto[4])
{
	LTC681x_set_cfgr_dcto(nIC, ic, dcto);
}
