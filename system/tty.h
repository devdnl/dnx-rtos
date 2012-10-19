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
#include "basic_types.h"
#include "system.h"

/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
#define TTY_COUNT             4
#define TTY_MSGS              40
#define TTY_LAST_MSG          0xFF

#define TTYD_NAME             "ttyd"
#define TTYD_STACK_SIZE       2*MINIMAL_STACK_SIZE


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern void ttyd(void *arg);
extern void TTY_AddMsg(u8_t tty, ch_t *msg);
extern void TTY_Clear(u8_t tty);
extern void TTY_ModifyLastMsg(u8_t tty, ch_t *newmsg);
extern u8_t TTY_CheckNewMsg(u8_t tty);
extern u8_t TTY_GetCurrTTY(void);

#ifdef __cplusplus
}
#endif

#endif /* TTY_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
