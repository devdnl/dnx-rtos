#ifndef SYSTYPES_H_
#define SYSTYPES_H_
/*=============================================================================================*//**
@file    systypes.h

@author  Daniel Zorychta

@brief   This file contains all system types

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
#include "FreeRTOSConfig.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
/** default system status */
typedef enum status_enum
{
   STD_STATUS_OK         = 0,
   STD_STATUS_ERROR      = 1,
} stdStatus_t;


/** device number type */
typedef u8_t dev_t;


/** IO request type */
typedef u8_t IORq_t;


/** stdio FIFO type */
typedef struct stdioFIFO_struct
{
      u16_t Level;                                  /* load level */
      u16_t TxIdx;                                  /* fifo Tx index */
      u16_t RxIdx;                                  /* fifo Rx index */
      ch_t  Buffer[configSTDIO_BUFFER_SIZE];        /* fifo buffer */
} stdioFIFO_t;


/** application standard arguments type */
typedef struct appArgs_struct
{
      void        *arg;                      /* pointer to the argument */
      stdioFIFO_t stdin;                     /* stdin fifo */
      stdioFIFO_t stdout;                    /* stdout fifo */
} appArgs_t;


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/


#ifdef __cplusplus
   }
#endif

#endif /* SYSTYPES_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
