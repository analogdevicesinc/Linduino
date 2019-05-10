/*! LTC6812: Multicell Battery Monitors
*
*@verbatim
*The LTC6812 is multi-cell battery stack monitor that measures up to 15 series 
*connected battery cells with a total measurement error of less than 2.2mV. 
*The cell measurement range of 0V to 5V makes the LTC6812 suitable for most 
*battery chemistries. All 15 cell voltages can be captured in 245uS, and lower 
*data acquisition rates can be selected for high noise reduction.
*Using the LTC6812-1, multiple devices are connected in a daisy-chain with one 
*host processor connection for all devices, permitting simultaneous cell monitoring 
*of long, high voltage battery strings.
*@endverbatim
*
* https://www.analog.com/en/products/ltc6812-1.html
* The schematic of DC2350A : https://www.analog.com/media/en/technical-documentation/eval-board-schematic/DC2350A-4-SCH.PDF 
* ( Ensure that you modify the DC2350A board as mentioned on the schematic to work for LTC6812 (DC2350A-A) )
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
//! @defgroup LTC6812-1 LTC6812-1: Multicell Battery Monitor
//! @}

/*! @file
    @ingroup LTC6812-1
    Library for LTC6812-1 Multicell Battery Monitor
*/

#include "stdint.h"
#include "LTC681x.h"
#include "LTC6812.h"

/* Helper function to initialize register limits. */
void LTC6812_init_reg_limits(uint8_t total_ic, //Number of ICs in the system
							cell_asic *ic //A two dimensional array that the function stores data
							)
{
    for(uint8_t cic=0; cic<total_ic; cic++)
    {
        ic[cic].ic_reg.cell_channels=15; 
        ic[cic].ic_reg.stat_channels=4;
        ic[cic].ic_reg.aux_channels=9;
        ic[cic].ic_reg.num_cv_reg=5; 
        ic[cic].ic_reg.num_gpio_reg=4;
        ic[cic].ic_reg.num_stat_reg=2;
    } 
}

/*
This command will write the configuration registers of the LTC6812-1s
connected in a daisy chain stack. The configuration is written in descending
order so the last device's configuration is written first.
*/
void LTC6812_wrcfg(uint8_t total_ic, //The number of ICs being written to
                     cell_asic *ic //A two dimensional array of the configuration data that will be written
                    )
{
	LTC681x_wrcfg(total_ic,ic);
}

/*
This command will write the configuration b registers of the LTC6812-1s
connected in a daisy chain stack. The configuration is written in descending
order so the last device's configuration is written first.
*/
void LTC6812_wrcfgb(uint8_t total_ic, //The number of ICs being written to
                 cell_asic *ic //A two dimensional array of the configuration data that will be written
                )
{
    LTC681x_wrcfgb(total_ic,ic);
}

/* Reads configuration registers of a LTC6812 daisy chain */
int8_t LTC6812_rdcfg(uint8_t total_ic, //Number of ICs in the system
                       cell_asic *ic //A two dimensional array that the function stores the read configuration data.
                      )
{
	int8_t pec_error = 0;
	pec_error = LTC681x_rdcfg(total_ic,ic);
	return(pec_error);
}

/* Reads configuration registers B of a LTC6812 daisy chain */
int8_t LTC6812_rdcfgb(uint8_t total_ic, //Number of ICs in the system
                   cell_asic *ic //A two dimensional array that the function stores the read configuration data.
                  )
{
    int8_t pec_error = 0;
    pec_error = LTC681x_rdcfgb(total_ic,ic);
    return(pec_error);
}

/* Starts cell voltage conversion */
void LTC6812_adcv(uint8_t MD, //ADC Mode
				  uint8_t DCP, //Discharge Permit
				  uint8_t CH //Cell Channels to be measured
				)
{
    LTC681x_adcv(MD,DCP,CH);
}

/* Start a GPIO and Vref2 Conversion */
void LTC6812_adax(uint8_t MD, //ADC Mode
				  uint8_t CHG //GPIO Channels to be measured)
				)
{
	LTC681x_adax(MD,CHG);
}

/* Start a Status ADC Conversion */
void LTC6812_adstat(uint8_t MD, //ADC Mode
				    uint8_t CHST //Stat Channels to be measured
				  )
{
	LTC681x_adstat(MD,CHST);
}

/* Starts cell voltage and SOC conversion */
void LTC6812_adcvsc(uint8_t MD, //ADC Mode
					uint8_t DCP //Discharge Permit
					)
{
    LTC681x_adcvsc(MD,DCP);
}

/* Starts cell voltage  and GPIO 1 & 2 conversion */
void LTC6812_adcvax(uint8_t MD, //ADC Mode
					uint8_t DCP //Discharge Permit
					)
{
    LTC681x_adcvax(MD,DCP);
}

/*
Reads and parses the LTC6812 cell voltage registers of the LTC6812. This function will send the requested
read commands parse the data and store the cell voltages in c_codes variable.
*/
uint8_t LTC6812_rdcv(uint8_t reg, // Controls which cell voltage register is read back.
                     uint8_t total_ic, // the number of ICs in the system
                     cell_asic *ic // Array of the parsed cell codes
                    )
{
	int8_t pec_error = 0;
	pec_error = LTC681x_rdcv(reg,total_ic,ic);
	return(pec_error);
}

/*
The function is used to read the  parsed GPIO codes of the LTC6812. This function will send the requested
read commands parse the data and store the gpio voltages in a_codes variable
*/
int8_t LTC6812_rdaux(uint8_t reg, //Determines which GPIO voltage register is read back.
                       uint8_t total_ic,//The number of ICs in the system
                       cell_asic *ic//A two dimensional array of the gpio voltage codes.
                      )
{
	int8_t pec_error = 0;
	LTC681x_rdaux(reg,total_ic,ic);
	return (pec_error);
}

/*
Reads and parses the LTC6812 stat registers.
The function is used to read the  parsed stat codes of the LTC6812. 
This function will send the requested read commands parse the data 
and store the stat voltages in stat_codes variable
*/
int8_t LTC6812_rdstat(uint8_t reg, //Determines which Stat  register is read back.
                        uint8_t total_ic,//The number of ICs in the system
                        cell_asic *ic //A two dimensional array of the stat codes.
                       )
{
	int8_t pec_error = 0;
	pec_error = LTC681x_rdstat(reg,total_ic,ic);
	return (pec_error);
}

/* Sends the poll ADC command */
uint8_t LTC6812_pladc()
{
	return(LTC681x_pladc());
}

/* This function will block operation until the ADC has finished it's conversion */
uint32_t LTC6812_pollAdc()
{
	return(LTC681x_pollAdc());
}

/*
The command clears the cell voltage registers and initializes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.
*/
void LTC6812_clrcell()
{
	LTC681x_clrcell();
}

/*
The command clears the Auxiliary registers and initializes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.
*/
void LTC6812_clraux()
{
	LTC681x_clraux();
}

/*
The command clears the Stat registers and initializes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.

*/
void LTC6812_clrstat()
{
	LTC681x_clrstat();
}
  
/* Starts the Mux Decoder diagnostic self test */
void LTC6812_diagn()
{
	LTC681x_diagn();
}  

/* Starts cell voltage self test conversion */
void LTC6812_cvst(uint8_t MD, //ADC Mode
				  uint8_t ST //Self Test
				)
{
    LTC681x_cvst(MD,ST);
}

/* Start an Auxiliary Register Self Test Conversion */
void LTC6812_axst(uint8_t MD, //ADC Mode
				  uint8_t ST //Self Test
				)
{
	LTC681x_axst(MD,ST);
}

/* Start a Status Register Self Test Conversion */
void LTC6812_statst(uint8_t MD, //ADC Mode
					uint8_t ST //Self Test
					)
{
    LTC681x_statst(MD,ST);
}

/* Starts cell voltage overlap conversion */
void LTC6812_adol(uint8_t MD, //ADC Mode
				  uint8_t DCP //Discharge Permit
				)
{
	LTC681x_adol(MD,DCP);
}

/* Start an GPIO Redundancy test */
void LTC6812_adaxd(uint8_t MD, //ADC Mode
				   uint8_t CHG //GPIO Channels to be measured
				 )
{
	LTC681x_adaxd(MD,CHG);
}

/* Start a Status register redundancy test Conversion */
void LTC6812_adstatd(uint8_t MD, //ADC Mode
					 uint8_t CHST //Stat Channels to be measured
					)
{
	LTC681x_adstatd(MD,CHST);
}

/* Runs the Digital Filter Self Test */
int16_t LTC6812_run_cell_adc_st(uint8_t adc_reg, // Type of register
								uint8_t total_ic, // Number of ICs in the daisy chain
								cell_asic *ic, // A two dimensional array that will store the data 
								uint8_t md, //ADC Mode
								bool adcopt // The ADCOPT bit in the configuration register
								)
{
	int16_t error = 0;
	error = LTC681x_run_cell_adc_st(adc_reg,total_ic,ic,md,adcopt);
	return(error);
}

/* Runs the ADC overlap test for the IC */
uint16_t LTC6812_run_adc_overlap(uint8_t total_ic, // Number of ICs in the daisy chain
							     cell_asic *ic // A two dimensional array that will store the data 
								 )
{
    uint16_t error = 0;
	int32_t measure_delta =0;
	int16_t failure_pos_limit = 20;
	int16_t failure_neg_limit = -20;
	uint32_t conv_time=0;  
	wakeup_idle(total_ic);
	LTC681x_adol(MD_7KHZ_3KHZ,DCP_DISABLED);
	conv_time = LTC681x_pollAdc();

	wakeup_idle(total_ic);
	error = LTC681x_rdcv(0, total_ic,ic);
	for (int cic = 0; cic<total_ic; cic++)
	{
		

		measure_delta = (int32_t)ic[cic].cells.c_codes[6]-(int32_t)ic[cic].cells.c_codes[7]; 
		if ((measure_delta>failure_pos_limit) || (measure_delta<failure_neg_limit))
		{
		  error = error | (1<<(cic-1));
		}
		measure_delta = (int32_t)ic[cic].cells.c_codes[12]-(int32_t)ic[cic].cells.c_codes[13]; 
		if ((measure_delta>failure_pos_limit) || (measure_delta<failure_neg_limit))
		{
		  error = error | (1<<(cic-1));
		}
	}
	return(error);
}

/* Runs the redundancy self test */
int16_t LTC6812_run_adc_redundancy_st(uint8_t adc_mode, //ADC Mode
									  uint8_t adc_reg, // Type of register
									  uint8_t total_ic, // Number of ICs in the daisy chain
									  cell_asic *ic // A two dimensional array that will store the data 
									  )
{
	int16_t error = 0;
	LTC681x_run_adc_redundancy_st(adc_mode,adc_reg,total_ic,ic);
	return(error);
}

/* Start an open wire Conversion */
void LTC6812_adow(uint8_t MD, // ADC Conversion Mode
				 uint8_t PUP, //Pull up/Pull down current
				 uint8_t CH,  // Sets which Cell channels are converted
				 uint8_t DCP  // Discharge permitted during conversion
				 )
{
    LTC681x_adow(MD,PUP,CH,DCP);
}

/* Start GPIOs open wire ADC conversion */
void LTC6812_axow( uint8_t MD, //ADC Mode
				  uint8_t PUP //Pull up/Pull down current
				)
{
	LTC681x_axow(MD, PUP);
}

/* Runs the data sheet algorithm for open wire for single cell detection */
void LTC6812_run_openwire_single(uint8_t total_ic, cell_asic *ic)
{
	LTC681x_run_openwire_single(total_ic, ic);
}

/* Runs the data sheet algorithm for open wire for multiple cell and two consecutive cells detection */
void LTC6812_run_openwire_multi(uint8_t total_ic, cell_asic *ic)
{
	LTC681x_run_openwire_multi(total_ic, ic);
}

/* Runs open wire for GPIOs */
void LTC6812_run_gpio_openwire(uint8_t total_ic, 
								cell_asic *ic
								)
{
	LTC681x_run_gpio_openwire(total_ic, ic);
}

/* Helper function to set discharge bit in CFG register */
void LTC6812_set_discharge(int Cell, uint8_t total_ic, cell_asic *ic)
{
	for (int i=0; i<total_ic; i++)
	{
		if (Cell==0)
		{
			ic[i].configb.tx_data[1] = ic[i].configb.tx_data[1] |(0x04);
		}
		else if (Cell<9)
		{
		  ic[i].config.tx_data[4] = ic[i].config.tx_data[4] | (1<<(Cell-1));
		}
		else if (Cell < 13)
		{
		  ic[i].config.tx_data[5] = ic[i].config.tx_data[5] | (1<<(Cell-9));
		}
		else if (Cell<16)
		{
		  ic[i].configb.tx_data[0] = ic[i].configb.tx_data[0] | (1<<(Cell-9));
		}
		else
		{
			break;
		}
	}
}

/* Clears all of the DCC bits in the configuration registers */
void LTC6812_clear_discharge(uint8_t total_ic, //Number of ICs in the system
                             cell_asic *ic //A two dimensional array of the data
							 )
{
    LTC681x_clear_discharge(total_ic,ic);

}

/* Writes the pwm registers of a LTC6812 daisy chain */
void LTC6812_wrpwm(uint8_t total_ic, //The number of ICs being written to
                     uint8_t pwmReg, // Register Select 
                     cell_asic *ic //A two dimensional array of the data that will be written
                    )
{
	LTC681x_wrpwm(total_ic,pwmReg,ic);
}

/* Reads pwm registers of a LTC6812 daisy chain */
int8_t LTC6812_rdpwm(uint8_t total_ic, //Number of ICs in the system
                       uint8_t pwmReg, //Register Select 
                       cell_asic *ic //A two dimensional array that the function stores the read data.
                      )
{
	int8_t pec_error =0;
	pec_error = LTC681x_rdpwm(total_ic,pwmReg,ic);
	return(pec_error);
}
  
/* Writes data in S control register the LTC6812-1s connected in a daisy chain stack. */ 
void LTC6812_wrsctrl(uint8_t total_ic, //Number of ICs in the daisy chain
                     uint8_t sctrl_reg, //Register Select 
                     cell_asic *ic //A two dimensional array of the data that will be written
                    )
{
	LTC681x_wrsctrl(total_ic, sctrl_reg, ic);
}
  
/* Reads sctrl registers of a LTC6812 daisy chain */   
int8_t LTC6812_rdsctrl(uint8_t total_ic, //Number of ICs in the daisy chain
                       uint8_t sctrl_reg, //Register Select 
                       cell_asic *ic //A two dimensional array that the function stores the read data
                      )
{
	LTC681x_rdsctrl( total_ic, sctrl_reg,ic )	;
 }

/* Start Sctrl data communication               
This command will start the sctrl pulse communication over the spins
*/
void LTC6812_stsctrl()
{
	LTC681x_stsctrl();
 }
  
/*
The command clears the Sctrl registers and initializes
all values to 0. The register will read back hexadecimal 0x00
after the command is sent.
*/
void LTC6812_clrsctrl()
{
	LTC681x_clrsctrl();
}
  
/* Write the 6812 PWM/Sctrl Register B */
void LTC6812_wrpsb(uint8_t total_ic, //Number of ICs in the daisy chain
					cell_asic *ic //A two dimensional array of the data that will be written
					)
{
	uint8_t cmd[2];
	uint8_t write_buffer[256];
	uint8_t c_ic = 0;
	
	cmd[0] = 0x00;
	cmd[1] = 0x1C;
	for(uint8_t current_ic = 0; current_ic<total_ic;current_ic++)
	{
		if(ic->isospi_reverse == true){c_ic = current_ic;}
		else{c_ic = total_ic - current_ic - 1;}
		
		write_buffer[0] = ic[c_ic].pwmb.tx_data[0];
		write_buffer[1] = ic[c_ic].pwmb.tx_data[1]&(0x0F);
		write_buffer[2]= 0x00;
		write_buffer[3] = ic[c_ic].sctrlb.tx_data[3];
		write_buffer[4] = ic[c_ic].sctrlb.tx_data[4]&(0x0F);
		write_buffer[5]= 0x00;
	}
	write_68(total_ic, cmd, write_buffer);		
}

/* Reading 6812 PWM/Sctrl Register B */
int8_t LTC6812_rdpsb(uint8_t total_ic, //Number of ICs in the daisy chain
                       cell_asic *ic //A two dimensional array that the function stores the read data
                      )	
{
	uint8_t cmd[4];
    uint8_t read_buffer[256];
    int8_t pec_error = 0;
    uint16_t data_pec;
    uint16_t calc_pec;
    uint8_t c_ic = 0;
    
   
      cmd[0] = 0x00;
      cmd[1] = 0x1E;
	 
    
    pec_error = read_68(total_ic, cmd, read_buffer);

    for(uint8_t current_ic =0; current_ic<total_ic; current_ic++)
    {	
        if(ic->isospi_reverse == false){c_ic = current_ic;}
        else{c_ic = total_ic - current_ic - 1;}
	
        for(int byte=0; byte<3;byte++)
        {
            ic[c_ic].pwmb.rx_data[byte] = read_buffer[byte+(8*current_ic)]; 			
        }
		for(int byte=3; byte<6;byte++)
        {
            ic[c_ic].sctrlb.rx_data[byte] = read_buffer[byte+(8*current_ic)]; 			
        }
		for(int byte=6; byte<8;byte++)
        {
            ic[c_ic].pwmb.rx_data[byte] = read_buffer[byte+(8*current_ic)]; 	
			ic[c_ic].sctrlb.rx_data[byte] = read_buffer[byte+(8*current_ic)];			
        }
		
        calc_pec = pec15_calc(6,&read_buffer[8*current_ic]);
        data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);
        if(calc_pec != data_pec )
        {
            ic[c_ic].pwmb.rx_pec_match = 1;
			ic[c_ic].sctrlb.rx_pec_match = 1;
        }
        else 
		{
			ic[c_ic].pwmb.rx_pec_match = 0;
			ic[c_ic].sctrlb.rx_pec_match = 0;
		}
    }
    return(pec_error);
}  
  
/*  Writes the COMM registers of a LTC6812 daisy chain */
void LTC6812_wrcomm(uint8_t total_ic, //The number of ICs being written to
                      cell_asic *ic //A two dimensional array of the comm data that will be written
                     )
{
	LTC681x_wrcomm(total_ic,ic);
}

/*  Reads COMM registers of a LTC6812 daisy chain */
int8_t LTC6812_rdcomm(uint8_t total_ic, //Number of ICs in the system
                        cell_asic *ic //A two dimensional array that the function stores the read data.
                       )
{
	int8_t pec_error = 0;
	LTC681x_rdcomm(total_ic, ic);
	return(pec_error);
}

/* Shifts data in COMM register out over LTC6812 SPI/I2C port */
void LTC6812_stcomm(uint8_t len) //Length of data to be transmitted 
{
    LTC681x_stcomm(len);
}

/* Mutes the LTC6812 discharge transistors */
void LTC6812_mute()
{
	uint8_t cmd[2];

	cmd[0] = 0x00;
	cmd[1] = 0x28;	
	cmd_68(cmd);
}

/* Clears the LTC6812 Mute Discharge */
void LTC6812_unmute()
{
	uint8_t cmd[2];

	cmd[0] = 0x00;
	cmd[1] = 0x29;
	cmd_68(cmd);
}

/* Helper function that increments PEC counters */
void LTC6812_check_pec(uint8_t total_ic,uint8_t reg, cell_asic *ic)
{
    LTC681x_check_pec(total_ic,reg,ic);
}

/* Helper Function to reset PEC counters */
void LTC6812_reset_crc_count(uint8_t total_ic, cell_asic *ic)
{
	LTC681x_reset_crc_count(total_ic,ic);
}

/* Helper function to initialize CFG variables */
void LTC6812_init_cfg(uint8_t total_ic, cell_asic *ic)
{
	LTC681x_init_cfg(total_ic,ic);
}
/* Helper function to set CFGR variable */
void LTC6812_set_cfgr(uint8_t nIC, cell_asic *ic, bool refon, bool adcopt, bool gpio[5],bool dcc[12],bool dcto[4], uint16_t uv, uint16_t  ov)
{
    LTC681x_set_cfgr_refon(nIC,ic,refon);
    LTC681x_set_cfgr_adcopt(nIC,ic,adcopt);
    LTC681x_set_cfgr_gpio(nIC,ic,gpio);
    LTC681x_set_cfgr_dis(nIC,ic,dcc);
	LTC681x_set_cfgr_dcto(nIC,ic,dcto);
	LTC681x_set_cfgr_uv(nIC, ic, uv);
    LTC681x_set_cfgr_ov(nIC, ic, ov);
}
/* Helper function to set the REFON bit */
void LTC6812_set_cfgr_refon(uint8_t nIC, cell_asic *ic, bool refon) 
{
	LTC681x_set_cfgr_refon(nIC,ic,refon);
}
/* Helper function to set the adcopt bit */
void LTC6812_set_cfgr_adcopt(uint8_t nIC, cell_asic *ic, bool adcopt)
{
	LTC681x_set_cfgr_adcopt(nIC,ic,adcopt);
}
/* Helper function to set GPIO bits */
void LTC6812_set_cfgr_gpio(uint8_t nIC, cell_asic *ic,bool gpio[5])
{  
	LTC681x_set_cfgr_gpio(nIC,ic,gpio);
}
/* Helper function to control discharge */
void LTC6812_set_cfgr_dis(uint8_t nIC, cell_asic *ic,bool dcc[12])
{  
	LTC681x_set_cfgr_dis(nIC,ic,dcc);
}
/* Helper Function to set dcto value in CFG register */
void LTC6812_set_cfgr_dcto(uint8_t nIC, cell_asic *ic,bool dcto[4])
{
    LTC681x_set_cfgr_dcto(nIC, ic, dcto);
}
/* Helper Function to set uv value in CFG register */
void LTC6812_set_cfgr_uv(uint8_t nIC, cell_asic *ic,uint16_t uv)
{
    LTC681x_set_cfgr_uv(nIC, ic, uv);
}
/* Helper function to set OV value in CFG register */
void LTC6812_set_cfgr_ov(uint8_t nIC, cell_asic *ic,uint16_t ov)
{
    LTC681x_set_cfgr_ov( nIC, ic, ov);
}

/*  Helper Function to initialize the CFGRB data structures */
void LTC6812_init_cfgb(uint8_t total_ic, cell_asic *ic)
{
	 for (uint8_t current_ic = 0; current_ic<total_ic;current_ic++)
    {
        for(int j =0; j<6;j++)
        {
            ic[current_ic].configb.tx_data[j] = 0;
        }
    }
}

/*  Helper Function to set the configuration register B */ 
void LTC6812_set_cfgrb(uint8_t nIC, cell_asic *ic,bool fdrf,bool dtmen,bool ps[2],bool gpiobits[4],bool dccbits[4])
{
	LTC6812_set_cfgrb_fdrf(nIC,ic,fdrf);
    LTC6812_set_cfgrb_dtmen(nIC,ic,dtmen);
    LTC6812_set_cfgrb_ps(nIC,ic,ps);
    LTC6812_set_cfgrb_gpio_b(nIC,ic,gpiobits);
	LTC6812_set_cfgrb_dcc_b(nIC,ic,dccbits);	
}

/* Helper function to set the FDRF bit */
void LTC6812_set_cfgrb_fdrf(uint8_t nIC, cell_asic *ic, bool fdrf) 
{
	if(fdrf) ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1]|0x40;
	else ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1]&0xBF;
}

/* Helper function to set the DTMEN bit */
void LTC6812_set_cfgrb_dtmen(uint8_t nIC, cell_asic *ic, bool dtmen) 
{
	if(dtmen) ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1]|0x08;
	else ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1]&0xF7;
}
	
/* Helper function to set the PATH SELECT bit */
void LTC6812_set_cfgrb_ps(uint8_t nIC, cell_asic *ic, bool ps[]) 
{
	for(int i =0;i<2;i++)
	{
      if(ps[i])ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1]|(0x01<<(i+4));
      else ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1]&(~(0x01<<(i+4)));
	}
}

/*  Helper function to set the gpio bits in config b register */
void LTC6812_set_cfgrb_gpio_b(uint8_t nIC, cell_asic *ic, bool gpiobits[]) 
{
	for(int i =0;i<4;i++)
	{
      if(gpiobits[i])ic[nIC].configb.tx_data[0] = ic[nIC].configb.tx_data[0]|(0x01<<i);
      else ic[nIC].configb.tx_data[0] = ic[nIC].configb.tx_data[0]&(~(0x01<<i));
	}
}

/*  Helper function to set the dcc bits in config b register */
void LTC6812_set_cfgrb_dcc_b(uint8_t nIC, cell_asic *ic, bool dccbits[]) 
{
	
	for(int i =0;i<4;i++)
   { 
		if(i==0)
		{
			if(dccbits[i])ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1]|0x04;
			else ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1]&0xFB;
		}
		else
		{	
			if(dccbits[i])ic[nIC].configb.tx_data[0] = ic[nIC].configb.tx_data[0]|(0x01<<(i+3));
			else ic[nIC].configb.tx_data[0] = ic[nIC].configb.tx_data[0]&(~(0x01<<(i+3)));
		}
		
   }
}
