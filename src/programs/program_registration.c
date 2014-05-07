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

/*==============================================================================
  Include files
==============================================================================*/
#include <dnx/misc.h>
#include "kernel/kwrapper.h"
#include "core/progman.h"

/*==============================================================================
  External objects
==============================================================================*/
_IMPORT_PROGRAM(card_init);
_IMPORT_PROGRAM(cat);
_IMPORT_PROGRAM(clear);
_IMPORT_PROGRAM(cp);
_IMPORT_PROGRAM(df);
_IMPORT_PROGRAM(dsh);
_IMPORT_PROGRAM(echo);
_IMPORT_PROGRAM(free);
_IMPORT_PROGRAM(fsstack);
_IMPORT_PROGRAM(helloworld);
_IMPORT_PROGRAM(httpserver);
_IMPORT_PROGRAM(ifconfig);
_IMPORT_PROGRAM(ls);
_IMPORT_PROGRAM(lsmod);
_IMPORT_PROGRAM(mkdir);
_IMPORT_PROGRAM(mkfifo);
_IMPORT_PROGRAM(mknod);
_IMPORT_PROGRAM(modinit);
_IMPORT_PROGRAM(mount);
_IMPORT_PROGRAM(reboot);
_IMPORT_PROGRAM(rm);
_IMPORT_PROGRAM(sync);
_IMPORT_PROGRAM(telnet);
_IMPORT_PROGRAM(time);
_IMPORT_PROGRAM(top);
_IMPORT_PROGRAM(touch);
_IMPORT_PROGRAM(umount);
_IMPORT_PROGRAM(uname);
_IMPORT_PROGRAM(uptime);

/*==============================================================================
  Exported objects
==============================================================================*/
const struct _prog_data _prog_table[] = {
        _PROGRAM_CONFIG(card_init , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(cat       , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(clear     , STACK_DEPTH_MINIMAL ),
        _PROGRAM_CONFIG(cp        , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(df        , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(dsh       , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(echo      , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(free      , STACK_DEPTH_VERY_LOW),
        _PROGRAM_CONFIG(fsstack   , STACK_DEPTH_LARGE   ),
        _PROGRAM_CONFIG(helloworld, STACK_DEPTH_VERY_LOW),
        _PROGRAM_CONFIG(httpserver, STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(ifconfig  , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(ls        , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(lsmod     , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(mkdir     , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(mkfifo    , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(mknod     , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(modinit   , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(mount     , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(reboot    , STACK_DEPTH_MINIMAL ),
        _PROGRAM_CONFIG(rm        , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(sync      , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(telnet    , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(time      , STACK_DEPTH_VERY_LOW),
        _PROGRAM_CONFIG(top       , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(touch     , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(umount    , STACK_DEPTH_LOW     ),
        _PROGRAM_CONFIG(uname     , STACK_DEPTH_MINIMAL ),
        _PROGRAM_CONFIG(uptime    , STACK_DEPTH_MINIMAL ),
};

const int _prog_table_size = ARRAY_SIZE(_prog_table);

/*==============================================================================
  End of file
==============================================================================*/
