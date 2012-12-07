#ifndef DS1307_H_
#define DS1307_H_
/*=============================================================================================*//**
@file    ds1307.h

@author  Daniel Zorychta

@brief   This file support DS1307

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
#include "ds1307_def.h"
#include "system.h"


/*==================================================================================================
                                  Exported symbolic constants/macros
==================================================================================================*/
#define DS1307_DEV_RTC        0
#define DS1307_DEV_NVM        1
#define DS1307_PART_NONE      0


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t DS1307_Init   (devx_t dev, fd_t fd);
extern stdRet_t DS1307_Open   (devx_t dev, fd_t fd);
extern stdRet_t DS1307_Close  (devx_t dev, fd_t fd);
extern size_t   DS1307_Write  (devx_t dev, fd_t fd, void *src, size_t size, size_t nitems, size_t seek);
extern size_t   DS1307_Read   (devx_t dev, fd_t fd, void *dst, size_t size, size_t nitems, size_t seek);
extern stdRet_t DS1307_IOCtl  (devx_t dev, fd_t fd, IORq_t ioRQ, void *data);
extern stdRet_t DS1307_Release(devx_t dev, fd_t fd);


#ifdef __cplusplus
}
#endif

#endif /* DS1307_H_ */
/*==================================================================================================
                                             End of file
==================================================================================================*/
