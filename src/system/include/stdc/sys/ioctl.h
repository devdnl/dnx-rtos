/*=========================================================================*//**
@file    ioctl.h

@author  Daniel Zorychta

@brief   Header contain all device control commands. Depend on existing drivers.

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

#ifndef _IOCTL_H_
#define _IOCTL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "core/ioctl_macros.h"
#include "core/vfs.h"

/* include here drivers definitions */
#include "tty_def.h"
#ifdef ARCH_stm32f1
#       include "stm32f1/gpio_def.h"
#       include "stm32f1/pll_def.h"
#       include "stm32f1/sdspi_def.h"
#       include "stm32f1/uart_def.h"
#       include "stm32f1/ethmac_def.h"
#       include "stm32f1/crc_def.h"
#       include "stm32f1/wdg_def.h"
#else
#       error "Unknown architecture!"
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
static inline int ioctl(FILE *stream, int request, ...)
{
        va_list arg;
        va_start(arg, request);
        int status = vfs_vioctl(stream, request, arg);
        va_end(arg);
        return status;
}

#ifdef __cplusplus
}
#endif

#endif /* _IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
