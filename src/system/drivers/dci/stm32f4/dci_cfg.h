/*==============================================================================
File     dcmi_cfg.h

Author   Daniel Zorychta

Brief    Digital Camera Interface Driver

         Copyright (C) 2017 Daniel Zorychta <daniel.zorychta>

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

#ifndef _DCI_CFG_H_
#define _DCI_CFG_H_

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
#define _DCI_CAM_RES_X         __DCI_CAM_RES_X__
#define _DCI_CAM_RES_Y         __DCI_CAM_RES_Y__
#define _DCI_BYTES_PER_PIXEL   __DCI_BYTES_PER_PIXEL__
#define _DCI_EDM               __DCI_EDM__
#define _DCI_FCRC              __DCI_FCRC__
#define _DCI_VSPOL             __DCI_VSPOL__
#define _DCI_HSPOL             __DCI_HSPOL__
#define _DCI_PCKPOL            __DCI_PCKPOL__
#define _DCI_JPEG              __DCI_JPEG__
#define _DCI_ESS               __DCI_ESS__
#define _DCI_FEC               __DCI_FEC__
#define _DCI_FEU               __DCI_FEU__
#define _DCI_LEC               __DCI_LEC__
#define _DCI_LEU               __DCI_LEU__
#define _DCI_LSC               __DCI_LSC__
#define _DCI_LSU               __DCI_LSU__
#define _DCI_FSC               __DCI_FSC__
#define _DCI_FSU               __DCI_FSU__
#define _DCI_CROP              __DCI_CROP__
#define _DCI_CROP_START_X      __DCI_CROP_START_X__
#define _DCI_CROP_START_Y      __DCI_CROP_START_Y__
#define _DCI_CROP_HEIGHT       __DCI_CROP_HEIGHT__
#define _DCI_CROP_WIDTH        __DCI_CROP_WIDTH__
#define _DCI_IRQ_PRIORITY      __DCI_IRQ_PRIORITY__

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

#endif /* _DCI_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
