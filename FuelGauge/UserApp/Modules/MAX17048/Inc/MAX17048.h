/*******************************************************************************
 *                     _           _           _    _                           *
 *         /\         (_)         | |         | |  | |                          *
 *        /  \   _ __  _ ___   ___| |__   __ _| | _| | _____  _   _ _ __        *
 *       / /\ \ | '_ \| / __| / __| '_ \ / _` | |/ / |/ / _ \| | | | '__|       *
 *      / ____ \| | | | \__ \ \__ \ | | | (_| |   <|   < (_) | |_| | |          *
 *     /_/    \_\_| |_|_|___/ |___/_| |_|\__,_|_|\_\_|\_\___/ \__,_|_|          *
 *                                                                              *
 *******************************************************************************/
/*******************************************************************************
 * File MAX17408.h
 *
 *  Created on: May 27, 2023
 *  Author: Anis Shakkour
 *  Email:  anis.shakkour399@gmail.com
 *
 *  @brief : IC accepts 16bit of read/write over I2C with speeds up to 400khz
 *  @note  :
 *  @todo  :
 *
 *
 * For more information on MAX17048,
 *    please refer to the "https://www.analog.com/media/en/technical-documentation/data-sheets/max17048-max17049.pdf".
 *
 *******************************************************************************/
/*
 *
 * SOC: State Of Charge
 * OCV: Open Circuit Voltage
 *
 */
/******************************************************************************
 * Multiple include protection
 *****************************************************************************/
#ifndef MAX17048_H_
#define MAX17048_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "FuelGauge.h"    /* Projects settings and configurations */

#include "I2C_Driver.h"   /* I2C driver */

/*******************************************************************************
 * Macros
 ******************************************************************************/

/*******************************************************************************
 * Defines
 ******************************************************************************/
#define MAX17048_ADDRESS        0x36  /*< Unshifted I2C address. Becomes 0x6C for write and 0x6D for read                                       */ //R
#define MAX17048_VERSION_REG    0x08  /*< IC production version                                                                                 */ //R

#define MAX17048_VCELL_REG      0x02  /*< Voltage between VDD-GND - average  of four ADC conversions every 250ms  or 45s when in hibernate mode */  //R
#define MAX17048_SOC_REG        0x04  /*< State-Of Charge - */                                                                                     //R
#define MAX17048_MODE_REG       0x06  /*< */ //W
#define MAX17048_HIBRT_REG      0x0A  /*< Thresholds for entering/exiting hibernate mode                                                        */ //R/W
#define MAX17048_CONFIG_REG     0x0C  /*< Compensation to optimize performance, sleep m ode, alerts and configurations                          */ //R/W
#define MAX17048_VALRT_REG      0x14  /*< Configures the VCEL range outside of which alerts are generated                                       */ //R/W
#define MAX17048_CRATE_REG      0x16  /*< Approximate charge or discharge rate of battery 0.208%/hr per LSB                                     */ //R
#define MAX17048_VRESET_ID_REG  0x18  /*< VCEL threshold where below IC resets. ID one factory programmed Identifier                            */ //R/W
#define MAX17048_STATUS_REG
#define MAX17048_TABLE_REG      //0x40-0x7f configures battery parameter. W
#define MAX17048_CMD_REG        0xFE /*< Send POR command                                                                                       */ //R/W


/////////////////////////////////
// MAX17048 Mode Register Bits //
/////////////////////////////////
#define MAX17048_MODE_QUICKSTART_BIT ((uint8_t)(1 << 6)) //MSB Address
#define MAX17048_MODE_ENSLEEP_BIT    ((uint8_t)(1 << 5)) //MSB Address
#define MAX17048_MODE_HIBSTAT_BIT    ((uint8_t)(1 << 4)) //MSB Address, read only

////////////////////////////////////
// MAX17048 Version Register Bits //
////////////////////////////////////
//#define MAX17048_VERSION

/*******************************************************************************
 * Typedef & Enums
 ******************************************************************************/

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/*******************************************************************************
 * Interface Functions
 ******************************************************************************/
RetVal_t MAX17048_IsConnected(void);

/*******************************************************************************
 * END
 ******************************************************************************/
#endif /* MAX17048_H_ */
