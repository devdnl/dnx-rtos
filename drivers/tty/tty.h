#ifndef TTY_H_
#define TTY_H_
/*=============================================================================================*//**
@file    tty.h

@author  Daniel Zorychta

@brief   This file support virtual terminal

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
#include "tty_def.h"
#include "basic_types.h"
#include "system.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
#define TTYD_NAME             "ttyd"
#define TTYD_STACK_SIZE       3*MINIMAL_STACK_SIZE


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/



/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t TTY_Init   (devx_t dev, fd_t fd);
extern stdRet_t TTY_Open   (devx_t dev, fd_t fd);
extern stdRet_t TTY_Close  (devx_t dev, fd_t fd);
extern size_t   TTY_Write  (devx_t dev, fd_t fd, void *src, size_t size, size_t nitems, size_t seek);
extern size_t   TTY_Read   (devx_t dev, fd_t fd, void *dst, size_t size, size_t nitems, size_t seek);
extern stdRet_t TTY_IOCtl  (devx_t dev, fd_t fd, IORq_t ioRQ, void *data);
extern stdRet_t TTY_Release(devx_t dev, fd_t fd);

#ifdef __cplusplus
}
#endif

#endif /* TTY_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
