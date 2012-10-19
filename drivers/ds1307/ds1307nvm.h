#ifndef DS1307NVM_H_
#define DS1307NVM_H_
/*=============================================================================================*//**
@file    ds1307nvm.h

@author  Daniel Zorychta

@brief   This file support DS1307 NVM

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
#include "ds1307nvm_def.h"
#include "system.h"


/*==================================================================================================
                                  Exported symbolic constants/macros
==================================================================================================*/
#define DS1307NVM_DEV_NONE          0

/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t DS1307NVM_Init(dev_t dev);
extern stdRet_t DS1307NVM_Open(dev_t dev);
extern stdRet_t DS1307NVM_Close(dev_t dev);
extern stdRet_t DS1307NVM_Write(dev_t dev, void *src, size_t size, size_t seek);
extern stdRet_t DS1307NVM_Read(dev_t dev, void *dst, size_t size, size_t seek);
extern stdRet_t DS1307NVM_IOCtl(dev_t dev, IORq_t ioRQ, void *data);
extern stdRet_t DS1307NVM_Release(dev_t dev);

#ifdef __cplusplus
}
#endif

#endif /* DS1307NVM_H_ */
/*==================================================================================================
                                             End of file
==================================================================================================*/
