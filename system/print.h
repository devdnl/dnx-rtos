#ifndef PRINTF_H_
#define PRINTF_H_
/*=============================================================================================*//**
@file    printf.h

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


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
#define printf(...)                 printt(tty, __VA_ARGS__)
#define scanf(format, result)       scant(tty, format, result)
#define putChar(c)                  putChart(tty, c)
#define getChar()                   getChart(tty)
#define ugetChar()                  ugetChart(tty)

/** VT100 terminal commands */
#define enableLineWrap(level)       printf("\x1B[?7h")
#define setNewLineMode(level)       printf("\x1B[20h")
#define setLineFeedMode(level)      printf("\x1B[20l")
#define clrscr()                    printf("\x1B[2J")
#define eraseLine(level)            printf("\x1B[2K")
#define eraseLineEnd(level)         printf("\x1B[K")
#define cursorHome(level)           printf("\x1B[H")
#define resetAttr(level)            printf("\x1B[0m")
#define fontBlink(level)            printf("\x1B[5m")
#define fontUnderl(level)           printf("\x1B[4m")
#define fontNormal(level)           printf("\x1B[0m")
#define fontBold(level)             printf("\x1B[1m")
#define fontBlack(level)            printf("\x1B[30m")
#define fontRed(level)              printf("\x1B[31m")
#define fontGreen(level)            printf("\x1B[32m")
#define fontYellow(level)           printf("\x1B[33m")
#define fontBlue(level)             printf("\x1B[34m")
#define fontMagenta(level)          printf("\x1B[35m")
#define fontCyan()                  printf("\x1B[36m")
#define fontWhite(level)            printf("\x1B[37m")
#define bgBlack(level)              printf("\x1B[40m")
#define bgRed(level)                printf("\x1B[41m")
#define bgGreen(level)              printf("\x1B[42m")
#define bgYellow(level)             printf("\x1B[43m")
#define bgBlue(level)               printf("\x1B[44m")
#define bgMagenta(level)            printf("\x1B[45m")
#define bgCyan(level)               printf("\x1B[46m")
#define bgWhite(level)              printf("\x1B[47m")

/** key definitions */
#define ASCII_LF                    0x0A
#define ASCII_CR                    0x0D
#define ASCII_BS                    0x08
#define ASCII_NULL                  0x00
#define ASCII_ESC                   0x1B
#define ASCII_CANCEL                0x18


/*==================================================================================================
                                  Exported types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                     Exported object declarations
==================================================================================================*/


/*==================================================================================================
                                     Exported function prototypes
==================================================================================================*/
extern ch_t *itoa(i32_t value, ch_t *buffer, u8_t base, bool_t unsignedValue, u8_t zerosRequired);
extern ch_t *atoi(ch_t *string, u8_t base, i32_t *value);
extern u32_t snprint(ch_t *stream, u32_t size, const ch_t *format, ...);
extern u32_t kprint(const ch_t *format, ...);
extern u32_t kprintOK(void);
extern u32_t kprintFail(void);
extern u32_t kprintErrorNo(i8_t errorNo);
extern u32_t printt(u8_t tty, const ch_t *format, ...);
extern void  EnableKprint(void);
extern void  DisableKprint(void);
extern void  ChangekprintTTY(u8_t tty);
extern void  putChart(u8_t tty, ch_t c);
extern ch_t  getChart(u8_t tty);
extern ch_t  ugetChart(u8_t tty);
extern u32_t scant(u8_t tty, const ch_t *format, void *var);

#ifdef __cplusplus
}
#endif

#endif /* PRINTF_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
