/*=============================================================================================*//**
@file    hooks.c

@author  Daniel Zorychta

@brief   This file support all system's hooks

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

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "hooks.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


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
 * @brief Stack overflow hook
 */
//================================================================================================//
void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{
      ( void ) pcTaskName;
      ( void ) pxTask;

      for( ;; );
}


//================================================================================================//
/**
 * @brief Hook when system tick was increased
 */
//================================================================================================//
void vApplicationTickHook(void)
{
}


//void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress ) /* DNLFIXME */
//{
//      /* These are volatile to try and prevent the compiler/linker optimising them
//      away as the variables never actually get used.  If the debugger won't show the
//      values of the variables, make them global my moving their declaration outside
//      of this function. */
//      volatile uint32_t r0;
//      volatile uint32_t r1;
//      volatile uint32_t r2;
//      volatile uint32_t r3;
//      volatile uint32_t r12;
//      volatile uint32_t lr; /* Link register. */
//      volatile uint32_t pc; /* Program counter. */
//      volatile uint32_t psr;/* Program status register. */
//
//      r0 = pulFaultStackAddress[ 0 ];
//      r1 = pulFaultStackAddress[ 1 ];
//      r2 = pulFaultStackAddress[ 2 ];
//      r3 = pulFaultStackAddress[ 3 ];
//
//      r12 = pulFaultStackAddress[ 4 ];
//      lr = pulFaultStackAddress[ 5 ];
//      pc = pulFaultStackAddress[ 6 ];
//      psr = pulFaultStackAddress[ 7 ];
//
//      /* When the following line is hit, the variables contain the register values. */
//      for( ;; );
//}


//================================================================================================//
/**
 * @brief Hard Fault ISR
 */
//================================================================================================//
void HardFault_Handler(void)
{
//      while (TRUE);  /* DNLFIXME */
//      __asm volatile
//      (
//          " tst lr, #4                                                \n"
//          " ite eq                                                    \n"
//          " mrseq r0, msp                                             \n"
//          " mrsne r0, psp                                             \n"
//          " ldr r1, [r0, #24]                                         \n"
//          " ldr r2, handler2_address_const                            \n"
//          " bx r2                                                     \n"
//          " handler2_address_const: .word prvGetRegistersFromStack    \n"
//      );
}


//================================================================================================//
/**
 * @brief Memory Management failure ISR
 */
//================================================================================================//
void MemManage_Handler(void)
{
      while (TRUE);
}


//================================================================================================//
/**
 * @brief Bus Fault ISR
 */
//================================================================================================//
void BusFault_Handler(void)
{
      while (TRUE);
}


//================================================================================================//
/**
 * @brief Usage Fault ISR
 */
//================================================================================================//
void UsageFault_Handler(void)
{
      while (TRUE);
}


/*==================================================================================================
                                            End of file
==================================================================================================*/
