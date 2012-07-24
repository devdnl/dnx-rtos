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


/*==================================================================================================
                                   Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
void InitSystem(void);


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object definitions
==================================================================================================*/


/*==================================================================================================
                                         Function definitions
==================================================================================================*/


void task1(void *argv)
{
      (void) argv;

      static u8_t *data;
      static u32_t stackFree;

      for (;;)
      {
            TaskDelay(2);
            data = (u8_t*) Malloc(50*1024*sizeof(u8_t));
            TaskDelay(10);
            Free(data);

            stackFree = GetStackFreeSpace(THIS_TASK);

            TaskTerminate();
      }
}

uint32_t heapsize;

void task2(void *argv)
{
      (void) argv;

      for (;;)
      {
            heapsize = GetFreeHeapSize();
            TaskDelay(5);
      }
}


//================================================================================================//
/**
 * @brief Main function
 */
//================================================================================================//
int main(void)
{
      InitSystem();

      TaskCreate(task1, "Task1", MINIMAL_STACK_SIZE, NULL, 1, NULL);
      TaskCreate(task2, "Task2", MINIMAL_STACK_SIZE, NULL, 1, NULL);

      vTaskStartScheduler();

      return 0;
}


//================================================================================================//
/**
 * @brief Initialise system
 */
//================================================================================================//
void InitSystem(void)
{
      /* set interrupt vectors and NVIC priority */
      SCB->VTOR  = 0x00 | (0x00 & (uint32_t)0x1FFFFF80);
      SCB->AIRCR = 0x05FA0000 | 0x300;

      /* PLL initialization */
      if (PLL_Init() != STD_STATUS_OK)
            while (TRUE);

      GPIO_Init();
}


#ifdef __cplusplus
   }
#endif

/*==================================================================================================
                                             End of file
==================================================================================================*/
