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
#define _TTY_DEFAULT_TERMINAL_COLUMNS   80

/* default terminal height (buffer size in lines) */
#define _TTY_DEFAULT_TERMINAL_ROWS      40

/* output stream size (output queue) */
#define _TTY_STREAM_SIZE                80

/* number of virtual terminals */
#define _TTY_NUMBER_OF_VT               4

/* enable (1) or disable (0) checking terminal size */
#define _TTY_TERM_SIZE_CHECK            1

/* input/output file (access from many threads) */
#define _TTY_IO_FILE                    "/dev/ttyS0"

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
