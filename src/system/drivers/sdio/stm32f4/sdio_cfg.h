/*==============================================================================
File     sdio_cfg.h

Author   Daniel Zorychta

Brief    SD Card Interface Driver

         Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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


==============================================================================*/

#ifndef _SDIO_CFG_H_
#define _SDIO_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "config.h"

/*==============================================================================
  Exported macros
==============================================================================*/
#define _SDIO_CFG_NEGEDGE       __SDIO_CFG_NEGEDGE__
#define _SDIO_CFG_BUS_WIDE      __SDIO_CFG_BUS_WIDE__
#define _SDIO_CFG_CLKDIV        __SDIO_CFG_CLKDIV__
#define _SDIO_CFG_PWRSAVE       __SDIO_CFG_PWRSAVE__
#define _SDIO_CFG_USEDMA        __SDIO_CFG_USEDMA__

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _SDIO_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
