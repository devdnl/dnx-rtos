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
#include "basic_types.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"


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


static void task1(void *arg)
{
    (void) arg;

    for ( ;; )
    {
        asm volatile ("nop");
    }
}

static void task2(void *arg)
{
    (void) arg;

    for ( ;; )
    {
        asm volatile ("nop");
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

    xTaskCreate(task1, (signed char *)"Task1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(task2, (signed char *)"Task2", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    vTaskStartScheduler();

    for ( ;; );
}


//================================================================================================//
/**
 * @brief Initialise system
 */
//================================================================================================//
void InitSystem(void)
{

}


#ifdef __cplusplus
   }
#endif

/*==================================================================================================
                                             End of file
==================================================================================================*/
