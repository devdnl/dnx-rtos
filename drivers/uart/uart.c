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
#define UARTP(dev)                              uart->port[dev]
#define BLOCK_TIME                              100

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


/** IRQ priorities */
#define IRQ_PRIORITY          0xDF


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/
/** USART handling structure */
struct uartCtrl
{
      /** Rx FIFO type */
      struct sRxFIFO
      {
            u8_t  *Buffer;
            u16_t Level;
            u16_t RxIdx;
            u16_t TxIdx;
      } RxFIFO;

      /** structure with tx buffer address and size */
      struct sTxBuffer
      {
            u8_t   *TxSrcPtr;
            size_t Size;
      } TxBuffer;

      USART_t *Address;             /* peripheral address */
      sem_t   sem;                  /* semaphore for sync between IRQ and task */
      mutex_t mtx;                  /* mutex for secure resources */
      task_t  TaskHandle;           /* task handler in IRQ */
};


/** type which contain port information */
typedef struct PortHandler_struct
{
      struct uartCtrl *port[UART_DEV_LAST];
} PortHandler_t;


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static void IRQCode(USART_t *usart, nod_t dev);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
/* addresses of UART devices */
static USART_t *const uartAddr[UART_DEV_LAST] =
{
#if defined(RCC_APB2ENR_USART1EN) && (UART_1_ENABLE > 0)
      USART1,
#endif

#if defined(RCC_APB1ENR_USART2EN) && (UART_2_ENABLE > 0)
      USART2,
#endif

#if defined(RCC_APB1ENR_USART3EN) && (UART_3_ENABLE > 0)
      USART3,
#endif

#if defined(RCC_APB1ENR_UART4EN) && (UART_4_ENABLE > 0)
      UART4,
#endif

#if defined(RCC_APB1ENR_UART5EN) && (UART_5_ENABLE > 0)
      UART5,
#endif
};

/** port configuration localizations */
static PortHandler_t *uart;


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Initialize USART devices
 *
 * @param[in] dev           UART device
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t UART_Init(nod_t dev)
{
      stdRet_t status = STD_RET_ERROR;

      if (uart == NULL)
      {
            uart = calloc(1, sizeof(PortHandler_t));

            if (uart == NULL)
                  goto UART_Init_end;
      }

      if (dev < UART_DEV_LAST)
      {
            if (UARTP(dev) == NULL)
            {
                  UARTP(dev) = calloc(1, sizeof(struct uartCtrl));

                  if (UARTP(dev) != NULL)
                  {
                        UARTP(dev)->mtx = CreateRecMutex();
                        CreateSemBin(UARTP(dev)->sem);

                        if (UARTP(dev)->mtx && UARTP(dev)->sem)
                        {
                              UARTP(dev)->Address = (USART_t*)uartAddr[dev];
                              status = STD_RET_OK;
                        }
                        else
                        {
                              if (UARTP(dev)->mtx)
                                    DeleteRecMutex(UARTP(dev)->mtx);

                              if (UARTP(dev)->sem)
                                    DeleteSemBin(UARTP(dev)->sem);

                              free(UARTP(dev));
                        }
                  }
            }
      }

      UART_Init_end:
      return status;
}


//================================================================================================//
/**
 * @brief Release USART devices
 *
 * @param[in] dev           I2C device
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t UART_Release(nod_t dev)
{
      stdRet_t status = STD_RET_ERROR;

      if (uart && dev < UART_DEV_LAST)
      {
            /* free i2c device data */
            if (UARTP(dev))
            {
                  DeleteSemBin(UARTP(dev)->sem);
                  DeleteRecMutex(UARTP(dev)->mtx);
                  free(UARTP(dev));

                  UARTP(dev) = NULL;
                  status = STD_RET_OK;
            }

            /* check if all devices are not used, if yes then free handler */
            for (u8_t i = 0; i < UART_DEV_LAST; i++)
            {
                  if (UARTP(i) != NULL)
                        goto UART_Release_End;
            }

            /* free i2c handler */
            free(uart);
            uart = NULL;
      }

      UART_Release_End:
      return status;
}


//================================================================================================//
/**
 * @brief Opens specified port and initialize default settings
 *
 * @param[in]  dev                        USART name (number)
 *
 * @retval STD_STATUS_OK                  operation success
 * @retval UART_STATUS_PORTLOCKED         port locked for other task
 * @retval UART_STATUS_PORTNOTEXIST       port number does not exist
 * @retval UART_STATUS_NOFREEMEM          no enough free memory to allocate RxBuffer
 */
//================================================================================================//
stdRet_t UART_Open(nod_t dev)
{
      stdRet_t status    = UART_STATUS_PORTNOTEXIST;
      USART_t  *usartPtr = NULL;

      /* check port range */
      if ((unsigned)dev < UART_DEV_LAST)
      {
            /* check that port is free */
            if (TakeRecMutex(UARTP(dev)->mtx, BLOCK_TIME) == OS_OK)
            {
                  UARTP(dev)->TaskHandle = TaskGetCurrentTaskHandle();

                  /* enable UART clock */
                  switch (dev)
                  {
                        #if defined(RCC_APB2ENR_USART1EN) && (UART_1_ENABLE > 0)
                        case UART_DEV_1:
                              RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
                              break;
                        #endif

                        #if defined(RCC_APB1ENR_USART2EN) && (UART_2_ENABLE > 0)
                        case UART_DEV_2:
                              RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
                              break;
                        #endif

                        #if defined(RCC_APB1ENR_USART3EN) && (UART_3_ENABLE > 0)
                        case UART_DEV_3:
                              RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
                              break;
                        #endif

                        #if defined(RCC_APB1ENR_UART4EN) && (UART_4_ENABLE > 0)
                        case UART_DEV_4:
                              RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
                              break;
                        #endif

                        #if defined(RCC_APB1ENR_UART5EN) && (UART_5_ENABLE > 0)
                        case UART_DEV_5:
                              RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
                              break;
                        #endif

                        default:
                              break;
                  }

                  /* set port address */
                  usartPtr = UARTP(dev)->Address;

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
                  UARTP(dev)->RxFIFO.Buffer = malloc(UART_RX_BUFFER_SIZE);

                  if (UARTP(dev)->RxFIFO.Buffer == NULL)
                  {
                        status = UART_STATUS_NOFREEMEM;
                        goto UART_Open_End;
                  }

                  /* enable interrupts */
                  switch (dev)
                  {
                        #if defined(RCC_APB2ENR_USART1EN) && (UART_1_ENABLE > 0)
                        case UART_DEV_1:
                              NVIC_EnableIRQ(USART1_IRQn);
                              NVIC_SetPriority(USART1_IRQn, IRQ_PRIORITY);
                              break;
                        #endif

                        #if defined(RCC_APB1ENR_USART2EN) && (UART_2_ENABLE > 0)
                        case UART_DEV_2:
                              NVIC_EnableIRQ(USART2_IRQn);
                              NVIC_SetPriority(USART2_IRQn, IRQ_PRIORITY);
                              break;
                        #endif

                        #if defined(RCC_APB1ENR_USART3EN) && (UART_3_ENABLE > 0)
                        case UART_DEV_3:
                              NVIC_EnableIRQ(USART3_IRQn);
                              NVIC_SetPriority(USART3_IRQn, IRQ_PRIORITY);
                              break;
                        #endif

                        #if defined(RCC_APB1ENR_UART4EN) && (UART_4_ENABLE > 0)
                        case UART_DEV_4:
                              NVIC_EnableIRQ(UART4_IRQn);
                              NVIC_SetPriority(UART4_IRQn, IRQ_PRIORITY);
                              break;
                        #endif

                        #if defined(RCC_APB1ENR_UART5EN) && (UART_5_ENABLE > 0)
                        case UART_DEV_5:
                              NVIC_EnableIRQ(UART5_IRQn);
                              NVIC_SetPriority(UART5_IRQn, IRQ_PRIORITY);
                              break;
                        #endif

                        default:
                              break;
                  }

                  EnableRxIRQ();

                  status = STD_RET_OK;
            }
            else
            {
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
 * @param[in]  dev                        USART name (number)
 *
 * @retval STD_STATUS_OK                  operation success
 * @retval UART_STATUS_PORTLOCKED         port locked for other task
 * @retval UART_STATUS_PORTNOTEXIST       port number does not exist
 */
//================================================================================================//
stdRet_t UART_Close(nod_t dev)
{
      stdRet_t status = UART_STATUS_PORTNOTEXIST;

      /* check port range */
      if ((unsigned)dev < UART_DEV_LAST)
      {
            /* check that port is reserved for this task */
            if (TakeRecMutex(UARTP(dev)->mtx, BLOCK_TIME) == OS_OK)
            {
                  /* turn off device */
                  switch (dev)
                  {
                        #if defined(RCC_APB2ENR_USART1EN) && (UART_1_ENABLE > 0)
                        case UART_DEV_1:
                              NVIC_DisableIRQ(USART1_IRQn);
                              RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
                              RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;
                              RCC->APB2ENR  &= ~RCC_APB2ENR_USART1EN;
                              break;
                        #endif

                        #if defined(RCC_APB1ENR_USART2EN) && (UART_2_ENABLE > 0)
                        case UART_DEV_2:
                              NVIC_DisableIRQ(USART2_IRQn);
                              RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;
                              RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
                              RCC->APB1ENR  &= ~RCC_APB1ENR_USART2EN;
                              break;
                        #endif

                        #if defined(RCC_APB1ENR_USART3EN) && (UART_3_ENABLE > 0)
                        case UART_DEV_3:
                              NVIC_DisableIRQ(USART3_IRQn);
                              RCC->APB1RSTR |= RCC_APB1RSTR_USART3RST;
                              RCC->APB1RSTR &= ~RCC_APB1RSTR_USART3RST;
                              RCC->APB1ENR  &= ~RCC_APB1ENR_USART3EN;
                              break;
                        #endif

                        #if defined(RCC_APB1ENR_UART4EN) && (UART_4_ENABLE > 0)
                        case UART_DEV_4:
                              NVIC_DisableIRQ(UART4_IRQn);
                              RCC->APB1RSTR |= RCC_APB1RSTR_UART4RST;
                              RCC->APB1RSTR &= ~RCC_APB1RSTR_UART4RST;
                              RCC->APB1ENR  &= ~RCC_APB1ENR_UART4EN;
                              break;
                        #endif

                        #if defined(RCC_APB1ENR_UART5EN) && (UART_5_ENABLE > 0)
                        case UART_DEV_5:
                              NVIC_DisableIRQ(UART5_IRQn);
                              RCC->APB1RSTR |= RCC_APB1RSTR_UART5RST;
                              RCC->APB1RSTR &= ~RCC_APB1RSTR_UART5RST;
                              RCC->APB1ENR  &= ~RCC_APB1ENR_UART5EN;
                              break;
                        #endif

                        default:
                              break;
                  }

                  /* free used memory for buffer */
                  free(UARTP(dev)->RxFIFO.Buffer);
                  UARTP(dev)->RxFIFO.Buffer = NULL;
                  UARTP(dev)->RxFIFO.Level  = 0;
                  UARTP(dev)->RxFIFO.RxIdx  = 0;
                  UARTP(dev)->RxFIFO.TxIdx  = 0;

                  /* delete tx buffer */
                  UARTP(dev)->TxBuffer.TxSrcPtr = NULL;

                  status = STD_RET_OK;

                  /* give this mutex */
                  GiveRecMutex(UARTP(dev)->mtx);

                  /* give mutex from open */
                  GiveRecMutex(UARTP(dev)->mtx);
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
 * @param[in]  dev                        dev number
 * @param[in]  *src                       source buffer
 * @param[in]  size                       item size
 * @param[in]  nitems                     number of items
 * @param[in]  seek                       seek
 *
 * @return number of transmitted nitems
 */
//================================================================================================//
size_t UART_Write(nod_t dev, void *src, size_t size, size_t nitems, size_t seek)
{
      (void)seek;

      size_t  n = 0;
      USART_t *usartPtr = NULL;

      /* check port range */
      if ((unsigned)dev < UART_DEV_LAST)
      {
            /* check that port is reserved for this task */
            if (TakeRecMutex(UARTP(dev)->mtx, BLOCK_TIME) == OS_OK)
            {
                  /* load data from FIFO */
                  if (nitems && size)
                  {
                         /* set port address */
                        usartPtr = UARTP(dev)->Address;
                        u8_t *dataPtr  = (u8_t*)src;
                        u32_t dataSize = nitems * size;

                        do
                        {
                              if (usartPtr->SR & USART_SR_TXE)
                              {
                                    usartPtr->DR = *(dataPtr++);
                                    dataSize--;
                                    n++;
                              }
                              else
                              {
//                                    TaskDelay(1); /* DNLFIXME in UART Tx use DMA */
                              }
                        }
                        while (dataSize);

                        n /= size;
                  }

                  GiveRecMutex(UARTP(dev)->mtx);
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Read data from UART Rx buffer
 *
 * @param[in]  dev                        dev number
 * @param[out] *dst                       destination buffer
 * @param[in]  size                       item size
 * @param[in]  nitems                     number of items
 * @param[in]  seek                       seek
 *
 * @return number of received nitems
 */
//================================================================================================//
size_t UART_Read(nod_t dev, void *dst, size_t size, size_t nitems, size_t seek)
{
      (void)seek;

      size_t n = 0;

      /* check port range */
      if ((unsigned)dev < UART_DEV_LAST)
      {
            /* check that port is reserved for this task */
            if (TakeRecMutex(UARTP(dev)->mtx, BLOCK_TIME) == OS_OK)
            {
                  /* load data from FIFO */
                  if (nitems && size)
                  {
                        struct sRxFIFO *RxFIFO  = &UARTP(dev)->RxFIFO;
                        u8_t  *dstPtr  = (u8_t*)dst;
                        u32_t dataSize = nitems * size;

                        do
                        {
                              TaskEnterCritical();

                              if (RxFIFO->Level > 0)
                              {
                                    *dstPtr = RxFIFO->Buffer[RxFIFO->RxIdx++];

                                    if (RxFIFO->RxIdx >= UART_RX_BUFFER_SIZE)
                                          RxFIFO->RxIdx = 0;

                                    RxFIFO->Level--;

                                    dataSize--;

                                    n++;

                                    TaskExitCritical();
                              }
                              else
                              {
                                    TaskExitCritical();
                                    TaskSuspend(THIS_TASK);
                              }
                        }
                        while (dataSize);

                        n /= size;
                  }

                  GiveRecMutex(UARTP(dev)->mtx);
            }
      }

      return n;
}


//================================================================================================//
/**
 * @brief Direct IO control
 *
 * @param[in]     dev               USART name (number)
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
stdRet_t UART_IOCtl(nod_t dev, IORq_t ioRQ, void *data)
{
      stdRet_t status = UART_STATUS_PORTNOTEXIST;

      /* check port range */
      if ((unsigned)dev < UART_DEV_LAST)
      {
            /* check that port is reserved for this task */
            if (TakeRecMutex(UARTP(dev)->mtx, BLOCK_TIME) == OS_OK)
            {
                  USART_t *usartPtr = UARTP(dev)->Address;

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
                              struct sRxFIFO *RxFIFO = &UARTP(dev)->RxFIFO;

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
                              usartPtr = UARTP(dev)->Address;

                              while (!(usartPtr->SR & USART_SR_TXE))
                                    TaskDelay(1);

                              usartPtr->DR = *(u8_t*)data;

					break;
                        }

                        case UART_IORQ_SET_BAUDRATE:
                        {
                             usartPtr = UARTP(dev)->Address;

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

                  GiveRecMutex(UARTP(dev)->mtx);
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
 * @brief Interrupt handling
 *
 * @param usart   usart address
 * @param dev     device number
 *
 * @retval STD_RET_OK
 */
//================================================================================================//
static void IRQCode(USART_t *usart, nod_t dev)
{
      if (usart->SR & USART_SR_RXNE)
      {
            struct sRxFIFO *RxFIFO = &UARTP(dev)->RxFIFO;

            u8_t DR = usart->DR;

            if (RxFIFO->Buffer)
            {
                  if (RxFIFO->Level < UART_RX_BUFFER_SIZE)
                  {
                        RxFIFO->Buffer[RxFIFO->TxIdx++] = DR;

                        if (RxFIFO->TxIdx >= UART_RX_BUFFER_SIZE)
                              RxFIFO->TxIdx = 0;

                        RxFIFO->Level++;
                  }

                  if (UARTP(dev)->TaskHandle)
                  {
                        if (TaskResumeFromISR(UARTP(dev)->TaskHandle) == pdTRUE)
                              TaskYield();
                  }
            }
      }
}


//================================================================================================//
/**
 * @brief USART1 Interrupt
 */
//================================================================================================//
#if defined(RCC_APB2ENR_USART1EN) && (UART_1_ENABLE > 0)
void USART1_IRQHandler(void)
{
      IRQCode(USART1, UART_DEV_1);
}
#endif


//================================================================================================//
/**
 * @brief USART2 Interrupt
 */
//================================================================================================//
#if defined(RCC_APB1ENR_USART2EN) && (UART_2_ENABLE > 0)
void USART2_IRQHandler(void)
{
      IRQCode(USART2, UART_DEV_2);
}
#endif


//================================================================================================//
/**
 * @brief USART3 Interrupt
 */
//================================================================================================//
#if defined(RCC_APB1ENR_USART3EN) && (UART_3_ENABLE > 0)
void USART3_IRQHandler(void)
{
      IRQCode(USART3, UART_DEV_3);
}
#endif


//================================================================================================//
/**
 * @brief UART4 Interrupt
 */
//================================================================================================//
#if defined(RCC_APB1ENR_UART4EN) && (UART_4_ENABLE > 0)
void UART4_IRQHandler(void)
{
      IRQCode(UART4, UART_DEV_4);
}
#endif


//================================================================================================//
/**
 * @brief UART5 Interrupt
 */
//================================================================================================//
#if defined(RCC_APB1ENR_UART5EN) && (UART_5_ENABLE > 0)
void UART5_IRQHandler(void)
{
      IRQCode(UART5, UART_DEV_5);
}
#endif

#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
