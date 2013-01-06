/*=============================================================================================*//**
@file    cpuctl.c

@author  Daniel Zorychta

@brief   This file support CPU control

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
#include "cpuctl.h"
#include "stm32f10x.h"
#include "oswrap.h"


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/
#define APB1FREQ                    36000000UL
#define TIM2FREQ                    1000000UL


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
static u32_t TotalCPUTime;


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief Restart CPU
 */
//================================================================================================//
void cpuctl_SystemReboot(void)
{
      NVIC_SystemReset();
}


//================================================================================================//
/**
 * @brief Start counter used in CPU load measurement
 */
//================================================================================================//
void cpuctl_CfgTimeStatCnt(void)
{
      /* enable clock */
      RCC->APB1ENR  |= RCC_APB1ENR_TIM2EN;

      /* reset timer */
      RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
      RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;

      /* configure timer */
      TIM2->PSC = (APB1FREQ/TIM2FREQ) - 1;
      TIM2->ARR = 0xFFFF;
      TIM2->CR1 = TIM_CR1_CEN;
}


//================================================================================================//
/**
 * @brief Function called after task go to ready state
 */
//================================================================================================//
void cpuctl_TaskSwitchedIn(void)
{
      TIM2->CNT = 0;
}


//================================================================================================//
/**
 * @brief Function called when task go out ready state
 */
//================================================================================================//
void cpuctl_TaskSwitchedOut(void)
{
      u16_t  cnt     = TIM2->CNT;
      task_t taskhdl = TaskGetCurrentTaskHandle();
      u32_t  tmp     = (u32_t)TaskGetTag(taskhdl) + cnt;
      TotalCPUTime  += cnt;
      TaskSetTag(taskhdl, (void*)tmp);
}


//================================================================================================//
/**
 * @brief Function returns CPU total time
 *
 * @return CPU total time
 */
//================================================================================================//
u32_t cpuctl_GetCPUTotalTime(void)
{
      return TotalCPUTime;
}


//================================================================================================//
/**
 * @brief Function clear CPU total time
 */
//================================================================================================//
void cpuctl_ClearCPUTotalTime(void)
{
      TotalCPUTime = 0;
}


#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
