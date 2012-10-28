#ifndef REGDRV_H_
#define REGDRV_H_
/*=============================================================================================*//**
@file    regdrv.h

@author  Daniel Zorychta

@brief   This file is used to registration drivers

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

/* include here device definition/request list */
#include "uart_def.h"
#include "pll_def.h"
#include "i2c_def.h"
#include "gpio_def.h"
#include "ether_def.h"
#include "ds1307_def.h"
#include "tty_def.h"
#include "mpl115a2_def.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
typedef struct
{
      stdRet_t (*open)(nod_t dev);
      stdRet_t (*close)(nod_t dev);
      size_t   (*write)(nod_t dev, void *src, size_t size, size_t nitems, size_t seek);
      size_t   (*read )(nod_t dev, void *dst, size_t size, size_t nitems, size_t seek);
      stdRet_t (*ioctl)(nod_t dev, IORq_t iroq, void *data);
      nod_t    device;
} regDrvData_t;


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t InitDrv(const ch_t *drvName, ch_t *nodeName);
extern stdRet_t ReleaseDrv(const ch_t *drvName);
extern stdRet_t GetDrvData(const ch_t *drvNode, regDrvData_t *drvdata);
extern void     REGDRV_opendir(DIR_t *dir);
extern dirent_t REGDRV_readdir(size_t seek);

#ifdef __cplusplus
}
#endif

#endif /* REGDRV_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
