/*=============================================================================================*//**
@file    mian.c

@author  Daniel Zorychta

@brief   This file provide system initialisation and RTOS start.

@note    Copyright (C) 2012  Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "system.h"
#include "pll.h"
#include "gpio.h"
#include "uart.h"
#include "terminal.h"


/*==================================================================================================
                                   Local symbolic constants/macros
==================================================================================================*/
/** InitTask stack size */
#define INITTASK_STACK_SIZE               (2 * MINIMAL_STACK_SIZE)


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static void InitSystem(void);
static void InitTask(void *arg);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                         Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Main function
 */
//================================================================================================//
int main(void)
{
      InitSystem();

      TaskCreate(InitTask, "init", INITTASK_STACK_SIZE, NULL, 3, NULL);

      vTaskStartScheduler();

      return 0;
}


//================================================================================================//
/**
 * @brief Initialise system
 * Insert here all initialize functions which should be initialized early before application start
 */
//================================================================================================//
static void InitSystem(void)
{
      /* set interrupt vectors and NVIC priority */
      SCB->VTOR  = 0x00 | (0x00 & (uint32_t)0x1FFFFF80);
      SCB->AIRCR = 0x05FA0000 | 0x300;

      /* PLL initialization */
      if (PLL_Init() != STD_STATUS_OK)
            while (TRUE);

      /* GPIO and AFIO initialization */
      GPIO_Init();

      /* initialize UART driver */
      UART_Init();
}


//================================================================================================//
/**
 * @brief Task which initialise high-level devices/applications etc
 * Task is responsible for low-level application runtime environment (stdio). Task connect
 * applications' stdios with hardware layer.
 */
//================================================================================================//
static void InitTask(void *arg)
{
      (void) arg;

      TaskDelay(2000);
      TaskSuspendAll();

      /* initialization kprint */
      UART_Open(UART_DEV_1);
      kprintEnable();
      kprint("\x1B[2J");
      kprint("Board powered by \x1b[32mFreeRTOS\x1b[0m\n");
      kprint("init [%d]: started kernel print\n", TaskGetTickCount());
      kprint("init [%d]: started init task\n", TaskGetTickCount());

      /* initialize drivers */

      /* starting first application */
      kprint("init [%d]: starting interactive console...", TaskGetTickCount());

      stdio_t *stdio = StartApplication(terminal, "terminal", TERMINAL_STACK_SIZE, NULL);

      if (stdio == NULL)
      {
            kprint("[\x1b[31mFAILED\x1b[0m]\n");
            kprint("Probably no enough free space. Restarting board...");
            TaskDelay(5000);
            NVIC_SystemReset();
      }
      else
      {
            kprint("[\x1b[32mSUCCESS\x1b[0m]\n");
      }

      kprint("init [%d]: free stack: %d\n", TaskGetTickCount(), TaskGetStackFreeSpace(THIS_TASK));

      TaskResumeAll();

      /* main loop which read stdios from applications */
      for (;;)
      {
            ch_t data;

            TaskSuspendAll();

            if (stdio->stdout.Level > 0)
            {
                  data = stdio->stdout.Buffer[stdio->stdout.RxIdx++];

                  if (data <= 1)
                  {
                        Free(stdio);

                        if (data == 0)
                              kprint("\ninit [%d]: terminal was terminated.", TaskGetTickCount());
                        else
                              kprint("\ninit [%d]: terminal was terminated with error.", TaskGetTickCount());

                        while (TRUE) TaskDelay(1000);
                  }

                  if (stdio->stdout.RxIdx >= configSTDIO_BUFFER_SIZE)
                        stdio->stdout.RxIdx = 0;

                  stdio->stdout.Level--;

                  UART_IOCtl(UART_DEV_1, UART_IORQ_SEND_BYTE, &data);
            }

            TaskResumeAll();

            if (UART_IOCtl(UART_DEV_1, UART_IORQ_GET_BYTE, &data) == STD_STATUS_OK)
            {
                  TaskSuspendAll();

                  if (stdio->stdin.Level < configSTDIO_BUFFER_SIZE)
                  {
                        stdio->stdin.Buffer[stdio->stdin.TxIdx++] = data;

                        if (stdio->stdin.TxIdx >= configSTDIO_BUFFER_SIZE)
                              stdio->stdin.TxIdx = 0;

                        stdio->stdin.Level++;
                  }

                  TaskResumeAll();
            }

            TaskDelay(1);
      }

      UART_Close(UART_DEV_1);

      TaskTerminate();
}


#ifdef __cplusplus
   }
#endif

/*==================================================================================================
                                             End of file
==================================================================================================*/
