#ifndef APPRUNTIME_H_
#define APPRUNTIME_H_
/*=========================================================================*//**
@file    appruntime.h

@author  Daniel Zorychta

@brief   This file support runtime environment for applications

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


*//*==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include "systypes.h"
#include "oswrap.h"

/*==============================================================================
  Exported symbolic constants/macros
==============================================================================*/
/** program section */
#define PROGRAM(name, stackMultiple)    const uint_t name##_stack_size =    \
                                        stackMultiple * MINIMAL_STACK_SIZE; \
                                        void name(void *_progarg_)

#define EXTERN_PROGRAM(name)            extern const uint_t name##_stack_size; \
                                        extern void name(void *_progarg_)



#define PROG_SEC_BEGIN                  {FILE_t *stdin  = ((prog_t*)_progarg_)->stdin; \
                                         FILE_t *stdout = ((prog_t*)_progarg_)->stdout;\
                                         ch_t   **argv  = ((prog_t*)_progarg_)->argv;  \
                                         int_t   argc   = ((prog_t*)_progarg_)->argc;  \
                                         ch_t   *cwd    = ((prog_t*)_progarg_)->cwd;   \
                                         (void)stdin; (void)stdout; (void)argv;        \
                                         (void)argc;  (void)cwd;

#define PROG_SEC_END                    exit(appmain(argv, argc));}

/** simpler definition of terminating application */
#define exit(exitCode)                  CloseProg(_progarg_, exitCode)

/*==============================================================================
  Exported types, enums definitions
==============================================================================*/
/** type which define parse possibilities */
typedef enum parseType_enum {
        PARSE_AS_BIN,
        PARSE_AS_OCT,
        PARSE_AS_DEC,
        PARSE_AS_HEX,
        PARSE_AS_STRING,
        PARSE_AS_CHAR,
        PARSE_AS_EXIST,
        PARSE_AS_UNKNOWN
} parseType_t;

/*==============================================================================
  Exported object declarations
==============================================================================*/

/*==============================================================================
  Exported function prototypes
==============================================================================*/
extern prog_t    *exec(const ch_t *name, ch_t *argv);
extern stdRet_t   StartDaemon(const ch_t *name, ch_t *argv);
extern stdRet_t   KillProg(prog_t *appArgs);
extern void       CloseProg(prog_t *appObj, stdRet_t exitCode);
extern stdRet_t   ParseArg(ch_t *argv, ch_t *findArg, parseType_t parseAs, void *result);

#ifdef __cplusplus
}
#endif

#endif /* APPRUNTIME_H_ */
/*==============================================================================
  End of file
==============================================================================*/
