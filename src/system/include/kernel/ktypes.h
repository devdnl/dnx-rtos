/*=========================================================================*//**
@file    ktypes.h

@author  Daniel Zorychta

@brief   This file contains kernel types

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _KTYPES_H_
#define _KTYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <sys/types.h>

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
/** KERNELSPACE: resource type */
typedef enum {
        RES_TYPE_UNKNOWN       = 0,
        RES_TYPE_PROCESS       = 0x958701BA,
        RES_TYPE_THREAD        = 0x1EE62243,
        RES_TYPE_MUTEX         = 0x300C6B74,
        RES_TYPE_SEMAPHORE     = 0x4E59901B,
        RES_TYPE_QUEUE         = 0x83D50ADB,
        RES_TYPE_FILE          = 0x7D129250,
        RES_TYPE_DIR           = 0x19586E97,
        RES_TYPE_MEMORY        = 0x9E834645
} res_type_t;

/** KERNELSPACE: object header (must be the first in object) */
typedef struct res_header {
        struct res_header *next;
        res_type_t         type;
} res_header_t;

/** KERNELSPACE: task type */
typedef void task_t;

/** KERNELSPACE: task function type */
typedef void (*task_func_t)(void*);

/** KERNELSPACE/USERSPACE: semaphore type */
typedef struct {
        res_header_t  header;
        void         *object;
} sem_t;

/** KERNELSPACE/USERSPACE: queue type */
typedef struct {
        res_header_t  header;
        void         *object;
} queue_t;

/** KERNELSPACE/USERSPACE: mutex type */
typedef struct {
        res_header_t  header;
        void         *object;
        bool          recursive;
} mutex_t;

/*==============================================================================
   Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _KTYPES_H_ */
/*==============================================================================
  End of file
==============================================================================*/
