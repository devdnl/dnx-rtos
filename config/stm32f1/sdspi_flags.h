/*=========================================================================*//**
@file    sdspi_flags.h

@author  Daniel Zorychta

@brief   SDSPI module configuration flags.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _SDSPI_FLAGS_H_
#define _SDSPI_FLAGS_H_

#define __SDSPI_TIMEOUT__ 500
#define __SDSPI_SPI_PORT__ 3
#define __SDSPI_SPI_CLK_DIV__ 4
#define __SDSPI_SD_CS_PIN__ SD_CS
#define __SDSPI_ENABLE_DMA__ __YES__
#define __SDSPI_DMA_IRQ_PRIORITY__ CONFIG_USER_IRQ_PRIORITY

#endif /* _SDSPI_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
