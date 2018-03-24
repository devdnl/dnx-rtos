/*=========================================================================*//**
@file    clk_ioctl.h

@author  Daniel Zorychta

@brief   CLK ioctl request codes.

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
@defgroup drv-clk CLK Driver

\section drv-clk-desc Description
Driver handles system clock peripheral (PLL).

\section drv-clk-sup-arch Supported architectures
\li stm32f1
\li stm32f4

\section drv-clk-ddesc Details
\subsection drv-clk-ddesc-num Meaning of major and minor numbers
There is no special meaning of major and minor numbers. Both numbers should be
set to 0.

\subsubsection drv-clk-ddesc-numres Numeration restrictions
Both driver numbers should be set to 0.

\subsection drv-clk-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("CLK", 0, 0, "/dev/clk");
@endcode

\subsection drv-clk-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("CLK", 0, 0);
@endcode

\subsection drv-clk-ddesc-cfg Driver configuration
Driver can be configured only in configuration tool. Runtime configuration is
not possible.

\subsection drv-clk-ddesc-write Data write
Write operation is not supported.

\subsection drv-clk-ddesc-read Data read
Read operation is not supported.

\subsection drv-clk-ddesc-ioctl Getting clock frequencies
The clock frequencies and clock names can be explored by ioctl() function.
Example:

@code
// ...

FILE *clk = fopen("/dev/clk", "r+");
if (clk) {

        CLK_info_t clkinf;
        clkinf.iterator = 0;

        while (  (ioctl(fileno(clk), IOCTL_CLK__GET_CLK_INFO, &clkinf) == 0)
              && (clkinf.name != NULL) ) {

                printf("Clock '%s': %d Hz\n", clkinf.name, clkinf.freq_Hz);
        }

        fclose(clk);
} else {
        perror("/dev/clk");
}

// ...
@endcode

@{
*/


#ifndef _CLK_IOCTL_H_
#define _CLK_IOCTL_H_

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
 *  @brief  Get frequency and name of selected clock [Hz].
 *  @param  [WR,RD] @ref CLK_info_t * clock information.
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_CLK__GET_CLK_INFO   _IOWR(CLK, 0x00, CLK_info_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type represent clock info object (iterator).
 */
typedef struct {
        u8_t        iterator;           /*!< [IN]  Clock iterator (starts from 0, auto incremented)*/
        u32_t       freq_Hz;            /*!< [OUT] Clock frequency in Hz*/
        const char *name;               /*!< [OUT] Clock name (NULL if clock does not exist -- end of iteration)*/
} CLK_info_t;

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

#endif /* _CLK_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
