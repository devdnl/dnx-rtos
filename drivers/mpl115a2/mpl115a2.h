#ifndef MPL115A2_H_
#define MPL115A2_H_
/*=============================================================================================*//**
@file    mpl115a2.h

@author  Daniel Zorychta

@brief   This file support temperature and pressure sensor - MPL115A2

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
#include "mpl115a2_def.h"
#include "system.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
#define MPL115A2_DEV_NONE                 0
#define MPL115A2_PART_NONE                0


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t MPL115A2_Init   (devx_t dev, fd_t fd);
extern stdRet_t MPL115A2_Open   (devx_t dev, fd_t fd);
extern stdRet_t MPL115A2_Close  (devx_t dev, fd_t fd);
extern size_t   MPL115A2_Write  (devx_t dev, fd_t fd, void *src, size_t size, size_t nitems, size_t seek);
extern size_t   MPL115A2_Read   (devx_t dev, fd_t fd , void *dst, size_t size, size_t nitems, size_t seek);
extern stdRet_t MPL115A2_IOCtl  (devx_t dev, fd_t fd, IORq_t ioRQ, void *data);
extern stdRet_t MPL115A2_Release(devx_t dev, fd_t fd);

#ifdef __cplusplus
}
#endif

#endif /* MPL115A2_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
