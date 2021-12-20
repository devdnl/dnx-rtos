/*=========================================================================*//**
@file    ktypes.h

@author  Daniel Zorychta

@brief   This file contains kernel types

@note    Copyright (C) 2012, 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _KTYPES_H_
#define _KTYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "lib/sys/types.h"
#include <stdbool.h>
#include <FreeRTOS.h>

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
        RES_TYPE_MUTEX         = 0x300C6B74,
        RES_TYPE_SEMAPHORE     = 0x4E59901B,
        RES_TYPE_QUEUE         = 0x83D50ADB,
        RES_TYPE_FILE          = 0x7D129250,
        RES_TYPE_DIR           = 0x19586E97,
        RES_TYPE_MEMORY        = 0x9E834645,
        RES_TYPE_SOCKET        = 0x63ACC316,
        RES_TYPE_FLAG          = 0x18FAEC0D
} res_type_t;

/** KERNELSPACE: object header (must be the first in object) */
typedef struct res_header {
        void              *self;
        struct res_header *next;
        res_type_t         type;
} res_header_t;

/** KERNELSPACE: task type */
typedef void task_t;

/** KERNELSPACE: task function type */
typedef void (*task_func_t)(void*);

/** KERNELSPACE/USERSPACE: semaphore type */
typedef struct {
        res_header_t      header;
        void             *object;
        StaticSemaphore_t buffer;
} ksem_t;

/** KERNELSPACE/USERSPACE: queue type */
typedef struct {
        res_header_t  header;
        void         *object;
        StaticQueue_t buffer;
        uint8_t       storage[];
} kqueue_t;

/** KERNELSPACE/USERSPACE: mutex type */
typedef struct {
        res_header_t      header;
        void             *object;
        StaticSemaphore_t buffer;
        bool              recursive;
} kmtx_t;

/** KERNELSPACE: flag type */
typedef struct {
        res_header_t       header;
        void              *object;
        StaticEventGroup_t buffer;
} kflag_t;

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
