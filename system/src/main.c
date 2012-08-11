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
#include "initd.h"


/*==================================================================================================
                                   Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/
static void InitSystem(void);


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

      TaskCreate(Initd, INITD_NAME, INITD_STACK_SIZE, NULL, 3, NULL);

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
      if (PLL_Init() != STD_RET_OK)
            while (TRUE);

      /* GPIO and AFIO initialization */
      GPIO_Init();

      /* initialize UART driver */
      UART_Init();
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                             End of file
==================================================================================================*/
