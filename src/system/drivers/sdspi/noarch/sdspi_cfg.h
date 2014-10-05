/*=========================================================================*//**
@file    sdspi_cfg.h

@author  Daniel Zorychta

@brief   This file support configuration for SD in SPI mode

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

#ifndef _SDSPI_CFG_H_
#define _SDSPI_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
#define SDSPI_NUMBER_OF_CARDS                   __SDSPI_NUMBER_OF_CARDS__
#define SDSPI_CARD0_TIMEOUT                     __SDSPI_CARD0_TIMEOUT__
#define SDSPI_CARD1_TIMEOUT                     __SDSPI_CARD1_TIMEOUT__
#define SDSPI_CARD0_FILE                        __SDSPI_CARD0_FILE__
#define SDSPI_CARD1_FILE                        __SDSPI_CARD1_FILE__

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

#endif /* _SDSPI_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
