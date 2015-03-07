/*=========================================================================*//**
@file    conv.h

@author  Daniel Zorychta

@brief   Module with calculation and convert functions.

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

#ifndef _CONV_H_
#define _CONV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>

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
extern double     _strtod        (const char*, char**);
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

#endif /* _CONV_H_ */
/*==============================================================================
  End of file
==============================================================================*/
