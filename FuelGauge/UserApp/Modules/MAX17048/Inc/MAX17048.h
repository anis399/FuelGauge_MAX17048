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
 *  @brief :
 *  @note  :
 *  @todo  :
 *
 *
 * For more information on MAX17048,
 *    please refer to the "https://www.analog.com/media/en/technical-documentation/data-sheets/max17048-max17049.pdf".
 *
*******************************************************************************/

/******************************************************************************
 * Multiple include protection
 *****************************************************************************/
#ifndef MAX17048_H_
#define MAX17048_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "FuelGauge.h"    /* Projects settings and configurations */

#include "I2C_Driver.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/

/*******************************************************************************
 * Defines
 ******************************************************************************/
#define MAX17048_ADDRESS 0x36     //Unshifted I2C address. Becomes 0x6C for write and 0x6D for read.


/*******************************************************************************
 * Typedef & Enums
 ******************************************************************************/

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/*******************************************************************************
 * Interface Functions
 ******************************************************************************/

/*******************************************************************************
 * END
 ******************************************************************************/
#endif /* MAX17048_H_ */
