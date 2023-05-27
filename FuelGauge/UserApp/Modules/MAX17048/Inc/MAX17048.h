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
 * ALSC: SOC Change Alert
 * ALRT
 * ATHD: empty
 * EnVR: voltage reset alert
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
#define IS_UINT8(x)     (((x) >= 0x00) && ((x) <= 0xFF))

/*******************************************************************************
 * Defines
 ******************************************************************************/
#define BATTERY_MIN_VAL                           ((float)3.125)
#define BATTERY_MAX_VAL                           ((float)4.210)
#define BATTERY_EMPTY_THRESHOLD_ALERT_PERCENTAGE  ((uint8_t)25) // possible values [1-30]%

//Scales
#define MAX17048_VALRT_SCALE    ((float)0.02)        //1LSB is 20mV, when setting voltage alerts
#define MAX17048_VCELL_SCALE    ((float)0.000078125) //78.125uV/LSB, when reading voltage register
//Registers
#define MAX17048_ADDRESS        0x36  /*< Unshifted I2C address. Becomes 0x6C for write and 0x6D for read                                       */ //R
#define MAX17048_SAddr          (MAX17048_ADDRESS << 1)
#define MAX17048_VERSION_REG    0x08  /*< IC production version                                                                                 */ //R

#define MAX17048_VCELL_REG      0x02  /*< Voltage between VDD-GND - average  of four ADC conversions every 250ms  or 45s when in hibernate mode */  //R
#define MAX17048_SOC_REG        0x04  /*< State-Of Charge - */                                                                                     //R
#define MAX17048_MODE_REG       0x06  /*< */ //W
#define MAX17048_HIBRT_REG      0x0A  /*< Thresholds for entering/exiting hibernate mode                                                        */ //R/W
#define MAX17048_CONFIG_REG     0x0C  /*< Compensation to optimize performance, sleep m ode, alerts and configurations                          */ //R/W
#define MAX17048_VALRT_REG      0x14  /*< Configures the VCEL range outside of which alerts are generated                                       */ //R/W
#define MAX17048_CRATE_REG      0x16  /*< Approximate charge or discharge rate of battery 0.208%/hr per LSB                                     */ //R
#define MAX17048_VRESET_ID_REG  0x18  /*< VCEL threshold where below IC resets. ID one factory programmed Identifier                            */ //R/W
#define MAX17048_STATUS_REG     0x1A  /*< Contains alerts generated when operating*/
#define MAX17048_CMD_REG        0xFE /*< Send POR command                                                                                       */ //R/W
#define MAX17048_TABLE_REG      //0x40-0x7f configures battery parameter. W


/////////////////////////////////
// MAX17048 Mode Register Bits //
/////////////////////////////////
#define MAX17048_MODE_QUICKSTART_BIT ((uint16_t)(1 << 14)) // Generates fast SOC, not recommended to be used.
#define MAX17048_MODE_ENSLEEP_BIT    ((uint16_t)(1 << 13)) // Enables sleep mode
#define MAX17048_MODE_HIBSTAT_BIT    ((uint16_t)(1 << 12)) // Read only

//////////////////////////////////////
// MAX17048 Hibernate Register Bits //
//////////////////////////////////////
#define MAX17048_HIBRT_ENHIB  0xFFFF  // always use hibernate mode
#define MAX17048_HIBRT_DISHIB 0x0000  // disable hibernate mode

//////////////////////////////////////////
// MAX17048 Configuration Register Bits //
//////////////////////////////////////////
#define MAX17048_CONFIG_ALRT_bit    ((uint8_t)(1 << 5 )) //Alerts bit, Clear to perform software reset.

///////////////////////////////////
// MAX17048 Status Register Bits //
///////////////////////////////////
#define MAX17048_STATUS_RI_bit    ((uint16_t)(1 << 8 )) //
#define MAX17048_STATUS_VH_bit    ((uint16_t)(1 << 9 )) // Over Voltage alert  - Vcell > VALRTMAX
#define MAX17048_STATUS_VL_bit    ((uint16_t)(1 << 10)) // Under Voltage alert - Vcell < VALRTMIN
#define MAX17048_STATUS_VR_bit    ((uint16_t)(1 << 11)) // Low SOC alert
#define MAX17048_STATUS_HD_bit    ((uint16_t)(1 << 12))
#define MAX17048_STATUS_SC_bit    ((uint16_t)(1 << 13)) //
#define MAX17048_STATUS_EnVR_bit  ((uint16_t)(1 << 14))

/*******************************************************************************
 * Typedef & Enums
 ******************************************************************************/
typedef struct {

  bool IsConnected;
  float batSOC;
  float batVoltage;

}MAX17048_Handle_s;



/*******************************************************************************
 * Extern
 ******************************************************************************/
//extern MAX17048_Handle_s MAX17048_Handle;


/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/*******************************************************************************
 * Interface Functions
 ******************************************************************************/
RetVal_t MAX17048_Config(void);

RetVal_t MAX17048_IsConnected(void);
RetVal_t MAX17048_GetBat_StateOfCharge(void);
RetVal_t MAX17048_GetBat_CellVoltage(void);
RetVal_t MAX17048_GetAlerts(void);
RetVal_t MAX17048_AlertsHandler(uint16_t alert);

RetVal_t MAX17048_ResetAlerts(void);
/*******************************************************************************
 * END
 ******************************************************************************/
#endif /* MAX17048_H_ */
