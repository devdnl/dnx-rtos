/*==============================================================================
File    misc.c

Author  Daniel Zorychta

Brief   miscellaneous CPU functions.

        Copyright (C) 2018 Daniel Zorychta <daniel.zorychta@gmail.com>

        This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation and modified by the dnx RTOS exception.

        NOTE: The modification  to the GPL is  included to allow you to
              distribute a combined work that includes dnx RTOS without
              being obliged to provide the source  code for proprietary
              components outside of the dnx RTOS.

        The dnx RTOS  is  distributed  in the hope  that  it will be useful,
        but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
        MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
        GNU General Public License for more details.

        Full license text is available on the following file: doc/license.txt.

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "misc.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define AIRCR_VECTKEY_MASK    ((uint32_t)0x05FA0000)

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
  * @brief  Configures the priority grouping: pre-emption priority and
  *         subpriority.
  * @param NVIC_PriorityGroup: specifies the priority grouping bits length.
  *   This parameter can be one of the following values:
  * @arg NVIC_PriorityGroup_0: 0 bits for pre-emption priority
  *                            4 bits for subpriority
  * @arg NVIC_PriorityGroup_1: 1 bits for pre-emption priority
  *                            3 bits for subpriority
  * @arg NVIC_PriorityGroup_2: 2 bits for pre-emption priority
  *                            2 bits for subpriority
  * @arg NVIC_PriorityGroup_3: 3 bits for pre-emption priority
  *                            1 bits for subpriority
  * @arg NVIC_PriorityGroup_4: 4 bits for pre-emption priority
  *                            0 bits for subpriority
 */
//==============================================================================
void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup)
{
        SCB->AIRCR = AIRCR_VECTKEY_MASK | NVIC_PriorityGroup;
}

//==============================================================================
/**
  * @brief  Sets the vector table location and Offset.
  * @param NVIC_VectTab: specifies if the vector table is in RAM or
  *   FLASH memory.
  *   This parameter can be one of the following values:
  * @arg NVIC_VectTab_RAM
  * @arg NVIC_VectTab_FLASH
  * @param Offset: Vector Table base offset field.
  *   This value must be a multiple of 0x100.
 */
//==============================================================================
void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset)
{
        SCB->VTOR = NVIC_VectTab | (Offset & (uint32_t)0x1FFFFF80);
}

/*==============================================================================
  End of file
==============================================================================*/
