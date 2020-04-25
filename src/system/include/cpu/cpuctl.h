/*=========================================================================*//**
@file    cpuctl.h

@author  Daniel Zorychta

@brief   This file support CPU control

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==========================================================================*/

#ifndef _SYS_CPUCTL_H_
#define _SYS_CPUCTL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#define _BYTE_ORDER_LITTLE_ENDIAN       0
#define _BYTE_ORDER_BIG_ENDIAN          1

#if defined(ARCH_stm32f1)
#       include "stm32f1/cpuctl.h"
#elif defined(ARCH_stm32f4)
#       include "stm32f4/cpuctl.h"
#elif defined(ARCH_stm32f7)
#       include "stm32f7/cpuctl.h"
#elif defined(ARCH_efr32)
#       include "efr32/cpuctl.h"
#endif

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _SYS_CPUCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
