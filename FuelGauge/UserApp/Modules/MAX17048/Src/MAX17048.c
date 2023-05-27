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
#include "MAX17048.h"

/*******************************************************************************
 * Data types
 ******************************************************************************/

MAX17048_Handle_s MAX17048_Handle = {
    .IsConnected = false,
    .batSOC = 0.0,
    .batVoltage = 0.0,

};

/*******************************************************************************
 * Extern
 ******************************************************************************/

/*******************************************************************************
 * Interface Functions
 ******************************************************************************/
/******************************************************************************
 * Function name: MAX17048_IsConnected
 *
 * Description  : Pings IC and reads the ID stored from factory.
 * Parameters   : [in] None.
 * Returns      : [out] RetVal_t
 *
 * Known issues :
 * Note         :
 *
 * ****************************************************************************/
RetVal_t MAX17048_IsConnected(void)
{
  RetVal_t funcRet = FuncSuccess, ret;

  uint8_t TxBuf[1] = {MAX17048_VERSION_REG};
  uint8_t RxBuf[2] = {0};

  ret = I2C_D_WriteRead(MAX17048_SAddr, TxBuf, 1, RxBuf, 2);
  if(ret == FuncSuccess)
    {
      MAX17048_Handle.IsConnected = true;
      uint16_t version = ((uint16_t)RxBuf[0] << 8) | ((uint16_t)RxBuf[1]);

#ifdef DEBUG_MAX17048
      printf("MAX17048 IsConnected: device ID %ld\n", version);
#endif
      funcRet =  FuncSuccess;
    }

  else
    {
#ifdef DEBUG_MAX17048
      printf("MAX17048 IsConnected: not connected\n");
#endif
      funcRet =  FuncFailed;
    }
  return funcRet;
}

/******************************************************************************
 * Function name:
 *
 * Description  :
 * Parameters   :
 * Returns      :
 *
 * Known issues : Does not check if the device is connected. or handle this kind of issue
 * Note         :
 *
 * ****************************************************************************/
RetVal_t MAX17048_Config(void)
{
  RetVal_t funcRet = FuncSuccess, ret;

  ///// Calculate Voltage alerts thresholds
  float valrtMAX = BATTERY_MIN_VAL / MAX17048_VALRT_SCALE;
  float valrtMIN = BATTERY_MAX_VAL / MAX17048_VALRT_SCALE;
  //Check values
  if(IS_UINT8(valrtMAX) != 1)
    {
      valrtMAX = 255.0;
    }

  if(IS_UINT8(valrtMIN) != 1)
    {
      valrtMIN = 0.0;
    }
  uint8_t VALRT_MIN = (uint8_t)valrtMIN; //MSB Byte
  uint8_t VALRT_MAX = (uint8_t)valrtMAX; //LSB Byte


  ///// Calculate empty alert threshold
  uint8_t ATHD = 0x20 - (BATTERY_EMPTY_THRESHOLD_ALERT_PERCENTAGE & 0b00011111); //masking the limits of the IC

  printf("%f  %f\n",valrtMAX, valrtMIN);
  return funcRet;
}


/******************************************************************************
 * Function name: MAX17048_GetBat_StateOfCharge
 *
 * Description  : reads battery state of charge
 * Parameters   : [in] None.
 * Returns      : [out] RetVal_t
 *
 * Known issues : does not handle errors if the IC is not responding and does not give back response of that kind of error
 * Note         :
 *
 * ****************************************************************************/
RetVal_t MAX17048_GetBat_StateOfCharge(void)
{
  RetVal_t funcRet = FuncSuccess, ret;

  uint8_t TxBuf[1] = {MAX17048_SOC_REG};
  uint8_t RxBuf[2] = {0};

  ret = I2C_D_WriteRead(MAX17048_SAddr, TxBuf, 1, RxBuf, 2);
  if(ret == FuncSuccess)
    {
      float batSOC = (float)RxBuf[0] + (float)RxBuf[1]/256.0;
      MAX17048_Handle.batSOC = batSOC;

#ifdef DEBUG_MAX17048
      printf("MAX17048 battery state of charge: SOC %f\n", batSOC);
#endif
      funcRet =  FuncSuccess;
    }

  else
    {
      MAX17048_Handle.batSOC = 0.0;
#ifdef DEBUG_MAX17048
      printf("MAX17048 battery state of charge: Error\n");
#endif
      funcRet =  FuncFailed;
    }


  return funcRet;
}


/******************************************************************************
 * Function name: MAX17048_GetBat_StateOfCharge
 *
 * Description  : reads battery cell voltage
 * Parameters   : [in] None.
 * Returns      : [out] RetVal_t
 *
 * Known issues : does not handle errors if the IC is not responding and does not give back response of that kind of error
 * Note         :
 *
 * ****************************************************************************/
RetVal_t MAX17048_GetBat_CellVoltage(void)
{
  RetVal_t funcRet = FuncSuccess, ret;

  uint8_t TxBuf[1] = {MAX17048_VCELL_REG};
  uint8_t RxBuf[2] = {0};

  ret = I2C_D_WriteRead(MAX17048_SAddr, TxBuf, 1, RxBuf, 2);
  if(ret == FuncSuccess)
    {
      float batVCELL = (float)(((uint16_t)RxBuf[0] << 8) + (uint16_t)RxBuf[1]) * MAX17048_VCELL_SCALE;
      MAX17048_Handle.batVoltage = batVCELL;

#ifdef DEBUG_MAX17048
      printf("MAX17048 Battery Cell Voltage: Voltage %f\n", batVCELL);
#endif
      funcRet =  FuncSuccess;
    }

  else
    {
      MAX17048_Handle.batVoltage = 0.0;
#ifdef DEBUG_MAX17048
      printf("MAX17048 Battery Cell Voltage: Error\n");
#endif
      funcRet =  FuncFailed;
    }


  return funcRet;
}

/******************************************************************************
 * Function name: MAX17048_GetAlerts
 *
 * Description  : Reads alerts from the status registers and decides what to do.
 * Parameters   : [in] None.
 * Returns      : [out] RetVal_t
 *
 * Known issues :
 * Note         :
 *
 * ****************************************************************************/
RetVal_t MAX17048_GetAlerts(void)
{
  RetVal_t ret;
  uint16_t statusReg = 0;

  uint8_t TxBuf[3] = {MAX17048_STATUS_REG};
  uint8_t RxBuf[2] = {0};

  //Get the default values
  ret = I2C_D_WriteRead(MAX17048_SAddr, TxBuf, 1, RxBuf, 2);
  if(ret != FuncSuccess)
    {
#ifdef DEBUG_MAX17048
      printf("MAX17048 Get alerts: Error reading alerts register\n");
#endif
      return FuncFailed;
    }

  statusReg = (uint16_t)(RxBuf[0] & (~0x3F)) << 8;
  ret = MAX17048_AlertsHandler(statusReg);


  return ret;
}


/******************************************************************************
 * Function name: MAX17048_AlertsHandler_GetAlerts
 *
 * Description  : Alerts handler
 * Parameters   : [in] uint16_t alerts where alerts are positions exactly as it
 *                     is from the memory from the IC.
 * Returns      : [out] RetVal_t
 *
 * Known issues :
 * Note         :
 *
 * ****************************************************************************/
RetVal_t MAX17048_AlertsHandler(uint16_t alert)
{
  switch(alert)
  {
    case 0:
#ifdef DEBUG_MAX17048
      printf("MAX17048 Get alerts: No Alerts\n");
#endif
      break;

    case MAX17048_STATUS_RI_bit:
#ifdef DEBUG_MAX17048
      printf("MAX17048 Get alerts: RI, Reset Indicator\n");
#endif
      break;

    case MAX17048_STATUS_VR_bit:
#ifdef DEBUG_MAX17048
      printf("MAX17048 Get alerts: VR, Voltage Reset\n");
#endif
      break;

    case MAX17048_STATUS_VH_bit:
#ifdef DEBUG_MAX17048
      printf("MAX17048 Get alerts: VH, voltage High\n");
#endif
      break;

    case MAX17048_STATUS_VL_bit:
#ifdef DEBUG_MAX17048
      printf("MAX17048 Get alerts: VL, Voltage Low\n");
#endif
      break;

    case MAX17048_STATUS_HD_bit:
#ifdef DEBUG_MAX17048
      printf("MAX17048 Get alerts: HD, SOC is low and reached CONFIG.ATHD\n");
#endif
      break;

    case MAX17048_STATUS_SC_bit:
#ifdef DEBUG_MAX17048
      printf("MAX17048 Get alerts: SC, voltage changed by what is configured at CONFIG.ALSC\n");
#endif
      break;
  }

  return FuncSuccess;
}
/******************************************************************************
 * Function name: MAX17048_ResetAlerts
 *
 * Description  : Resets the alerts bit in the configuration register.
 * Parameters   : [in] None.
 * Returns      : [out] RetVal_t
 *
 * Known issues :
 * Note         :
 *
 * ****************************************************************************/
RetVal_t MAX17048_ResetAlerts(void)
{
  RetVal_t ret;

  uint8_t TxBuf[3] = {MAX17048_CONFIG_REG, 0};
  uint8_t RxBuf[2] = {0};

  //Get the default values
  ret = I2C_D_WriteRead(MAX17048_SAddr, TxBuf, 1, RxBuf, 2);
  if(ret != FuncSuccess)
    {
#ifdef DEBUG_MAX17048
      printf("MAX17048 Reset Alert: Error reading default configuration values\n");
#endif
      return FuncFailed;

    }

  //write the new values
  TxBuf[0] = MAX17048_CONFIG_REG;
  TxBuf[1] = RxBuf[0];
  TxBuf[2] = RxBuf[1] & ~MAX17048_CONFIG_ALRT_bit;
  ret = I2C_D_Write(MAX17048_SAddr, TxBuf, 3);
  if(ret != FuncSuccess)
    {
#ifdef DEBUG_MAX17048
      printf("MAX17048 Reset Alert: Error setting new configuration values\n");
#endif
      return FuncFailed;

    }

#ifdef DEBUG_MAX17048
      printf("MAX17048 Reset Alert: Alerts have been reset\n");
#endif

  return FuncSuccess;
}
/*******************************************************************************
 * Private Functions
 ******************************************************************************/
/******************************************************************************
 * Function name:
 *
 * Description :
 * Parameters   :
 * Returns      :
 *
 * Known issues :
 * Note         :
 *
 * ****************************************************************************/

