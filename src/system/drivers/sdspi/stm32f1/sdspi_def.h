/*=========================================================================*//**
@file    sdspi_def.h

@author  Daniel Zorychta

@brief   This file support statuses and requests for SD card in SPI mode

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

#ifndef _SDSPI_DEF_H_
#define _SDSPI_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "stm32f1/sdspi_ioctl.h"
#include "stm32f1/sdspi_cfg.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/* major device number */
enum {
        #if SDSPI_NUMBER_OF_CARDS >= 1
        _SDSPI_CARD_0,
        #endif
        #if SDSPI_NUMBER_OF_CARDS >= 2
        _SDSPI_CARD_1,
        #endif
        _NUMBER_OF_SDSPI_CARDS
};

/* minor numbers */
enum {
        _SDSPI_FULL_VOLUME,
        _SDSPI_PARTITION_1,
        _SDSPI_PARTITION_2,
        _SDSPI_PARTITION_3,
        _SDSPI_PARTITION_4
};

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
 Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _SDSPI_DEF_H_ */
/*==============================================================================
  End of file
==============================================================================*/
