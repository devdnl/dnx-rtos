/*=========================================================================*//**
@file    program_registration.c

@author  Daniel Zorychta

@brief   This file is used to register user program.

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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "kernel/kwrapper.h"
#include "core/progman.h"

/*==============================================================================
  External objects
==============================================================================*/
_IMPORT_PROGRAM(helloworld);
_IMPORT_PROGRAM(top);
_IMPORT_PROGRAM(terminal);
_IMPORT_PROGRAM(cat);
_IMPORT_PROGRAM(cp);
_IMPORT_PROGRAM(telnet);
_IMPORT_PROGRAM(httpserver);
_IMPORT_PROGRAM(ifconfig);

/*==============================================================================
  Exported objects
==============================================================================*/
const struct _prog_data _prog_table[] = {
        _PROGRAM_CONFIG(helloworld, STACK_DEPTH_VERY_LOW),
        _PROGRAM_CONFIG(top       , STACK_DEPTH_VERY_LOW),
        _PROGRAM_CONFIG(terminal  , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(cat       , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(cp        , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(telnet    , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(httpserver, STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(ifconfig  , STACK_DEPTH_VERY_LOW),
};

const int _prog_table_size = ARRAY_SIZE(_prog_table);

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
