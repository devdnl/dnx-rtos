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
#include "system.h"


/*==================================================================================================
                                 Exported symbolic constants/macros
==================================================================================================*/
#define print(...)                  fprint(stdout, __VA_ARGS__)
#define scan(format, result)        fscan(stdin, stdout, format, result)
#define putChar(c)                  fputChar(stdout, c)
#define getChar()                   fgetChar(stdin)
#define ugetChar()                  ufgetChar(stdin)
#define clearSTDIN()                fclearSTDIO(stdin)
#define clearSTDOUT()               fclearSTDIO(stdout)

/** VT100 terminal commands */
#define enableLineWrap(level)       level##print("\x1B[?7h")
#define clrscr(level)               level##print("\x1B[2J")
#define eraseLine(level)            level##print("\x1B[2K")
#define eraseLineEnd(level)         level##print("\x1B[K")
#define cursorHome(level)           level##print("\x1B[H")
#define resetAttr(level)            level##print("\x1B[0m")
#define fontBlink(level)            level##print("\x1B[5m")
#define fontUnderl(level)           level##print("\x1B[4m")
#define fontBlack(level)            level##print("\x1B[30m")
#define fontRed(level)              level##print("\x1B[31m")
#define fontGreen(level)            level##print("\x1B[32m")
#define fontYellow(level)           level##print("\x1B[33m")
#define fontBlue(level)             level##print("\x1B[34m")
#define fontMagenta(level)          level##print("\x1B[35m")
#define fontCyan(level)             level##print("\x1B[36m")
#define fontWhite(level)            level##print("\x1B[37m")
#define bgBlack(level)              level##print("\x1B[40m")
#define bgRed(level)                level##print("\x1B[41m")
#define bgGreen(level)              level##print("\x1B[42m")
#define bgYellow(level)             level##print("\x1B[43m")
#define bgBlue(level)               level##print("\x1B[44m")
#define bgMagenta(level)            level##print("\x1B[45m")
#define bgCyan(level)               level##print("\x1B[46m")
#define bgWhite(level)              level##print("\x1B[47m")

/** key definitions */
#define ASCII_ENTER                 0x0D
#define ASCII_BACKSPACE             0x08
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
extern ch_t *itoa(i32_t value, ch_t *buffer, u8_t base, bool_t unsignedValue);
extern u32_t snprint(ch_t *stream, u32_t size, const ch_t *format, ...);
extern u32_t kprint(const ch_t *format, ...);
extern u32_t fprint(stdioFIFO_t *stdout, const ch_t *format, ...);
extern void  kprintEnable(void);
extern void  fputChar(stdioFIFO_t *stdout, ch_t c);
extern ch_t  fgetChar(stdioFIFO_t *stdin);
extern ch_t  ufgetChar(stdioFIFO_t *stdin);
extern void  fclearSTDIO(stdioFIFO_t *stdio);
extern u32_t fscan(stdioFIFO_t *stdin, stdioFIFO_t *stdout, const ch_t *format, void *var);


#ifdef __cplusplus
}
#endif

#endif /* PRINTF_H_ */
/*==================================================================================================
                                            End of file
==================================================================================================*/
