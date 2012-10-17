/*=============================================================================================*//**
@file    usart.c

@author  Daniel Zorychta

@brief   This file support USART peripherals

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


/*
 * INFO:
 * - driver don't support DMA (future support)
 */

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "uart.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define PORT_FREE                   (u16_t)EMPTY_TASK

/** UART wake method: idle line (0) or address mark (1) */
#define SetAddressMarkWakeMethod(enable)        \
      if (enable)                               \
            usartPtr->CR1 |= USART_CR1_WAKE;    \
      else                                      \
            usartPtr->CR1 &= ~USART_CR1_WAKE


/** parity enable (1) or disable (0) */
#define ParityCheckEnable(enable)               \
      if (enable)                               \
            usartPtr->CR1 |= USART_CR1_PCE;     \
      else                                      \
            usartPtr->CR1 &= ~USART_CR1_PCE


/** even parity (0) or odd parity (1) */
#define SetOddParity(enable)                    \
      if (enable)                               \
            usartPtr->CR1 |= USART_CR1_PS;      \
      else                                      \
            usartPtr->CR1 &= ~USART_CR1_PS


/** disable (0) or enable (1) UART transmitter */
#define TransmitterEnable(enable)               \
      if (enable)                               \
            usartPtr->CR1 |= USART_CR1_TE;      \
      else                                      \
            usartPtr->CR1 &= ~USART_CR1_TE


/** disable (0) or enable (1) UART receiver */
#define ReceiverEnable(enable)                  \
      if (enable)                               \
            usartPtr->CR1 |= USART_CR1_RE;      \
      else                                      \
            usartPtr->CR1 &= ~USART_CR1_RE


/** receiver wakeup: active mode (0) or mute mode (1) */
#define ReceiverWakeupMuteEnable(enable)        \
      if (enable)                               \
            usartPtr->CR1 |= USART_CR1_RWU;     \
      else                                      \
            usartPtr->CR1 &= ~USART_CR1_RWU


/** LIN mode disable (0) or enable (1) */
#define LINModeEnable(enable)                   \
      if (enable)                               \
            usartPtr->CR2 |= USART_CR2_LINEN;   \
      else                                      \
            usartPtr->CR2 &= ~USART_CR2_LINEN


/** 1 stop bit (0) or 2 stop bits (1) */
#define Set2StopBits(enable)                    \
      if (enable)                               \
      {                                         \
            usartPtr->CR2 &= USART_CR2_STOP;    \
            usartPtr->CR2 |= USART_CR2_STOP_1;  \
      }                                         \
      else                                      \
            usartPtr->CR2 &= USART_CR2_STOP


/** LIN break detector length: 10 bits (0) or 11 bits (1) */
#define LINBreakDet11Bits(enable)               \
      if (enable)                               \
            usartPtr->CR2 |= USART_CR2_LBDL;    \
      else                                      \
            usartPtr->CR2 &= ~USART_CR2_LBDL


/** address of the USART node (in the multiprocessor mode), 4-bit length */
#define SetAddressNode(adr)                     \
      usartPtr->CR2 &= ~USART_CR2_ADD;          \
      usartPtr->CR2 |= (adr & USART_CR2_ADD)    \


/** baud rate */
#define SetBaudRate(clk, baud)                  \
      usartPtr->BRR = (u16_t)((clk / baud) + 1)


/** CTS hardware flow control enable (1) or disable (0) */
#define CTSEnable(enable)                       \
      if (enable)                               \
            usartPtr->CR3 |= USART_CR3_CTSE;    \
      else                                      \
            usartPtr->CR3 &= ~USART_CR3_CTSE


/** RTS hardware flow control enable (1) or disable (0) */
#define RTSEnable(enable)                       \
      if (enable)                               \
            usartPtr->CR3 |= USART_CR3_RTSE;    \
      else                                      \
            usartPtr->CR3 &= ~USART_CR3_RTSE


/** enable UART */
#define UARTEnable()                            \
      usartPtr->CR1 |= USART_CR1_UE


/** disable UART */
#define UARTDisable()                           \
      usartPtr->CR1 &= ~UART_CR1_UE1


/** enable RX interrupt */
#define EnableRxIRQ()                           \
      usartPtr->CR1 |= USART_CR1_RXNEIE


/** enable RX interrupt */
#define DisableRxIRQ()                          \
      usartPtr->CR1 &= ~UART_CR1_RXNEIE


/** enable TXE interrupt */
#define EnableTXEIRQ()                          \
      usartPtr->CR1 |= USART_CR1_TXEIE


/** disable TXE interrupt */
#define DisableTXEIRQ()                         \
      usartPtr->CR1 &= ~USART_CR1_TXEIE


/** interrupt definition */
#define IRQCode(usartBase, devName)                                                             \
      if (usartBase->SR & USART_SR_RXNE)                                                        \
      {                                                                                         \
            RxFIFO_t *RxFIFO = &PortHandle[devName].RxFIFO;                                     \
                                                                                                \
            u8_t DR = usartBase->DR;                                                            \
                                                                                                \
            if (RxFIFO->Buffer)                                                                 \
            {                                                                                   \
                  if (RxFIFO->Level < UART_RX_BUFFER_SIZE)                                      \
                  {                                                                             \
                        RxFIFO->Buffer[RxFIFO->TxIdx++] = DR;                                   \
                                                                                                \
                        if (RxFIFO->TxIdx >= UART_RX_BUFFER_SIZE)                               \
                              RxFIFO->TxIdx = 0;                                                \
                                                                                                \
                        RxFIFO->Level++;                                                        \
                  }                                                                             \
                                                                                                \
                  if (PortHandle[devName].TaskHandle)                                           \
                  {                                                                             \
                        if (TaskResumeFromISR(PortHandle[devName].TaskHandle) == pdTRUE)        \
                              TaskYield();                                                      \
                  }                                                                             \
            }                                                                                   \
      }


/** IRQ priorities */
#define IRQ_PRIORITY          0xDF


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
/** Rx FIFO type */
typedef struct RxFIFO_struct
{
      u8_t  *Buffer;
      u16_t Level;
      u16_t RxIdx;
      u16_t TxIdx;
} RxFIFO_t;


/** type which contains tx buffer address and size */
typedef struct TxBuffer_struct
{
      u8_t   *TxSrcPtr;
      size_t Size;
} TxBuffer_t;


/** type which contain port information */
typedef struct PortHandle_struct
{
      USART_t     *Address;             /* peripheral address */
      RxFIFO_t    RxFIFO;               /* Rx FIFO for IRQ */
      TxBuffer_t  TxBuffer;             /* Tx Buffer for IRQ */
      xTaskHandle TaskHandle;           /* task handle variable for IRQ */
      u16_t       Lock;                 /* port reservation */
} PortHandle_t;


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
/** port localizations */
static PortHandle_t PortHandle[] =
{
      #ifdef RCC_APB2ENR_USART1EN
      #if (UART_1_ENABLE > 0)
      {
            .Address    = USART1,
            .RxFIFO     = {NULL, 0, 0, 0},
            .TxBuffer   = {NULL, 0},
            .TaskHandle = NULL,
            .Lock       = PORT_FREE
      },
      #endif
      #endif

      #ifdef RCC_APB1ENR_USART2EN
      #if (UART_2_ENABLE > 0)
      {
            .Address    = USART2,
            .RxFIFO     = {NULL, 0, 0, 0},
            .TxBuffer   = {NULL, 0},
            .TaskHandle = NULL,
            .Lock       = PORT_FREE
      },
      #endif
      #endif

      #ifdef RCC_APB1ENR_USART3EN
      #if (UART_3_ENABLE > 0)
      {
            .Address    = USART3,
            .RxFIFO     = {NULL, 0, 0, 0},
            .TxBuffer   = {NULL, 0},
            .TaskHandle = NULL,
            .Lock       = PORT_FREE
      },
      #endif
      #endif

      #ifdef RCC_APB1ENR_UART4EN
      #if (UART_4_ENABLE > 0)
      {
            .Address    = UART4,
            .RxFIFO     = {NULL, 0, 0, 0},
            .TxBuffer   = {NULL, 0},
            .TaskHandle = NULL,
            .Lock       = PORT_FREE
      },
      #endif
      #endif

      #ifdef RCC_APB1ENR_UART5EN
      #if (UART_5_ENABLE > 0)
      {
            .Address    = UART5,
            .RxFIFO     = {NULL, 0, 0, 0},
            .TxBuffer   = {NULL, 0},
            .TaskHandle = NULL,
            .Lock       = PORT_FREE
      },
      #endif
      #endif
};


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Initialize USART devices. Here is created USART driver node
 *
 * @param usartName     device number
 *
 * @retval STD_RET_OK
 */
//================================================================================================//
stdRet_t UART_Init(dev_t usartName)
{
      (void) usartName;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief Opens specified port and initialize default settings
 *
 * @param[in]  usartName                  USART name (number)
 *
 * @retval STD_STATUS_OK                  operation success
 * @retval UART_STATUS_PORTLOCKED         port locked for other task
 * @retval UART_STATUS_PORTNOTEXIST       port number does not exist
 * @retval UART_STATUS_NOFREEMEM          no enough free memory to allocate RxBuffer
 */
//================================================================================================//
stdRet_t UART_Open(dev_t usartName)
{
      stdRet_t status    = UART_STATUS_PORTNOTEXIST;
      USART_t  *usartPtr = NULL;

      /* check port range */
      if ((unsigned)usartName < UART_DEV_LAST)
      {
            /* lock task switching */
            TaskSuspendAll();

            /* check that port is free */
            if (PortHandle[usartName].Lock == PORT_FREE)
            {
                  /* registered port for current task */
                  PortHandle[usartName].Lock = TaskGetPID();
                  TaskResumeAll();

                  /* set task handle for IRQs */
                  PortHandle[usartName].TaskHandle = TaskGetCurrentTaskHandle();

                  /* enable UART clock */
                  switch (usartName)
                  {
                        #ifdef RCC_APB2ENR_USART1EN
                        #if (UART_1_ENABLE > 0)
                        case UART_DEV_1:
                              RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_USART2EN
                        #if (UART_2_ENABLE > 0)
                        case UART_DEV_2:
                              RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_USART3EN
                        #if (UART_3_ENABLE > 0)
                        case UART_DEV_3:
                              RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_UART4EN
                        #if (UART_4_ENABLE > 0)
                        case UART_DEV_4:
                              RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_UART5EN
                        #if (UART_5_ENABLE > 0)
                        case UART_DEV_5:
                              RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
                              break;
                        #endif
                        #endif

                        default:
                              break;
                  }

                  /* set port address */
                  usartPtr = PortHandle[usartName].Address;

                  /* default settings */
                  if ((u32_t)usartPtr == USART1_BASE)
                        SetBaudRate(UART_PCLK2_FREQ, UART_DEFAULT_BAUDRATE);
                  else
                        SetBaudRate(UART_PCLK1_FREQ, UART_DEFAULT_BAUDRATE);

                  SetAddressMarkWakeMethod(UART_DEFAULT_WAKE_METHOD);

                  ParityCheckEnable(UART_DEFAULT_PARITY_ENABLE);

                  SetOddParity(UART_DEFAULT_PARITY_SELECTION);

                  TransmitterEnable(UART_DEFAULT_TX_ENABLE);

                  ReceiverEnable(UART_DEFAULT_RX_ENABLE);

                  ReceiverWakeupMuteEnable(UART_DEFAULT_RX_WAKEUP_MODE);

                  LINModeEnable(UART_DEFAULT_LIN_ENABLE);

                  Set2StopBits(UART_DEFAULT_STOP_BITS);

                  LINBreakDet11Bits(UART_DEFAULT_LIN_BREAK_LEN_DET);

                  SetAddressNode(UART_DEFAULT_MULTICOM_ADDRESS);

                  CTSEnable(UART_DEFAULT_CTS_ENABLE);

                  RTSEnable(UART_DEFAULT_RTS_ENABLE);

                  UARTEnable();

                  /* allocate memory for RX buffer */
                  PortHandle[usartName].RxFIFO.Buffer = (u8_t*)Malloc(UART_RX_BUFFER_SIZE);

                  if (PortHandle[usartName].RxFIFO.Buffer == NULL)
                  {
                        status = UART_STATUS_NOFREEMEM;
                        goto UART_Open_End;
                  }

                  /* enable interrupts */
                  switch (usartName)
                  {
                        #ifdef RCC_APB2ENR_USART1EN
                        #if (UART_1_ENABLE > 0)
                        case UART_DEV_1:
                              NVIC_EnableIRQ(USART1_IRQn);
                              NVIC_SetPriority(USART1_IRQn, IRQ_PRIORITY);
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_USART2EN
                        #if (UART_2_ENABLE > 0)
                        case UART_DEV_2:
                              NVIC_EnableIRQ(USART2_IRQn);
                              NVIC_SetPriority(USART2_IRQn, IRQ_PRIORITY);
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_USART3EN
                        #if (UART_3_ENABLE > 0)
                        case UART_DEV_3:
                              NVIC_EnableIRQ(USART3_IRQn);
                              NVIC_SetPriority(USART3_IRQn, IRQ_PRIORITY);
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_UART4EN
                        #if (UART_4_ENABLE > 0)
                        case UART_DEV_4:
                              NVIC_EnableIRQ(UART4_IRQn);
                              NVIC_SetPriority(UART4_IRQn, IRQ_PRIORITY);
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_UART5EN
                        #if (UART_5_ENABLE > 0)
                        case UART_DEV_5:
                              NVIC_EnableIRQ(UART5_IRQn);
                              NVIC_SetPriority(UART5_IRQn, IRQ_PRIORITY);
                              break;
                        #endif
                        #endif

                        default:
                              break;
                  }

                  EnableRxIRQ();

                  status = STD_RET_OK;
            }
            else
            {
                  TaskResumeAll();

                  if (PortHandle[usartName].Lock == TaskGetPID())
                        status = STD_RET_OK;
                  else
                        status = UART_STATUS_PORTLOCKED;
            }
      }

UART_Open_End:
      return status;
}


//================================================================================================//
/**
 * @brief Function close opened port
 *
 * @param[in]  usartName                  USART name (number)
 *
 * @retval STD_STATUS_OK                  operation success
 * @retval UART_STATUS_PORTLOCKED         port locked for other task
 * @retval UART_STATUS_PORTNOTEXIST       port number does not exist
 */
//================================================================================================//
stdRet_t UART_Close(dev_t usartName)
{
      stdRet_t status = UART_STATUS_PORTNOTEXIST;

      /* check port range */
      if ((unsigned)usartName < UART_DEV_LAST)
      {
            /* check that port is reserved for this task */
            if (PortHandle[usartName].Lock == TaskGetPID())
            {
                  /* turn off device */
                  switch (usartName)
                  {
                        #ifdef RCC_APB2ENR_USART1EN
                        #if (UART_1_ENABLE > 0)
                        case UART_DEV_1:
                              NVIC_DisableIRQ(USART1_IRQn);
                              RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
                              RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;
                              RCC->APB2ENR  &= ~RCC_APB2ENR_USART1EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_USART2EN
                        #if (UART_2_ENABLE > 0)
                        case UART_DEV_2:
                              NVIC_DisableIRQ(USART2_IRQn);
                              RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;
                              RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
                              RCC->APB1ENR  &= ~RCC_APB1ENR_USART2EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_USART3EN
                        #if (UART_3_ENABLE > 0)
                        case UART_DEV_3:
                              NVIC_DisableIRQ(USART3_IRQn);
                              RCC->APB1RSTR |= RCC_APB1RSTR_USART3RST;
                              RCC->APB1RSTR &= ~RCC_APB1RSTR_USART3RST;
                              RCC->APB1ENR  &= ~RCC_APB1ENR_USART3EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_UART4EN
                        #if (UART_4_ENABLE > 0)
                        case UART_DEV_4:
                              NVIC_DisableIRQ(UART4_IRQn);
                              RCC->APB1RSTR |= RCC_APB1RSTR_UART4RST;
                              RCC->APB1RSTR &= ~RCC_APB1RSTR_UART4RST;
                              RCC->APB1ENR  &= ~RCC_APB1ENR_UART4EN;
                              break;
                        #endif
                        #endif

                        #ifdef RCC_APB1ENR_UART5EN
                        #if (UART_5_ENABLE > 0)
                        case UART_DEV_5:
                              NVIC_DisableIRQ(UART5_IRQn);
                              RCC->APB1RSTR |= RCC_APB1RSTR_UART5RST;
                              RCC->APB1RSTR &= ~RCC_APB1RSTR_UART5RST;
                              RCC->APB1ENR  &= ~RCC_APB1ENR_UART5EN;
                              break;
                        #endif
                        #endif

                        default:
                              break;
                  }

                  /* free used memory for buffer */
                  Free(PortHandle[usartName].RxFIFO.Buffer);
                  PortHandle[usartName].RxFIFO.Buffer = NULL;
                  PortHandle[usartName].RxFIFO.Level  = 0;
                  PortHandle[usartName].RxFIFO.RxIdx  = 0;
                  PortHandle[usartName].RxFIFO.TxIdx  = 0;

                  /* unlock device */
                  PortHandle[usartName].Lock = PORT_FREE;

                  /* delete from task handle */
                  PortHandle[usartName].TaskHandle = NULL;

                  /* delete tx buffer */
                  PortHandle[usartName].TxBuffer.TxSrcPtr = NULL;

                  status = STD_RET_OK;
            }
            else
            {
                  status = UART_STATUS_PORTLOCKED;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Write data to UART (ISR or DMA)
 *
 * @param[in]  usartName                  USART name (number)
 * @param[in]  *src                       source buffer
 * @param[in]  size                       buffer size
 * @param[in]  seek                       seek
 *
 * @retval STD_STATUS_OK                  operation success
 * @retval UART_STATUS_PORTLOCKED         port locked for other task
 * @retval UART_STATUS_PORTNOTEXIST       port number does not exist
 * @retval UART_STATUS_INCORRECTSIZE      incorrect size
 */
//================================================================================================//
stdRet_t UART_Write(dev_t usartName, void *src, size_t size, size_t seek)
{
      (void)seek;

      stdRet_t status    = UART_STATUS_PORTNOTEXIST;
      USART_t     *usartPtr = NULL;

      /* check port range */
      if ((unsigned)usartName < UART_DEV_LAST)
      {
            /* check that port is reserved for this task */
            if (PortHandle[usartName].Lock == TaskGetPID())
            {
                  /* load data from FIFO */
                  if (size)
                  {
                         /* set port address */
                        usartPtr = PortHandle[usartName].Address;
                        u8_t *dataPtr = (u8_t*)src;

                        do
                        {
                              if (usartPtr->SR & USART_SR_TXE)
                              {
                                    usartPtr->DR = *(dataPtr++);
                                    size--;
                              }
                              else
                              {
//                                    TaskDelay(1); /* DNLFIXME to powinno byc zrobione na DMA */
                              }
                        }
                        while (size);

                        status = STD_RET_OK;
                  }
                  else
                  {
                        status = UART_STATUS_INCORRECTSIZE;
                  }
            }
            else
            {
                  status = UART_STATUS_PORTLOCKED;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Read data from UART Rx buffer
 *
 * @param[in]  usartName                  USART name (number)
 * @param[out] *dst                       destination buffer
 * @param[in]  size                       buffer size
 * @param[in]  seek                       seek
 *
 * @retval STD_STATUS_OK                  operation success
 * @retval UART_STATUS_PORTLOCKED         port locked for other task
 * @retval UART_STATUS_PORTNOTEXIST       port number does not exist
 * @retval UART_STATUS_INCORRECTSIZE      incorrect size
 */
//================================================================================================//
stdRet_t UART_Read(dev_t usartName, void *dst, size_t size, size_t seek)
{
      (void)seek;

      stdRet_t status = UART_STATUS_PORTNOTEXIST;

      /* check port range */
      if ((unsigned)usartName < UART_DEV_LAST)
      {
            /* check that port is reserved for this task */
            if (PortHandle[usartName].Lock == TaskGetPID())
            {
                  /* load data from FIFO */
                  if (size)
                  {
                        RxFIFO_t *RxFIFO = &PortHandle[usartName].RxFIFO;
                        u8_t     *dstPtr = (u8_t*)dst;

                        do
                        {
                              TaskEnterCritical();

                              if (RxFIFO->Level > 0)
                              {
                                    *dstPtr = RxFIFO->Buffer[RxFIFO->RxIdx++];

                                    if (RxFIFO->RxIdx >= UART_RX_BUFFER_SIZE)
                                          RxFIFO->RxIdx = 0;

                                    RxFIFO->Level--;

                                    size--;

                                    TaskExitCritical();
                              }
                              else
                              {
                                    TaskExitCritical();
                                    TaskSuspend(THIS_TASK);
                              }
                        }
                        while (size);

                        status = STD_RET_OK;
                  }
                  else
                  {
                        status = UART_STATUS_INCORRECTSIZE;
                  }
            }
            else
            {
                  status = UART_STATUS_PORTLOCKED;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Direct IO control
 *
 * @param[in]     usartName               USART name (number)
 * @param[in,out] ioRQ                    IO request
 * @param[in,out] *data                   IO data (arguments, results, etc)
 *
 * @retval STD_STATUS_OK                  operation success
 * @retval UART_STATUS_PORTLOCKED         port locked for other task
 * @retval UART_STATUS_PORTNOTEXIST       port number does not exist
 * @retval UART_STATUS_BUFFEREMPTY        rx buffer empty
 * @retval UART_STATUS_BADRQ              bad request
 */
//================================================================================================//
stdRet_t UART_IOCtl(dev_t usartName, IORq_t ioRQ, void *data)
{
      stdRet_t status = UART_STATUS_PORTNOTEXIST;

      /* check port range */
      if ((unsigned)usartName < UART_DEV_LAST)
      {
            /* check that port is reserved for this task */
            if (PortHandle[usartName].Lock == TaskGetPID())
            {
                  USART_t *usartPtr = PortHandle[usartName].Address;

                  status = STD_RET_OK;

                  switch (ioRQ)
                  {
                        case UART_IORQ_ENABLE_WAKEUP_IDLE:
                              SetAddressMarkWakeMethod(FALSE);
                              break;

                        case UART_IORQ_ENABLE_WAKEUP_ADDRESS_MARK:
                              SetAddressMarkWakeMethod(TRUE);
					break;

                        case UART_IORQ_ENABLE_PARITY_CHECK:
                              ParityCheckEnable(TRUE);
					break;

                        case UART_IORQ_DISABLE_PARITY_CHECK:
                              ParityCheckEnable(FALSE);
					break;

                        case UART_IORQ_SET_ODD_PARITY:
                              SetOddParity(TRUE);
					break;

                        case UART_IORQ_SET_EVEN_PARITY:
                              SetOddParity(FALSE);
					break;

                        case UART_IORQ_ENABLE_RECEIVER_WAKEUP_MUTE:
                              ReceiverWakeupMuteEnable(TRUE);
					break;

                        case UART_IORQ_DISABLE_RECEIVER_WAKEUP_MUTE:
                              ReceiverWakeupMuteEnable(FALSE);
					break;

                        case UART_IORQ_ENABLE_LIN_MODE:
                              LINModeEnable(TRUE);
					break;

                        case UART_IORQ_DISABLE_LIN_MODE:
                              LINModeEnable(FALSE);
					break;

                        case UART_IORQ_SET_1_STOP_BIT:
                              Set2StopBits(FALSE);
					break;

                        case UART_IORQ_SET_2_STOP_BITS:
                              Set2StopBits(TRUE);
					break;

                        case UART_IORQ_SET_LIN_BRK_DETECTOR_11_BITS:
                              LINBreakDet11Bits(TRUE);
					break;

                        case UART_IORQ_SET_LIN_BRK_DETECTOR_10_BITS:
                              LINBreakDet11Bits(FALSE);
					break;

                        case UART_IORQ_SET_ADDRESS_NODE:
                              SetAddressNode(*(u8_t*)data);
					break;

                        case UART_IORQ_ENABLE_CTS:
                              CTSEnable(TRUE);
					break;

                        case UART_IORQ_DISABLE_CTS:
                              CTSEnable(FALSE);
					break;

                        case UART_IORQ_ENABLE_RTS:
                              RTSEnable(TRUE);
					break;

                        case UART_IORQ_DISABLE_RTS:
                              RTSEnable(FALSE);
					break;

                        case UART_IORQ_GET_BYTE:
                        {
                              RxFIFO_t *RxFIFO = &PortHandle[usartName].RxFIFO;

                              TaskEnterCritical();

                              if (RxFIFO->Level > 0)
                              {
                                    *(u8_t*)data = RxFIFO->Buffer[RxFIFO->RxIdx++];

                                    if (RxFIFO->RxIdx >= UART_RX_BUFFER_SIZE)
                                          RxFIFO->RxIdx = 0;

                                    RxFIFO->Level--;
                              }
                              else
                              {
                                    status = UART_STATUS_BUFFEREMPTY;
                              }

                              TaskExitCritical();
					break;
                        }

                        case UART_IORQ_SEND_BYTE:
                        {
                              usartPtr = PortHandle[usartName].Address;

                              while (!(usartPtr->SR & USART_SR_TXE))
                                    TaskDelay(1);

                              usartPtr->DR = *(u8_t*)data;

					break;
                        }

                        case UART_IORQ_SET_BAUDRATE:
                        {
                             usartPtr = PortHandle[usartName].Address;

                             if ((u32_t)usartPtr == USART1_BASE)
                                   SetBaudRate(UART_PCLK2_FREQ, *(u32_t*)data);
                             else
                                   SetBaudRate(UART_PCLK1_FREQ, *(u32_t*)data);
                              break;
                        }

                        default:
                              status = UART_STATUS_BADRQ;
                              break;

                  }
            }
            else
            {
                  status = UART_STATUS_PORTLOCKED;
            }
      }

      return status;
}


//================================================================================================//
/**
 * @brief Release USART devices. Here is removed driver node and reseted device
 *
 * @param usartName     device number
 *
 * @retval STD_RET_OK
 */
//================================================================================================//
stdRet_t UART_Release(dev_t usartName)
{
      (void) usartName;

      return STD_RET_OK;
}


//================================================================================================//
/**
 * @brief USART1 Interrupt
 */
//================================================================================================//
#ifdef RCC_APB2ENR_USART1EN
#if (UART_1_ENABLE > 0)
void USART1_IRQHandler(void)
{
      IRQCode(USART1, UART_DEV_1);
}
#endif
#endif


//================================================================================================//
/**
 * @brief USART2 Interrupt
 */
//================================================================================================//
#ifdef RCC_APB1ENR_USART2EN
#if (UART_2_ENABLE > 0)
void USART2_IRQHandler(void)
{
      IRQCode(USART2, UART_DEV_2);
}
#endif
#endif


//================================================================================================//
/**
 * @brief USART3 Interrupt
 */
//================================================================================================//
#ifdef RCC_APB1ENR_USART3EN
#if (UART_3_ENABLE > 0)
void USART3_IRQHandler(void)
{
      IRQCode(USART3, UART_DEV_3);
}
#endif
#endif


//================================================================================================//
/**
 * @brief UART4 Interrupt
 */
//================================================================================================//
#ifdef RCC_APB1ENR_UART4EN
#if (UART_4_ENABLE > 0)
void UART4_IRQHandler(void)
{
      IRQCode(UART4, UART_DEV_4);
}
#endif
#endif


//================================================================================================//
/**
 * @brief UART5 Interrupt
 */
//================================================================================================//
#ifdef RCC_APB1ENR_UART5EN
#if (UART_5_ENABLE > 0)
void UART5_IRQHandler(void)
{
      IRQCode(UART5, UART_DEV_5);
}
#endif
#endif

#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
