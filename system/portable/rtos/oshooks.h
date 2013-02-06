#ifndef OSHOOKS_H_
#define OSHOOKS_H_
/*=========================================================================*//**
@file    oshooks.h

@author  Daniel Zorychta

@brief   This file support all operating system hooks

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


*//*==========================================================================*/

#ifdef __cplusplus
   extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "oswrap.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern void  vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName);
extern void  vApplicationTickHook(void);
extern u32_t GetUptimeCnt(void);

#ifdef __cplusplus
   }
#endif

#endif /* OSHOOKS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
