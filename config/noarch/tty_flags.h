/*=========================================================================*//**
@file    tty_flags.h

@author  Daniel Zorychta

@brief   TTY module configuration flags.

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

#ifndef _TTY_FLAGS_H_
#define _TTY_FLAGS_H_

#define __TTY_TERM_COLS__ 80
#define __TTY_TERM_ROWS__ 24
#define __TTY_OUT_STREAM_LEN__ 80
#define __TTY_NUMBER_OF_TERM__ 4
#define __TTY_ENABLE_TERM_SIZE_CHECK__ _YES_
#define __TTY_TERM_IN_FILE__ "/dev/ttyS0"
#define __TTY_TERM_OUT_FILE__ "/dev/ttyS0"

#endif /* _TTY_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
