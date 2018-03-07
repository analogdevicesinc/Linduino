/*!
LTC4155: Dual-Input Power Manager / 3.5A Li-Ion Battery Charger with I²C Control and USB OTG

@verbatim
The LTC®4155 is a 15 watt I²C controlled power manager with PowerPath™ instant-
on operation, high efficiency switching battery charging and USB compatibility.
The LTC4155 seamlessly manages power distribution from two 5V sources, such as a
USB port and a wall adapter, to a single-cell rechargeable Lithium-Ion/Polymer
battery and a system load.
@endverbatim

http://www.linear.com/product/LTC4155

http://www.linear.com/product/LTC4155#demoboards


Copyright 2018(c) Analog Devices, Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
 - Neither the name of Analog Devices, Inc. nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.
 - The use of this software may or may not infringe the patent rights
   of one or more patent holders.  This license does not release you
   from the requirement that you obtain separate licenses from these
   patent holders to use this software.
 - Use of the software either in source or binary form, must be run
   on or directly connected to an Analog Devices Inc. component.

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Generated on: 2016-01-19
*/

/*! @file
 *  @ingroup LTC4155
 *  @brief LTC4155 Register Map Definition Header
 *
 *
 *  This file contains LTC4155 definitions for each command_code as well as
 *  each individual bit field for the case when a register contains multiple
 *  bit-packed fields smaller than the register width.
 *  Each bit field name is prepended with LTC4155_.
 *  Each bit field has individual definitions for its _SIZE, _OFFSET (LSB) and _MASK,
 *  as well as the three fields stored in a single 16-bit word for use with the access
 *  functions provided by LTC4155.c and LTC4155.h.
 *  In the case that the bit field contents represent an enumeration, _PRESET
 *  definitions exists to translate from human readable format to the encoded value.
 *  See @ref LTC4155_register_map for detailed descriptions of each bit field.
 */

/*! @defgroup LTC4155_register_map LTC4155 Register Map Definitions
 *  @ingroup LTC4155
 */

#ifndef LTC4155_REG_DEFS_H_
#define LTC4155_REG_DEFS_H_

#define LTC4155_ADDR_09 0x9 //!<LTC4155 I2C address in 7-bit format
/*! @defgroup LTC4155_REG0 REG0
 *  @ingroup LTC4155_register_map
 *  @brief REG0 Register
 *
 * |                  7 |                  6 |              5 |     4:0 |
 * |:------------------:|:------------------:|:--------------:|:-------:|
 * | DISABLE_INPUT_UVCL | EN_BAT_CONDITIONER | LOCKOUT_ID_PIN | USBILIM |
 *
 *   - CommandCode: 0x00
 *   - Contains Bit Fields:
 *     + @ref LTC4155_DISABLE_INPUT_UVCL "DISABLE_INPUT_UVCL" : Disables automatic power reduction in response to low input voltage.
 *     + @ref LTC4155_EN_BAT_CONDITIONER "EN_BAT_CONDITIONER" : Enables autonomous Overtemperature Battery Conditioner Load when the thermistor temperature exceeds 60°C.
 *     + @ref LTC4155_LOCKOUT_ID_PIN "LOCKOUT_ID_PIN" : Disables autonomous USB OTG operation when the ID pin is grounded by an appropriate USB cable.
 *     + @ref LTC4155_USBILIM "USBILIM" : USB total input current limit setting. Register contents will be modified by the LTC4155 upon certain events. See the datasheet section Input Current Regulation.
*/

//!@{
#define LTC4155_REG0_SUBADDR 0x00
#define LTC4155_REG0 (0 << 12 | (16 - 1) << 8 | LTC4155_REG0_SUBADDR)
//!@}
/*! @defgroup LTC4155_DISABLE_INPUT_UVCL DISABLE_INPUT_UVCL
 *  @ingroup LTC4155_register_map
 *  @brief DISABLE_INPUT_UVCL Bit Field
 *
 *  Disables automatic power reduction in response to low input voltage.
 *   - Register: @ref LTC4155_REG0 "REG0"
 *   - CommandCode: 0x00
 *   - Size: 1
 *   - Offset: 7
 *   - MSB: 7
 *   - MASK: 0x80
 *   - Access: R/W
 *   - Default: 0
 */
//!@{
#define LTC4155_DISABLE_INPUT_UVCL_SUBADDR LTC4155_REG0_SUBADDR //!< @ref LTC4155_DISABLE_INPUT_UVCL "DISABLE_INPUT_UVCL"
#define LTC4155_DISABLE_INPUT_UVCL_SIZE 1
#define LTC4155_DISABLE_INPUT_UVCL_OFFSET 7
#define LTC4155_DISABLE_INPUT_UVCL_MASK 0x80
#define LTC4155_DISABLE_INPUT_UVCL (LTC4155_DISABLE_INPUT_UVCL_OFFSET << 12 | (LTC4155_DISABLE_INPUT_UVCL_SIZE - 1) << 8 | LTC4155_DISABLE_INPUT_UVCL_SUBADDR)
//!@}
/*! @defgroup LTC4155_EN_BAT_CONDITIONER EN_BAT_CONDITIONER
 *  @ingroup LTC4155_register_map
 *  @brief EN_BAT_CONDITIONER Bit Field
 *
 *  Enables autonomous Overtemperature Battery Conditioner Load when the thermistor temperature exceeds 60°C.
 *   - Register: @ref LTC4155_REG0 "REG0"
 *   - CommandCode: 0x00
 *   - Size: 1
 *   - Offset: 6
 *   - MSB: 6
 *   - MASK: 0x40
 *   - Access: R/W
 *   - Default: 0
 */
//!@{
#define LTC4155_EN_BAT_CONDITIONER_SUBADDR LTC4155_REG0_SUBADDR //!< @ref LTC4155_EN_BAT_CONDITIONER "EN_BAT_CONDITIONER"
#define LTC4155_EN_BAT_CONDITIONER_SIZE 1
#define LTC4155_EN_BAT_CONDITIONER_OFFSET 6
#define LTC4155_EN_BAT_CONDITIONER_MASK 0x40
#define LTC4155_EN_BAT_CONDITIONER (LTC4155_EN_BAT_CONDITIONER_OFFSET << 12 | (LTC4155_EN_BAT_CONDITIONER_SIZE - 1) << 8 | LTC4155_EN_BAT_CONDITIONER_SUBADDR)
//!@}
/*! @defgroup LTC4155_LOCKOUT_ID_PIN LOCKOUT_ID_PIN
 *  @ingroup LTC4155_register_map
 *  @brief LOCKOUT_ID_PIN Bit Field
 *
 *  Disables autonomous USB OTG operation when the ID pin is grounded by an appropriate USB cable.
 *   - Register: @ref LTC4155_REG0 "REG0"
 *   - CommandCode: 0x00
 *   - Size: 1
 *   - Offset: 5
 *   - MSB: 5
 *   - MASK: 0x20
 *   - Access: R/W
 *   - Default: 0
 */
//!@{
#define LTC4155_LOCKOUT_ID_PIN_SUBADDR LTC4155_REG0_SUBADDR //!< @ref LTC4155_LOCKOUT_ID_PIN "LOCKOUT_ID_PIN"
#define LTC4155_LOCKOUT_ID_PIN_SIZE 1
#define LTC4155_LOCKOUT_ID_PIN_OFFSET 5
#define LTC4155_LOCKOUT_ID_PIN_MASK 0x20
#define LTC4155_LOCKOUT_ID_PIN (LTC4155_LOCKOUT_ID_PIN_OFFSET << 12 | (LTC4155_LOCKOUT_ID_PIN_SIZE - 1) << 8 | LTC4155_LOCKOUT_ID_PIN_SUBADDR)
//!@}
/*! @defgroup LTC4155_USBILIM USBILIM
 *  @ingroup LTC4155_register_map
 *  @brief USBILIM Bit Field
 *
 *  USB total input current limit setting. Register contents will be modified by the LTC4155 upon certain events. See the datasheet section Input Current Regulation.
 *   - Register: @ref LTC4155_REG0 "REG0"
 *   - CommandCode: 0x00
 *   - Size: 5
 *   - Offset: 0
 *   - MSB: 4
 *   - MASK: 0x1F
 *   - Access: R/W
 *   - Default: n/a
 */
//!@{
#define LTC4155_USBILIM_SUBADDR LTC4155_REG0_SUBADDR //!< @ref LTC4155_USBILIM "USBILIM"
#define LTC4155_USBILIM_SIZE 5
#define LTC4155_USBILIM_OFFSET 0
#define LTC4155_USBILIM_MASK 0x1F
#define LTC4155_USBILIM (LTC4155_USBILIM_OFFSET << 12 | (LTC4155_USBILIM_SIZE - 1) << 8 | LTC4155_USBILIM_SUBADDR)
#define LTC4155_USBILIM_PRESET__100MA 0x0
#define LTC4155_USBILIM_PRESET__500MA 0x1
#define LTC4155_USBILIM_PRESET__600MA 0x2
#define LTC4155_USBILIM_PRESET__700MA 0x3
#define LTC4155_USBILIM_PRESET__800MA 0x4
#define LTC4155_USBILIM_PRESET__900MA 0x5
#define LTC4155_USBILIM_PRESET__1P00A 0x6
#define LTC4155_USBILIM_PRESET__1P25A 0x7
#define LTC4155_USBILIM_PRESET__1P50A 0x8
#define LTC4155_USBILIM_PRESET__1P75A 0x9
#define LTC4155_USBILIM_PRESET__2P00A 0xA
#define LTC4155_USBILIM_PRESET__2P25A 0xB
#define LTC4155_USBILIM_PRESET__2P50A 0xC
#define LTC4155_USBILIM_PRESET__2P75A 0xD
#define LTC4155_USBILIM_PRESET__3P00A 0xE
#define LTC4155_USBILIM_PRESET__2P5MA_SUSPEND 0xF
#define LTC4155_USBILIM_PRESET_CLPROG_1 0x1F
//!@}

/*! @defgroup LTC4155_REG1 REG1
 *  @ingroup LTC4155_register_map
 *  @brief REG1 Register
 *
 * |        7 |   6:5 |      4:0 |
 * |:--------:|:-----:|:--------:|
 * | PRIORITY | TIMER | WALLILIM |
 *
 *   - CommandCode: 0x01
 *   - Contains Bit Fields:
 *     + @ref LTC4155_PRIORITY "PRIORITY" : Input connector priority swap setting. Setting PRIORITY will cause power to be drawn from USB when both WALL and USB power are present.
 *     + @ref LTC4155_TIMER "TIMER" : User Programmed Li-Ion charger safety timer setting. This internal timer begins at the onset of constant-voltage charging. Charging terminates at the expiration of the selected time.
 *     + @ref LTC4155_WALLILIM "WALLILIM" : WALL total input current limit setting. Register contents will be modified by the LTC4155 upon certain events. See the datasheet section Input Current Regulation.
*/

//!@{
#define LTC4155_REG1_SUBADDR 0x01
#define LTC4155_REG1 (0 << 12 | (16 - 1) << 8 | LTC4155_REG1_SUBADDR)
//!@}
/*! @defgroup LTC4155_PRIORITY PRIORITY
 *  @ingroup LTC4155_register_map
 *  @brief PRIORITY Bit Field
 *
 *  Input connector priority swap setting. Setting PRIORITY will cause power to be drawn from USB when both WALL and USB power are present.
 *   - Register: @ref LTC4155_REG1 "REG1"
 *   - CommandCode: 0x01
 *   - Size: 1
 *   - Offset: 7
 *   - MSB: 7
 *   - MASK: 0x80
 *   - Access: R/W
 *   - Default: 0
 */
//!@{
#define LTC4155_PRIORITY_SUBADDR LTC4155_REG1_SUBADDR //!< @ref LTC4155_PRIORITY "PRIORITY"
#define LTC4155_PRIORITY_SIZE 1
#define LTC4155_PRIORITY_OFFSET 7
#define LTC4155_PRIORITY_MASK 0x80
#define LTC4155_PRIORITY (LTC4155_PRIORITY_OFFSET << 12 | (LTC4155_PRIORITY_SIZE - 1) << 8 | LTC4155_PRIORITY_SUBADDR)
#define LTC4155_PRIORITY_PRESET_WALL 0x0
#define LTC4155_PRIORITY_PRESET_USB 0x1
//!@}
/*! @defgroup LTC4155_TIMER TIMER
 *  @ingroup LTC4155_register_map
 *  @brief TIMER Bit Field
 *
 *  User Programmed Li-Ion charger safety timer setting. This internal timer begins at the onset of constant-voltage charging. Charging terminates at the expiration of the selected time.
 *   - Register: @ref LTC4155_REG1 "REG1"
 *   - CommandCode: 0x01
 *   - Size: 2
 *   - Offset: 5
 *   - MSB: 6
 *   - MASK: 0x60
 *   - Access: R/W
 *   - Default: 0
 */
//!@{
#define LTC4155_TIMER_SUBADDR LTC4155_REG1_SUBADDR //!< @ref LTC4155_TIMER "TIMER"
#define LTC4155_TIMER_SIZE 2
#define LTC4155_TIMER_OFFSET 5
#define LTC4155_TIMER_MASK 0x60
#define LTC4155_TIMER (LTC4155_TIMER_OFFSET << 12 | (LTC4155_TIMER_SIZE - 1) << 8 | LTC4155_TIMER_SUBADDR)
#define LTC4155_TIMER_PRESET__4_HOUR 0x0
#define LTC4155_TIMER_PRESET__8_HOUR_OR_COVERX 0x1
#define LTC4155_TIMER_PRESET__1_HOUR 0x2
#define LTC4155_TIMER_PRESET__2_HOUR 0x3
//!@}
/*! @defgroup LTC4155_WALLILIM WALLILIM
 *  @ingroup LTC4155_register_map
 *  @brief WALLILIM Bit Field
 *
 *  WALL total input current limit setting. Register contents will be modified by the LTC4155 upon certain events. See the datasheet section Input Current Regulation.
 *   - Register: @ref LTC4155_REG1 "REG1"
 *   - CommandCode: 0x01
 *   - Size: 5
 *   - Offset: 0
 *   - MSB: 4
 *   - MASK: 0x1F
 *   - Access: R/W
 *   - Default: n/a
 */
//!@{
#define LTC4155_WALLILIM_SUBADDR LTC4155_REG1_SUBADDR //!< @ref LTC4155_WALLILIM "WALLILIM"
#define LTC4155_WALLILIM_SIZE 5
#define LTC4155_WALLILIM_OFFSET 0
#define LTC4155_WALLILIM_MASK 0x1F
#define LTC4155_WALLILIM (LTC4155_WALLILIM_OFFSET << 12 | (LTC4155_WALLILIM_SIZE - 1) << 8 | LTC4155_WALLILIM_SUBADDR)
#define LTC4155_WALLILIM_PRESET__100MA 0x0
#define LTC4155_WALLILIM_PRESET__500MA 0x1
#define LTC4155_WALLILIM_PRESET__600MA 0x2
#define LTC4155_WALLILIM_PRESET__700MA 0x3
#define LTC4155_WALLILIM_PRESET__800MA 0x4
#define LTC4155_WALLILIM_PRESET__900MA 0x5
#define LTC4155_WALLILIM_PRESET__1P00A 0x6
#define LTC4155_WALLILIM_PRESET__1P25A 0x7
#define LTC4155_WALLILIM_PRESET__1P50A 0x8
#define LTC4155_WALLILIM_PRESET__1P75A 0x9
#define LTC4155_WALLILIM_PRESET__2P00A 0xA
#define LTC4155_WALLILIM_PRESET__2P25A 0xB
#define LTC4155_WALLILIM_PRESET__2P50A 0xC
#define LTC4155_WALLILIM_PRESET__2P75A 0xD
#define LTC4155_WALLILIM_PRESET__3P00A 0xE
#define LTC4155_WALLILIM_PRESET__2P5MA_SUSPEND 0xF
#define LTC4155_WALLILIM_PRESET_CLPROG_1 0x1F
//!@}

/*! @defgroup LTC4155_REG2 REG2
 *  @ingroup LTC4155_register_map
 *  @brief REG2 Register
 *
 * |     7:4 |    3:2 |   1:0 |
 * |:-------:|:------:|:-----:|
 * | ICHARGE | VFLOAT | CXSET |
 *
 *   - CommandCode: 0x02
 *   - Contains Bit Fields:
 *     + @ref LTC4155_ICHARGE "ICHARGE" : Battery charger current limit setting.
 *     + @ref LTC4155_VFLOAT "VFLOAT" : Battery charge voltage setting.
 *     + @ref LTC4155_CXSET "CXSET" : End-of-Charge indication current threshold setting. This threshold can also be used for charge termination if TIMER is set to 8_Hour_or_CoverX.
*/

//!@{
#define LTC4155_REG2_SUBADDR 0x02
#define LTC4155_REG2 (0 << 12 | (16 - 1) << 8 | LTC4155_REG2_SUBADDR)
//!@}
/*! @defgroup LTC4155_ICHARGE ICHARGE
 *  @ingroup LTC4155_register_map
 *  @brief ICHARGE Bit Field
 *
 *  Battery charger current limit setting.
 *   - Register: @ref LTC4155_REG2 "REG2"
 *   - CommandCode: 0x02
 *   - Size: 4
 *   - Offset: 4
 *   - MSB: 7
 *   - MASK: 0xF0
 *   - Access: R/W
 *   - Default: 0xF
 */
//!@{
#define LTC4155_ICHARGE_SUBADDR LTC4155_REG2_SUBADDR //!< @ref LTC4155_ICHARGE "ICHARGE"
#define LTC4155_ICHARGE_SIZE 4
#define LTC4155_ICHARGE_OFFSET 4
#define LTC4155_ICHARGE_MASK 0xF0
#define LTC4155_ICHARGE (LTC4155_ICHARGE_OFFSET << 12 | (LTC4155_ICHARGE_SIZE - 1) << 8 | LTC4155_ICHARGE_SUBADDR)
#define LTC4155_ICHARGE_PRESET_CHARGER_DISABLED 0x0
#define LTC4155_ICHARGE_PRESET__12P50PCT 0x1
#define LTC4155_ICHARGE_PRESET__18P75PCT 0x2
#define LTC4155_ICHARGE_PRESET__25P00PCT 0x3
#define LTC4155_ICHARGE_PRESET__31P25PCT 0x4
#define LTC4155_ICHARGE_PRESET__37P50PCT 0x5
#define LTC4155_ICHARGE_PRESET__43P75PCT 0x6
#define LTC4155_ICHARGE_PRESET__50P00PCT 0x7
#define LTC4155_ICHARGE_PRESET__56P25PCT 0x8
#define LTC4155_ICHARGE_PRESET__62P50PCT 0x9
#define LTC4155_ICHARGE_PRESET__68P75PCT 0xA
#define LTC4155_ICHARGE_PRESET__75P00PCT 0xB
#define LTC4155_ICHARGE_PRESET__81P25PCT 0xC
#define LTC4155_ICHARGE_PRESET__87P50PCT 0xD
#define LTC4155_ICHARGE_PRESET__93P75PCT 0xE
#define LTC4155_ICHARGE_PRESET__100P0PCT 0xF
//!@}
/*! @defgroup LTC4155_VFLOAT VFLOAT
 *  @ingroup LTC4155_register_map
 *  @brief VFLOAT Bit Field
 *
 *  Battery charge voltage setting.
 *   - Register: @ref LTC4155_REG2 "REG2"
 *   - CommandCode: 0x02
 *   - Size: 2
 *   - Offset: 2
 *   - MSB: 3
 *   - MASK: 0x0C
 *   - Access: R/W
 *   - Default: 0
 */
//!@{
#define LTC4155_VFLOAT_SUBADDR LTC4155_REG2_SUBADDR //!< @ref LTC4155_VFLOAT "VFLOAT"
#define LTC4155_VFLOAT_SIZE 2
#define LTC4155_VFLOAT_OFFSET 2
#define LTC4155_VFLOAT_MASK 0x0C
#define LTC4155_VFLOAT (LTC4155_VFLOAT_OFFSET << 12 | (LTC4155_VFLOAT_SIZE - 1) << 8 | LTC4155_VFLOAT_SUBADDR)
#define LTC4155_VFLOAT_PRESET__4P05V 0x0
#define LTC4155_VFLOAT_PRESET__4P10V 0x1
#define LTC4155_VFLOAT_PRESET__4P15V 0x2
#define LTC4155_VFLOAT_PRESET__4P20V 0x3
//!@}
/*! @defgroup LTC4155_CXSET CXSET
 *  @ingroup LTC4155_register_map
 *  @brief CXSET Bit Field
 *
 *  End-of-Charge indication current threshold setting. This threshold can also be used for charge termination if TIMER is set to 8_Hour_or_CoverX.
 *   - Register: @ref LTC4155_REG2 "REG2"
 *   - CommandCode: 0x02
 *   - Size: 2
 *   - Offset: 0
 *   - MSB: 1
 *   - MASK: 0x03
 *   - Access: R/W
 *   - Default: 0
 */
//!@{
#define LTC4155_CXSET_SUBADDR LTC4155_REG2_SUBADDR //!< @ref LTC4155_CXSET "CXSET"
#define LTC4155_CXSET_SIZE 2
#define LTC4155_CXSET_OFFSET 0
#define LTC4155_CXSET_MASK 0x03
#define LTC4155_CXSET (LTC4155_CXSET_OFFSET << 12 | (LTC4155_CXSET_SIZE - 1) << 8 | LTC4155_CXSET_SUBADDR)
#define LTC4155_CXSET_PRESET__10PCT 0x0
#define LTC4155_CXSET_PRESET__20PCT 0x1
#define LTC4155_CXSET_PRESET__2PCT 0x2
#define LTC4155_CXSET_PRESET__5PCT 0x3
//!@}

/*! @defgroup LTC4155_REG3 REG3
 *  @ingroup LTC4155_register_map
 *  @brief REG3 Register
 *
 * |            7:5 |             4 |           3 |     2:1 |      0 |
 * |:--------------:|:-------------:|:-----------:|:-------:|:------:|
 * | CHARGER_STATUS | ID_PIN_DETECT | OTG_ENABLED | NTCSTAT | LOWBAT |
 *
 *   - CommandCode: 0x03
 *   - Contains Bit Fields:
 *     + @ref LTC4155_CHARGER_STATUS "CHARGER_STATUS" : Battery charger status.
 *     + @ref LTC4155_ID_PIN_DETECT "ID_PIN_DETECT" : On-the-Go boost ID pin detection status.
 *     + @ref LTC4155_OTG_ENABLED "OTG_ENABLED" : On-the-Go boost converter enable status.
 *     + @ref LTC4155_NTCSTAT "NTCSTAT" : Thermistor reading status.
 *     + @ref LTC4155_LOWBAT "LOWBAT" : Battery below ~2.8V or charger disabled.
*/

//!@{
#define LTC4155_REG3_SUBADDR 0x03
#define LTC4155_REG3 (0 << 12 | (16 - 1) << 8 | LTC4155_REG3_SUBADDR)
//!@}
/*! @defgroup LTC4155_CHARGER_STATUS CHARGER_STATUS
 *  @ingroup LTC4155_register_map
 *  @brief CHARGER_STATUS Bit Field
 *
 *  Battery charger status.
 *   - Register: @ref LTC4155_REG3 "REG3"
 *   - CommandCode: 0x03
 *   - Size: 3
 *   - Offset: 5
 *   - MSB: 7
 *   - MASK: 0xE0
 *   - Access: R
 *   - Default: n/a
 */
//!@{
#define LTC4155_CHARGER_STATUS_SUBADDR LTC4155_REG3_SUBADDR //!< @ref LTC4155_CHARGER_STATUS "CHARGER_STATUS"
#define LTC4155_CHARGER_STATUS_SIZE 3
#define LTC4155_CHARGER_STATUS_OFFSET 5
#define LTC4155_CHARGER_STATUS_MASK 0xE0
#define LTC4155_CHARGER_STATUS (LTC4155_CHARGER_STATUS_OFFSET << 12 | (LTC4155_CHARGER_STATUS_SIZE - 1) << 8 | LTC4155_CHARGER_STATUS_SUBADDR)
#define LTC4155_CHARGER_STATUS_PRESET_CHARGER_OFF 0x0
#define LTC4155_CHARGER_STATUS_PRESET_LOW_BAT 0x1
#define LTC4155_CHARGER_STATUS_PRESET_CONSTANT_CURRENT 0x2
#define LTC4155_CHARGER_STATUS_PRESET_CONSTANT_VOLTAGE_I_GREATER_THAN_COVERX 0x3
#define LTC4155_CHARGER_STATUS_PRESET_CONSTANT_VOLTAGE_I_LESS_THAN_COVERX 0x4
#define LTC4155_CHARGER_STATUS_PRESET_NTC_TOO_WARM_TO_CHARGE 0x5
#define LTC4155_CHARGER_STATUS_PRESET_NTC_TOO_COLD_TO_CHARGE 0x6
#define LTC4155_CHARGER_STATUS_PRESET_NTC_CRITICALLY_HOT 0x7
//!@}
/*! @defgroup LTC4155_ID_PIN_DETECT ID_PIN_DETECT
 *  @ingroup LTC4155_register_map
 *  @brief ID_PIN_DETECT Bit Field
 *
 *  On-the-Go boost ID pin detection status.
 *   - Register: @ref LTC4155_REG3 "REG3"
 *   - CommandCode: 0x03
 *   - Size: 1
 *   - Offset: 4
 *   - MSB: 4
 *   - MASK: 0x10
 *   - Access: R
 *   - Default: n/a
 */
//!@{
#define LTC4155_ID_PIN_DETECT_SUBADDR LTC4155_REG3_SUBADDR //!< @ref LTC4155_ID_PIN_DETECT "ID_PIN_DETECT"
#define LTC4155_ID_PIN_DETECT_SIZE 1
#define LTC4155_ID_PIN_DETECT_OFFSET 4
#define LTC4155_ID_PIN_DETECT_MASK 0x10
#define LTC4155_ID_PIN_DETECT (LTC4155_ID_PIN_DETECT_OFFSET << 12 | (LTC4155_ID_PIN_DETECT_SIZE - 1) << 8 | LTC4155_ID_PIN_DETECT_SUBADDR)
//!@}
/*! @defgroup LTC4155_OTG_ENABLED OTG_ENABLED
 *  @ingroup LTC4155_register_map
 *  @brief OTG_ENABLED Bit Field
 *
 *  On-the-Go boost converter enable status.
 *   - Register: @ref LTC4155_REG3 "REG3"
 *   - CommandCode: 0x03
 *   - Size: 1
 *   - Offset: 3
 *   - MSB: 3
 *   - MASK: 0x08
 *   - Access: R
 *   - Default: n/a
 */
//!@{
#define LTC4155_OTG_ENABLED_SUBADDR LTC4155_REG3_SUBADDR //!< @ref LTC4155_OTG_ENABLED "OTG_ENABLED"
#define LTC4155_OTG_ENABLED_SIZE 1
#define LTC4155_OTG_ENABLED_OFFSET 3
#define LTC4155_OTG_ENABLED_MASK 0x08
#define LTC4155_OTG_ENABLED (LTC4155_OTG_ENABLED_OFFSET << 12 | (LTC4155_OTG_ENABLED_SIZE - 1) << 8 | LTC4155_OTG_ENABLED_SUBADDR)
//!@}
/*! @defgroup LTC4155_NTCSTAT NTCSTAT
 *  @ingroup LTC4155_register_map
 *  @brief NTCSTAT Bit Field
 *
 *  Thermistor reading status.
 *   - Register: @ref LTC4155_REG3 "REG3"
 *   - CommandCode: 0x03
 *   - Size: 2
 *   - Offset: 1
 *   - MSB: 2
 *   - MASK: 0x06
 *   - Access: R
 *   - Default: n/a
 */
//!@{
#define LTC4155_NTCSTAT_SUBADDR LTC4155_REG3_SUBADDR //!< @ref LTC4155_NTCSTAT "NTCSTAT"
#define LTC4155_NTCSTAT_SIZE 2
#define LTC4155_NTCSTAT_OFFSET 1
#define LTC4155_NTCSTAT_MASK 0x06
#define LTC4155_NTCSTAT (LTC4155_NTCSTAT_OFFSET << 12 | (LTC4155_NTCSTAT_SIZE - 1) << 8 | LTC4155_NTCSTAT_SUBADDR)
#define LTC4155_NTCSTAT_PRESET_NORMAL 0x0
#define LTC4155_NTCSTAT_PRESET_TOO_COLD 0x1
#define LTC4155_NTCSTAT_PRESET_TOO_WARM 0x2
#define LTC4155_NTCSTAT_PRESET_HOT_FAULT 0x3
//!@}
/*! @defgroup LTC4155_LOWBAT LOWBAT
 *  @ingroup LTC4155_register_map
 *  @brief LOWBAT Bit Field
 *
 *  Battery below ~2.8V or charger disabled.
 *   - Register: @ref LTC4155_REG3 "REG3"
 *   - CommandCode: 0x03
 *   - Size: 1
 *   - Offset: 0
 *   - MSB: 0
 *   - MASK: 0x01
 *   - Access: R
 *   - Default: n/a
 */
//!@{
#define LTC4155_LOWBAT_SUBADDR LTC4155_REG3_SUBADDR //!< @ref LTC4155_LOWBAT "LOWBAT"
#define LTC4155_LOWBAT_SIZE 1
#define LTC4155_LOWBAT_OFFSET 0
#define LTC4155_LOWBAT_MASK 0x01
#define LTC4155_LOWBAT (LTC4155_LOWBAT_OFFSET << 12 | (LTC4155_LOWBAT_SIZE - 1) << 8 | LTC4155_LOWBAT_SUBADDR)
//!@}

/*! @defgroup LTC4155_REG4 REG4
 *  @ingroup LTC4155_register_map
 *  @brief REG4 Register
 *
 * |            7 |           6 |            5 |             4 |                 3 |          2 |         1 |        0 |
 * |:------------:|:-----------:|:------------:|:-------------:|:-----------------:|:----------:|:---------:|:--------:|
 * | EXT_PWR_GOOD | USBSNS_GOOD | WALLSNS_GOOD | AT_INPUT_ILIM | INPUT_UVCL_ACTIVE | OVP_ACTIVE | OTG_FAULT | BAD_CELL |
 *
 *   - CommandCode: 0x04
 *   - Contains Bit Fields:
 *     + @ref LTC4155_EXT_PWR_GOOD "EXT_PWR_GOOD" : External power (Wall or USB) available.
 *     + @ref LTC4155_USBSNS_GOOD "USBSNS_GOOD" : USB input voltage valid.
 *     + @ref LTC4155_WALLSNS_GOOD "WALLSNS_GOOD" : WALL input voltage valid.
 *     + @ref LTC4155_AT_INPUT_ILIM "AT_INPUT_ILIM" : Input current limit control circuit engaged.
 *     + @ref LTC4155_INPUT_UVCL_ACTIVE "INPUT_UVCL_ACTIVE" : Input undervoltage limit control circuit engaged (brownout).
 *     + @ref LTC4155_OVP_ACTIVE "OVP_ACTIVE" : Overvoltage protection fault.
 *     + @ref LTC4155_OTG_FAULT "OTG_FAULT" : USB On-The-Go step-up regulator fault shutdown.
 *     + @ref LTC4155_BAD_CELL "BAD_CELL" : Battery unresponsive to charging for 30 minutes.
*/

//!@{
#define LTC4155_REG4_SUBADDR 0x04
#define LTC4155_REG4 (0 << 12 | (16 - 1) << 8 | LTC4155_REG4_SUBADDR)
//!@}
/*! @defgroup LTC4155_EXT_PWR_GOOD EXT_PWR_GOOD
 *  @ingroup LTC4155_register_map
 *  @brief EXT_PWR_GOOD Bit Field
 *
 *  External power (Wall or USB) available.
 *   - Register: @ref LTC4155_REG4 "REG4"
 *   - CommandCode: 0x04
 *   - Size: 1
 *   - Offset: 7
 *   - MSB: 7
 *   - MASK: 0x80
 *   - Access: R
 *   - Default: n/a
 */
//!@{
#define LTC4155_EXT_PWR_GOOD_SUBADDR LTC4155_REG4_SUBADDR //!< @ref LTC4155_EXT_PWR_GOOD "EXT_PWR_GOOD"
#define LTC4155_EXT_PWR_GOOD_SIZE 1
#define LTC4155_EXT_PWR_GOOD_OFFSET 7
#define LTC4155_EXT_PWR_GOOD_MASK 0x80
#define LTC4155_EXT_PWR_GOOD (LTC4155_EXT_PWR_GOOD_OFFSET << 12 | (LTC4155_EXT_PWR_GOOD_SIZE - 1) << 8 | LTC4155_EXT_PWR_GOOD_SUBADDR)
//!@}
/*! @defgroup LTC4155_USBSNS_GOOD USBSNS_GOOD
 *  @ingroup LTC4155_register_map
 *  @brief USBSNS_GOOD Bit Field
 *
 *  USB input voltage valid.
 *   - Register: @ref LTC4155_REG4 "REG4"
 *   - CommandCode: 0x04
 *   - Size: 1
 *   - Offset: 6
 *   - MSB: 6
 *   - MASK: 0x40
 *   - Access: R
 *   - Default: n/a
 */
//!@{
#define LTC4155_USBSNS_GOOD_SUBADDR LTC4155_REG4_SUBADDR //!< @ref LTC4155_USBSNS_GOOD "USBSNS_GOOD"
#define LTC4155_USBSNS_GOOD_SIZE 1
#define LTC4155_USBSNS_GOOD_OFFSET 6
#define LTC4155_USBSNS_GOOD_MASK 0x40
#define LTC4155_USBSNS_GOOD (LTC4155_USBSNS_GOOD_OFFSET << 12 | (LTC4155_USBSNS_GOOD_SIZE - 1) << 8 | LTC4155_USBSNS_GOOD_SUBADDR)
//!@}
/*! @defgroup LTC4155_WALLSNS_GOOD WALLSNS_GOOD
 *  @ingroup LTC4155_register_map
 *  @brief WALLSNS_GOOD Bit Field
 *
 *  WALL input voltage valid.
 *   - Register: @ref LTC4155_REG4 "REG4"
 *   - CommandCode: 0x04
 *   - Size: 1
 *   - Offset: 5
 *   - MSB: 5
 *   - MASK: 0x20
 *   - Access: R
 *   - Default: n/a
 */
//!@{
#define LTC4155_WALLSNS_GOOD_SUBADDR LTC4155_REG4_SUBADDR //!< @ref LTC4155_WALLSNS_GOOD "WALLSNS_GOOD"
#define LTC4155_WALLSNS_GOOD_SIZE 1
#define LTC4155_WALLSNS_GOOD_OFFSET 5
#define LTC4155_WALLSNS_GOOD_MASK 0x20
#define LTC4155_WALLSNS_GOOD (LTC4155_WALLSNS_GOOD_OFFSET << 12 | (LTC4155_WALLSNS_GOOD_SIZE - 1) << 8 | LTC4155_WALLSNS_GOOD_SUBADDR)
//!@}
/*! @defgroup LTC4155_AT_INPUT_ILIM AT_INPUT_ILIM
 *  @ingroup LTC4155_register_map
 *  @brief AT_INPUT_ILIM Bit Field
 *
 *  Input current limit control circuit engaged.
 *   - Register: @ref LTC4155_REG4 "REG4"
 *   - CommandCode: 0x04
 *   - Size: 1
 *   - Offset: 4
 *   - MSB: 4
 *   - MASK: 0x10
 *   - Access: R
 *   - Default: n/a
 */
//!@{
#define LTC4155_AT_INPUT_ILIM_SUBADDR LTC4155_REG4_SUBADDR //!< @ref LTC4155_AT_INPUT_ILIM "AT_INPUT_ILIM"
#define LTC4155_AT_INPUT_ILIM_SIZE 1
#define LTC4155_AT_INPUT_ILIM_OFFSET 4
#define LTC4155_AT_INPUT_ILIM_MASK 0x10
#define LTC4155_AT_INPUT_ILIM (LTC4155_AT_INPUT_ILIM_OFFSET << 12 | (LTC4155_AT_INPUT_ILIM_SIZE - 1) << 8 | LTC4155_AT_INPUT_ILIM_SUBADDR)
//!@}
/*! @defgroup LTC4155_INPUT_UVCL_ACTIVE INPUT_UVCL_ACTIVE
 *  @ingroup LTC4155_register_map
 *  @brief INPUT_UVCL_ACTIVE Bit Field
 *
 *  Input undervoltage limit control circuit engaged (brownout).
 *   - Register: @ref LTC4155_REG4 "REG4"
 *   - CommandCode: 0x04
 *   - Size: 1
 *   - Offset: 3
 *   - MSB: 3
 *   - MASK: 0x08
 *   - Access: R
 *   - Default: n/a
 */
//!@{
#define LTC4155_INPUT_UVCL_ACTIVE_SUBADDR LTC4155_REG4_SUBADDR //!< @ref LTC4155_INPUT_UVCL_ACTIVE "INPUT_UVCL_ACTIVE"
#define LTC4155_INPUT_UVCL_ACTIVE_SIZE 1
#define LTC4155_INPUT_UVCL_ACTIVE_OFFSET 3
#define LTC4155_INPUT_UVCL_ACTIVE_MASK 0x08
#define LTC4155_INPUT_UVCL_ACTIVE (LTC4155_INPUT_UVCL_ACTIVE_OFFSET << 12 | (LTC4155_INPUT_UVCL_ACTIVE_SIZE - 1) << 8 | LTC4155_INPUT_UVCL_ACTIVE_SUBADDR)
//!@}
/*! @defgroup LTC4155_OVP_ACTIVE OVP_ACTIVE
 *  @ingroup LTC4155_register_map
 *  @brief OVP_ACTIVE Bit Field
 *
 *  Overvoltage protection fault.
 *   - Register: @ref LTC4155_REG4 "REG4"
 *   - CommandCode: 0x04
 *   - Size: 1
 *   - Offset: 2
 *   - MSB: 2
 *   - MASK: 0x04
 *   - Access: R
 *   - Default: n/a
 */
//!@{
#define LTC4155_OVP_ACTIVE_SUBADDR LTC4155_REG4_SUBADDR //!< @ref LTC4155_OVP_ACTIVE "OVP_ACTIVE"
#define LTC4155_OVP_ACTIVE_SIZE 1
#define LTC4155_OVP_ACTIVE_OFFSET 2
#define LTC4155_OVP_ACTIVE_MASK 0x04
#define LTC4155_OVP_ACTIVE (LTC4155_OVP_ACTIVE_OFFSET << 12 | (LTC4155_OVP_ACTIVE_SIZE - 1) << 8 | LTC4155_OVP_ACTIVE_SUBADDR)
//!@}
/*! @defgroup LTC4155_OTG_FAULT OTG_FAULT
 *  @ingroup LTC4155_register_map
 *  @brief OTG_FAULT Bit Field
 *
 *  USB On-The-Go step-up regulator fault shutdown.
 *   - Register: @ref LTC4155_REG4 "REG4"
 *   - CommandCode: 0x04
 *   - Size: 1
 *   - Offset: 1
 *   - MSB: 1
 *   - MASK: 0x02
 *   - Access: R
 *   - Default: n/a
 */
//!@{
#define LTC4155_OTG_FAULT_SUBADDR LTC4155_REG4_SUBADDR //!< @ref LTC4155_OTG_FAULT "OTG_FAULT"
#define LTC4155_OTG_FAULT_SIZE 1
#define LTC4155_OTG_FAULT_OFFSET 1
#define LTC4155_OTG_FAULT_MASK 0x02
#define LTC4155_OTG_FAULT (LTC4155_OTG_FAULT_OFFSET << 12 | (LTC4155_OTG_FAULT_SIZE - 1) << 8 | LTC4155_OTG_FAULT_SUBADDR)
//!@}
/*! @defgroup LTC4155_BAD_CELL BAD_CELL
 *  @ingroup LTC4155_register_map
 *  @brief BAD_CELL Bit Field
 *
 *  Battery unresponsive to charging for 30 minutes.
 *   - Register: @ref LTC4155_REG4 "REG4"
 *   - CommandCode: 0x04
 *   - Size: 1
 *   - Offset: 0
 *   - MSB: 0
 *   - MASK: 0x01
 *   - Access: R
 *   - Default: n/a
 */
//!@{
#define LTC4155_BAD_CELL_SUBADDR LTC4155_REG4_SUBADDR //!< @ref LTC4155_BAD_CELL "BAD_CELL"
#define LTC4155_BAD_CELL_SIZE 1
#define LTC4155_BAD_CELL_OFFSET 0
#define LTC4155_BAD_CELL_MASK 0x01
#define LTC4155_BAD_CELL (LTC4155_BAD_CELL_OFFSET << 12 | (LTC4155_BAD_CELL_SIZE - 1) << 8 | LTC4155_BAD_CELL_SUBADDR)
//!@}

/*! @defgroup LTC4155_REG5 REG5
 *  @ingroup LTC4155_register_map
 *  @brief REG5 Register
 *
 * |    7:1 |           0 |
 * |:------:|:-----------:|
 * | NTCVAL | NTC_WARNING |
 *
 *   - CommandCode: 0x05
 *   - Contains Bit Fields:
 *     + @ref LTC4155_NTCVAL "NTCVAL" : NTC pin (thermistor) ADC conversion result.
 *     + @ref LTC4155_NTC_WARNING "NTC_WARNING" : Thermistor temperature out of range for battery charging.
*/

//!@{
#define LTC4155_REG5_SUBADDR 0x05
#define LTC4155_REG5 (0 << 12 | (16 - 1) << 8 | LTC4155_REG5_SUBADDR)
//!@}
/*! @defgroup LTC4155_NTCVAL NTCVAL
 *  @ingroup LTC4155_register_map
 *  @brief NTCVAL Bit Field
 *
 *  NTC pin (thermistor) ADC conversion result.
 *   - Register: @ref LTC4155_REG5 "REG5"
 *   - CommandCode: 0x05
 *   - Size: 7
 *   - Offset: 1
 *   - MSB: 7
 *   - MASK: 0xFE
 *   - Access: R
 *   - Default: n/a
 */
//!@{
#define LTC4155_NTCVAL_SUBADDR LTC4155_REG5_SUBADDR //!< @ref LTC4155_NTCVAL "NTCVAL"
#define LTC4155_NTCVAL_SIZE 7
#define LTC4155_NTCVAL_OFFSET 1
#define LTC4155_NTCVAL_MASK 0xFE
#define LTC4155_NTCVAL (LTC4155_NTCVAL_OFFSET << 12 | (LTC4155_NTCVAL_SIZE - 1) << 8 | LTC4155_NTCVAL_SUBADDR)
//!@}
/*! @defgroup LTC4155_NTC_WARNING NTC_WARNING
 *  @ingroup LTC4155_register_map
 *  @brief NTC_WARNING Bit Field
 *
 *  Thermistor temperature out of range for battery charging.
 *   - Register: @ref LTC4155_REG5 "REG5"
 *   - CommandCode: 0x05
 *   - Size: 1
 *   - Offset: 0
 *   - MSB: 0
 *   - MASK: 0x01
 *   - Access: R
 *   - Default: n/a
 */
//!@{
#define LTC4155_NTC_WARNING_SUBADDR LTC4155_REG5_SUBADDR //!< @ref LTC4155_NTC_WARNING "NTC_WARNING"
#define LTC4155_NTC_WARNING_SIZE 1
#define LTC4155_NTC_WARNING_OFFSET 0
#define LTC4155_NTC_WARNING_MASK 0x01
#define LTC4155_NTC_WARNING (LTC4155_NTC_WARNING_OFFSET << 12 | (LTC4155_NTC_WARNING_SIZE - 1) << 8 | LTC4155_NTC_WARNING_SUBADDR)
//!@}

/*! @defgroup LTC4155_REG6 REG6
 *  @ingroup LTC4155_register_map
 *  @brief REG6 Register
 *
 * |                  7 |                6 |                 5 |              4 |                  3 |                     2 |           1 |        0 |
 * |:------------------:|:----------------:|:-----------------:|:--------------:|:------------------:|:---------------------:|:-----------:|:--------:|
 * | ENABLE_CHARGER_INT | ENABLE_FAULT_INT | ENABLE_EXTPWR_INT | ENABLE_OTG_INT | ENABLE_AT_ILIM_INT | ENABLE_INPUT_UVCL_INT | REQUEST_OTG | RESERVED |
 *
 * Interrupt Mask Resister
 *   - CommandCode: 0x06
 *   - Contains Bit Fields:
 *     + @ref LTC4155_ENABLE_CHARGER_INT "ENABLE_CHARGER_INT" : Enable battery CHARGER_STATUS interrupts.
 *     + @ref LTC4155_ENABLE_FAULT_INT "ENABLE_FAULT_INT" : Enable OVP_ACTIVE, OTG_FAULT, BAD_CELL, and NTCSTAT = Hot_Fault interrupts.
 *     + @ref LTC4155_ENABLE_EXTPWR_INT "ENABLE_EXTPWR_INT" : Enable USBSNS_GOOD, WALLSNS_GOOD, EXT_PWR_GOOD external power available interrupts.
 *     + @ref LTC4155_ENABLE_OTG_INT "ENABLE_OTG_INT" : Enable ID_PIN_DETECT and OTG_ENABLED USB On-The-Go interrupts.
 *     + @ref LTC4155_ENABLE_AT_ILIM_INT "ENABLE_AT_ILIM_INT" : Enable AT_INPUT_ILIM interrupts.
 *     + @ref LTC4155_ENABLE_INPUT_UVCL_INT "ENABLE_INPUT_UVCL_INT" : Enable INPUT_UVCL_ACTIVE interrupts.
 *     + @ref LTC4155_REQUEST_OTG "REQUEST_OTG" : USB On-The-Go step-up voltage converter manual activation setting.
 *     + @ref LTC4155_RESERVED "RESERVED" : Always set to 0.
*/

//!@{
#define LTC4155_REG6_SUBADDR 0x06
#define LTC4155_REG6 (0 << 12 | (16 - 1) << 8 | LTC4155_REG6_SUBADDR)
//!@}
/*! @defgroup LTC4155_ENABLE_CHARGER_INT ENABLE_CHARGER_INT
 *  @ingroup LTC4155_register_map
 *  @brief ENABLE_CHARGER_INT Bit Field
 *
 *  Enable battery CHARGER_STATUS interrupts.
 *   - Register: @ref LTC4155_REG6 "REG6"
 *   - CommandCode: 0x06
 *   - Size: 1
 *   - Offset: 7
 *   - MSB: 7
 *   - MASK: 0x80
 *   - Access: R/W
 *   - Default: 0
 */
//!@{
#define LTC4155_ENABLE_CHARGER_INT_SUBADDR LTC4155_REG6_SUBADDR //!< @ref LTC4155_ENABLE_CHARGER_INT "ENABLE_CHARGER_INT"
#define LTC4155_ENABLE_CHARGER_INT_SIZE 1
#define LTC4155_ENABLE_CHARGER_INT_OFFSET 7
#define LTC4155_ENABLE_CHARGER_INT_MASK 0x80
#define LTC4155_ENABLE_CHARGER_INT (LTC4155_ENABLE_CHARGER_INT_OFFSET << 12 | (LTC4155_ENABLE_CHARGER_INT_SIZE - 1) << 8 | LTC4155_ENABLE_CHARGER_INT_SUBADDR)
//!@}
/*! @defgroup LTC4155_ENABLE_FAULT_INT ENABLE_FAULT_INT
 *  @ingroup LTC4155_register_map
 *  @brief ENABLE_FAULT_INT Bit Field
 *
 *  Enable OVP_ACTIVE, OTG_FAULT, BAD_CELL, and NTCSTAT = Hot_Fault interrupts.
 *   - Register: @ref LTC4155_REG6 "REG6"
 *   - CommandCode: 0x06
 *   - Size: 1
 *   - Offset: 6
 *   - MSB: 6
 *   - MASK: 0x40
 *   - Access: R/W
 *   - Default: 0
 */
//!@{
#define LTC4155_ENABLE_FAULT_INT_SUBADDR LTC4155_REG6_SUBADDR //!< @ref LTC4155_ENABLE_FAULT_INT "ENABLE_FAULT_INT"
#define LTC4155_ENABLE_FAULT_INT_SIZE 1
#define LTC4155_ENABLE_FAULT_INT_OFFSET 6
#define LTC4155_ENABLE_FAULT_INT_MASK 0x40
#define LTC4155_ENABLE_FAULT_INT (LTC4155_ENABLE_FAULT_INT_OFFSET << 12 | (LTC4155_ENABLE_FAULT_INT_SIZE - 1) << 8 | LTC4155_ENABLE_FAULT_INT_SUBADDR)
//!@}
/*! @defgroup LTC4155_ENABLE_EXTPWR_INT ENABLE_EXTPWR_INT
 *  @ingroup LTC4155_register_map
 *  @brief ENABLE_EXTPWR_INT Bit Field
 *
 *  Enable USBSNS_GOOD, WALLSNS_GOOD, EXT_PWR_GOOD external power available interrupts.
 *   - Register: @ref LTC4155_REG6 "REG6"
 *   - CommandCode: 0x06
 *   - Size: 1
 *   - Offset: 5
 *   - MSB: 5
 *   - MASK: 0x20
 *   - Access: R/W
 *   - Default: 0
 */
//!@{
#define LTC4155_ENABLE_EXTPWR_INT_SUBADDR LTC4155_REG6_SUBADDR //!< @ref LTC4155_ENABLE_EXTPWR_INT "ENABLE_EXTPWR_INT"
#define LTC4155_ENABLE_EXTPWR_INT_SIZE 1
#define LTC4155_ENABLE_EXTPWR_INT_OFFSET 5
#define LTC4155_ENABLE_EXTPWR_INT_MASK 0x20
#define LTC4155_ENABLE_EXTPWR_INT (LTC4155_ENABLE_EXTPWR_INT_OFFSET << 12 | (LTC4155_ENABLE_EXTPWR_INT_SIZE - 1) << 8 | LTC4155_ENABLE_EXTPWR_INT_SUBADDR)
//!@}
/*! @defgroup LTC4155_ENABLE_OTG_INT ENABLE_OTG_INT
 *  @ingroup LTC4155_register_map
 *  @brief ENABLE_OTG_INT Bit Field
 *
 *  Enable ID_PIN_DETECT and OTG_ENABLED USB On-The-Go interrupts.
 *   - Register: @ref LTC4155_REG6 "REG6"
 *   - CommandCode: 0x06
 *   - Size: 1
 *   - Offset: 4
 *   - MSB: 4
 *   - MASK: 0x10
 *   - Access: R/W
 *   - Default: 0
 */
//!@{
#define LTC4155_ENABLE_OTG_INT_SUBADDR LTC4155_REG6_SUBADDR //!< @ref LTC4155_ENABLE_OTG_INT "ENABLE_OTG_INT"
#define LTC4155_ENABLE_OTG_INT_SIZE 1
#define LTC4155_ENABLE_OTG_INT_OFFSET 4
#define LTC4155_ENABLE_OTG_INT_MASK 0x10
#define LTC4155_ENABLE_OTG_INT (LTC4155_ENABLE_OTG_INT_OFFSET << 12 | (LTC4155_ENABLE_OTG_INT_SIZE - 1) << 8 | LTC4155_ENABLE_OTG_INT_SUBADDR)
//!@}
/*! @defgroup LTC4155_ENABLE_AT_ILIM_INT ENABLE_AT_ILIM_INT
 *  @ingroup LTC4155_register_map
 *  @brief ENABLE_AT_ILIM_INT Bit Field
 *
 *  Enable AT_INPUT_ILIM interrupts.
 *   - Register: @ref LTC4155_REG6 "REG6"
 *   - CommandCode: 0x06
 *   - Size: 1
 *   - Offset: 3
 *   - MSB: 3
 *   - MASK: 0x08
 *   - Access: R/W
 *   - Default: 0
 */
//!@{
#define LTC4155_ENABLE_AT_ILIM_INT_SUBADDR LTC4155_REG6_SUBADDR //!< @ref LTC4155_ENABLE_AT_ILIM_INT "ENABLE_AT_ILIM_INT"
#define LTC4155_ENABLE_AT_ILIM_INT_SIZE 1
#define LTC4155_ENABLE_AT_ILIM_INT_OFFSET 3
#define LTC4155_ENABLE_AT_ILIM_INT_MASK 0x08
#define LTC4155_ENABLE_AT_ILIM_INT (LTC4155_ENABLE_AT_ILIM_INT_OFFSET << 12 | (LTC4155_ENABLE_AT_ILIM_INT_SIZE - 1) << 8 | LTC4155_ENABLE_AT_ILIM_INT_SUBADDR)
//!@}
/*! @defgroup LTC4155_ENABLE_INPUT_UVCL_INT ENABLE_INPUT_UVCL_INT
 *  @ingroup LTC4155_register_map
 *  @brief ENABLE_INPUT_UVCL_INT Bit Field
 *
 *  Enable INPUT_UVCL_ACTIVE interrupts.
 *   - Register: @ref LTC4155_REG6 "REG6"
 *   - CommandCode: 0x06
 *   - Size: 1
 *   - Offset: 2
 *   - MSB: 2
 *   - MASK: 0x04
 *   - Access: R/W
 *   - Default: 0
 */
//!@{
#define LTC4155_ENABLE_INPUT_UVCL_INT_SUBADDR LTC4155_REG6_SUBADDR //!< @ref LTC4155_ENABLE_INPUT_UVCL_INT "ENABLE_INPUT_UVCL_INT"
#define LTC4155_ENABLE_INPUT_UVCL_INT_SIZE 1
#define LTC4155_ENABLE_INPUT_UVCL_INT_OFFSET 2
#define LTC4155_ENABLE_INPUT_UVCL_INT_MASK 0x04
#define LTC4155_ENABLE_INPUT_UVCL_INT (LTC4155_ENABLE_INPUT_UVCL_INT_OFFSET << 12 | (LTC4155_ENABLE_INPUT_UVCL_INT_SIZE - 1) << 8 | LTC4155_ENABLE_INPUT_UVCL_INT_SUBADDR)
//!@}
/*! @defgroup LTC4155_REQUEST_OTG REQUEST_OTG
 *  @ingroup LTC4155_register_map
 *  @brief REQUEST_OTG Bit Field
 *
 *  USB On-The-Go step-up voltage converter manual activation setting.
 *   - Register: @ref LTC4155_REG6 "REG6"
 *   - CommandCode: 0x06
 *   - Size: 1
 *   - Offset: 1
 *   - MSB: 1
 *   - MASK: 0x02
 *   - Access: R/W
 *   - Default: 0
 */
//!@{
#define LTC4155_REQUEST_OTG_SUBADDR LTC4155_REG6_SUBADDR //!< @ref LTC4155_REQUEST_OTG "REQUEST_OTG"
#define LTC4155_REQUEST_OTG_SIZE 1
#define LTC4155_REQUEST_OTG_OFFSET 1
#define LTC4155_REQUEST_OTG_MASK 0x02
#define LTC4155_REQUEST_OTG (LTC4155_REQUEST_OTG_OFFSET << 12 | (LTC4155_REQUEST_OTG_SIZE - 1) << 8 | LTC4155_REQUEST_OTG_SUBADDR)
//!@}
/*! @defgroup LTC4155_RESERVED RESERVED
 *  @ingroup LTC4155_register_map
 *  @brief RESERVED Bit Field
 *
 *  Always set to 0.
 *   - Register: @ref LTC4155_REG6 "REG6"
 *   - CommandCode: 0x06
 *   - Size: 1
 *   - Offset: 0
 *   - MSB: 0
 *   - MASK: 0x01
 *   - Access: R/W
 *   - Default: 0
 */
//!@{
#define LTC4155_RESERVED_SUBADDR LTC4155_REG6_SUBADDR //!< @ref LTC4155_RESERVED "RESERVED"
#define LTC4155_RESERVED_SIZE 1
#define LTC4155_RESERVED_OFFSET 0
#define LTC4155_RESERVED_MASK 0x01
#define LTC4155_RESERVED (LTC4155_RESERVED_OFFSET << 12 | (LTC4155_RESERVED_SIZE - 1) << 8 | LTC4155_RESERVED_SUBADDR)
//!@}

/*! @defgroup LTC4155_REG7 REG7
 *  @ingroup LTC4155_register_map
 *  @brief REG7 Register
 *
 * |          7:0 |
 * |:------------:|
 * | ARM_SHIPMODE |
 *
 *   - CommandCode: 0x07
 *   - Contains Bit Fields:
 *     + @ref LTC4155_ARM_SHIPMODE "ARM_SHIPMODE" : Write any data to this address to enable ship-and-store shutdown mode
*/

//!@{
#define LTC4155_REG7_SUBADDR 0x07
#define LTC4155_REG7 (0 << 12 | (16 - 1) << 8 | LTC4155_REG7_SUBADDR)
//!@}
/*! @defgroup LTC4155_ARM_SHIPMODE ARM_SHIPMODE
 *  @ingroup LTC4155_register_map
 *  @brief ARM_SHIPMODE Bit Field
 *
 *  Write any data to this address to enable ship-and-store shutdown mode
 *   - Register: @ref LTC4155_REG7 "REG7"
 *   - CommandCode: 0x07
 *   - Size: 8
 *   - Offset: 0
 *   - MSB: 7
 *   - MASK: 0xFF
 *   - Access: W
 *   - Default: 0
 */
//!@{
#define LTC4155_ARM_SHIPMODE_SUBADDR LTC4155_REG7_SUBADDR //!< @ref LTC4155_ARM_SHIPMODE "ARM_SHIPMODE"
#define LTC4155_ARM_SHIPMODE_SIZE 8
#define LTC4155_ARM_SHIPMODE_OFFSET 0
#define LTC4155_ARM_SHIPMODE_MASK 0xFF
#define LTC4155_ARM_SHIPMODE (LTC4155_ARM_SHIPMODE_OFFSET << 12 | (LTC4155_ARM_SHIPMODE_SIZE - 1) << 8 | LTC4155_ARM_SHIPMODE_SUBADDR)
#define LTC4155_ARM_SHIPMODE_PRESET_ARM 0x01
//!@}

#endif
