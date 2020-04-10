/*=========================================================================*//**
@file    conv.h

@author  Daniel Zorychta

@brief   Module with calculation and convert functions.

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

#ifndef _LIB_CONV_H_
#define _LIB_CONV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>
#include <stdbool.h>
#include "kernel/builtinfunc.h"

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/
extern struct tm _tmbuf;
extern int       _ltimeoff;

/*==============================================================================
  Exported functions
==============================================================================*/
extern char      *_itoa          (i64_t, char*, u8_t, bool, u8_t);
extern int        _dtoa          (double, char*, int, int);
extern double     _strtod        (const char*, char**);
extern float      _strtof        (const char*, char**);
extern i32_t      _atoi          (const char*);
extern char      *_strtoi        (const char*, int, i32_t*);
extern double     _atof          (const char*);
extern u32_t      _mktime        (struct tm*);
extern struct tm *_gmtime_r      (const time_t*, struct tm*);
extern struct tm *_localtime_r   (const time_t*, struct tm*);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _LIB_CONV_H_ */
/*==============================================================================
  End of file
==============================================================================*/
