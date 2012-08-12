/*=============================================================================================*//**
@file    i2c.c

@author  Daniel Zorychta

@brief   This driver support I2C

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "i2c.h"


/*==================================================================================================
                                  Local symbolic constants/macros

==================================================================================================*/
#define PORT_FREE                         (u16_t)EMPTY_TASK

/** define I2C error mask */
#define I2C_ERROR_MASK_BM                 (I2C_SR1_OVR | I2C_SR1_AF | I2C_SR1_ARLO | I2C_SR1_BERR)

#define SetNACK(interface)                interface->CR1 &= ~(I2C_CR1_ACK);
#define SetACK(interface)                 interface->CR1 |= I2C_CR1_ACK;
#define SetSCLFreq(SCLFreq)               (((u32_t)I2C_PERIPHERAL_FREQ * 1000000UL) / (2UL * (u32_t)SCLFreq))
#define SetSCLTRise()                     (I2C_PERIPHERAL_FREQ + 1)


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
/** type which contain port information */
typedef struct PortHandle_struct
{
      I2C_t       *Address;             /* peripheral address */
      xTaskHandle TaskHandle;           /* task handle variable for IRQ */
      u16_t       Lock;                 /* port reservation */
      u8_t        SlaveAddress;         /* slave address */
} PortHandle_t;


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static stdRet_t StartCondition(I2C_t *i2c);
static stdRet_t SendSlaveAddress(I2C_t *i2c, u8_t slaveAddr, bool_t twr_frd);
static stdRet_t SendData(I2C_t *i2c, u8_t *src, size_t size);
static stdRet_t ReadData(I2C_t *i2c, u8_t *dst, size_t size, bool_t ackSupport);
static void     StopCondition(I2C_t *i2c);
static stdRet_t CheckStatus(I2C_t *i2c);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
/** port localizations */
static PortHandle_t PortHandle[] =
{
      #ifdef RCC_APB1ENR_I2C1EN
      #if (I2C1_ENABLE > 0)
      {
            .Address      = I2C1,
            .TaskHandle   = NULL,
            .Lock         = PORT_FREE,
            .SlaveAddress = 0,
      },
      #endif
      #endif

      #ifdef RCC_APB1ENR_I2C2EN
      #if (I2C2_ENABLE > 0)
      {
            .Address      = I2C2,
            .TaskHandle   = NULL,
            .Lock         = PORT_FREE,
            .SlaveAddress = 0,
      },
      #endif
      #endif
};


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
stdRet_t I2C_Init(void)
{
      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
stdRet_t I2C_Open(dev_t dev)
{
      stdRet_t status  = I2C_STATUS_PORTNOTEXIST;
      I2C_t    *i2cPtr = NULL;

      /* check port range */
      if ((unsigned)dev < I2C_DEV_LAST)
      {
            /* lock task switching */
            TaskSuspendAll();

            /* check that port is free */
            if (PortHandle[dev].Lock == PORT_FREE)
            {
                  /* registered port for current task */
                  PortHandle[dev].Lock = TaskGetPID();

                  TaskResumeAll();

                  /* set task handle for IRQs */
                  PortHandle[dev].TaskHandle = TaskGetCurrentTaskHandle();

                  /* enable I2C clock */
                  switch (dev)
                  {
                        #ifdef RCC_APB1ENR_I2C1EN
                        #if (I2C1_ENABLE > 0)
                        case I2C_DEV_1:
                              RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_I2C2EN
                        #if (I2C1_ENABLE > 0)
                        case I2C_DEV_2:
                              RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
                              break;
                        #endif
                        #endif

                        default:
                              break;
                  }

                  /* set port address */
                  i2cPtr = PortHandle[dev].Address;

                  /* set default port settings */
                  i2cPtr->CR1   |= I2C_CR1_SWRST;
                  i2cPtr->CR1    = 0;
                  i2cPtr->CR2    = I2C_PERIPHERAL_FREQ;
                  i2cPtr->SR1    = 0;
                  i2cPtr->CCR    = SetSCLFreq(I2C_DEFAULT_SCL_FREQ);
                  i2cPtr->TRISE  = SetSCLTRise();
                  i2cPtr->CR1   |= I2C_CR1_PE;

                  status = STD_RET_OK;
            }
            else
            {
                  TaskResumeAll();

                  if (PortHandle[dev].Lock == TaskGetPID())
                        status = STD_RET_OK;
                  else
                        status = I2C_STATUS_PORTLOCKED;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
stdRet_t I2C_Close(dev_t dev)
{
      stdRet_t status = I2C_STATUS_PORTNOTEXIST;

      /* check port range */
      if ((unsigned)dev < I2C_DEV_LAST)
      {
            /* check that port is reserved for this task */
            if (PortHandle[dev].Lock == TaskGetPID())
            {
                  /* turn off device */
                  switch (dev)
                  {
                        #ifdef RCC_APB1ENR_I2C1EN
                        #if (I2C1_ENABLE > 0)
                        case I2C_DEV_1:
                              RCC->APB1ENR &= ~RCC_APB1ENR_I2C1EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_I2C2EN
                        #if (I2C2_ENABLE > 0)
                        case I2C_DEV_2:
                              RCC->APB1ENR &= ~RCC_APB1ENR_I2C2EN;
                              break;
                        #endif
                        #endif

                        default:
                              break;
                  }

                  /* unlock device */
                  PortHandle[dev].Lock = PORT_FREE;

                  /* delete from task handle */
                  PortHandle[dev].TaskHandle = NULL;

                  status = STD_RET_OK;
            }
            else
            {
                  status = I2C_STATUS_PORTLOCKED;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Function write data to the
 */
//================================================================================================//
stdRet_t I2C_Write(dev_t dev, void *src, size_t size, size_t seek)
{
      stdRet_t status = I2C_STATUS_PORTNOTEXIST;

      /* check port range */
      if ((unsigned)dev < I2C_DEV_LAST)
      {
            /* check arguments */
            if (!src || !size)
            {
                  status = I2C_STATUS_BADARG;
                  goto I2C_Write_end;
            }

            /* check that port is reserved for this task */
            if (PortHandle[dev].Lock == TaskGetPID())
            {
                  I2C_t *i2cPtr = PortHandle[dev].Address;
                  u8_t  *data   = (u8_t *)src;

                  /* start condition */
                  if ((status = StartCondition(i2cPtr)) != STD_RET_OK)
                        goto I2C_Write_end;

                  /* send slave address */
                  if ((status = SendSlaveAddress(i2cPtr,
                                                 PortHandle[dev].SlaveAddress,
                                                 TRUE)) != STD_RET_OK)
                        goto I2C_Write_end;

                  /* register address */
                  if ((status = SendData(i2cPtr, (u8_t*)&seek, 1)) != STD_RET_OK)
                        goto I2C_Write_end;

                  /* send data */
                  if ((status = SendData(i2cPtr, data, size)) != STD_RET_OK)
                        goto I2C_Write_end;

                  /* stop condition */
                  StopCondition(i2cPtr);
            }
            else
            {
                  status = I2C_STATUS_PORTLOCKED;
            }
      }

      I2C_Write_end:
            return status;
}


//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
stdRet_t I2C_Read(dev_t dev, void *dst, size_t size, size_t seek)
{
      stdRet_t status = I2C_STATUS_PORTNOTEXIST;

      /* check port range */
      if ((unsigned)dev < I2C_DEV_LAST)
      {
            /* check arguments */
            if (!dst || !size)
            {
                  status = I2C_STATUS_BADARG;
                  goto I2C_Read_end;
            }

            /* check that port is reserved for this task */
            if (PortHandle[dev].Lock == TaskGetPID())
            {
                  I2C_t *i2cPtr = PortHandle[dev].Address;
                  u8_t  *data   = (u8_t *)dst;

                  /* start condition */
                  if ((status = StartCondition(i2cPtr)) != STD_RET_OK)
                        goto I2C_Read_end;

                  /* send slave address */
                  if ((status = SendSlaveAddress(i2cPtr,
                                                 PortHandle[dev].SlaveAddress,
                                                 TRUE)) != STD_RET_OK)
                        goto I2C_Read_end;

                  /* register address */
                  if ((status = SendData(i2cPtr, (u8_t*)&seek, 1)) != STD_RET_OK)
                        goto I2C_Read_end;

                  /* repeat start condition */
                  if ((status = StartCondition(i2cPtr)) != STD_RET_OK)
                        goto I2C_Read_end;

                  /* check if is only 1 byte to read */
                  if (size == 1)
                        SetNACK(i2cPtr);

                  /* send slave address */
                  if ((status = SendSlaveAddress(i2cPtr,
                                                 PortHandle[dev].SlaveAddress,
                                                 FALSE)) != STD_RET_OK)
                        goto I2C_Read_end;

                  /* read data */
                  if ((status = ReadData(i2cPtr, data, size, TRUE)) != STD_RET_OK)
                        goto I2C_Read_end;

                  /* stop condition */
                  StopCondition(i2cPtr);
            }
            else
            {
                  status = I2C_STATUS_PORTLOCKED;
            }
      }

      I2C_Read_end:
            return status;
}


//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
stdRet_t I2C_IOCtl(dev_t dev, IORq_t ioRQ, void *data)
{
      stdRet_t status = I2C_STATUS_PORTNOTEXIST;

      /* check port range */
      if ((unsigned)dev < I2C_DEV_LAST)
      {
            /* check that port is reserved for this task */
            if (PortHandle[dev].Lock == TaskGetPID())
            {
                  I2C_t *i2cPtr = PortHandle[dev].Address;

                  status = STD_RET_OK;

                  switch (ioRQ)
                  {
                        case I2C_IORQ_SETSLAVEADDR:
                        {
                              PortHandle[dev].SlaveAddress = *((u8_t *)data);
                              break;
                        }

                        case I2C_IORQ_GETSLAVEADDR:
                        {
                              *((u8_t *)data) = PortHandle[dev].SlaveAddress;
                              break;
                        }

                        case I2C_IORQ_SETSCLFREQ:
                        {
                              i2cPtr->CCR = SetSCLFreq(*((u32_t *)data));
                              break;
                        }

                        default:
                        {
                              status = I2C_STATUS_BADRQ;
                              break;
                        }
                  }
            }
            else
            {
                  status = I2C_STATUS_PORTLOCKED;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Generate start condition
 *
 * @param *i2c          i2c device
 *
 * @retval STATUS_OK                  start condition generated successfully
 * @retval STATUS_TIMEOUT             timeout occur
 * @retval STATUS_OVERRUN             overrun
 * @retval STATUS_ACK_FAILURE         ack failure
 * @retval STATUS_ARB_LOST            arbitration lost error
 * @retval STATUS_BUS_ERROR           bus error
 * @retval STATUS_ERROR               more than 1 error
 */
//================================================================================================//
static stdRet_t StartCondition(I2C_t *i2c)
{
      u8_t timeout;

      /* generate start condition */
      i2c->CR1 |= I2C_CR1_START;

      /* waiting for start condition was generated */
      for (timeout = 150; timeout > 0; timeout--)
      {
            if (i2c->SR1 & I2C_SR1_SB)
                  break;
            else
                  TaskDelay(1);
      }

      /* check status */
      if (!timeout)
            return I2C_STATUS_TIMEOUT;
      else
            return CheckStatus(i2c);
}


//================================================================================================//
/**
 * @brief Function set slave address
 *
 * @param *i2c          i2c device
 * @param slaveAddr     slave address
 * @param twr_frd       write (TRUE), read (FALSE) mode
 *
 * @retval STATUS_OK                  start condition generated successfully
 * @retval STATUS_TIMEOUT             timeout occur
 * @retval STATUS_OVERRUN             overrun
 * @retval STATUS_ACK_FAILURE         ack failure
 * @retval STATUS_ARB_LOST            arbitration lost error
 * @retval STATUS_BUS_ERROR           bus error
 * @retval STATUS_ERROR               more than 1 error
 */
//================================================================================================//
static stdRet_t SendSlaveAddress(I2C_t *i2c, u8_t slaveAddr, bool_t twr_frd)
{
      u8_t timeout;

      /* set correct address */
      if (twr_frd)
            i2c->DR = (slaveAddr << 1) & 0xFE;
      else
            i2c->DR = (slaveAddr << 1) | 0x01;

      /* waiting for transmit */
      for (timeout = 100; timeout > 0; timeout--)
      {
            if (i2c->SR1 & I2C_SR1_ADDR)
                  break;
            else
                  TaskDelay(10);
      }

      /* check status */
      if (!timeout)
            return I2C_STATUS_TIMEOUT;
      else
            return CheckStatus(i2c);
}


//================================================================================================//
/**
 * @brief Function send data to the slave
 *
 * @param *i2c          i2c device
 * @param *src          data source
 * @param size          data size
 *
 * @retval STATUS_OK                  start condition generated successfully
 * @retval STATUS_TIMEOUT             timeout occur
 * @retval STATUS_OVERRUN             overrun
 * @retval STATUS_ACK_FAILURE         ack failure
 * @retval STATUS_ARB_LOST            arbitration lost error
 * @retval STATUS_BUS_ERROR           bus error
 * @retval STATUS_ERROR               more than 1 error
 */
//================================================================================================//
static stdRet_t SendData(I2C_t *i2c, u8_t *src, size_t size)
{
      stdRet_t status = STD_RET_OK;

      if (!size)
      {
            status = I2C_STATUS_BADARG;
            goto SendData_end;
      }

      /* send buffer */
      do
      {
            u8_t timeout;

            for (timeout = 200; timeout > 0; timeout--)
            {
                  if (i2c->SR1 & I2C_SR1_TXE)
                        break;
                  else
                        TaskDelay(1);
            }

            /* check errors */
            if (!timeout)
            {
                  status = I2C_STATUS_TIMEOUT;
                  goto SendData_end;
            }

            if ((status = CheckStatus(i2c)) == STD_RET_OK)
                  i2c->DR = *(src++);
            else
                  goto SendData_end;
      }
      while (--size);

      /* wait for finish send operation */
      while (!(i2c->SR1 & I2C_SR1_BTF) || !(i2c->SR1 & I2C_SR1_TXE))
      {
            TaskDelay(1);
      }

      SendData_end:
            return status;
}


//================================================================================================//
/**
 * @brief Read data from slave
 */
//================================================================================================//
static stdRet_t ReadData(I2C_t *i2c, u8_t *dst, size_t size, bool_t ackSupport)
{
      stdRet_t status = STD_RET_OK;

      if (!size)
      {
            status = I2C_STATUS_BADARG;
            goto ReadData_end;
      }

      /* send buffer */
      while (size--)
      {
            u8_t timeout;

            /* waiting for byte receive */
            for (timeout = 200; timeout > 0; timeout--)
            {
                  if (i2c->SR1 & I2C_SR1_RXNE)
                        break;
                  else
                        TaskDelay(1);
            }

            /* check timeout */
            if (!timeout)
            {
                  status = I2C_STATUS_TIMEOUT;
                  goto ReadData_end;
            }

            if ((status = CheckStatus(i2c)) != STD_RET_OK)
                  goto ReadData_end;

            /* if 2 byte after buffer end set NACK */
            if (ackSupport && size == 2)
                  SetNACK(i2c);

            /* read byte from DR */
            *(dst++) = i2c->DR;
      }

      ReadData_end:
            return status;
}


//================================================================================================//
/**
 * @brief Function send stop condition
 *
 * @param *i2c          i2c device
 */
//================================================================================================//
static void StopCondition(I2C_t *i2c)
{
      i2c->CR1 |= I2C_CR1_STOP;
}


//================================================================================================//
/**
 * @brief Function check I2C errors
 *
 * @param *i2c          i2c device
 *
 * @retval STATUS_OK                  no error
 * @retval STATUS_OVERRUN             overrun
 * @retval STATUS_ACK_FAILURE         ack failure
 * @retval STATUS_ARB_LOST            arbitration lost error
 * @retval STATUS_BUS_ERROR           bus error
 * @retval STATUS_ERROR               more than 1 error
 */
//================================================================================================//
static stdRet_t CheckStatus(I2C_t *i2c)
{
      stdRet_t status = STD_RET_OK;

      switch (i2c->SR1 & I2C_ERROR_MASK_BM)
      {
         case I2C_SR1_OVR:  status = I2C_STATUS_OVERRUN;     break;
         case I2C_SR1_AF:   status = I2C_STATUS_ACK_FAILURE; break;
         case I2C_SR1_ARLO: status = I2C_STATUS_ARB_LOST;    break;
         case I2C_SR1_BERR: status = I2C_STATUS_BUS_ERROR;   break;
         default:           status = I2C_STATUS_ERROR;       break;
      }

      /* when error occur generate stop condition */
      if (status != STD_RET_OK)
            StopCondition(i2c);

      return status;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
