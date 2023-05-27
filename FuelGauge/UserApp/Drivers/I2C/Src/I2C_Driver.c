/*******************************************************************************
 * File I2C_Driver.c
 *
 *  Created on: July 9, 2022
 *  Author: Anis Shakkour
 *  Email:  anis.shakkour399@gmail.com
 ******************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "I2C_Driver.h"

/******************************************************************************
 * Data types
 *****************************************************************************/

/******************************************************************************
 * Extern
 *****************************************************************************/

/******************************************************************************
 * Private Function Prototypes
 *****************************************************************************/

/*******************************************************************************
 * Function name: I2C_D_Config
 *
 * Description  :
 * Parameters   :
 * Returns      :
 *
 * Known issues :
 * Note         :
 ******************************************************************************/
RetVal_t I2C_D_Config(void)
{
  sl_i2cspm_init_instances();
  return FuncSuccess;
}

/*******************************************************************************
 * Function name: I2C_D_Write
 *
 * Description  :
 * Parameters   :
 * Returns      :
 *
 * Known issues :
 * Note         :
 ******************************************************************************/
RetVal_t I2C_D_Write(uint16_t Slave_Add, uint8_t* TxBuf, uint8_t TxLen)
{
  I2C_TransferReturn_TypeDef ret;
  I2C_TransferSeq_TypeDef seq;

  seq.addr = Slave_Add;
  seq.flags = I2C_FLAG_WRITE;
  seq.buf[I2C_TRANSMIT_INDEX].data = TxBuf;
  seq.buf[I2C_TRANSMIT_INDEX].len = TxLen;

  ret = I2CSPM_Transfer(I2C_HANDLE, &seq);

#ifdef DEBUG_I2C_TRANSFER_RETURN
  I2C_D_Debug_Ecode(ret);
#endif

  if(ret == i2cTransferDone)
    return FuncSuccess;
  return FuncFailed;
}

/*******************************************************************************
 * Function name: I2C_D_WriteRead
 *
 * Description  :
 * Parameters   :
 * Returns      :
 *
 * Known issues :
 * Note         :
 ******************************************************************************/
RetVal_t I2C_D_WriteRead(uint16_t Slave_Add, uint8_t* TxBuf, uint8_t TxLen, uint8_t* RxBuf, uint8_t RxLen)
{
  I2C_TransferReturn_TypeDef ret;
  I2C_TransferSeq_TypeDef seq;

  seq.addr = Slave_Add;
  seq.flags = I2C_FLAG_WRITE_READ;
  seq.buf[I2C_TRANSMIT_INDEX].data = TxBuf;
  seq.buf[I2C_TRANSMIT_INDEX].len  = TxLen;
  seq.buf[I2C_RECEIVE_INDEX].data  = RxBuf;
  seq.buf[I2C_RECEIVE_INDEX].len   = RxLen;

  ret = I2CSPM_Transfer(I2C_HANDLE, &seq);

#ifdef DEBUG_I2C_TRANSFER_RETURN
  I2C_D_Debug_Ecode(ret);
#endif

  if(ret == i2cTransferDone)
    return FuncSuccess;
  return FuncFailed;
}

/*****************************************************************************
 * Function name: UART_D_Debug_Ecode
 *
 * Description: All ongoing or queued operations of the given abort type will be aborted
 *
 *
 * Parameters:  None.
 * Returns:     Abort type – whether to abort only Tx, only Rx, or both.
 ******************************************************************************/
void I2C_D_Debug_Ecode(I2C_TransferReturn_TypeDef Code)
{

  switch (Code)
  {
    case i2cTransferInProgress:
      printf("I2C DEBUG ECODE: Transfer in progress\n");
      break;

    case i2cTransferDone:
      printf("I2C DEBUG ECODE: Transfer done\n");
      break;

    case i2cTransferNack:
      printf("I2C DEBUG ECODE: Transfer NACK\n");
      break;

    case i2cTransferBusErr:
      printf("I2C DEBUG ECODE: Transfer bus error\n");
      break;

    case i2cTransferArbLost:
      printf("I2C DEBUG ECODE: Transfer ARB lost\n");
      break;

    case i2cTransferUsageFault:
      printf("I2C DEBUG ECODE: Transfer usage fault\n");
      break;

    default:
      printf("I2C DEBUG ECODE: UnKnown \n");
      break;
  }
}
