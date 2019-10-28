/*  ! @file
    @ingroup LTM9057
    Header for LTM9057: Anyside™ I2C Power, Charge and Energy μModule Isolator
*/
 
#ifndef LTM9057_H
#define LTM9057_H

#define LTM_9057_CTRLA_REG                  0x00
#define LTM_9057_CTRLB_REG                  0x01
#define LTM_9057_ALERT1_REG                 0x02
#define LTM_9057_STATUS1_REG                0x03
#define LTM_9057_FAULT1_REG                 0x04
#define LTM_9057_POWER_MSB2_REG             0x05
#define LTM_9057_POWER_MSB1_REG             0x06
#define LTM_9057_POWER_LSB_REG              0x07
#define LTM_9057_MAXPWR_MSB2_REG            0x08
#define LTM_9057_MAXPWR_MSB1_REG            0x09
#define LTM_9057_MAXPWR_LSB_REG             0x0A
#define LTM_9057_MINPWR_MSB2_REG            0x0B
#define LTM_9057_MINPWR_MSB1_REG            0x0C
#define LTM_9057_MINPWR_LSB_REG             0x0D
#define LTM_9057_MAXPWRTHLD_MSB2_REG        0x0E
#define LTM_9057_MAXPWRTHLD_MSB1_REG        0x0F
#define LTM_9057_MAXPWRTHLD_LSB_REG         0x10
#define LTM_9057_MAXPWRTHLD_MSB2_REG        0X11
#define LTM_9057_MAXPWRTHLD_MSB1_REG        0x12
#define LTM_9057_MAXPWRTHLD_LSB_REG         0x13
#define LTM_9057_dSENSE_MSB_REG             0x14
#define LTM_9057_dSENSE_LSB_REG             0x15
#define LTM_9057_MAX_dSENSE_MSB_REG         0x16
#define LTM_9057_MAX_dSENSE_LSB_REG         0x17
#define LTM_9057_MIN_dSENSE_MSB_REG         0x18
#define LTM_9057_MIN_dSENSE_MSB_REG         0x19
#define LTM_9057_MAX_dSENSE_THLD_MSB_REG    0x1A
#define LTM_9057_MAX_dSENSE_THLD_LSB_REG    0x1B
#define LTM_9057_MIN_dSENSE_THLD_MSB_REG    0x1C
#define LTM_9057_MIN_dSENSE_THLD_LSB_REG    0x1D
#define LTM_9057_VIN_MSB_REG                0x1E
#define LTM_9057_VIN_LSB_REG                0x1F
#define LTM_9057_MAX_VIN_MSB_REG            0x20
#define LTM_9057_MAX_VIN_LSB_REG            0X21
#define LTM_9057_MIN_VIN_MSB_REG            0x22
#define LTM_9057_MIN_VIN_LSB_REG            0x23
#define LTM_9057_MAX_VIN_THLD_MSB_REG       0x24
#define LTM_9057_MAX_VIN_THLD_LSB_REG       0x25
#define LTM_9057_MIN_VIN_THLD_MSB_REG       0x26
#define LTM_9057_MIN_VIN_THLD_LSB_REG       0x27
#define LTM_9057_ADIN_MSB_REG               0x28
#define LTM_9057_ADIN_LSB_REG               0x29
#define LTM_9057_MAX_ADIN_MSB_REG           0x2A
#define LTM_9057_MAX_ADIN_LSB_REG           0x2B
#define LTM_9057_MIN_ADIN_MSB_REG           0x2C
#define LTM_9057_MIN_ADIN_LSB_REG           0x2D
#define LTM_9057_MAX_ADIN_THLD_MSB_REG      0x2E
#define LTM_9057_MAX_ADIN_THLD_LSB_REG      0x2F
#define LTM_9057_MIN_ADIN_THLD_MSB_REG      0x30
#define LTM_9057_MIN_ADIN_THLD_LSB_REG      0X31
#define LTM_9057_ALERT2_REG                 0x32
#define LTM_9057_GPIO_CFG_REG               0x33
#define LTM_9057_TIME_CTR_MSB3_REG          0x34
#define LTM_9057_TIME_CTR_MSB2_REG          0x35
#define LTM_9057_TIME_CTR_MSB1_REG          0x36
#define LTM_9057_TIME_CTR_LSB_REG           0x37
#define LTM_9057_CHARGE_MSB3_REG            0x38
#define LTM_9057_CHARGE_MSB2_REG            0x39
#define LTM_9057_CHARGE_MSB1_REG            0x3A
#define LTM_9057_CHARGE_LSB_REG             0x3B
#define LTM_9057_ENERGY_MSB3_REG            0x3C
#define LTM_9057_ENERGY_MSB2_REG            0x3D
#define LTM_9057_ENERGY_MSB1_REG            0x3E
#define LTM_9057_ENERGY_LSB_REG             0x3F
#define LTM_9057_STATUS2_REG                0x40
#define LTM_9057_FAULT2_REG                 0X41
#define LTM_9057_GPIO3_CTRL_REG             0x42
#define LTM_9057_CLOCK_DIV_REG              0x43


#define LTM_9057_STATUS1_POWOV              0x07
#define LTM_9057_STATUS1_POWUV              0x06
#define LTM_9057_STATUS1_SNSOV              0x05
#define LTM_9057_STATUS1_SNSUV              0x04
#define LTM_9057_STATUS1_VINOV              0x03
#define LTM_9057_STATUS1_VINUV              0x02
#define LTM_9057_STATUS1_ADINOV             0x01
#define LTM_9057_STATUS1_ADINUV             0x00

#define LTM_9057_STATUS2_RSVD             0x07
#define LTM_9057_STATUS2_GPIO1            0x06
#define LTM_9057_STATUS2_GPIO2            0x05
#define LTM_9057_STATUS2_GPIO3            0x04
#define LTM_9057_STATUS2_ADC_BSY          0x03
#define LTM_9057_STATUS2_ENGREG_OFLO      0x02
#define LTM_9057_STATUS2_CHGREG_OFLO      0x01
#define LTM_9057_STATUS2_TCTR_OFLO        0x00

#define LTM_9057_FAULT1_ADIN_UV           0x00
#define LTM_9057_FAULT1_ADIN_OV           0x01
#define LTM_9057_FAULT1_VIN_UV            0x02
#define LTM_9057_FAULT1_VIN_OV            0x03
#define LTM_9057_FAULT1_ISNS_UV           0x04
#define LTM_9057_FAULT1_ISNS_OV           0x05
#define LTM_9057_FAULT1_PWR_UV            0x06
#define LTM_9057_FAULT1_PWR_OV            0x07

#define LTM_9057_FAULT2_TIME_CTR_REG_OFLO     0x00
#define LTM_9057_FAULT2_CHG_REG_OFLO          0x01
#define LTM_9057_FAULT2_ENG_REG_OFLO          0x02
#define LTM_9057_FAULT2_STK_BUS_TMO_WKUP      0x03
#define LTM_9057_FAULT2_GPIO3_IN              0x04
#define LTM_9057_FAULT2_GPIO2_IN              0x05
#define LTM_9057_FAULT2_GPIO1_IN              0x06
#define LTM_9057_FAULT2_RESERVED              0x07

#define LTM_9057_ALERT1_MIN_ADIN          0X00    
#define LTM_9057_ALERT1_MAX_ADIN          0X01    
#define LTM_9057_ALERT1_MIN_VIN           0X02    
#define LTM_9057_ALERT1_MAX_VIN           0X03    
#define LTM_9057_ALERT1_MIN_ISNS          0X04    
#define LTM_9057_ALERT1_MAX_ISNS          0X05
#define LTM_9057_ALERT1_MIN_PWR           0X06    
#define LTM_9057_ALERT1_MAX_PWR           0X07    

#define LTM_9057_ALERT2_TIME_CTR_OFLO       0X00    
#define LTM_9057_ALERT2_CHARGE_OFLO         0X01    
#define LTM_9057_ALERT2_ENERGY_OFLO         0X02    
#define LTM_9057_ALERT2_STK_BUS_TMO_WKUP    0X03    
#define LTM_9057_ALERT2_RESERVED            0X04    
#define LTM_9057_ALERT2_GPIO2_IN            0X05
#define LTM_9057_ALERT2_GPIO1_INR           0X06    
#define LTM_9057_ALERT2_ADC_CNV_DONE        0X07    

#define LTM_9057_CTRLA_CHANNEL_CONFIG     0X00    
#define LTM_9057_CTRLA_VOLT_SELECT        0X03
#define LTM_9057_CTRLA_OFFSET_CAL         0X05    
#define LTM_9057_CTRLA_ADIN_CONFIG        0X07    

#define LTM_9057_CTRLB_AUTO_RESET         0X00    
#define LTM_9057_CTRLB_ENABLE_ACCUM       0X02
#define LTM_9057_CTRLB_STUCK_BUS          0X04    
#define LTM_9057_CTRLB_CLEAR_ON_READ      0X05    
#define LTM_9057_CTRLB_SHUTDOWN           0X06    
#define LTM_9057_CTRLB_ALERT_CLEAR_EN     0X07

#define  LTM_9057_dSENSE_mV_PER_TICK 0.025   // LSB value of dISENSE measurement
#define  LTM_9057_VIN_V_PER_TICK 0.025       // LSB value of VIN measurement
#define  LTM_9057_ADIN_V_PER_TICK 0.0005     // LSB value of ADIN measurement

extern float sense_resistor;
extern float adin_gain;

extern uint8_t i2c_address;       //7-bit version of LTM9057 I2C device addr determined by ADR0 & ADR1 (divide datasheet address by 2). ADR0/1 low: 0x20 -> 0x10
extern uint8_t reg_read_list[68];
extern uint8_t reg_write_list[66];

int8_t LTM9057_register_read(uint8_t i2c_address, uint8_t register_address, uint8_t *register_data);
int8_t LTM9057_adc_read(uint8_t i2c_address, uint8_t base_address, float *register_data);
int8_t LTM9057_register_write(uint8_t i2c_address, uint8_t register_address, uint8_t register_data);
int8_t LTM9057_bit_set(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number);
int8_t LTM9057_bit_clear(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number);
int8_t LTM9057_bit_read(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number, uint8_t *register_data);
int8_t LTM9057_alert1_read(uint8_t *register_data);
int8_t LTM9057_alert2_read(uint8_t *register_data);
int8_t LTM9057_print_all_registers(uint8_t i2c_address);
int8_t LTM9057_continuous_read_all_registers(uint8_t i2c_address);
int8_t menu_configure_CTRLA(uint8_t i2c_address);
int8_t menu_configure_CTRLB(uint8_t i2c_address);
int8_t LTM9057_prog_VOLTAGE_SELECTION(uint8_t i2c_address);
int8_t LTM9057_prog_ADC_CHAN_CONFIG(uint8_t i2c_address);
int8_t LTM_9057_set2bits(uint8_t i2c_address, uint8_t user_select, uint8_t register_address, uint8_t bit_number);
int8_t LTM_9057_set3bits(uint8_t i2c_address, uint8_t user_select, uint8_t register_address, uint8_t bit_number);
boolean valid_register(uint8_t user_register, uint8_t register_array[], uint8_t array_length);

#endif
