#ifndef SYSDRV_H_
#define SYSDRV_H_
/*=============================================================================================*//**
@file    sysdrv.h

@author  Daniel Zorychta

@brief   This function provide all required function needed to write drivers.

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
#include "basic_types.h"
#include "systypes.h"
#include "memman.h"
#include "oswrap.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
#ifndef calloc
#define calloc(nmemb, msize)             mm_calloc(nmemb, msize)
#endif

#ifndef malloc
#define malloc(size)                     mm_malloc(size)
#endif

#ifndef free
#define free(mem)                        mm_free(mem)
#endif


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/


#ifdef __cplusplus
}
#endif

#endif /* SYSDRV_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
