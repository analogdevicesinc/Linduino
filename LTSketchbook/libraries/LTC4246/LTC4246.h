////////////// LTC4246  registers //////////////
#ifndef LTC4246
#define LTC4246

#define LTC4246_NO_OP			0x00
#define LTC4246_ENABLE			0x01
#define LTC4246_FAULT_PIN 		0x02
#define LTC4246_FAULT_REG		0x03
#define LTC4246_CH0_THRESHOLD		0x04
#define LTC4246_CH1_THRESHOLD  		0x05
#define LTC4246_CH2_THRESHOLD  		0x06
#define LTC4246_CH3_THRESHOLD		0x07
#define LTC4246_CH4_THRESHOLD		0x08
#define LTC4246_CH5_THRESHOLD		0x09
#define LTC4246_CH6_THRESHOLD		0x0A
#define LTC4246_CH7_THRESHOLD		0x0B
#define LTC4246_CH0_AFFINITY		0x0C
#define LTC4246_CH1_AFFINITY		0x0D
#define LTC4246_CH2_AFFINITY		0x0E
#define LTC4246_CH3_AFFINITY		0x0F
#define LTC4246_CH4_AFFINITY		0x10
#define LTC4246_CH5_AFFINITY		0x11
#define LTC4246_CH6_AFFINITY		0x12
#define LTC4246_CH7_AFFINITY		0x13
#define LTC4246_RESET			0x14
#define LTC4246_ID_VER			0x15

#endif
////////////// LTC4246 values //////////////

// CB THRESHOLD register is 5 bits (bits 4-0 of the threshold reg)
// CB THRESHOLD LSB size is 0x01 = 50mA
// Max CB THRESHOLD current is 0x1E = 1.5A
// CB THRESHOLD = 0x1F removes the CB limit (CB does not trip)

// CB OCP_TIMER register is 2 bits (bits 6-7)
// CB OCP_TIMER vales are 0x0=10us; 0x1=20us; 0x2=50us; 0x3=100us
// 2x OCP gives 1us response when enabled (bit 5)

// value written to perform a soft reset
//  reset all digital
# define LTC4246_RESET_DIGITAL		0x0D
// reset the FAULTb and enter "quick reset" mode
// the 0x05 value is "sticky"
# define LTC4246_RESET_FAULT		0x50

// NOTE: AFFINITY is mutual. Setting 0x0C=0x80 (CH0 to CH7)
//   also automagically sets 0x13=0x01 (CH7 to CH0). The bits are linked.
//   This means that if CH7 faults it will disable CH0, and a CH0 fault will disable CH7.
// AFFINITY with the FINb pin is the only possible "non-mutual" affinity setting.
