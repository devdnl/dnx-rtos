#ifndef ETH_H_
#define ETH_H_
/*=============================================================================================*//**
@file    eth.h

@author  Daniel Zorychta

@brief   This file support ethernet peripheral

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
#include "ether_cfg.h"
#include "ether_def.h"
#include "system.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
#define ETH_PART_NONE         0


/** port names */
enum ETH_DEV_NUMBER_enum
{
      ETH_DEV_1,
      ETH_DEV_LAST
};


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t ETHER_Init   (devx_t eth, fd_t fd);
extern stdRet_t ETHER_Open   (devx_t eth, fd_t fd);
extern stdRet_t ETHER_Close  (devx_t eth, fd_t fd);
extern size_t   ETHER_Write  (devx_t eth, fd_t fd, void *src, size_t size, size_t nitems, size_t seek);
extern size_t   ETHER_Read   (devx_t eth, fd_t fd, void *dst, size_t size, size_t nitems, size_t seek);
extern stdRet_t ETHER_IOCtl  (devx_t eth, fd_t fd, IORq_t ioRQ, void *data);
extern stdRet_t ETHER_Release(devx_t eth, fd_t fd);

#ifdef __cplusplus
}
#endif

#endif /* ETH_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
