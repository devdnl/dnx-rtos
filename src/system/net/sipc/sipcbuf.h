/*==============================================================================
File    sipcbuf.h

Author  Daniel Zorychta

Brief   .

        Copyright (C) 2019 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/**
@defgroup SIPCBUF_H_ SIPCBUF_H_

Detailed Doxygen description.
*/
/**@{*/

#ifndef _SIPCBUF_H_
#define _SIPCBUF_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "kernel/ktypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
typedef struct sipcbuf sipcbuf_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int  sipcbuf__create(sipcbuf_t **sipcbuf, size_t max_capacity);
extern void sipcbuf__destroy(sipcbuf_t *sipcbuf);
extern int  sipcbuf__write(sipcbuf_t *sipcbuf, const u8_t *data, size_t size, bool reference);
extern int  sipcbuf__read(sipcbuf_t *sipcbuf, u8_t *data, size_t size, size_t *rdctr);
extern void sipcbuf__clear(sipcbuf_t *sipcbuf);
extern bool sipcbuf__is_full(sipcbuf_t *sipcbuf);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _SIPCBUF_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
