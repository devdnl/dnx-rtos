/*=========================================================================*//**
@file    endian.h

@author  Daniel Zorychta

@brief   CPU endianness.

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
\defgroup sys-endian-h <sys/endian.h>

The library is used to check CPU endianness.

*/
/**@{*/

#ifndef _ENDIAN_H_
#define _ENDIAN_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <cpu/cpuctl.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief Little endian switch
 */
#define LITTLE_ENDIAN           _BYTE_ORDER_LITTLE

/**
 * @brief Big endian switch
 */
#define BIG_ENDIAN              _BYTE_ORDER_BIG

/**
 * @brief Macro determines current CPU endianness
 *
 * Only one of defined byte orders can be used in the same time:
 * @ref LITTLE_ENDIAN or @ref BIG_ENDIAN.
 *
 * @b Example
 * @code
         #if BYTE_ORDER == LITTLE_ENDIAN
                 // code for little endian CPU

         #elif BYTE_ORDER == BIG_ENDIAN
                 // code for big endian CPU

         #else
                 #error "Unknown CPU endianness"
         #endif
   @endcode
 */
#define BYTE_ORDER              _CPUCTL_BYTE_ORDER

/*==============================================================================
  Exported object types
==============================================================================*/

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

#endif /* _ENDIAN_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
