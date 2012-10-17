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


/*==================================================================================================
                                  Exported symbolic constants/macros
==================================================================================================*/
#define configSTDIO_BUFFER_SIZE           100
#define constKPRINT_BUFFER_SIZE           80


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/
/** default system status */
enum status_enum
{
   STD_RET_OK                 = 0,
   STD_RET_ERROR              = 1,
   STD_RET_ALLOCERROR         = 2,
   STD_RET_UNKNOWN            = 127,
};


/** universal status type */
typedef signed char stdRet_t;


/** device number type */
typedef u8_t dev_t;


/** IO request type */
typedef u8_t IORq_t;


/** file type */
typedef struct
{
      stdRet_t (*close)(dev_t);
      size_t   (*write)(dev_t, void*, size_t, size_t);
      size_t   (*read)(dev_t, void*, size_t, size_t);
      stdRet_t (*ioctl)(dev_t, IORq_t, void*);
} FILE_t;


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
      void        *arg;                         /* pointer to the argument */
      stdioFIFO_t *stdin;                       /* stdin fifo */
      stdioFIFO_t *stdout;                      /* stdout fifo */
      void        *ChildTaskHandle;             /* FreeRTOS task handling for children */
      void        *ParentTaskHandle;            /* FreeRTOS task handling for parent */
      stdRet_t    exitCode;                     /* exit code */
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
