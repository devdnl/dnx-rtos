#ifndef REGAPP_H_
#define REGAPP_H_
/*=============================================================================================*//**
@file    regapp.h

@author  Daniel Zorychta

@brief   This file is used to registration applications

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
#include "systypes.h"

/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
typedef struct
{
      const ch_t   *appName;
      void         (*appPtr)(void *argv);
      const uint_t *stackSize;
} regAppData_t;


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern regAppData_t  regapp_GetAppData   (const ch_t *appName);
extern regAppData_t *regapp_GetAppListPtr(void);
extern int_t         regapp_GetAppCount  (void);

#ifdef __cplusplus
}
#endif

#endif /* REGAPP_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
