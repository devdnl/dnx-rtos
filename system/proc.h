#ifndef PROC_H_
#define PROC_H_
/*=============================================================================================*//**
@file    proc.h

@author  Daniel Zorychta

@brief   This file support /proc files

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "basic_types.h"
#include "systypes.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
typedef struct diren
{
      ch_t  *name;
      size_t size;
} diren_t;

typedef size_t procfd_t;


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern stdRet_t PROC_RmFile(const ch_t *name);
extern size_t   PROC_GetFileSize(procfd_t fd);
extern procfd_t PROC_open(const ch_t *name);
extern size_t   PROC_write(nod_t fd, void *src, size_t size, size_t nitems, size_t seek);
extern size_t   PROC_read(nod_t fd, void *dst, size_t size, size_t nitmes, size_t seek);
extern diren_t  PROC_readdir(u16_t fno);


#ifdef __cplusplus
}
#endif

#endif /* PROC_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
