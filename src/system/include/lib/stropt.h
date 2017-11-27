/*==============================================================================
File     stropt.h

Author   Daniel Zorychta

Brief    String options analyze library.

         Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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
@defgroup STROPT_H_ STROPT_H_

Detailed Doxygen description.
*/
/**@{*/

#ifndef _STROPT_H_
#define _STROPT_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int _stropt_get_int(const char *opts, const char *var, int defval);
extern const char *_stropt_get_string_ref(const char *opts, const char *var, size_t *len);
extern size_t _stropt_get_string_copy(const char *opts, const char *var, char *buf, size_t buflen);
extern int _stropt_get_bool(const char *opts, const char *var, int defval);
extern bool _stropt_is_flag(const char *opts, const char *flag);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _STROPT_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
