/*=========================================================================*//**
@file    tty_cfg.h

@author  Daniel Zorychta

@brief   This file support configuration of TTY.

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

#ifndef _TTY_CFG_H_
#define _TTY_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/

/*==============================================================================
  Exported macros
==============================================================================*/
/* default terminal width */
#define _TTY_DEFAULT_TERMINAL_COLUMNS   __TTY_TERM_COLS__

/* default terminal height (buffer size in lines) */
#define _TTY_DEFAULT_TERMINAL_ROWS      __TTY_TERM_ROWS__

/* output stream size (output queue) */
#define _TTY_STREAM_SIZE                __TTY_OUT_STREAM_LEN__

/* number of virtual terminals */
#define _TTY_NUMBER_OF_VT               __TTY_NUMBER_OF_TERM__

/* enable (1) or disable (0) checking terminal size */
#define _TTY_TERM_SIZE_CHECK            __TTY_ENABLE_TERM_SIZE_CHECK__

/* input file (access from many threads) */
#define _TTY_IN_FILE                    __TTY_TERM_IN_FILE__

/* output file (access from many threads) */
#define _TTY_OUT_FILE                   __TTY_TERM_OUT_FILE__

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _TTY_CFG_H_ */
/*==============================================================================
  End of file
==============================================================================*/
