/*=========================================================================*//**
@file    usbd_def.h

@author  Daniel Zorychta

@brief   USB-Device module.

@note    Copyright (C) 2014  Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _USBD_DEF_H_
#define _USBD_DEF_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "usbd_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
#define _USBD_MAJOR_NUMBER      0

enum {
        _USBD_MINOR_NUMBER_EP_0 = 0,
        _USBD_MINOR_NUMBER_EP_1,
        _USBD_MINOR_NUMBER_EP_2,
        _USBD_MINOR_NUMBER_EP_3,
        _USBD_MINOR_NUMBER_EP_4,
        _USBD_MINOR_NUMBER_EP_5,
        _USBD_MINOR_NUMBER_EP_6,
        _USBD_MINOR_NUMBER_EP_7,
};

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

#endif /* _USBD_DEF_H_ */
/*==============================================================================
  End of file
==============================================================================*/
