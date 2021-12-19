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
#include <config.h>
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
#if (__OS_PRINTF_ENABLE__ > 0)
        static const char *errstr[] = {
#if (__OS_ERRNO_STRING_LEN__ == 0)
                /* empty */
#elif (__OS_ERRNO_STRING_LEN__ == 1)
                [ESUCC       ] = NUMBER_TO_STR(ESUCC),
                [EPERM       ] = NUMBER_TO_STR(EPERM),
                [ENOENT      ] = NUMBER_TO_STR(ENOENT),
                [ESRCH       ] = NUMBER_TO_STR(ESRCH),
                [EIO         ] = NUMBER_TO_STR(EIO),
                [ENXIO       ] = NUMBER_TO_STR(ENXIO),
                [E2BIG       ] = NUMBER_TO_STR(E2BIG),
                [ENOEXEC     ] = NUMBER_TO_STR(ENOEXEC),
                [EAGAIN      ] = NUMBER_TO_STR(EAGAIN),
                [ENOMEM      ] = NUMBER_TO_STR(ENOMEM),
                [EACCES      ] = NUMBER_TO_STR(EACCES),
                [EFAULT      ] = NUMBER_TO_STR(EFAULT),
                [EBUSY       ] = NUMBER_TO_STR(EBUSY),
                [EEXIST      ] = NUMBER_TO_STR(EEXIST),
                [ENODEV      ] = NUMBER_TO_STR(ENODEV),
                [ENOTDIR     ] = NUMBER_TO_STR(ENOTDIR),
                [EISDIR      ] = NUMBER_TO_STR(EISDIR),
                [EINVAL      ] = NUMBER_TO_STR(EINVAL),
                [EMFILE      ] = NUMBER_TO_STR(EMFILE),
                [EFBIG       ] = NUMBER_TO_STR(EFBIG),
                [ENOSPC      ] = NUMBER_TO_STR(ENOSPC),
                [ESPIPE      ] = NUMBER_TO_STR(ESPIPE),
                [EROFS       ] = NUMBER_TO_STR(EROFS),
                [EDOM        ] = NUMBER_TO_STR(EDOM),
                [ERANGE      ] = NUMBER_TO_STR(ERANGE),
                [EILSEQ      ] = NUMBER_TO_STR(EILSEQ),
                [ENAMETOOLONG] = NUMBER_TO_STR(ENAMETOOLONG),
                [ENOTEMPTY   ] = NUMBER_TO_STR(ENOTEMPTY),
                [EBADRQC     ] = NUMBER_TO_STR(EBADRQC),
                [ETIME       ] = NUMBER_TO_STR(ETIME),
                [ENONET      ] = NUMBER_TO_STR(ENONET),
                [EUSERS      ] = NUMBER_TO_STR(EUSERS),
                [EADDRINUSE  ] = NUMBER_TO_STR(EADDRINUSE),
                [ENOMEDIUM   ] = NUMBER_TO_STR(ENOMEDIUM),
                [EMEDIUMTYPE ] = NUMBER_TO_STR(EMEDIUMTYPE),
                [ECANCELED   ] = NUMBER_TO_STR(ECANCELED),
                [ENOTSUP     ] = NUMBER_TO_STR(ENOTSUP),
                [ENOSYS      ] = NUMBER_TO_STR(ENOSYS),
                [ECONNABORTED] = NUMBER_TO_STR(ECONNABORTED),
                [ECONNREFUSED] = NUMBER_TO_STR(ECONNREFUSED),
                [ECONNRESET  ] = NUMBER_TO_STR(ECONNRESET),
                [EISCONN     ] = NUMBER_TO_STR(EISCONN),
                [EALREADY    ] = NUMBER_TO_STR(EALREADY),
#elif (__OS_ERRNO_STRING_LEN__ == 2)
                [ESUCC       ] = TO_STR(ESUCC),
                [EPERM       ] = TO_STR(EPERM),
                [ENOENT      ] = TO_STR(ENOENT),
                [ESRCH       ] = TO_STR(ESRCH),
                [EIO         ] = TO_STR(EIO),
                [ENXIO       ] = TO_STR(ENXIO),
                [E2BIG       ] = TO_STR(E2BIG),
                [ENOEXEC     ] = TO_STR(ENOEXEC),
                [EAGAIN      ] = TO_STR(EAGAIN),
                [ENOMEM      ] = TO_STR(ENOMEM),
                [EACCES      ] = TO_STR(EACCES),
                [EFAULT      ] = TO_STR(EFAULT),
                [EBUSY       ] = TO_STR(EBUSY),
                [EEXIST      ] = TO_STR(EEXIST),
                [ENODEV      ] = TO_STR(ENODEV),
                [ENOTDIR     ] = TO_STR(ENOTDIR),
                [EISDIR      ] = TO_STR(EISDIR),
                [EINVAL      ] = TO_STR(EINVAL),
                [EMFILE      ] = TO_STR(EMFILE),
                [EFBIG       ] = TO_STR(EFBIG),
                [ENOSPC      ] = TO_STR(ENOSPC),
                [ESPIPE      ] = TO_STR(ESPIPE),
                [EROFS       ] = TO_STR(EROFS),
                [EDOM        ] = TO_STR(EDOM),
                [ERANGE      ] = TO_STR(ERANGE),
                [EILSEQ      ] = TO_STR(EILSEQ),
                [ENAMETOOLONG] = TO_STR(ENAMETOOLONG),
                [ENOTEMPTY   ] = TO_STR(ENOTEMPTY),
                [EBADRQC     ] = TO_STR(EBADRQC),
                [ETIME       ] = TO_STR(ETIME),
                [ENONET      ] = TO_STR(ENONET),
                [EUSERS      ] = TO_STR(EUSERS),
                [EADDRINUSE  ] = TO_STR(EADDRINUSE),
                [ENOMEDIUM   ] = TO_STR(ENOMEDIUM),
                [EMEDIUMTYPE ] = TO_STR(EMEDIUMTYPE),
                [ECANCELED   ] = TO_STR(ECANCELED),
                [ENOTSUP     ] = TO_STR(ENOTSUP),
                [ENOSYS      ] = TO_STR(ENOSYS),
                [ECONNABORTED] = TO_STR(ECONNABORTED),
                [ECONNREFUSED] = TO_STR(ECONNREFUSED),
                [ECONNRESET  ] = TO_STR(ECONNRESET),
                [EISCONN     ] = TO_STR(EISCONN),
                [EALREADY    ] = TO_STR(EALREADY),
#elif (__OS_ERRNO_STRING_LEN__ == 3)
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
#else
#error "__OS_ERRNO_STRING_LEN__ should be in range 0 - 3!"
#endif
        };

        if (__OS_ERRNO_STRING_LEN__ == 0) {
                return "";
        } else if ((errnum >= 0) && (errnum < _ENUMBER)) {
                return errstr[errnum];
        } else {
                return "Unknown error";
        }
#else
        (void) errnum;
        return "";
#endif
}

/*==============================================================================
  End of file
==============================================================================*/
