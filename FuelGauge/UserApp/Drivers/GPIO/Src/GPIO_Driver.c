/*******************************************************************************
 * File Name.c
 *
 *  Created on: April 03, 2023
 *  Author: Anis Shakkour
 *  Email:  anis.shakkour399@gmail.com
 ******************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "GPIO_Driver.h"

/******************************************************************************
 * Extern
 *****************************************************************************/

/******************************************************************************
 * Data types
 *****************************************************************************/

/******************************************************************************
 * Public Function Prototypes
 *****************************************************************************/
/*******************************************************************************
 * Function name:
 *
 * Description  : Function is blocking.
 * Parameters   :
 * Returns      :
 *
 * Known issues :
 * Note         :
 ******************************************************************************/
void GPIO_D_Config(void)
{

#if defined(DEBUG_GPIO)
  printf("GPIO Configuration\n");
#endif


  // make sure clock is enabled to
  CMU_ClockEnable(cmuClock_GPIO, true);

  //******************************** Fuel Gauge I/O
  /* MAX17048_INT */
#if defined(MAX17048_INT_port)
  GPIO_PinModeSet(MAX17048_INT_port, MAX17048_INT_pin, MAX17048_INT_mode, MAX17048_INT_init_state);

#endif
  //I2C pins are already configured automatically by the component


}

/*******************************************************************************
 * Function name:
 *
 * Description  : Function is blocking.
 * Parameters   :
 * Returns      :
 *
 * Known issues :
 * Note         :
 ******************************************************************************/
void GPIO_D_DeConfig(void)
{

#if defined(DEBUG_GPIO)
  printf("GPIO De-Configuration\n");
#endif


  // make sure clock is enabled to
  CMU_ClockEnable(cmuClock_GPIO, true);

  //******************************** Fuel Gauge I/O
  /* MAX17048_INT */
#if defined(MAX17048_INT_port)
  GPIO_PinModeSet(MAX17048_INT_port, MAX17048_INT_pin, gpioModeDisabled, 0);

#endif


  // Disable the clock
  CMU_ClockEnable(cmuClock_GPIO, false);
}

/*******************************************************************************
 * Function name:
 *
 * Description  : Read GPIO pin, when pin is high true will return else false
 * Parameters   :
 * Returns      :
 *
 * Known issues :
 * Note         :
 ******************************************************************************/
bool GPIO_D_Read(GPIO_Port_TypeDef port, unsigned int pin)
{
  unsigned int val = GPIO_PinInGet(port, pin);
  return val ? true : false;
}

/*******************************************************************************
 * Function name:
 *
 * Description  : Function is blocking.
 * Parameters   :
 * Returns      :
 *
 * Known issues :
 * Note         :
 ******************************************************************************/
void GPIO_D_Set(GPIO_Port_TypeDef port, unsigned int pin)
{
  GPIO_PinOutSet(port, pin);
}

/*******************************************************************************
 * Function name:
 *
 * Description  : Function is blocking.
 * Parameters   :
 * Returns      :
 *
 * Known issues :
 * Note         :
 ******************************************************************************/
void GPIO_D_Clear(GPIO_Port_TypeDef port, unsigned int pin)
{
  GPIO_PinOutClear(port, pin);
}
