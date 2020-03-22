/*==============================================================================
File    misc.h

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

/**
@defgroup HDR_H_ HDR_H_

Detailed Doxygen description.
*/
/**@{*/

#ifndef _MISC_H_
#define _MISC_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "stm32f4xx.h"

/*==============================================================================
  Exported macros
==============================================================================*/
#define NVIC_VectTab_RAM             ((uint32_t)0x20000000)
#define NVIC_VectTab_FLASH           ((uint32_t)0x08000000)

#define NVIC_LP_SEVONPEND            ((uint8_t)0x10)
#define NVIC_LP_SLEEPDEEP            ((uint8_t)0x04)
#define NVIC_LP_SLEEPONEXIT          ((uint8_t)0x02)

#define NVIC_PriorityGroup_0         ((uint32_t)0x700) /* 0 bits for pre-emption priority
                                                          4 bits for subpriority */
#define NVIC_PriorityGroup_1         ((uint32_t)0x600) /* 1 bits for pre-emption priority
                                                          3 bits for subpriority */
#define NVIC_PriorityGroup_2         ((uint32_t)0x500) /* 2 bits for pre-emption priority
                                                          2 bits for subpriority */
#define NVIC_PriorityGroup_3         ((uint32_t)0x400) /* 3 bits for pre-emption priority
                                                          1 bits for subpriority */
#define NVIC_PriorityGroup_4         ((uint32_t)0x300) /* 4 bits for pre-emption priority
                                                          0 bits for subpriority */

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup);
extern void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _MISC_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
