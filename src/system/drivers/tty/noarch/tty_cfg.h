/*=========================================================================*//**
@file    tty_cfg.h

@author  Daniel Zorychta

@brief   This file support configuration of TTY.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


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
#define _TTY_TERMINAL_COLUMNS           __TTY_TERM_COLS__

/* default terminal height (buffer size in lines) */
#define _TTY_TERMINAL_ROWS              __TTY_TERM_ROWS__

/* output stream size (output queue) */
#define _TTY_STREAM_SIZE                __TTY_OUT_STREAM_LEN__

/* number of virtual terminals */
#define _TTY_NUMBER_OF_VT               __TTY_NUMBER_OF_TERM__

/* input file (access from many threads) */
#define _TTY_IN_FILE                    __TTY_TERM_IN_FILE__

/* output file (access from many threads) */
#define _TTY_OUT_FILE                   __TTY_TERM_OUT_FILE__

/* clear screen at initialization */
#define _TTY_CLR_SCR_AT_INIT            __TTY_CLR_SCR_AT_INIT__

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
