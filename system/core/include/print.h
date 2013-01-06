#ifndef PRINT_H_
#define PRINT_H_
/*=============================================================================================*//**
@file    print.h

@author  Daniel Zorychta

@brief   This file support message printing

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
#include <stdarg.h>
#include "basic_types.h"
#include "systypes.h"
#include "config.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
/** translate function to STDC */
#define printf(...)                             pr_printf(stdout, __VA_ARGS__)
#define fprintf(...)                            pr_printf(__VA_ARGS__)
#define kprint(...)                             pr_kprint(__VA_ARGS__)
#define kprintOK()                              pr_kprintOK()
#define kprintFail()                            pr_kprintFail()
#define kprintErrorNo(errorNo)                  pr_kprintErrorNo(errorNo)
#define kprintEnable(path)                      pr_kprintEnable(path)
#define kprintDisable()                         pr_kprintDisable()
#define snprintf(stream, size, ...)             pr_snprintf(stream, size, __VA_ARGS__)
#define scanf(format, result)                   pr_scanf(stdin, stdout, format, result)
#define putchar(c)                              pr_putchar(stdout, c)
#define getchar()                               pr_getchar(stdin)
#define ugetchar()                              pr_ugetchar(stdin)
#define atoi(string, base, valuePtr)            pr_atoi(string, base, valuePtr)
#define getcwd(buf, size)                       strncpy(buf, cwd, size)

/** VT100 terminal commands */
#define enableLineWrap()                        printf("\x1B[?7h")
#define setNewLineMode()                        printf("\x1B[20h")
#define setLineFeedMode()                       printf("\x1B[20l")
#define clrscr()                                printf("\x1B[2J")
#define eraseLine()                             printf("\x1B[2K")
#define eraseLineEnd()                          printf("\x1B[K")
#define cursorHome()                            printf("\x1B[H")
#define resetAttr()                             printf("\x1B[0m")
#define fontBlink()                             printf("\x1B[5m")
#define fontUnderl()                            printf("\x1B[4m")
#define fontNormal()                            printf("\x1B[0m")
#define fontBold()                              printf("\x1B[1m")
#define fontBlack()                             printf("\x1B[30m")
#define fontRed()                               printf("\x1B[31m")
#define fontGreen()                             printf("\x1B[32m")
#define fontYellow()                            printf("\x1B[33m")
#define fontBlue()                              printf("\x1B[34m")
#define fontMagenta()                           printf("\x1B[35m")
#define fontCyan()                              printf("\x1B[36m")
#define fontWhite()                             printf("\x1B[37m")
#define bgBlack()                               printf("\x1B[40m")
#define bgRed()                                 printf("\x1B[41m")
#define bgGreen()                               printf("\x1B[42m")
#define bgYellow()                              printf("\x1B[43m")
#define bgBlue()                                printf("\x1B[44m")
#define bgMagenta()                             printf("\x1B[45m")
#define bgCyan()                                printf("\x1B[46m")
#define bgWhite()                               printf("\x1B[47m")

/** key definitions */
#define ASCII_LF                                0x0A
#define ASCII_CR                                0x0D
#define ASCII_BS                                0x08
#define ASCII_NULL                              0x00
#define ASCII_ESC                               0x1B
#define ASCII_CANCEL                            0x18


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
#if (CONFIG_PRINT_ENABLE == 1)
extern ch_t  *pr_atoi(ch_t *string, u8_t base, i32_t *value);
extern int_t  pr_snprintf(ch_t *stream, u32_t size, const ch_t *format, ...);
extern int_t  pr_kprint(const ch_t *format, ...);
extern int_t  pr_kprintOK(void);
extern int_t  pr_kprintFail(void);
extern int_t  pr_kprintErrorNo(i8_t errorNo);
extern int_t  pr_printf(FILE_t *file, const ch_t *format, ...);
extern void   pr_kprintEnable(ch_t *filename);
extern void   pr_kprintDisable(void);
extern void   pr_putchar(FILE_t *stdout, ch_t c);
extern ch_t   pr_getchar(FILE_t *stdin);
extern ch_t   pr_ugetchar(FILE_t *stdin);
extern int_t  pr_scanf(FILE_t *stdin, FILE_t *stdout, const ch_t *format, void *var);
#else
#define pr_atoi(string, base, value)                  NULL
#define pr_snprintf(stream, size, format, ...)        0
#define pr_kprint(format, ...)                        0
#define pr_kprintOK()                                 0
#define pr_kprintFail()                               0
#define pr_kprintErrorNo(errorNo)                     0
#define pr_printf(file, format, ...)                  0
#define pr_kprintEnableOn(filename)                   (void)0
#define pr_kprintDisable()                            (void)0
#define pr_putchar(stdout, c)                         (void)0
#define pr_getchar(stdin)                             '\0'
#define pr_ugetchar(stdin)                            '\0'
#define pr_scanf(stdin, stdout, format, var)          0
#endif

#ifdef __cplusplus
}
#endif

#endif /* PRINT_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
