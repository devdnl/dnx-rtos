/*=========================================================================*//**
File     inet_types.h

Author   Daniel Zorychta

Brief

         Copyright (C) 2016 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/**
@defgroup INET_TYPES_H_ INET_TYPES_H_

Detailed Doxygen description.
*/
/**@{*/

#ifndef _INET_TYPES_H_
#define _INET_TYPES_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stdbool.h>
#include "kernel/sysfunc.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct {
        mutex_t        *access;
        FILE           *if_file;
        void           *if_mem;
        thread_t        if_thread;
        struct netif    netif;
        uint            rx_packets;
        uint            tx_packets;
        uint            rx_bytes;
        uint            tx_bytes;
        bool            ready:1;
        bool            disconnected:1;
        bool            configured:1;
} inet_t;

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

#endif /* _INET_TYPES_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
