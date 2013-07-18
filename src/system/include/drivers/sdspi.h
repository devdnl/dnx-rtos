#ifndef _SDSPI_H_
#define _SDSPI_H_
/*=========================================================================*//**
@file    sdspi.h

@author  Daniel Zorychta

@brief   This file support SD card in SPI mode

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "system/dnxmodule.h"
#include "drivers/sdspi_def.h"

#if defined ARCH_stm32f1
        #include "stm32f1/sdspi_cfg.h"
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
DRIVER_INTERFACE(SDSPI);

#ifdef __cplusplus
}
#endif

#endif /* _SDSPI_H_ */
/*==============================================================================
  End of file
==============================================================================*/
