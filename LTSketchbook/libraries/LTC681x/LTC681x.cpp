/*! General BMS Library
***************************************************************************//**
*   @file     LTC681x.cpp  
*   @author BMS (bms.support@analog.com)

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

/*! @file
    Library for LTC681x Multi-cell Battery Monitor
*/

#include <stdint.h>
#include "LTC681x.h"
#include "bms_hardware.h"

#ifdef LINDUINO
#include <Arduino.h>
#endif

/* Wake isoSPI up from IDlE state and enters the READY state */
void wakeup_idle(uint8_t total_ic) //Number of ICs in the system
{
	for (int i =0; i<total_ic; i++)
	{
	   cs_low(CS_PIN);
	   spi_read_byte(0xff);//Guarantees the isoSPI will be in ready mode
	   cs_high(CS_PIN);
	}
}

/* Generic wakeup command to wake the LTC681x from sleep state */
void wakeup_sleep(uint8_t total_ic) //Number of ICs in the system
{
	for (int i =0; i<total_ic; i++)
	{
	   cs_low(CS_PIN);
	   delay_u(300); // Guarantees the LTC681x will be in standby
	   cs_high(CS_PIN);
	   delay_u(10);
	}
}

/* Generic function to write 68xx commands. Function calculates PEC for tx_cmd data. */
void cmd_68(uint8_t tx_cmd[2]) //The command to be transmitted
{
	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t md_bits;
	
	cmd[0] = tx_cmd[0];
	cmd[1] =  tx_cmd[1];
	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	
	cs_low(CS_PIN);
	spi_write_array(4,cmd);
	cs_high(CS_PIN);
}

/* 
Generic function to write 68xx commands and write payload data. 
Function calculates PEC for tx_cmd data and the data to be transmitted.
 */
void write_68(uint8_t total_ic, //Number of ICs to be written to 
			  uint8_t tx_cmd[2], //The command to be transmitted 
			  uint8_t data[] // Payload Data
			  )
{
	const uint8_t BYTES_IN_REG = 6;
	const uint8_t CMD_LEN = 4+(8*total_ic);
	uint8_t *cmd;
	uint16_t data_pec;
	uint16_t cmd_pec;
	uint8_t cmd_index;
	
	cmd = (uint8_t *)malloc(CMD_LEN*sizeof(uint8_t));
	cmd[0] = tx_cmd[0];
	cmd[1] = tx_cmd[1];
	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	
	cmd_index = 4;
	for (uint8_t current_ic = total_ic; current_ic > 0; current_ic--)               // Executes for each LTC681x, this loops starts with the last IC on the stack.
    {	                                                                            //The first configuration written is received by the last IC in the daisy chain
		for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
		{
			cmd[cmd_index] = data[((current_ic-1)*6)+current_byte];
			cmd_index = cmd_index + 1;
		}
		
		data_pec = (uint16_t)pec15_calc(BYTES_IN_REG, &data[(current_ic-1)*6]);    // Calculating the PEC for each ICs configuration register data
		cmd[cmd_index] = (uint8_t)(data_pec >> 8);
		cmd[cmd_index + 1] = (uint8_t)data_pec;
		cmd_index = cmd_index + 2;
	}
	
	cs_low(CS_PIN);
	spi_write_array(CMD_LEN, cmd);
	cs_high(CS_PIN);
	
	free(cmd);
}

/* Generic function to write 68xx commands and read data. Function calculated PEC for tx_cmd data */
int8_t read_68( uint8_t total_ic, // Number of ICs in the system 
				uint8_t tx_cmd[2], // The command to be transmitted 
				uint8_t *rx_data // Data to be read
				)
{
	const uint8_t BYTES_IN_REG = 8;
	uint8_t cmd[4];
	uint8_t data[256];
	int8_t pec_error = 0;
	uint16_t cmd_pec;
	uint16_t data_pec;
	uint16_t received_pec;
	
	cmd[0] = tx_cmd[0];
	cmd[1] = tx_cmd[1];
	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	
	cs_low(CS_PIN);
	spi_write_read(cmd, 4, data, (BYTES_IN_REG*total_ic));         //Transmits the command and reads the configuration data of all ICs on the daisy chain into rx_data[] array
	cs_high(CS_PIN);                                         

	for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++) //Executes for each LTC681x in the daisy chain and packs the data
	{																//into the rx_data array as well as check the received data for any bit errors
		for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
		{
			rx_data[(current_ic*8)+current_byte] = data[current_byte + (current_ic*BYTES_IN_REG)];
		}
		
		received_pec = (rx_data[(current_ic*8)+6]<<8) + rx_data[(current_ic*8)+7];
		data_pec = pec15_calc(6, &rx_data[current_ic*8]);
		
		if (received_pec != data_pec)
		{
		  pec_error = -1;
		}
	}
	
	return(pec_error);
}

/* Calculates  and returns the CRC15 */
uint16_t pec15_calc(uint8_t len, //Number of bytes that will be used to calculate a PEC
                    uint8_t *data //Array of data that will be used to calculate  a PEC
                   )
{
	uint16_t remainder,addr;
	remainder = 16;//initialize the PEC
	
	for (uint8_t i = 0; i<len; i++) // loops for each byte in data array
	{
		addr = ((remainder>>7)^data[i])&0xff;//calculate PEC table address
		#ifdef MBED
			remainder = (remainder<<8)^crc15Table[addr];
		#else
			remainder = (remainder<<8)^pgm_read_word_near(crc15Table+addr);
		#endif
	}
	
	return(remainder*2);//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
}

/* Write the LTC681x CFGRA */
void LTC681x_wrcfg(uint8_t total_ic, //The number of ICs being written to
                   cell_asic ic[]  // A two dimensional array of the configuration data that will be written
                  )
{
	uint8_t cmd[2] = {0x00 , 0x01} ;
	uint8_t write_buffer[256];
	uint8_t write_count = 0;
	uint8_t c_ic = 0;
	
	for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
	{
		if (ic->isospi_reverse == false)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = total_ic - current_ic - 1;
		}
		
		for (uint8_t data = 0; data<6; data++)
		{
			write_buffer[write_count] = ic[c_ic].config.tx_data[data];
			write_count++;
		}
	}
	write_68(total_ic, cmd, write_buffer);
}

/* Write the LTC681x CFGRB */
void LTC681x_wrcfgb(uint8_t total_ic, //The number of ICs being written to
                    cell_asic ic[] // A two dimensional array of the configuration data that will be written
                   )
{
	uint8_t cmd[2] = {0x00 , 0x24} ;
	uint8_t write_buffer[256];
	uint8_t write_count = 0;
	uint8_t c_ic = 0;
	
	for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
	{
		if (ic->isospi_reverse == false)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = total_ic - current_ic - 1;
		}
		
		for (uint8_t data = 0; data<6; data++)
		{
			write_buffer[write_count] = ic[c_ic].configb.tx_data[data];
			write_count++;
		}
	}
	write_68(total_ic, cmd, write_buffer);
}

/* Read the LTC681x CFGA */
int8_t LTC681x_rdcfg(uint8_t total_ic, //Number of ICs in the system
                     cell_asic ic[] // A two dimensional array that the function stores the read configuration data.
                    )
{
	uint8_t cmd[2]= {0x00 , 0x02};
	uint8_t read_buffer[256];
	int8_t pec_error = 0;
	uint16_t data_pec;
	uint16_t calc_pec;
	uint8_t c_ic = 0;
	
	pec_error = read_68(total_ic, cmd, read_buffer);
	
	for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
	{
		if (ic->isospi_reverse == false)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = total_ic - current_ic - 1;
		}
		
		for (int byte=0; byte<8; byte++)
		{
			ic[c_ic].config.rx_data[byte] = read_buffer[byte+(8*current_ic)];
		}
		
		calc_pec = pec15_calc(6,&read_buffer[8*current_ic]);
		data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);
		if (calc_pec != data_pec )
		{
			ic[c_ic].config.rx_pec_match = 1;
		}
		else ic[c_ic].config.rx_pec_match = 0;
	}
	LTC681x_check_pec(total_ic,CFGR,ic);
	
	return(pec_error);
}

/* Reads the LTC681x CFGB */
int8_t LTC681x_rdcfgb(uint8_t total_ic, //Number of ICs in the system
                      cell_asic ic[] // A two dimensional array that the function stores the read configuration data.
                     )
{
	uint8_t cmd[2]= {0x00 , 0x26};
	uint8_t read_buffer[256];
	int8_t pec_error = 0;
	uint16_t data_pec;
	uint16_t calc_pec;
	uint8_t c_ic = 0;
	
	pec_error = read_68(total_ic, cmd, read_buffer);
	
	for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
	{
		if (ic->isospi_reverse == false)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = total_ic - current_ic - 1;
		}
		
		for (int byte=0; byte<8; byte++)
		{
			ic[c_ic].configb.rx_data[byte] = read_buffer[byte+(8*current_ic)];
		}
		
		calc_pec = pec15_calc(6,&read_buffer[8*current_ic]);
		data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);
		if (calc_pec != data_pec )
		{
			ic[c_ic].configb.rx_pec_match = 1;
		}
		else ic[c_ic].configb.rx_pec_match = 0;
	}
	LTC681x_check_pec(total_ic,CFGRB,ic);
	
	return(pec_error);
}

/* Starts ADC conversion for cell voltage */
void LTC681x_adcv( uint8_t MD, //ADC Mode
				   uint8_t DCP, //Discharge Permit
				   uint8_t CH //Cell Channels to be measured
                 )
{
	uint8_t cmd[2];
	uint8_t md_bits;
	
	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x02;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + 0x60 + (DCP<<4) + CH;
	
	cmd_68(cmd);
}

/* Start ADC Conversion for GPIO and Vref2  */
void LTC681x_adax(uint8_t MD, //ADC Mode
				  uint8_t CHG //GPIO Channels to be measured
				  )
{
	uint8_t cmd[4];
	uint8_t md_bits;
	
	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] = md_bits + 0x60 + CHG ;
	
	cmd_68(cmd);
}

/* Start ADC Conversion for Status  */
void LTC681x_adstat(uint8_t MD, //ADC Mode
				    uint8_t CHST //Stat Channels to be measured
				    )
{
	uint8_t cmd[4];
	uint8_t md_bits;
	
	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] = md_bits + 0x68 + CHST ;
	
	cmd_68(cmd);
}

/* Starts cell voltage and SOC conversion */
void LTC681x_adcvsc(uint8_t MD, //ADC Mode
					uint8_t DCP //Discharge Permit
					)
{
	uint8_t cmd[2];
	uint8_t md_bits;
	
	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits | 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits | 0x60 | (DCP<<4) | 0x07;
	
	cmd_68(cmd);
}

/* Starts cell voltage and GPIO 1&2 conversion */
void LTC681x_adcvax(uint8_t MD, //ADC Mode
					uint8_t DCP //Discharge Permit
				   )
{
	uint8_t cmd[2];
	uint8_t md_bits;
	
	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits | 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits | ((DCP&0x01)<<4) + 0x6F;
	
	cmd_68(cmd);
}

/*
Reads and parses the LTC681x cell voltage registers.
The function is used to read the parsed Cell voltages codes of the LTC681x. 
This function will send the requested read commands parse the data 
and store the cell voltages in c_codes variable.
*/
uint8_t LTC681x_rdcv(uint8_t reg, // Controls which cell voltage register is read back.
                     uint8_t total_ic, // The number of ICs in the system
                     cell_asic *ic // Array of the parsed cell codes
                    )
{
	int8_t pec_error = 0;
	uint8_t *cell_data;
	uint8_t c_ic = 0;
	cell_data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));

	if (reg == 0)
	{
		for (uint8_t cell_reg = 1; cell_reg<ic[0].ic_reg.num_cv_reg+1; cell_reg++) //Executes once for each of the LTC681x cell voltage registers
		{
			LTC681x_rdcv_reg(cell_reg, total_ic,cell_data );
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
			pec_error = pec_error + parse_cells(current_ic,cell_reg, cell_data,
												&ic[c_ic].cells.c_codes[0],
												&ic[c_ic].cells.pec_match[0]);
			}
		}
	}

	else
	{
		LTC681x_rdcv_reg(reg, total_ic,cell_data);

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
			pec_error = pec_error + parse_cells(current_ic,reg, &cell_data[8*c_ic],
											  &ic[c_ic].cells.c_codes[0],
											  &ic[c_ic].cells.pec_match[0]);
		}
	}
	LTC681x_check_pec(total_ic,CELL,ic);
	free(cell_data);

	return(pec_error);
}

/*
The function is used to read the  parsed GPIO codes of the LTC681x. 
This function will send the requested read commands parse the data 
and store the gpio voltages in a_codes variable.
*/
int8_t LTC681x_rdaux(uint8_t reg, //Determines which GPIO voltage register is read back.
                     uint8_t total_ic,//The number of ICs in the system
                     cell_asic *ic//A two dimensional array of the gpio voltage codes.
                    )
{
	uint8_t *data;
	int8_t pec_error = 0;
	uint8_t c_ic =0;
	data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));

	if (reg == 0)
	{
		for (uint8_t gpio_reg = 1; gpio_reg<ic[0].ic_reg.num_gpio_reg+1; gpio_reg++) //Executes once for each of the LTC681x aux voltage registers
		{
			LTC681x_rdaux_reg(gpio_reg, total_ic,data);                 //Reads the raw auxiliary register data into the data[] array
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
				pec_error = parse_cells(current_ic,gpio_reg, data,
										&ic[c_ic].aux.a_codes[0],
										&ic[c_ic].aux.pec_match[0]);
			}
		}
	}
	else
	{
		LTC681x_rdaux_reg(reg, total_ic, data);

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
			pec_error = parse_cells(current_ic,reg, data,
								  &ic[c_ic].aux.a_codes[0],
								  &ic[c_ic].aux.pec_match[0]);
		}
	}
	LTC681x_check_pec(total_ic,AUX,ic);
	free(data);

	return (pec_error);
}

/*
Reads and parses the LTC681x stat registers.
The function is used to read the  parsed Stat codes of the LTC681x. 
This function will send the requested read commands parse the data 
and store the gpio voltages in stat_codes variable.
*/
int8_t LTC681x_rdstat(uint8_t reg, //Determines which Stat  register is read back.
                      uint8_t total_ic,//The number of ICs in the system
                      cell_asic *ic //A two dimensional array of the stat codes.
                     )

{
	const uint8_t BYT_IN_REG = 6;
	const uint8_t STAT_IN_REG = 3;
	uint8_t *data;
	uint8_t data_counter = 0;
	int8_t pec_error = 0;
	uint16_t parsed_stat;
	uint16_t received_pec;
	uint16_t data_pec;
	uint8_t c_ic = 0;
	
	data = (uint8_t *) malloc((12*total_ic)*sizeof(uint8_t));
	
	if (reg == 0)
	{
		for (uint8_t stat_reg = 1; stat_reg< 3; stat_reg++)                      //Executes once for each of the LTC681x stat voltage registers
		{
			data_counter = 0;
			LTC681x_rdstat_reg(stat_reg, total_ic,data);                            //Reads the raw status register data into the data[] array
		
			for (uint8_t current_ic = 0 ; current_ic < total_ic; current_ic++)      // Executes for every LTC681x in the daisy chain
			{																		// current_ic is used as the IC counter
				if (ic->isospi_reverse == false)
				{
					c_ic = current_ic;
				}
				else
				{
					c_ic = total_ic - current_ic - 1;
				}

				if (stat_reg ==1)
				{
					for (uint8_t current_stat = 0; current_stat< STAT_IN_REG; current_stat++) // This loop parses the read back data into Status registers, 
					{																		 // it loops once for each of the 3 stat codes in the register
						parsed_stat = data[data_counter] + (data[data_counter+1]<<8);       //Each stat codes is received as two bytes and is combined to create the parsed status code
						ic[c_ic].stat.stat_codes[current_stat] = parsed_stat;
						data_counter=data_counter+2;                                       //Because stat codes are two bytes the data counter
					}
				}
				else if (stat_reg == 2)
				{
					parsed_stat = data[data_counter] + (data[data_counter+1]<<8);          //Each stat is received as two bytes and is combined to create the parsed status code
					data_counter = data_counter +2;
					ic[c_ic].stat.stat_codes[3] = parsed_stat;
					ic[c_ic].stat.flags[0] = data[data_counter++];
					ic[c_ic].stat.flags[1] = data[data_counter++];
					ic[c_ic].stat.flags[2] = data[data_counter++];
					ic[c_ic].stat.mux_fail[0] = (data[data_counter] & 0x02)>>1;
					ic[c_ic].stat.thsd[0] = data[data_counter++] & 0x01;
				}
			
				received_pec = (data[data_counter]<<8)+ data[data_counter+1];        //The received PEC for the current_ic is transmitted as the 7th and 8th
																					//after the 6 status data bytes
				data_pec = pec15_calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT]);
			
				if (received_pec != data_pec)
				{
					pec_error = -1;                         //The pec_error variable is simply set negative if any PEC errors
					ic[c_ic].stat.pec_match[stat_reg-1]=1;  //are detected in the received serial data
					
				}
				else
				{
					ic[c_ic].stat.pec_match[stat_reg-1]=0;
				}
			
				data_counter=data_counter+2;    //Because the transmitted PEC code is 2 bytes long the data_counter
											//must be incremented by 2 bytes to point to the next ICs status data
			}
		}
	}
	else
	{
		LTC681x_rdstat_reg(reg, total_ic, data);
		for (int current_ic = 0 ; current_ic < total_ic; current_ic++)            // Executes for every LTC681x in the daisy chain
		{																		  // current_ic is used as an IC counter																			
			if (ic->isospi_reverse == false)
			{
			c_ic = current_ic;
			}
			else
			{
			c_ic = total_ic - current_ic - 1;
			}
			if (reg ==1)
			{
				for (uint8_t current_stat = 0; current_stat< STAT_IN_REG; current_stat++) // This loop parses the read back data into Status voltages, it
				{																		  // loops once for each of the 3 stat codes in the register
					
					parsed_stat = data[data_counter] + (data[data_counter+1]<<8);           //Each stat codes is received as two bytes and is combined to
																							// create the parsed stat code
			
					ic[c_ic].stat.stat_codes[current_stat] = parsed_stat;
					data_counter=data_counter+2;                     //Because stat codes are two bytes the data counter
																	//must increment by two for each parsed stat code
				}
			}
			else if (reg == 2)
			{
				parsed_stat = data[data_counter++] + (data[data_counter++]<<8); //Each stat codes is received as two bytes and is combined to
				ic[c_ic].stat.stat_codes[3] = parsed_stat;
				ic[c_ic].stat.flags[0] = data[data_counter++];
				ic[c_ic].stat.flags[1] = data[data_counter++];
				ic[c_ic].stat.flags[2] = data[data_counter++];
				ic[c_ic].stat.mux_fail[0] = (data[data_counter] & 0x02)>>1;
				ic[c_ic].stat.thsd[0] = data[data_counter++] & 0x01;
			}
		
			received_pec = (data[data_counter]<<8)+ data[data_counter+1]; //The received PEC for the current_ic is transmitted as the 7th and 8th
																		  //after the 6 status data bytes
			data_pec = pec15_calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT]);
			if (received_pec != data_pec)
			{
				pec_error = -1;                  //The pec_error variable is simply set negative if any PEC errors
				ic[c_ic].stat.pec_match[reg-1]=1;
			}
		
			data_counter=data_counter+2;
		}
	}
	LTC681x_check_pec(total_ic,STAT,ic);
	
	free(data);
	
	return (pec_error);
}

/* Writes the command and reads the raw cell voltage register data */
void LTC681x_rdcv_reg(uint8_t reg, //Determines which cell voltage register is read back
                      uint8_t total_ic, //the number of ICs in the
                      uint8_t *data //An array of the unparsed cell codes
                     )
{
	const uint8_t REG_LEN = 8; //Number of bytes in each ICs register + 2 bytes for the PEC
	uint8_t cmd[4];
	uint16_t cmd_pec;

	if (reg == 1)     //1: RDCVA
	{
		cmd[1] = 0x04;
		cmd[0] = 0x00;
	}
	else if (reg == 2) //2: RDCVB
	{
		cmd[1] = 0x06;
		cmd[0] = 0x00;
	}
	else if (reg == 3) //3: RDCVC
	{
		cmd[1] = 0x08;
		cmd[0] = 0x00;
	}
	else if (reg == 4) //4: RDCVD
	{
		cmd[1] = 0x0A;
		cmd[0] = 0x00;
	}
	else if (reg == 5) //4: RDCVE
	{
		cmd[1] = 0x09;
		cmd[0] = 0x00;
	}
	else if (reg == 6) //4: RDCVF
	{
		cmd[1] = 0x0B;
		cmd[0] = 0x00;
	}

	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	cs_low(CS_PIN);
	spi_write_read(cmd,4,data,(REG_LEN*total_ic));
	cs_high(CS_PIN);
}

/*
The function reads a single GPIO voltage register and stores the read data
in the *data point as a byte array. This function is rarely used outside of
the LTC681x_rdaux() command.
*/
void LTC681x_rdaux_reg(uint8_t reg, //Determines which GPIO voltage register is read back
                       uint8_t total_ic, //The number of ICs in the system
                       uint8_t *data //Array of the unparsed auxiliary codes
                      )
{
	const uint8_t REG_LEN = 8; // Number of bytes in the register + 2 bytes for the PEC
	uint8_t cmd[4];
	uint16_t cmd_pec;

	if (reg == 1)     //Read back auxiliary group A
	{
		cmd[1] = 0x0C;
		cmd[0] = 0x00;
	}
	else if (reg == 2)  //Read back auxiliary group B
	{
		cmd[1] = 0x0E;
		cmd[0] = 0x00;
	}
	else if (reg == 3)  //Read back auxiliary group C
	{
		cmd[1] = 0x0D;
		cmd[0] = 0x00;
	}
	else if (reg == 4)  //Read back auxiliary group D
	{
		cmd[1] = 0x0F;
		cmd[0] = 0x00;
	}
	else          //Read back auxiliary group A
	{
		cmd[1] = 0x0C;
		cmd[0] = 0x00;
	}

	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	cs_low(CS_PIN);
	spi_write_read(cmd,4,data,(REG_LEN*total_ic));
	cs_high(CS_PIN);
}

/*
The function reads a single stat  register and stores the read data
in the *data point as a byte array. This function is rarely used outside of
the LTC681x_rdstat() command.
*/
void LTC681x_rdstat_reg(uint8_t reg, //Determines which stat register is read back
                        uint8_t total_ic, //The number of ICs in the system
                        uint8_t *data //Array of the unparsed stat codes
                       )
{
	const uint8_t REG_LEN = 8; // number of bytes in the register + 2 bytes for the PEC
	uint8_t cmd[4];
	uint16_t cmd_pec;

	if (reg == 1)     //Read back status group A
	{
		cmd[1] = 0x10;
		cmd[0] = 0x00;
	}
	else if (reg == 2)  //Read back status group B
	{
		cmd[1] = 0x12;
		cmd[0] = 0x00;
	}

	else          //Read back status group A
	{
		cmd[1] = 0x10;
		cmd[0] = 0x00;
	}

	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	cs_low(CS_PIN);
	spi_write_read(cmd,4,data,(REG_LEN*total_ic));
	cs_high(CS_PIN);
}

/* Helper function that parses voltage measurement registers */
int8_t parse_cells(uint8_t current_ic, // Current IC
					uint8_t cell_reg,  // Type of register
					uint8_t cell_data[], // Unparsed data
					uint16_t *cell_codes, // Parsed data
					uint8_t *ic_pec // PEC error
					)
{
	const uint8_t BYT_IN_REG = 6;
	const uint8_t CELL_IN_REG = 3;
	int8_t pec_error = 0;
	uint16_t parsed_cell;
	uint16_t received_pec;
	uint16_t data_pec;
	uint8_t data_counter = current_ic*NUM_RX_BYT; //data counter


	for (uint8_t current_cell = 0; current_cell<CELL_IN_REG; current_cell++) // This loop parses the read back data into the register codes, it
	{																		// loops once for each of the 3 codes in the register
																			
		parsed_cell = cell_data[data_counter] + (cell_data[data_counter + 1] << 8);//Each code is received as two bytes and is combined to
																				   // create the parsed code
		cell_codes[current_cell  + ((cell_reg - 1) * CELL_IN_REG)] = parsed_cell;

		data_counter = data_counter + 2;                       //Because the codes are two bytes, the data counter
															  //must increment by two for each parsed code
	}
	received_pec = (cell_data[data_counter] << 8) | cell_data[data_counter+1]; //The received PEC for the current_ic is transmitted as the 7th and 8th
																			   //after the 6 cell voltage data bytes
	data_pec = pec15_calc(BYT_IN_REG, &cell_data[(current_ic) * NUM_RX_BYT]);

	if (received_pec != data_pec)
	{
		pec_error = 1;                             //The pec_error variable is simply set negative if any PEC errors
		ic_pec[cell_reg-1]=1;
	}
	else
	{
		ic_pec[cell_reg-1]=0;
	}
	data_counter=data_counter+2;
	
	return(pec_error);
}

/* Sends the poll ADC command */
uint8_t LTC681x_pladc()
{
	uint8_t cmd[4];
	uint8_t adc_state = 0xFF;
	uint16_t cmd_pec;
	
	cmd[0] = 0x07;
	cmd[1] = 0x14;
	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	
	cs_low(CS_PIN);
	spi_write_array(4,cmd);
	adc_state = spi_read_byte(0xFF);
	cs_high(CS_PIN);
	
	return(adc_state);
}

/* This function will block operation until the ADC has finished it's conversion */
uint32_t LTC681x_pollAdc()
{
	uint32_t counter = 0;
	uint8_t finished = 0;
	uint8_t current_time = 0;
	uint8_t cmd[4];
	uint16_t cmd_pec;
	
	cmd[0] = 0x07;
	cmd[1] = 0x14;
	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	
	cs_low(CS_PIN);
	spi_write_array(4,cmd);
	while ((counter<200000)&&(finished == 0))
	{
		current_time = spi_read_byte(0xff);
		if (current_time>0)
		{
			finished = 1;
		}
		else
		{
			counter = counter + 10;
		}
	}
	cs_high(CS_PIN);
	
	return(counter);
}

/*
The command clears the cell voltage registers and initializes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.
*/
void LTC681x_clrcell()
{
	uint8_t cmd[2]= {0x07 , 0x11};
	cmd_68(cmd);
}

/*
The command clears the Auxiliary registers and initializes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.
*/
void LTC681x_clraux()
{
	uint8_t cmd[2]= {0x07 , 0x12};
	cmd_68(cmd);
}

/*
The command clears the Stat registers and initializes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.

*/
void LTC681x_clrstat()
{
	uint8_t cmd[2]= {0x07 , 0x13};
	cmd_68(cmd);
}

/* Starts the Mux Decoder diagnostic self test */
void LTC681x_diagn()
{
	uint8_t cmd[2] = {0x07 , 0x15};
	cmd_68(cmd);
}

/* Starts cell voltage self test conversion */
void LTC681x_cvst(uint8_t MD, //ADC Mode
				  uint8_t ST //Self Test
                 )
{
	uint8_t cmd[2];
	uint8_t md_bits;
	
	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x02;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + ((ST)<<5) +0x07;
	
	cmd_68(cmd);
}

/* Start an Auxiliary Register Self Test Conversion */
void LTC681x_axst(uint8_t MD, //ADC Mode
				  uint8_t ST //Self Test
				  )
{
	uint8_t cmd[2];
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + ((ST&0x03)<<5) +0x07;

	cmd_68(cmd);
}

/* Start a Status Register Self Test Conversion */
void LTC681x_statst(uint8_t MD, //ADC Mode
				    uint8_t ST //Self Test
					)
{
	uint8_t cmd[2];
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + ((ST&0x03)<<5) +0x0F;

	cmd_68(cmd);
}

/* Starts cell voltage overlap conversion */
void LTC681x_adol(uint8_t MD, //ADC Mode
                  uint8_t DCP //Discharge Permit
				 )
{
	uint8_t cmd[2];
	uint8_t md_bits;
	
	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x02;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + (DCP<<4) +0x01;
	
	cmd_68(cmd);
}

/* Start an GPIO Redundancy test */
void LTC681x_adaxd(uint8_t MD, //ADC Mode
				   uint8_t CHG //GPIO Channels to be measured
				   )
{
	uint8_t cmd[4];
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] = md_bits + CHG ;

	cmd_68(cmd);
}

/* Start a Status register redundancy test Conversion */
void LTC681x_adstatd(uint8_t MD, //ADC Mode
					 uint8_t CHST //Stat Channels to be measured
					 )
{
	uint8_t cmd[2];
	uint8_t md_bits;
	
	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] = md_bits + 0x08 + CHST ;
	
	cmd_68(cmd);
}

/* Runs the Digital Filter Self Test */
int16_t LTC681x_run_cell_adc_st(uint8_t adc_reg, // Type of register
								uint8_t total_ic, // Number of ICs in the daisy chain
								cell_asic *ic, // A two dimensional array that will store the data
								uint8_t md, // ADC Mode
								bool adcopt // ADCOPT bit in the configuration register
								)
{
	int16_t error = 0;
	uint16_t expected_result = 0;

	for (int self_test = 1; self_test<3; self_test++)
	{
		expected_result = LTC681x_st_lookup(md,self_test,adcopt);
		wakeup_idle(total_ic);
		
		switch (adc_reg)
		{
		  case CELL:
			  wakeup_idle(total_ic);
			  LTC681x_clrcell();
			  LTC681x_cvst(md,self_test);
			  LTC681x_pollAdc();
				 
			  wakeup_idle(total_ic);
			  error = LTC681x_rdcv(0, total_ic,ic);      
			  for (int cic = 0; cic < total_ic; cic++)
				{
				  for (int channel=0; channel< ic[cic].ic_reg.cell_channels; channel++)
				  {
								 
					if (ic[cic].cells.c_codes[channel] != expected_result)
					{
					  error = error+1;
					}
				  }
				}
			break;
		  case AUX:
			  error = 0;
			  wakeup_idle(total_ic);
			  LTC681x_clraux();
			  LTC681x_axst(md,self_test);
			  LTC681x_pollAdc();
			  
			  wakeup_idle(total_ic);
			  LTC681x_rdaux(0, total_ic,ic);     
			  for (int cic = 0; cic < total_ic; cic++)
				{
				  for (int channel=0; channel< ic[cic].ic_reg.aux_channels; channel++)
				  { 
					 
					if (ic[cic].aux.a_codes[channel] != expected_result)
					{
					  error = error+1;
					}
				  }
				}
			break;
		  case STAT:
			  wakeup_idle(total_ic);
			  LTC681x_clrstat();
			  LTC681x_statst(md,self_test);
			  LTC681x_pollAdc();
			  
			  wakeup_idle(total_ic);
			  error = LTC681x_rdstat(0,total_ic,ic);
			  for (int cic = 0; cic < total_ic; cic++)
				{
				  for (int channel=0; channel< ic[cic].ic_reg.stat_channels; channel++)
				  {
					if (ic[cic].stat.stat_codes[channel] != expected_result)
					{
					  error = error+1;
					}
				  }
				}
			break;

		  default:
			error = -1;
			break;
		}
	}
	
	return(error);
}

/* Runs the ADC overlap test for the IC */
uint16_t LTC681x_run_adc_overlap(uint8_t total_ic,  // Number of ICs in the daisy chain
								 cell_asic *ic  // A two dimensional array that will store the data
								 )
{
	uint16_t error = 0;
	int32_t measure_delta =0;
	int16_t failure_pos_limit = 20;
	int16_t failure_neg_limit = -20;
	int32_t a, b;
	
	wakeup_idle(total_ic);
	LTC681x_adol(MD_7KHZ_3KHZ,DCP_DISABLED);
	LTC681x_pollAdc();
	wakeup_idle(total_ic);
	error = LTC681x_rdcv(0, total_ic,ic);

	  for (int cic = 0; cic<total_ic; cic++)
	  {
		  measure_delta = (int32_t)ic[cic].cells.c_codes[6]-(int32_t)ic[cic].cells.c_codes[7];
		
		 if ((measure_delta>failure_pos_limit) || (measure_delta<failure_neg_limit))
		 {
		  error = error | (1<<(cic-1));
		 }
	  }
	  
    return(error);
}

/* Runs the redundancy self test */
int16_t LTC681x_run_adc_redundancy_st(uint8_t adc_mode, // ADC Mode
									  uint8_t adc_reg, // Type of register
									  uint8_t total_ic, // Number of ICs in the daisy chain
									  cell_asic *ic // A two dimensional array that will store the data
									  )
{
	int16_t error = 0;
	for (int self_test = 1; self_test<3; self_test++)
	{
		wakeup_idle(total_ic);
		switch (adc_reg)
		{
			case AUX:
			LTC681x_clraux();
			LTC681x_adaxd(adc_mode,AUX_CH_ALL);
			LTC681x_pollAdc();
		
			wakeup_idle(total_ic);
			error = LTC681x_rdaux(0, total_ic,ic);
			for (int cic = 0; cic < total_ic; cic++)
			{
				for (int channel=0; channel< ic[cic].ic_reg.aux_channels; channel++)
				{ 
					if (ic[cic].aux.a_codes[channel] >= 65280)
					{
						error = error+1;
					}
				}
			}
			break;
			case STAT:
			LTC681x_clrstat();
			LTC681x_adstatd(adc_mode,STAT_CH_ALL);
			LTC681x_pollAdc();
			wakeup_idle(total_ic);
			error = LTC681x_rdstat(0,total_ic,ic);
			for (int cic = 0; cic < total_ic; cic++)
			{
				for (int channel=0; channel< ic[cic].ic_reg.stat_channels; channel++)
				{
					if (ic[cic].stat.stat_codes[channel] >= 65280)
					{
						error = error+1;
					}
				}
			}
			break;
		
			default:
			error = -1;
			break;
		}
	}
	return(error);
}

/* Looks up the result pattern for digital filter self test */
uint16_t LTC681x_st_lookup(uint8_t MD, //ADC Mode
						   uint8_t ST, //Self Test
						   bool adcopt // ADCOPT bit in the configuration register
						  )
{
	uint16_t test_pattern = 0;

    if (MD == 1)
    { 
		if ( adcopt == false)
		{	
			if (ST == 1)
			{
				test_pattern = 0x9565;
			}
			else
			{
				test_pattern = 0x6A9A;
			}
		}
		else
		{	
			if (ST == 1)
			{
				test_pattern = 0x9553;
			}
			else
			{
				test_pattern = 0x6AAC;
			}	
		}
    }
    else
    {
		if (ST == 1)
		{
		   test_pattern = 0x9555;
		}
		else
		{
		   test_pattern = 0x6AAA;
		}
    }
    return(test_pattern);
}

/* Start an open wire Conversion */
void LTC681x_adow(uint8_t MD, //ADC Mode
				  uint8_t PUP,//Pull up/Pull down current
				  uint8_t CH, //Channels
				  uint8_t DCP//Discharge Permit
				 )
{
	uint8_t cmd[2];
	uint8_t md_bits;
	
	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x02;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + 0x28 + (PUP<<6) + CH+(DCP<<4);
	
	cmd_68(cmd);
}

/* Start GPIOs open wire ADC conversion */
void LTC681x_axow(uint8_t MD, //ADC Mode
				  uint8_t PUP //Pull up/Pull down current
				 )
{
	uint8_t cmd[2];
	uint8_t md_bits;
	
	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + 0x10+ (PUP<<6) ;//+ CH;
	
	cmd_68(cmd);
}

/* Runs the data sheet algorithm for open wire for single cell detection */
void LTC681x_run_openwire_single(uint8_t total_ic, // Number of ICs in the daisy chain
								cell_asic ic[] // A two dimensional array that will store the data
								)
{				  
	uint16_t OPENWIRE_THRESHOLD = 4000;
	const uint8_t  N_CHANNELS = ic[0].ic_reg.cell_channels;
	
	uint16_t pullUp[total_ic][N_CHANNELS];
	uint16_t pullDwn[total_ic][N_CHANNELS];
	int16_t openWire_delta[total_ic][N_CHANNELS];
	
	int8_t error;
	int8_t i;
	uint32_t conv_time=0;

	wakeup_sleep(total_ic);
	LTC681x_clrcell();
	
	// Pull Ups
	for (i = 0; i < 3; i++)
	{ 
	  wakeup_idle(total_ic);
	  LTC681x_adow(MD_26HZ_2KHZ,PULL_UP_CURRENT,CELL_CH_ALL,DCP_DISABLED);
	  conv_time =LTC681x_pollAdc();
	} 
	
	wakeup_idle(total_ic);
	error=LTC681x_rdcv(0, total_ic,ic);
	
	for (int cic=0; cic<total_ic; cic++)
	{
	    for (int cell=0; cell<N_CHANNELS; cell++)
		{
		  pullUp[cic][cell] = ic[cic].cells.c_codes[cell];
		}	
	}

	// Pull Downs
	for (i = 0; i < 3; i++)
	{  
	  wakeup_idle(total_ic);
	  LTC681x_adow(MD_26HZ_2KHZ,PULL_DOWN_CURRENT,CELL_CH_ALL,DCP_DISABLED);
	  conv_time =LTC681x_pollAdc();
	}
	
	wakeup_idle(total_ic);
	error=LTC681x_rdcv(0, total_ic,ic); 
	
	for (int cic=0; cic<total_ic; cic++)
	{		  
	    for (int cell=0; cell<N_CHANNELS; cell++)
		{
		   pullDwn[cic][cell] = ic[cic].cells.c_codes[cell];
		}
	}

	for (int cic=0; cic<total_ic; cic++)
	{
	  ic[cic].system_open_wire = 0xFFFF;
	  
		for (int cell=0; cell<N_CHANNELS; cell++)
		{
			if (pullDwn[cic][cell] < pullUp[cic][cell])                   
			{
				openWire_delta[cic][cell] = (pullUp[cic][cell] - pullDwn[cic][cell]);
			}
			else
			{
				openWire_delta[cic][cell] = 0;                                             
			}  
				
			if (openWire_delta[cic][cell]>OPENWIRE_THRESHOLD)
			{
				ic[cic].system_open_wire = cell+1;
			}
		}
		
		if (pullUp[cic][0] == 0)
		{
		  ic[cic].system_open_wire = 0;
		}
		
		if (pullUp[cic][(N_CHANNELS-1)] == 0)//checking the Pull up value of the top measured channel
		{
		  ic[cic].system_open_wire = N_CHANNELS;
		}	
	}
}

/* Runs the data sheet algorithm for open wire for multiple cell and two consecutive cells detection */
 void LTC681x_run_openwire_multi(uint8_t total_ic, // Number of ICs in the daisy chain
						  cell_asic ic[] // A two dimensional array that will store the data
						  )
{              
	uint16_t OPENWIRE_THRESHOLD = 4000;
	const uint8_t  N_CHANNELS = ic[0].ic_reg.cell_channels;

	uint16_t pullUp[total_ic][N_CHANNELS];
	uint16_t pullDwn[total_ic][N_CHANNELS];
	uint16_t openWire_delta[total_ic][N_CHANNELS];

	int8_t error;
	int8_t opencells[N_CHANNELS];
	int8_t n=0;
	int8_t i,j,k;
	uint32_t conv_time=0;

	wakeup_sleep(total_ic);
	LTC681x_clrcell();

	// Pull Ups
	for (i = 0; i < 5; i++)
	{ 
		wakeup_idle(total_ic);
		LTC681x_adow(MD_26HZ_2KHZ,PULL_UP_CURRENT,CELL_CH_ALL,DCP_DISABLED);
		conv_time =LTC681x_pollAdc();
	} 

	wakeup_idle(total_ic);
	error = LTC681x_rdcv(0, total_ic,ic);

	for (int cic=0; cic<total_ic; cic++)
	{
	    for (int cell=0; cell<N_CHANNELS; cell++)
		{
		  pullUp[cic][cell] = ic[cic].cells.c_codes[cell];
		}
	}

	// Pull Downs
	for (i = 0; i < 5; i++)
	{  
	  wakeup_idle(total_ic);
	  LTC681x_adow(MD_26HZ_2KHZ,PULL_DOWN_CURRENT,CELL_CH_ALL,DCP_DISABLED);
	  conv_time =   LTC681x_pollAdc();
	}

	wakeup_idle(total_ic);
	error = LTC681x_rdcv(0, total_ic,ic); 

	for (int cic=0; cic<total_ic; cic++)
	{
		for (int cell=0; cell<N_CHANNELS; cell++)
		{
		   pullDwn[cic][cell] = ic[cic].cells.c_codes[cell];
		}
	}

	for (int cic=0; cic<total_ic; cic++)
	{			  
		for (int cell=0; cell<N_CHANNELS; cell++)
		{
			if (pullDwn[cic][cell] < pullUp[cic][cell])                   
				{
					openWire_delta[cic][cell] = (pullUp[cic][cell] - pullDwn[cic][cell]);
				}
				else
				{
					openWire_delta[cic][cell] = 0;                                             
				}
		}  
	}

	for (int cic=0; cic<total_ic; cic++)
	{ 
		n=0;
						
		Serial.print("IC:");
		Serial.println(cic+1, DEC);
		
		for (int cell=0; cell<N_CHANNELS; cell++)
		{  
		 
		  if (openWire_delta[cic][cell]>OPENWIRE_THRESHOLD)
			{
				opencells[n] = cell+1;
				n++;
				for (int j = cell; j < N_CHANNELS-3 ; j++)                       
				{
					if (pullUp[cic][j + 2] == 0)
					{
					opencells[n] = j+2;
					n++;
					}
				}
				if((cell==N_CHANNELS-4) && (pullDwn[cic][N_CHANNELS-3] == 0))
				{
					  opencells[n] = N_CHANNELS-2;
					  n++;
				}
			}
		}
		if (pullDwn[cic][0] == 0)
		{
		  opencells[n] = 0;
		  Serial.println("Cell 0 is Open and multiple open wires maybe possible.");
		  n++;
		}
					
		if (pullDwn[cic][N_CHANNELS-1] == 0)
		{
		  opencells[n] = N_CHANNELS;
		  n++;
		}
					
		if (pullDwn[cic][N_CHANNELS-2] == 0)
		{  
		  opencells[n] = N_CHANNELS-1;
		  n++;
		}
		
	//Removing repetitive elements
		for(i=0;i<n;i++)
		{
			for(j=i+1;j<n;)
			{
				if(opencells[i]==opencells[j])
				{
					for(k=j;k<n;k++)
						opencells[k]=opencells[k+1];
						
					n--;
				}
				else
					j++;
			}
		}
					
	// Sorting open cell array
		for(int i=0; i<n; i++)
		{
			for(int j=0; j<n-1; j++)
			{
				if( opencells[j] > opencells[j+1] )
				{
					k = opencells[j];
					opencells[j] = opencells[j+1];
					opencells[j+1] = k;
				}
			}
		}
					
	//Checking the value of n				
		Serial.println("Number of Open wires:");
		Serial.println(n);
		   
	//Printing open cell array
		Serial.println("OPEN CELLS:");
		if(n==0)
		{
			Serial.println("No Open wires");
		}
		else
		{				
			for(i=0;i<n;i++)
			{
					Serial.println(opencells[i]);	
			}
		}
	}
	Serial.println("\n");
}

/* Runs open wire for GPIOs */
void LTC681x_run_gpio_openwire(uint8_t total_ic, // Number of ICs in the daisy chain
								cell_asic ic[] // A two dimensional array that will store the data
								)
 {				  
	uint16_t OPENWIRE_THRESHOLD = 150;
	const uint8_t  N_CHANNELS = ic[0].ic_reg.aux_channels +1;
	
	uint16_t aux_val[total_ic][N_CHANNELS];
	uint16_t pDwn[total_ic][N_CHANNELS];
	uint16_t ow_delta[total_ic][N_CHANNELS];
	
	int8_t error;
	int8_t i;
	uint32_t conv_time=0;

	wakeup_sleep(total_ic); 
	LTC681x_clraux();
	 
	for (i = 0; i < 3; i++)
	{ 
	   wakeup_idle(total_ic);
	   LTC681x_adax(MD_7KHZ_3KHZ, AUX_CH_ALL);
	   conv_time= LTC681x_pollAdc();
	}
	
	wakeup_idle(total_ic);
	error = LTC681x_rdaux(0, total_ic,ic);
	
	for (int cic=0; cic<total_ic; cic++)
	{
	    for (int channel=0; channel<N_CHANNELS; channel++)
		{
			aux_val[cic][channel]=ic[cic].aux.a_codes[channel];
		}
	}	
	LTC681x_clraux();
	
	// pull downs
	for (i = 0; i < 3; i++)
	{ 
	   wakeup_idle(total_ic);
	   LTC681x_axow(MD_7KHZ_3KHZ,PULL_DOWN_CURRENT);
	   conv_time =LTC681x_pollAdc();
	} 
	
	wakeup_idle(total_ic);
	error = LTC681x_rdaux(0, total_ic,ic);
	
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
			
			if(channel<5)
			{
				if (ow_delta[cic][channel] > OPENWIRE_THRESHOLD) 
				{
					ic[cic].system_open_wire= channel+1;
					
				}  
			}
			else if(channel>5)
			{
				if (ow_delta[cic][channel] > OPENWIRE_THRESHOLD) 
				{
					ic[cic].system_open_wire= channel;
					
				}  
			}	
		}
	}	  
}

/* Clears all of the DCC bits in the configuration registers */
void LTC681x_clear_discharge(uint8_t total_ic, // Number of ICs in the daisy chain
							 cell_asic *ic // A two dimensional array that will store the data
							 )
{
	for (int i=0; i<total_ic; i++)
	{
	   ic[i].config.tx_data[4] = 0;
	   ic[i].config.tx_data[5] =ic[i].config.tx_data[5]&(0xF0);
	   ic[i].configb.tx_data[0]=ic[i].configb.tx_data[0]&(0x0F); 
	   ic[i].configb.tx_data[1]=ic[i].configb.tx_data[1]&(0xF0);
	}
}

/* Writes the pwm register */
void LTC681x_wrpwm(uint8_t total_ic, // Number of ICs in the daisy chain
                   uint8_t pwmReg, // The PWM Register to be written A or B
                   cell_asic ic[] // A two dimensional array that stores the data to be written
                  )
{
	uint8_t cmd[2];
	uint8_t write_buffer[256];
	uint8_t write_count = 0;
	uint8_t c_ic = 0;
	if (pwmReg == 0)
	{
	cmd[0] = 0x00;
	cmd[1] = 0x20;
	}
	else
	{
	cmd[0] = 0x00;
	cmd[1] = 0x1C;
	}
	
	for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
	{
		if (ic->isospi_reverse == false)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = total_ic - current_ic - 1;
		}
		
		for (uint8_t data = 0; data<6; data++)
		{
			write_buffer[write_count] = ic[c_ic].pwm.tx_data[data];
			write_count++;
		}
	}
	write_68(total_ic, cmd, write_buffer);
}


/* Reads pwm registers of a LTC681x daisy chain */
int8_t LTC681x_rdpwm(uint8_t total_ic, //Number of ICs in the system
                     uint8_t pwmReg, // The PWM Register to be written A or B
                     cell_asic ic[] // A two dimensional array that will store the data
                    )
{
	const uint8_t BYTES_IN_REG = 8;
	uint8_t cmd[4];
	uint8_t read_buffer[256];
	int8_t pec_error = 0;
	uint16_t data_pec;
	uint16_t calc_pec;
	uint8_t c_ic = 0;
	
	if (pwmReg == 0)
	{
		cmd[0] = 0x00;
		cmd[1] = 0x22;
	}
	else
	{
		cmd[0] = 0x00;
		cmd[1] = 0x1E;
	}
	
	pec_error = read_68(total_ic, cmd, read_buffer);
	for (uint8_t current_ic =0; current_ic<total_ic; current_ic++)
	{
		if (ic->isospi_reverse == false)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = total_ic - current_ic - 1;
		}
		
		for (int byte=0; byte<8; byte++)
		{
			ic[c_ic].pwm.rx_data[byte] = read_buffer[byte+(8*current_ic)];
		}
		
		calc_pec = pec15_calc(6,&read_buffer[8*current_ic]);
		data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);
		if (calc_pec != data_pec )
		{
			ic[c_ic].pwm.rx_pec_match = 1;
		}
		else ic[c_ic].pwm.rx_pec_match = 0;
	}
	return(pec_error);
}

/*  Write the LTC681x Sctrl register */
void LTC681x_wrsctrl(uint8_t total_ic, // Number of ICs in the daisy chain
                     uint8_t sctrl_reg, // The Sctrl Register to be written A or B
                     cell_asic *ic  // A two dimensional array that stores the data to be written
                    )
{
	uint8_t cmd[2];
    uint8_t write_buffer[256];
    uint8_t write_count = 0;
    uint8_t c_ic = 0;
    if (sctrl_reg == 0)
    {
      cmd[0] = 0x00;
      cmd[1] = 0x14;
    }
    else
    {
      cmd[0] = 0x00;
      cmd[1] = 0x1C;
    }
    
    for(uint8_t current_ic = 0; current_ic<total_ic;current_ic++)
    {
        if(ic->isospi_reverse == false){c_ic = current_ic;}
        else{c_ic = total_ic - current_ic - 1;}
		

        for(uint8_t data = 0; data<6;data++)
        {
            write_buffer[write_count] = ic[c_ic].sctrl.tx_data[data];
            write_count++;
        }
    }
    write_68(total_ic, cmd, write_buffer);
}					
					
/*  Reads sctrl registers of a LTC681x daisy chain */    
int8_t LTC681x_rdsctrl(uint8_t total_ic, // Number of ICs in the daisy chain
                       uint8_t sctrl_reg, // The Sctrl Register to be written A or B
                       cell_asic *ic // A two dimensional array that the function stores the read data
                      )	
{
    uint8_t cmd[4];
    uint8_t read_buffer[256];
    int8_t pec_error = 0;
    uint16_t data_pec;
    uint16_t calc_pec;
    uint8_t c_ic = 0;
    
    if (sctrl_reg == 0)
    {
      cmd[0] = 0x00;
      cmd[1] = 0x16;
	  }
    else
    {
      cmd[0] = 0x00;
      cmd[1] = 0x1E;
	}
    
    pec_error = read_68(total_ic, cmd, read_buffer);

    for(uint8_t current_ic =0; current_ic<total_ic; current_ic++)
    {	
        if(ic->isospi_reverse == false){c_ic = current_ic;}
        else{c_ic = total_ic - current_ic - 1;}
	
		
        for(int byte=0; byte<8;byte++)
        {
            ic[c_ic].sctrl.rx_data[byte] = read_buffer[byte+(8*current_ic)]; 	
        }
		
        calc_pec = pec15_calc(6,&read_buffer[8*current_ic]);
        data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);
        if(calc_pec != data_pec )
        {
            ic[c_ic].sctrl.rx_pec_match = 1;
        }
        else ic[c_ic].sctrl.rx_pec_match = 0;
		
    }
    return(pec_error);
}

/* 
Start Sctrl data communication       
This command will start the sctrl pulse communication over the spins
*/
void LTC681x_stsctrl()
{
	uint8_t cmd[4];
    uint16_t cmd_pec;
    
    cmd[0] = 0x00;
    cmd[1] = 0x19;
    cmd_pec = pec15_calc(2, cmd);
    cmd[2] = (uint8_t)(cmd_pec >> 8);
    cmd[3] = (uint8_t)(cmd_pec);
    
    cs_low(CS_PIN);
    spi_write_array(4,cmd);          
    cs_high(CS_PIN);
}

/*
The command clears the Sctrl registers and initializes
all values to 0. The register will read back hexadecimal 0x00
after the command is sent.
*/
void LTC681x_clrsctrl()
{
	uint8_t cmd[2]= {0x00 , 0x18};
	cmd_68(cmd);
}

/* Writes the comm register */
void LTC681x_wrcomm(uint8_t total_ic, //The number of ICs being written to
                    cell_asic ic[] // A two dimensional array that stores the data to be written
                   )
{
	uint8_t cmd[2]= {0x07 , 0x21};
	uint8_t write_buffer[256];
	uint8_t write_count = 0;
	uint8_t c_ic = 0;
	for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
	{
		if (ic->isospi_reverse == false)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = total_ic - current_ic - 1;
		}
	
		for (uint8_t data = 0; data<6; data++)
		{
			write_buffer[write_count] = ic[c_ic].com.tx_data[data];
			write_count++;
		}
	}
	write_68(total_ic, cmd, write_buffer);
}

/* Reads COMM registers of a LTC681x daisy chain */
int8_t LTC681x_rdcomm(uint8_t total_ic, //Number of ICs in the system
                      cell_asic ic[] //A two dimensional array that stores the read data
                     )
{
	uint8_t cmd[2]= {0x07 , 0x22};
	uint8_t read_buffer[256];
	int8_t pec_error = 0;
	uint16_t data_pec;
	uint16_t calc_pec;
	uint8_t c_ic=0;
	
	pec_error = read_68(total_ic, cmd, read_buffer);
	
	for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
	{
		if (ic->isospi_reverse == false)
		{
			c_ic = current_ic;
		}
		else
		{
			c_ic = total_ic - current_ic - 1;
		}
	
		for (int byte=0; byte<8; byte++)
		{
			ic[c_ic].com.rx_data[byte] = read_buffer[byte+(8*current_ic)];
		}
		
		calc_pec = pec15_calc(6,&read_buffer[8*current_ic]);
		data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);
		if (calc_pec != data_pec )
		{
			ic[c_ic].com.rx_pec_match = 1;
		}
		else ic[c_ic].com.rx_pec_match = 0;
	}
	
    return(pec_error);
}

/* Shifts data in COMM register out over LTC681x SPI/I2C port */
void LTC681x_stcomm(uint8_t len) //Length of data to be transmitted 
{
	uint8_t cmd[4];
	uint16_t cmd_pec;

	cmd[0] = 0x07;
	cmd[1] = 0x23;
	cmd_pec = pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	cs_low(CS_PIN);
	spi_write_array(4,cmd);
	for (int i = 0; i<len*3; i++)
	{
	  spi_read_byte(0xFF);
	}
	cs_high(CS_PIN);
}

/* Helper function that increments PEC counters */
void LTC681x_check_pec(uint8_t total_ic, //Number of ICs in the system
					   uint8_t reg, //Type of Register
					   cell_asic *ic //A two dimensional array that stores the data
					   )
{
	switch (reg)
	{
		case CFGR:
		  for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
		  {
			ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].config.rx_pec_match;
			ic[current_ic].crc_count.cfgr_pec = ic[current_ic].crc_count.cfgr_pec + ic[current_ic].config.rx_pec_match;
		  }
		break;

		case CFGRB:
		  for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
		  {
			ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].configb.rx_pec_match;
			ic[current_ic].crc_count.cfgr_pec = ic[current_ic].crc_count.cfgr_pec + ic[current_ic].configb.rx_pec_match;
		  }
		break;
		case CELL:
		  for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
		  {
			for (int i=0; i<ic[0].ic_reg.num_cv_reg; i++)
			{
			  ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].cells.pec_match[i];
			  ic[current_ic].crc_count.cell_pec[i] = ic[current_ic].crc_count.cell_pec[i] + ic[current_ic].cells.pec_match[i];
			}
		  }
		break;
		case AUX:
		  for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
		  {
			for (int i=0; i<ic[0].ic_reg.num_gpio_reg; i++)
			{
			  ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + (ic[current_ic].aux.pec_match[i]);
			  ic[current_ic].crc_count.aux_pec[i] = ic[current_ic].crc_count.aux_pec[i] + (ic[current_ic].aux.pec_match[i]);
			}
		  }

		break;
		case STAT:
		  for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
		  {

			for (int i=0; i<ic[0].ic_reg.num_stat_reg-1; i++)
			{
			  ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].stat.pec_match[i];
			  ic[current_ic].crc_count.stat_pec[i] = ic[current_ic].crc_count.stat_pec[i] + ic[current_ic].stat.pec_match[i];
			}
		  }
		break;
		default:
		break;
	}
}

/* Helper Function to reset PEC counters */
void LTC681x_reset_crc_count(uint8_t total_ic, //Number of ICs in the system
							 cell_asic *ic //A two dimensional array that stores the data
							 )
{
	for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
	{
		ic[current_ic].crc_count.pec_count = 0;
		ic[current_ic].crc_count.cfgr_pec = 0;
		for (int i=0; i<6; i++)
		{
			ic[current_ic].crc_count.cell_pec[i]=0;
		
		}
		for (int i=0; i<4; i++)
		{
			ic[current_ic].crc_count.aux_pec[i]=0;
		}
		for (int i=0; i<2; i++)
		{
			ic[current_ic].crc_count.stat_pec[i]=0;
		}
	}
}

/* Helper function to initialize CFG variables */
void LTC681x_init_cfg(uint8_t total_ic, //Number of ICs in the system
					  cell_asic *ic //A two dimensional array that stores the data
					  )
{
	for (uint8_t current_ic = 0; current_ic<total_ic;current_ic++)  
	{
		for (int j =0; j<6; j++)
		{
		  ic[current_ic].config.tx_data[j] = 0;
		}
	}
}

/* Helper function to set CFGR variable */
void LTC681x_set_cfgr(uint8_t nIC, // Current IC
					 cell_asic *ic, // A two dimensional array that stores the data
					 bool refon, // The REFON bit
					 bool adcopt, // The ADCOPT bit
					 bool gpio[5], // The GPIO bits
					 bool dcc[12], // The DCC bits
					 bool dcto[4], // The Dcto bits
					 uint16_t uv, // The UV value
					 uint16_t  ov // The OV value
					 )
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
void LTC681x_set_cfgr_refon(uint8_t nIC, cell_asic *ic, bool refon)
{
	if (refon) ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0]|0x04;
	else ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0]&0xFB;
}

/* Helper function to set the ADCOPT bit */
void LTC681x_set_cfgr_adcopt(uint8_t nIC, cell_asic *ic, bool adcopt)
{
	if (adcopt) ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0]|0x01;
	else ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0]&0xFE;
}

/* Helper function to set GPIO bits */
void LTC681x_set_cfgr_gpio(uint8_t nIC, cell_asic *ic,bool gpio[5])
{
	for (int i =0; i<5; i++)
	{
		if (gpio[i])ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0]|(0x01<<(i+3));
		else ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0]&(~(0x01<<(i+3)));
	}
}

/* Helper function to control discharge */
void LTC681x_set_cfgr_dis(uint8_t nIC, cell_asic *ic,bool dcc[12])
{
	for (int i =0; i<8; i++)
	{
		if (dcc[i])ic[nIC].config.tx_data[4] = ic[nIC].config.tx_data[4]|(0x01<<i);
		else ic[nIC].config.tx_data[4] = ic[nIC].config.tx_data[4]& (~(0x01<<i));
	}
	for (int i =0; i<4; i++)
	{
		if (dcc[i+8])ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]|(0x01<<i);
		else ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]&(~(0x01<<i));
	}
}

/* Helper function to control discharge time value */
void LTC681x_set_cfgr_dcto(uint8_t nIC, cell_asic *ic,bool dcto[4])
{  
	for(int i =0;i<4;i++)
	{
		if(dcto[i])ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]|(0x01<<(i+4));
		else ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]&(~(0x01<<(i+4)));
	} 
}

/* Helper Function to set UV value in CFG register */
void LTC681x_set_cfgr_uv(uint8_t nIC, cell_asic *ic,uint16_t uv)
{
	uint16_t tmp = (uv/16)-1;
	ic[nIC].config.tx_data[1] = 0x00FF & tmp;
	ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2]&0xF0;
	ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2]|((0x0F00 & tmp)>>8);
}

/* Helper function to set OV value in CFG register */
void LTC681x_set_cfgr_ov(uint8_t nIC, cell_asic *ic,uint16_t ov)
{
	uint16_t tmp = (ov/16);
	ic[nIC].config.tx_data[3] = 0x00FF & (tmp>>4);
	ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2]&0x0F;
	ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2]|((0x000F & tmp)<<4);
}
