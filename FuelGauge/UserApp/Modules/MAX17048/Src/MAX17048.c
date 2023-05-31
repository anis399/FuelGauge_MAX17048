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

#if defined(DEBUG_MAX17048)
  printf("MAX17048 IsConnected\n");
#endif

  RetVal_t funcRet = FuncSuccess, ret;

  uint8_t TxBuf[1] = {MAX17048_VERSION_REG};
  uint8_t RxBuf[2] = {0};

  ret = I2C_D_WriteRead(MAX17048_SAddr, TxBuf, 1, RxBuf, 2);
  if(ret == FuncSuccess)
    {
      //MAX17048_Handle.IsConnected = true;
      uint16_t version = ((uint16_t)RxBuf[0] << 8) | ((uint16_t)RxBuf[1]);
      funcRet =  FuncSuccess;

#if defined(DEBUG_MAX17048_FUNCSUCCESS)
      printf("MAX17048 IsConnected: device ID %ld\n", version);
#endif
    }

  else
    {
      funcRet =  FuncFailed;
#if defined(DEBUG_MAX17048_FUNCSUCCESS)
      printf("MAX17048 IsConnected: not connected\n");
#endif
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
 * TODO         : clear: InConfig.EnSleep -> Disable sleep
 *
 * ****************************************************************************/
RetVal_t MAX17048_Config(void)
{

#if defined(DEBUG_MAX17048)
  printf("MAX17048 Configuration\n");
#endif


  RetVal_t funcRet = FuncSuccess;

  funcRet = MAX17048_IsConnected();
  if(funcRet == FuncFailed){
      MAX17048_Handle.IsConnected = false;
      return funcRet;
  }

  MAX17048_Handle.IsConnected = true;
  ////////////////////// Calculate Voltage alerts thresholds
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


  ///////////////////////////// Calculate empty alert threshold
  uint8_t ATHD = 0x20 - (BATTERY_EMPTY_THRESHOLD_ALERT_PERCENTAGE & 0b00011111); //masking the limits of the IC

  return funcRet;
}


/******************************************************************************
 * Function name: MAX17084_MonitorSOC
 *
 * Description  : Function reads battery voltage and state of charge in a state machine,
 *                User should ping this function in a main loop.
 *                Reading the drivers handle user may get the battery data. user may also add his special code
 *                in case '2' to update his variables 2.
 * Parameters   :
 * Returns      :
 *
 * Known issues : Does not check if the device is connected. or handle this kind of issue
 * Note         :
 *
 * TODO         : clear: InConfig.EnSleep -> Disable sleep
 *
 * ****************************************************************************/
RetVal_t MAX17084_MonitorSOC(void)
{

  RetVal_t funcRet = FuncOnGoing, ret;
  float batVCELL = 0, batSOC = 0;

  static uint8_t taskSteps = 0; //holds the SM step
  static uint32_t initTick = 0; //holds the system tick from last call


  if(MAX17048_Handle.IsConnected == false)
    {
#if defined(DEBUG_MAX17048_FUNCFAILED)
      printf("MAX17048 MonitorSOC: Sensor is not connected\n");
#endif

      /// Do more stuff to update that battery is not connected
      ///
      ///
      /// END
      return FuncFailed;
    }


  switch(taskSteps)
  {
    /********************* Idle / Sample delay *********************/
    case 0: //Idle

      funcRet = FuncOnGoing;
      if(sysTick <= (initTick + BATTERY_SOC_SAMPLE_PERIOD))
        {
          break;
        }

#if defined(DEBUG_MAX17048)
      printf("MAX17048 MonitorSOC\n");
#endif
      taskSteps++; //check battery
      break;

      /***************** Read battery State of charge ****************/
    case 1:
      ret = MAX17048_GetBat_StateOfCharge(&batSOC);
      if(ret != FuncSuccess)
        {
          taskSteps ^= taskSteps; //reset the SM
          funcRet = FuncFailed;
        }

      ret = MAX17048_GetBat_CellVoltage(&batVCELL);
      if(ret != FuncSuccess)
        {
          taskSteps ^= taskSteps; //reset the SM
          funcRet = FuncFailed;
        }

      MAX17048_Handle.batSOC = batSOC;
      MAX17048_Handle.batVoltage = batVCELL;
      funcRet = FuncOnGoing;
      taskSteps++; //next case
      break;

      /***************** Do what ever you want  ****************/
    case 2:

      /// Add your code
      ///
      ///
      ///
      /// End
      funcRet = FuncSuccess;
      taskSteps++; //next case
      break;


    default:
      taskSteps ^= taskSteps; //reset the SM
      initTick = sysTick; // reset initTick to system tick
      break;
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
 * TODO         : clear: InConfig.EnSleep -> Disable sleep
 *
 * ****************************************************************************/
RetVal_t MAX17084_MonitorAlerts(void)
{
  RetVal_t funcRet = FuncOnGoing, ret;
  uint16_t alerts;
  static uint8_t taskSteps = 0; //holds the SM step
  //static uint32_t initTick = 0; //holds the system tick from last call


  if(MAX17048_Handle.IsConnected == false)
    {
#if defined(DEBUG_MAX17048_FUNCFAILED)
      printf("MAX17048 MonitorAlerts: Sensor is not connected\n");
#endif

      /// Do more stuff to update that battery is not connected
      ///
      ///
      /// END
      return FuncFailed;
    }


  switch(taskSteps)
  {

    /********************* Check GPIO *********************/
    case 0: //Check GPIO Interrupt

      if(MAX17048_requestingINT() == false)
        {
          funcRet = FuncOnGoing;
          break;
        }

#if defined(DEBUG_MAX17048)
      printf("MAX17048 MonitorAlerts: ALERT!\n");
#endif
      taskSteps++;
      break;

      /************** Read alerts from MAX17048 *************/
    case 1:
      ret = MAX17048_GetAlerts(&alerts);
      if(ret != FuncSuccess)
        {
          taskSteps ^= taskSteps; //reset the SM
          funcRet = FuncFailed;
          break;
        }

      ret = MAX17048_AlertsHandler(alerts);
      if(ret != FuncSuccess)
        {
          taskSteps ^= taskSteps; //reset the SM
          funcRet = FuncFailed;
          break;
        }


      funcRet = FuncOnGoing;
      taskSteps++; //next case
      break;

      /***************** Reset Alerts  ****************/
    case 2:

      ret = MAX17048_ResetAlerts();
      if(ret == FuncSuccess)
        {
          taskSteps ^= taskSteps; //reset the SM
          funcRet = FuncSuccess;
          break;
        }

      //could not reset alerts, try once again
      //TODO add a timeout, max error when trying to reset alerts and it is not successful
      break;


    default:
      taskSteps ^= taskSteps; //reset the SM
      // reset initTick to system tick
      break;
  }

  return funcRet;



}











/*******************************************************************************
 * Private Functions
 ******************************************************************************/
/******************************************************************************
 * Function name: MAX17048_GetBat_StateOfCharge
 *
 * Description  : reads battery state of charge
 * Parameters   : [in] pointer to a float that will hold the output data.
 * Returns      : [out] RetVal_t
 *
 * Known issues : does not handle errors if the IC is not responding and does not give back response of that kind of error
 * Note         :
 *
 * ****************************************************************************/
static RetVal_t MAX17048_GetBat_StateOfCharge(float* batSOC)
{

#if defined(DEBUG_MAX17048)
  printf("MAX17048 battery state of charge\n");
#endif

  RetVal_t funcRet = FuncSuccess, ret;
  uint8_t TxBuf[1] = {MAX17048_SOC_REG};
  uint8_t RxBuf[2] = {0};

  ret = I2C_D_WriteRead(MAX17048_SAddr, TxBuf, 1, RxBuf, 2);
  if(ret == FuncSuccess)
    {
      *batSOC = (float)RxBuf[0] + (float)RxBuf[1]/256.0;

#if defined(DEBUG_MAX17048_FUNCSUCCESS)
      printf("MAX17048 battery state of charge: SOC %f\n", *batSOC);
#endif
      funcRet =  FuncSuccess;
    }

  else
    {
#if defined(DEBUG_MAX17048_FUNCFAILED)
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
 * Parameters   : [in] pointer to a float that will hold the output data.
 * Returns      : [out] RetVal_t
 *
 * Known issues : does not handle errors if the IC is not responding and does not give back response of that kind of error
 * Note         :
 *
 * ****************************************************************************/
static RetVal_t MAX17048_GetBat_CellVoltage(float* batVCELL)
{

#if defined(DEBUG_MAX17048)
  printf("MAX17048 Battery Cell Voltage\n");
#endif

  RetVal_t funcRet = FuncSuccess, ret;

  uint8_t TxBuf[1] = {MAX17048_VCELL_REG};
  uint8_t RxBuf[2] = {0};

  ret = I2C_D_WriteRead(MAX17048_SAddr, TxBuf, 1, RxBuf, 2);
  if(ret == FuncSuccess)
    {
      *batVCELL = (float)(((uint16_t)RxBuf[0] << 8) + (uint16_t)RxBuf[1]) * MAX17048_VCELL_SCALE;

#if defined(DEBUG_MAX17048_FUNCSUCCESS)
      printf("MAX17048 Battery Cell Voltage: Voltage %f\n", *batVCELL);
#endif
      funcRet =  FuncSuccess;
    }

  else
    {
#if defined(DEBUG_MAX17048_FUNCFAILED)
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
 * Parameters   : [in] pointer to a uint16_t that will hold the output alerts.
 * Returns      : [out] RetVal_t
 *
 * Known issues :
 * Note         :
 *
 * ****************************************************************************/
static RetVal_t MAX17048_GetAlerts(uint16_t* alerts)
{
  RetVal_t ret;
  uint16_t statusReg = 0;

  uint8_t TxBuf[3] = {MAX17048_STATUS_REG};
  uint8_t RxBuf[2] = {0};

  //Get the default values
  ret = I2C_D_WriteRead(MAX17048_SAddr, TxBuf, 1, RxBuf, 2);
  if(ret == FuncSuccess)
    {
      *alerts = (uint16_t)(RxBuf[0] & (~0x3F)) << 8; //calculate alerts the same as stored in the sensors memory
      ret = FuncSuccess;
#if defined(DEBUG_MAX17048_FUNCFAILED)
      printf("MAX17048 Get alerts: Error reading alerts register\n");
#endif
    }

  else
    {
#if defined(DEBUG_MAX17048_FUNCFAILED)
      printf("MAX17048 Get alerts: Error reading alerts register\n");
#endif
      ret = FuncFailed;
    }

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
static RetVal_t MAX17048_AlertsHandler(uint16_t alert)
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
static RetVal_t MAX17048_ResetAlerts(void)
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

/******************************************************************************
 * Function name: MAX17048_MAX17048_requestingINT
 *
 * Description  : INT is active low, and when it is high the MAX has no interrupts
 * Parameters   : [in] None.
 * Returns      : [out] bool if MAX17048 has an alert
 *
 * Known issues :
 * Note         :
 *
 * ****************************************************************************/
static bool MAX17048_requestingINT(void)
{

  bool INTVal = GPIO_D_Read(MAX17048_INT_port, MAX17048_INT_pin);
  if(INTVal == true)
    {
      //GPIO Value is set, MAX has no alerts
      return false;
    }

  //GPIO Value is set, MAX has an alerts
  return true;
}
