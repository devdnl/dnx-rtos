/*=========================================================================*//**
@file    tty_ioctl.h

@author  Daniel Zorychta

@brief   This file support TTY ioctl request codes.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/**
@defgroup drv-tty TTY Driver

\section drv-tty-desc Description
Driver handles terminal virtual device. Driver creates VT100 virtual terminal.
Driver is necessary to handle user input and output. Each terminal can handle
at least one application. It is possible to connect many application to single
terminal but output may be mixed. In most cases the TTY driver input and output
is UART device.

Device (drivers) connection table
| Grade | Device       | In                  | Out        |
| ----: | :----------- | :------------------ | :----------|
| 0     | UART/file    | -                   | /dev/ttyS0 |
| 1     | TTY driver   | /dev/ttyS0          | /dev/tty0  |
| 2     | Application  | /dev/tty0           | /dev/tty0  |

\section drv-tty-sup-arch Supported architectures
\li Any (noarch)

\section drv-tty-ddesc Details
\subsection drv-tty-ddesc-num Meaning of major and minor numbers
The major number determine number of terminal. The minor number should be set
always to 0.

\subsubsection drv-tty-ddesc-numres Numeration restrictions
Major number should be used in range from 0 to maximum number of terminals
configured in module options. Minor number should be always 0.

\subsection drv-tty-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("TTY", 0, 0, "/dev/tty0");          // terminal 0
driver_init("TTY", 1, 0, "/dev/tty1");          // terminal 1
@endcode

\subsection drv-tty-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("TTY", 0, 0);
driver_release("TTY", 1, 0);
@endcode

\subsection drv-tty-ddesc-cfg Driver configuration
Driver can be configured in Configtool. Runtime configuration is not possible.

\subsection drv-tty-ddesc-write Data write
Writing data to device is the same as writing data to regular file.

\subsection drv-tty-ddesc-read Data read
Reading data from device is the same as reading data from regular file.

\subsection drv-tty-ddesc-switch Terminal switching
Terminals can be switched by using F1 - F4 keys or by using ioctl() function.

@{
*/

#ifndef _TTY_IOCTL_H_
#define _TTY_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/ioctl_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 *  @brief  Set TTY configuration
 *  @param  [WR] TTY_config_t * terminal configuration
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_TTY__CONFIGURE                    _IOW(TTY, 0x00, const TTY_config_t*)

/**
 *  @brief  Gets current TTY number.
 *  @param  [RD] int * terminal number
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_TTY__GET_CURRENT_TTY              _IOR(TTY, 0x01, int*)

/**
 *  @brief  Switch terminal to selected one.
 *  @param  [WR] int * terminal number
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_TTY__SWITCH_TTY_TO                _IOW(TTY, 0x02, int*)

/**
 *  @brief  Gets number of columns.
 *  @param  [RD] int * number of columns
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_TTY__GET_COL                      _IOR(TTY, 0x03, int*)

/**
 *  @brief  Gets number of rows.
 *  @param  [RD] int * number of rows
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_TTY__GET_ROW                      _IOR(TTY, 0x04, int*)

/**
 *  @brief  Clear screen.
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_TTY__CLEAR_SCR                    _IO(TTY, 0x05)

/**
 *  @brief  Enable terminal echo.
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_TTY__ECHO_ON                      _IO(TTY, 0x06)

/**
 *  @brief  Disable terminal echo.
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_TTY__ECHO_OFF                     _IO(TTY, 0x07)

/**
 *  @brief  Set edit line to specified user's text (string).
 *  @param  [WR] const char * string passed to edit line
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_TTY__SET_EDITLINE                 _IOW(TTY, 0x08, const char*)

/**
 *  @brief  Gets number of virtual terminals.
 *  @param  [RD] int * number of virtual terminals
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_TTY__GET_NUMBER_OF_TTYS           _IOR(TTY, 0x09, int*)

/**
 *  @brief  Refreshes last line.
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_TTY__REFRESH_LAST_LINE            _IO(TTY, 0x0A)

/**
 *  @brief  Read buffer.
 *  @param  [RD, WR] TTY_buffer_t*      buffer descriptor
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_TTY__READ_BUFFER                  _IOWR(TTY, 0x0B, TTY_buffer_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct {
        const char *input_file;         //!< input file path
        const char *output_file;        //!< output file path
        bool        clear_screen;       //!< clear screen at initialization
} TTY_config_t;

typedef struct {
        char  *ptr;     //!< pointer to buffer
        size_t size;    //!< buffer size and bytes left after ioctl()
} TTY_buffer_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _TTY_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
