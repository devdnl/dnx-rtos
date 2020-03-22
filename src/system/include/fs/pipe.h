/*=========================================================================*//**
@file    pipe.h

@author  Daniel Zorychta

@brief   File support creating of pipies in file systems.

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

#ifndef _PIPE_H_
#define _PIPE_H_

/*==============================================================================
  Include files
==============================================================================*/
#include <stddef.h>
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
typedef struct pipe pipe_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/
extern int  _pipe_create    (pipe_t**);
extern int  _pipe_destroy   (pipe_t*);
extern int  _pipe_get_length(pipe_t*, size_t*);
extern int  _pipe_read      (pipe_t*, u8_t*, size_t, size_t*, bool);
extern int  _pipe_write     (pipe_t*, const u8_t*, size_t, size_t*, bool);
extern int  _pipe_close     (pipe_t*);
extern int  _pipe_clear     (pipe_t*);
extern int  _pipe_permanent (pipe_t*);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _PIPE_H_ */
/*==============================================================================
  End of file
==============================================================================*/
