/*=========================================================================*//**
@file    strerror.c

@author  Daniel Zorychta

@brief   strerror() function implementation. User space code.

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

/*==============================================================================
  Include files
==============================================================================*/
#include <string.h>
#include <errno.h>

/*==============================================================================
  Local macros
==============================================================================*/
#define TO_STR(str)                     #str
#define NUMBER_TO_STR(val)              TO_STR(val)

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function returns error string
 *
 * @param errnum        error number
 *
 * @return error number string
 */
//==============================================================================
const char *strerror(int errnum)
{
        static const char *errstr[] = {
                [ESUCC       ] = "Success",
                [EPERM       ] = "Operation not permitted",
                [ENOENT      ] = "No such file or directory",
                [ESRCH       ] = "No such process",
                [EIO         ] = "I/O error",
                [ENXIO       ] = "No such device or address",
                [E2BIG       ] = "Argument list too long",
                [ENOEXEC     ] = "Exec format error",
                [EAGAIN      ] = "Try again",
                [ENOMEM      ] = "Out of memory",
                [EACCES      ] = "Permission denied",
                [EFAULT      ] = "Bad address",
                [EBUSY       ] = "Device or resource busy",
                [EEXIST      ] = "File exists",
                [ENODEV      ] = "No such device",
                [ENOTDIR     ] = "Not a directory",
                [EISDIR      ] = "Is a directory",
                [EINVAL      ] = "Invalid argument",
                [EMFILE      ] = "Too many open files",
                [EFBIG       ] = "File too large",
                [ENOSPC      ] = "No space left on device",
                [ESPIPE      ] = "Illegal seek",
                [EROFS       ] = "Read-only file system",
                [EDOM        ] = "Math argument out of domain of function",
                [ERANGE      ] = "Math result not representable",
                [EILSEQ      ] = "Illegal byte sequence",
                [ENAMETOOLONG] = "File name too long",
                [ENOTEMPTY   ] = "Directory not empty",
                [EBADRQC     ] = "Invalid request code",
                [ETIME       ] = "Timer expired",
                [ENONET      ] = "Machine is not on the network",
                [EUSERS      ] = "Too many users",
                [EADDRINUSE  ] = "Address already in use",
                [ENOMEDIUM   ] = "No medium found",
                [EMEDIUMTYPE ] = "Wrong medium type",
                [ECANCELED   ] = "Operation Canceled",
                [ENOTSUP     ] = "Not supported",
                [ENOSYS      ] = "Function not implemented",
                [ECONNABORTED] = "Connection aborted",
                [ECONNREFUSED] = "Connection refused",
                [ECONNRESET  ] = "Connection reset",
                [EISCONN     ] = "Socket is connected",
                [EALREADY    ] = "Connection already in progress",
        };

        if ((errnum >= 0) && (errnum < _ENUMBER)) {
                return errstr[errnum];
        } else {
                return "Unknown error";
        }
}

/*==============================================================================
  End of file
==============================================================================*/
