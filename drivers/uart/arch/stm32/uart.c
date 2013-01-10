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


/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "uart.h"
#include "stm32f10x.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define UARTP(dev)                              uart->port[dev]
#define UARTA(dev)                              uartAddr[dev]
#define BLOCK_TIME                              0
#define TXC_WAIT_TIME                           60000

/** UART wake method: idle line (0) or address mark (1) */
#define SetAddressMarkWakeMethod(usart, enable) \
      if (enable)                               \
            usart->CR1 |= USART_CR1_WAKE;       \
      else                                      \
            usart->CR1 &= ~USART_CR1_WAKE


/** parity enable (1) or disable (0) */
#define ParityCheckEnable(usart, enable)        \
      if (enable)                               \
            usart->CR1 |= USART_CR1_PCE;        \
      else                                      \
            usart->CR1 &= ~USART_CR1_PCE


/** even parity (0) or odd parity (1) */
#define SetOddParity(usart, enable)             \
      if (enable)                               \
            usart->CR1 |= USART_CR1_PS;         \
      else                                      \
            usart->CR1 &= ~USART_CR1_PS


/** disable (0) or enable (1) UART transmitter */
#define TransmitterEnable(usart, enable)        \
      if (enable)                               \
            usart->CR1 |= USART_CR1_TE;         \
      else                                      \
            usart->CR1 &= ~USART_CR1_TE


/** disable (0) or enable (1) UART receiver */
#define ReceiverEnable(usart, enable)           \
      if (enable)                               \
            usart->CR1 |= USART_CR1_RE;         \
      else                                      \
            usart->CR1 &= ~USART_CR1_RE


/** receiver wakeup: active mode (0) or mute mode (1) */
#define ReceiverWakeupMuteEnable(usart, enable) \
      if (enable)                               \
            usart->CR1 |= USART_CR1_RWU;        \
      else                                      \
            usart->CR1 &= ~USART_CR1_RWU


/** LIN mode disable (0) or enable (1) */
#define LINModeEnable(usart, enable)            \
      if (enable)                               \
            usart->CR2 |= USART_CR2_LINEN;      \
      else                                      \
            usart->CR2 &= ~USART_CR2_LINEN


/** 1 stop bit (0) or 2 stop bits (1) */
#define Set2StopBits(usart, enable)             \
      if (enable)                               \
      {                                         \
            usart->CR2 &= USART_CR2_STOP;       \
            usart->CR2 |= USART_CR2_STOP_1;     \
      }                                         \
      else                                      \
            usart->CR2 &= USART_CR2_STOP


/** LIN break detector length: 10 bits (0) or 11 bits (1) */
#define LINBreakDet11Bits(usart, enable)        \
      if (enable)                               \
            usart->CR2 |= USART_CR2_LBDL;       \
      else                                      \
            usart->CR2 &= ~USART_CR2_LBDL


/** address of the USART node (in the multiprocessor mode), 4-bit length */
#define SetAddressNode(usart, adr)              \
      usart->CR2 &= ~USART_CR2_ADD;             \
      usart->CR2 |= (adr & USART_CR2_ADD)       \


/** baud rate */
#define SetBaudRate(usart, clk, baud)           \
      usart->BRR = (u16_t)((clk / baud) + 1)


/** CTS hardware flow control enable (1) or disable (0) */
#define CTSEnable(usart, enable)                \
      if (enable)                               \
            usart->CR3 |= USART_CR3_CTSE;       \
      else                                      \
            usart->CR3 &= ~USART_CR3_CTSE


/** RTS hardware flow control enable (1) or disable (0) */
#define RTSEnable(usart, enable)                \
      if (enable)                               \
            usart->CR3 |= USART_CR3_RTSE;       \
      else                                      \
            usart->CR3 &= ~USART_CR3_RTSE


/** enable UART */
#define UARTEnable(usart)                       \
      usart->CR1 |= USART_CR1_UE


/** disable UART */
#define UARTDisable(usart)                      \
      usart->CR1 &= ~UART_CR1_UE1


/** enable RX interrupt */
#define EnableRxIRQ(usart)                      \
      usart->CR1 |= USART_CR1_RXNEIE


/** enable RX interrupt */
#define DisableRxIRQ(usart)                     \
      usart->CR1 &= ~UART_CR1_RXNEIE


/** enable TXE interrupt */
#define EnableTXEIRQ(usart)                     \
      usart->CR1 |= USART_CR1_TXEIE


/** disable TXE interrupt */
#define DisableTXEIRQ(usart)                    \
      usart->CR1 &= ~USART_CR1_TXEIE


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
static void IRQCode(USART_t *usart, devx_t dev);


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
 * @param[in] part          device part
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t UART_Init(devx_t dev, fd_t part)
{
      (void)part;

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
                        UARTP(dev)->sem = CreateSemBin();

                        if (UARTP(dev)->mtx && UARTP(dev)->sem)
                        {
                              TakeSemBin(UARTP(dev)->sem, 1);
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
 * @param[in] part          device part
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//================================================================================================//
stdRet_t UART_Release(devx_t dev, fd_t part)
{
      (void)part;

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
 * @param[in]  part                       device part
 *
 * @retval STD_STATUS_OK                  operation success
 * @retval UART_STATUS_PORTLOCKED         port locked for other task
 * @retval UART_STATUS_PORTNOTEXIST       port number does not exist
 * @retval UART_STATUS_NOFREEMEM          no enough free memory to allocate RxBuffer
 */
//================================================================================================//
stdRet_t UART_Open(devx_t dev, fd_t part)
{
      (void)part;

      stdRet_t status = UART_STATUS_PORTNOTEXIST;

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
                  USART_t *uart_p = UARTA(dev);

                  /* default settings */
                  UARTEnable(uart_p);

                  if ((u32_t)uart_p == USART1_BASE)
                        SetBaudRate(uart_p, UART_PCLK2_FREQ, UART_DEFAULT_BAUDRATE);
                  else
                        SetBaudRate(uart_p, UART_PCLK1_FREQ, UART_DEFAULT_BAUDRATE);

                  SetAddressMarkWakeMethod(uart_p, UART_DEFAULT_WAKE_METHOD);

                  ParityCheckEnable(uart_p, UART_DEFAULT_PARITY_ENABLE);

                  SetOddParity(uart_p, UART_DEFAULT_PARITY_SELECTION);

                  TransmitterEnable(uart_p, UART_DEFAULT_TX_ENABLE);

                  ReceiverEnable(uart_p, UART_DEFAULT_RX_ENABLE);

                  ReceiverWakeupMuteEnable(uart_p, UART_DEFAULT_RX_WAKEUP_MODE);

                  LINModeEnable(uart_p, UART_DEFAULT_LIN_ENABLE);

                  Set2StopBits(uart_p, UART_DEFAULT_STOP_BITS);

                  LINBreakDet11Bits(uart_p, UART_DEFAULT_LIN_BREAK_LEN_DET);

                  SetAddressNode(uart_p, UART_DEFAULT_MULTICOM_ADDRESS);

                  CTSEnable(uart_p, UART_DEFAULT_CTS_ENABLE);

                  RTSEnable(uart_p, UART_DEFAULT_RTS_ENABLE);

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

                  EnableRxIRQ(uart_p);

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
 * @param[in]  part                       device part
 *
 * @retval STD_STATUS_OK                  operation success
 * @retval UART_STATUS_PORTLOCKED         port locked for other task
 * @retval UART_STATUS_PORTNOTEXIST       port number does not exist
 */
//================================================================================================//
stdRet_t UART_Close(devx_t dev, fd_t part)
{
      (void)part;

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
 * @param[in]  part                       device part
 * @param[in]  *src                       source buffer
 * @param[in]  size                       item size
 * @param[in]  nitems                     number of items
 * @param[in]  seek                       seek
 *
 * @return number of transmitted nitems
 */
//================================================================================================//
size_t UART_Write(devx_t dev, fd_t part, void *src, size_t size, size_t nitems, size_t seek)
{
      (void)part;
      (void)seek;

      size_t  n = 0;

      /* check port range */
      if ((unsigned)dev < UART_DEV_LAST)
      {
            /* check that port is reserved for this task */
            if (TakeRecMutex(UARTP(dev)->mtx, BLOCK_TIME) == OS_OK)
            {
                  /* load data from FIFO */
                  if (nitems && size)
                  {
                        UARTP(dev)->TxBuffer.TxSrcPtr = src;
                        UARTP(dev)->TxBuffer.Size     = size * nitems;

                        EnableTXEIRQ(UARTA(dev));
                        TakeSemBin(UARTP(dev)->sem, TXC_WAIT_TIME);

                        n = nitems;
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
 * @param[in]  part                       device part
 * @param[out] *dst                       destination buffer
 * @param[in]  size                       item size
 * @param[in]  nitems                     number of items
 * @param[in]  seek                       seek
 *
 * @return number of received nitems
 */
//================================================================================================//
size_t UART_Read(devx_t dev, fd_t part, void *dst, size_t size, size_t nitems, size_t seek)
{
      (void)part;
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
 * @param[in]     dev                     USART name (number)
 * @param[in]     part                    device part
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
stdRet_t UART_IOCtl(devx_t dev, fd_t part, IORq_t ioRQ, void *data)
{
      (void)part;

      stdRet_t status = UART_STATUS_PORTNOTEXIST;

      /* check port range */
      if ((unsigned)dev < UART_DEV_LAST)
      {
            /* check that port is reserved for this task */
            if (TakeRecMutex(UARTP(dev)->mtx, BLOCK_TIME) == OS_OK)
            {
                  USART_t *uart_p = UARTA(dev);

                  status = STD_RET_OK;

                  switch (ioRQ)
                  {
                        case UART_IORQ_ENABLE_WAKEUP_IDLE:
                              SetAddressMarkWakeMethod(uart_p, FALSE);
                              break;

                        case UART_IORQ_ENABLE_WAKEUP_ADDRESS_MARK:
                              SetAddressMarkWakeMethod(uart_p, TRUE);
					break;

                        case UART_IORQ_ENABLE_PARITY_CHECK:
                              ParityCheckEnable(uart_p, TRUE);
					break;

                        case UART_IORQ_DISABLE_PARITY_CHECK:
                              ParityCheckEnable(uart_p, FALSE);
					break;

                        case UART_IORQ_SET_ODD_PARITY:
                              SetOddParity(uart_p, TRUE);
					break;

                        case UART_IORQ_SET_EVEN_PARITY:
                              SetOddParity(uart_p, FALSE);
					break;

                        case UART_IORQ_ENABLE_RECEIVER_WAKEUP_MUTE:
                              ReceiverWakeupMuteEnable(uart_p, TRUE);
					break;

                        case UART_IORQ_DISABLE_RECEIVER_WAKEUP_MUTE:
                              ReceiverWakeupMuteEnable(uart_p, FALSE);
					break;

                        case UART_IORQ_ENABLE_LIN_MODE:
                              LINModeEnable(uart_p, TRUE);
					break;

                        case UART_IORQ_DISABLE_LIN_MODE:
                              LINModeEnable(uart_p, FALSE);
					break;

                        case UART_IORQ_SET_1_STOP_BIT:
                              Set2StopBits(uart_p, FALSE);
					break;

                        case UART_IORQ_SET_2_STOP_BITS:
                              Set2StopBits(uart_p, TRUE);
					break;

                        case UART_IORQ_SET_LIN_BRK_DETECTOR_11_BITS:
                              LINBreakDet11Bits(uart_p, TRUE);
					break;

                        case UART_IORQ_SET_LIN_BRK_DETECTOR_10_BITS:
                              LINBreakDet11Bits(uart_p, FALSE);
					break;

                        case UART_IORQ_SET_ADDRESS_NODE:
                              SetAddressNode(uart_p, *(u8_t*)data);
					break;

                        case UART_IORQ_ENABLE_CTS:
                              CTSEnable(uart_p, TRUE);
					break;

                        case UART_IORQ_DISABLE_CTS:
                              CTSEnable(uart_p, FALSE);
					break;

                        case UART_IORQ_ENABLE_RTS:
                              RTSEnable(uart_p, TRUE);
					break;

                        case UART_IORQ_DISABLE_RTS:
                              RTSEnable(uart_p, FALSE);
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
                              uart_p = UARTA(dev);

                              while (!(uart_p->SR & USART_SR_TXE))
                                    TaskDelay(1);

                              uart_p->DR = *(u8_t*)data;

					break;
                        }

                        case UART_IORQ_SET_BAUDRATE:
                        {
                              uart_p = UARTA(dev);

                             if ((u32_t)uart_p == USART1_BASE)
                                   SetBaudRate(uart_p, UART_PCLK2_FREQ, *(u32_t*)data);
                             else
                                   SetBaudRate(uart_p, UART_PCLK1_FREQ, *(u32_t*)data);
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
static void IRQCode(USART_t *usart, devx_t dev)
{
      if (usart->SR & USART_SR_TXE)
      {
            if (UARTP(dev)->TxBuffer.Size && UARTP(dev)->TxBuffer.TxSrcPtr)
            {
                  usart->DR = *(UARTP(dev)->TxBuffer.TxSrcPtr++);

                  if (--UARTP(dev)->TxBuffer.Size == 0)
                  {
                        UARTP(dev)->TxBuffer.TxSrcPtr = NULL;

                        i32_t woke;
                        DisableTXEIRQ(UARTA(dev));
                        GiveSemBinFromISR(UARTP(dev)->sem, &woke);
                  }
            }
      }

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
