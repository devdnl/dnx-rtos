#ifndef HELLOWORLD_H_
#define HELLOWORLD_H_
/*=========================================================================*//**
@file    helloworld.h

@author  Daniel Zorychta

@brief   The simple example program

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
#include "dnx.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/

/*==============================================================================
  Exported object declarations
==============================================================================*/
EXPORT_PROGRAM_PARAMS(helloworld);

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern int PROGRAM_MAIN(helloworld, int, char**);

#ifdef __cplusplus
}
#endif

#endif /* HELLOWORLD_H_ */
/*==============================================================================
  End of file
==============================================================================*/
