/***************************************************************************//**
 * @file
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
/******************************************************************************
 * File Name.c
 *
 *  Created on: May 27, 2023
 *  Author: Anis Shakkour
 *  Email:  anis.shakkour399@gmail.com
 ******************************************************************************/

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "app.h"

/*******************************************************************************
 * Data types
 ******************************************************************************/
volatile uint32_t sysTick = 0;

/*******************************************************************************
 * Extern
 ******************************************************************************/

/*******************************************************************************
 * Interface Functions
 ******************************************************************************/
/*******************************************************************************
 * Function name:
 *
 * Description  :
 * Parameters   :
 * Returns      :
 *
 * Known issues :
 * Note         :
 ******************************************************************************/
void app_init(void)
{
  printf("\nFuelGauge (MAX17048) project\n");
  while(SysTick_Config(SystemCoreClock / 1000) != 0);
  GPIO_D_Config();
  MAX17048_Config();

}

/*******************************************************************************
 * Function name:
 *
 * Description  :
 * Parameters   :
 * Returns      :
 *
 * Known issues :
 * Note         :
 ******************************************************************************/
void app_process_action(void)
{
  RetVal_t FuncRet;

  FuncRet = MAX17084_MonitorSOC();
  FuncRet = MAX17084_MonitorAlerts();

}

/*******************************************************************************
 * Private Functions
 ******************************************************************************/
/*******************************************************************************
 * Function name:
 *
 * Description  :
 * Parameters   :
 * Returns      :
 *
 * Known issues :
 * Note         :
 ******************************************************************************/
void SysTick_Handler(void)
{

  sysTick++;
  if(!sysTick) //to disable Zero-devision
    {
      sysTick++;
    }
}

