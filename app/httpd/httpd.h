#ifndef HTTPD_H_
#define HTTPD_H_
/*=============================================================================================*//**
@file    httpd.h

@author  Daniel Zorychta

@brief   HTTP server

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
#include "system.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
/** define stack size for this application */
#define HTTPD_STACK_SIZE               (10 * MINIMAL_STACK_SIZE)

/** define application name */
#define HTTPD_NAME                     "myhttpd"


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern APPLICATION(httpd);

#ifdef __cplusplus
}
#endif

#endif /* HTTPD_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
