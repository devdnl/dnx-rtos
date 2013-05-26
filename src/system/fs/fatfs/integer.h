/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/

#ifndef _INTEGER
#define _INTEGER

#ifdef _WIN32        /* FatFs development platform */

#include <windows.h>
#include <tchar.h>

#else                        /* Embedded platform */
#include "core/systypes.h"

/* These types must be 16-bit, 32-bit or larger integer */
typedef int                     INT;
typedef unsigned int            UINT;

/* These types must be 8-bit integer */
typedef char                    CHAR;
typedef u8_t                    UCHAR;
typedef u8_t                    BYTE;

/* These types must be 16-bit integer */
typedef i16_t                   SHORT;
typedef u16_t                   USHORT;
typedef u16_t                   WORD;
typedef u16_t                   WCHAR;

/* These types must be 32-bit integer */
typedef i32_t                   LONG;
typedef u32_t                   ULONG;
typedef u32_t                   DWORD;

#endif

#endif
