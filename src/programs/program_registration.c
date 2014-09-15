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
_IMPORT_PROGRAM(keyboard);
_IMPORT_PROGRAM(ls);
_IMPORT_PROGRAM(lsmod);
_IMPORT_PROGRAM(mbus_daemon);
_IMPORT_PROGRAM(mkdir);
_IMPORT_PROGRAM(mkfifo);
_IMPORT_PROGRAM(mknod);
_IMPORT_PROGRAM(modinit);
_IMPORT_PROGRAM(mount);
_IMPORT_PROGRAM(mouse);
_IMPORT_PROGRAM(reboot);
_IMPORT_PROGRAM(rm);
_IMPORT_PROGRAM(sync);
_IMPORT_PROGRAM(serial);
_IMPORT_PROGRAM(storage);
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
        _PROGRAM_CONFIG(card_init  ),
        _PROGRAM_CONFIG(cat        ),
        _PROGRAM_CONFIG(clear      ),
        _PROGRAM_CONFIG(cp         ),
        _PROGRAM_CONFIG(df         ),
        _PROGRAM_CONFIG(dsh        ),
        _PROGRAM_CONFIG(echo       ),
        _PROGRAM_CONFIG(free       ),
        _PROGRAM_CONFIG(fsstack    ),
        _PROGRAM_CONFIG(helloworld ),
        _PROGRAM_CONFIG(httpserver ),
        _PROGRAM_CONFIG(ifconfig   ),
        _PROGRAM_CONFIG(keyboard   ),
        _PROGRAM_CONFIG(ls         ),
        _PROGRAM_CONFIG(lsmod      ),
        _PROGRAM_CONFIG(mbus_daemon),
        _PROGRAM_CONFIG(mkdir      ),
        _PROGRAM_CONFIG(mkfifo     ),
        _PROGRAM_CONFIG(mknod      ),
        _PROGRAM_CONFIG(modinit    ),
        _PROGRAM_CONFIG(mount      ),
        _PROGRAM_CONFIG(mouse      ),
        _PROGRAM_CONFIG(reboot     ),
        _PROGRAM_CONFIG(rm         ),
        _PROGRAM_CONFIG(sync       ),
        _PROGRAM_CONFIG(serial     ),
        _PROGRAM_CONFIG(storage    ),
        _PROGRAM_CONFIG(telnet     ),
        _PROGRAM_CONFIG(time       ),
        _PROGRAM_CONFIG(top        ),
        _PROGRAM_CONFIG(touch      ),
        _PROGRAM_CONFIG(umount     ),
        _PROGRAM_CONFIG(uname      ),
        _PROGRAM_CONFIG(uptime     ),
};

const int _prog_table_size = ARRAY_SIZE(_prog_table);

/*==============================================================================
  End of file
==============================================================================*/
