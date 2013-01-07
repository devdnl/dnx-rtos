#ifndef TTY_H_
#define TTY_H_
/*=============================================================================================*//**
@file    tty.h

@author  Daniel Zorychta

@brief   This file support virtual terminal

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
#include "tty_def.h"
#include "sysdrv.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
#define TTYD_NAME             "ttydrv"
#define TTYD_STACK_SIZE       3*MINIMAL_STACK_SIZE


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/



/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
DRIVER_INTERFACE_CLASS(TTY);


#ifdef __cplusplus
}
#endif

#endif /* TTY_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
