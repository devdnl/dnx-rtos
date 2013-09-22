/*=========================================================================*//**
@file    libfat.c

@author  Daniel Zorychta

@brief   FAT file system library based od ChaN's code.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

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
/*-----------------------------------------------------------------------------/
/  FatFs - FAT file system module  R0.09b                 (C)ChaN, 2013
/------------------------------------------------------------------------------/
/ FatFs module is a generic FAT file system module for small embedded systems.
/ This is a free software that opened for education, research and commercial
/ developments under license policy of following terms.
/
/  Copyright (C) 2013, ChaN, all right reserved.
/
/ * The FatFs module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-----------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <string.h>
#include "libfat.h"
#include "libfat_user.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
/* Definitions on sector size */
#if _LIBFAT_MAX_SS != 512 && _LIBFAT_MAX_SS != 1024 && _LIBFAT_MAX_SS != 2048 && _LIBFAT_MAX_SS != 4096
#       error Wrong sector size.
#endif

#if _LIBFAT_MAX_SS != 512
#       define SS(fs)           ((fs)->ssize)   /* Variable sector size */
#else
#       define SS(fs)           512U            /* Fixed sector size */
#endif

#define ENTER_FF(fs)            {if (!lock_fs(fs)) return FR_TIMEOUT;}
#define LEAVE_FF(fs, res)       {unlock_fs(fs, res); return res;}
#define ABORT(fs, res)          {fp->flag |= LIBFAT_FA__ERROR; LEAVE_FF(fs, res);}

/* DBCS code ranges and SBCS extend char conversion table */
#if   _LIBFAT_CODE_PAGE == 932        /* Japanese Shift-JIS */
#define _DF1S        0x81             /* DBC 1st byte range 1 start */
#define _DF1E        0x9F             /* DBC 1st byte range 1 end */
#define _DF2S        0xE0             /* DBC 1st byte range 2 start */
#define _DF2E        0xFC             /* DBC 1st byte range 2 end */
#define _DS1S        0x40             /* DBC 2nd byte range 1 start */
#define _DS1E        0x7E             /* DBC 2nd byte range 1 end */
#define _DS2S        0x80             /* DBC 2nd byte range 2 start */
#define _DS2E        0xFC             /* DBC 2nd byte range 2 end */

#elif _LIBFAT_CODE_PAGE == 936        /* Simplified Chinese GBK */
#define _DF1S        0x81
#define _DF1E        0xFE
#define _DS1S        0x40
#define _DS1E        0x7E
#define _DS2S        0x80
#define _DS2E        0xFE

#elif _LIBFAT_CODE_PAGE == 949        /* Korean */
#define _DF1S        0x81
#define _DF1E        0xFE
#define _DS1S        0x41
#define _DS1E        0x5A
#define _DS2S        0x61
#define _DS2E        0x7A
#define _DS3S        0x81
#define _DS3E        0xFE

#elif _LIBFAT_CODE_PAGE == 950        /* Traditional Chinese Big5 */
#define _DF1S        0x81
#define _DF1E        0xFE
#define _DS1S        0x40
#define _DS1E        0x7E
#define _DS2S        0xA1
#define _DS2E        0xFE

#elif _LIBFAT_CODE_PAGE == 437        /* U.S. (OEM) */
#define _DF1S        0
#define _EXCVT {0x80,0x9A,0x90,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x49,0x49,0x49,0x8E,0x8F,0x90,0x92,0x92,0x4F,0x99,0x4F,0x55,0x55,0x59,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
                0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _LIBFAT_CODE_PAGE == 720        /* Arabic (OEM) */
#define _DF1S        0
#define _EXCVT {0x80,0x81,0x45,0x41,0x84,0x41,0x86,0x43,0x45,0x45,0x45,0x49,0x49,0x8D,0x8E,0x8F,0x90,0x92,0x92,0x93,0x94,0x95,0x49,0x49,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
                0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _LIBFAT_CODE_PAGE == 737        /* Greek (OEM) */
#define _DF1S        0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x92,0x92,0x93,0x94,0x95,0x96,0x97,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87, \
                0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0xAA,0x92,0x93,0x94,0x95,0x96,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0x97,0xEA,0xEB,0xEC,0xE4,0xED,0xEE,0xE7,0xE8,0xF1,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _LIBFAT_CODE_PAGE == 775        /* Baltic (OEM) */
#define _DF1S        0
#define _EXCVT {0x80,0x9A,0x91,0xA0,0x8E,0x95,0x8F,0x80,0xAD,0xED,0x8A,0x8A,0xA1,0x8D,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0x95,0x96,0x97,0x97,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
                0xA0,0xA1,0xE0,0xA3,0xA3,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xB5,0xB6,0xB7,0xB8,0xBD,0xBE,0xC6,0xC7,0xA5,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE3,0xE8,0xE8,0xEA,0xEA,0xEE,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _LIBFAT_CODE_PAGE == 850        /* Multilingual Latin 1 (OEM) */
#define _DF1S        0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0xDE,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x59,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
                0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE7,0xE9,0xEA,0xEB,0xED,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _LIBFAT_CODE_PAGE == 852        /* Latin 2 (OEM) */
#define _DF1S        0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xDE,0x8F,0x80,0x9D,0xD3,0x8A,0x8A,0xD7,0x8D,0x8E,0x8F,0x90,0x91,0x91,0xE2,0x99,0x95,0x95,0x97,0x97,0x99,0x9A,0x9B,0x9B,0x9D,0x9E,0x9F, \
                0xB5,0xD6,0xE0,0xE9,0xA4,0xA4,0xA6,0xA6,0xA8,0xA8,0xAA,0x8D,0xAC,0xB8,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBD,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC6,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD1,0xD1,0xD2,0xD3,0xD2,0xD5,0xD6,0xD7,0xB7,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE3,0xD5,0xE6,0xE6,0xE8,0xE9,0xE8,0xEB,0xED,0xED,0xDD,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xEB,0xFC,0xFC,0xFE,0xFF}

#elif _LIBFAT_CODE_PAGE == 855        /* Cyrillic (OEM) */
#define _DF1S        0
#define _EXCVT {0x81,0x81,0x83,0x83,0x85,0x85,0x87,0x87,0x89,0x89,0x8B,0x8B,0x8D,0x8D,0x8F,0x8F,0x91,0x91,0x93,0x93,0x95,0x95,0x97,0x97,0x99,0x99,0x9B,0x9B,0x9D,0x9D,0x9F,0x9F, \
                0xA1,0xA1,0xA3,0xA3,0xA5,0xA5,0xA7,0xA7,0xA9,0xA9,0xAB,0xAB,0xAD,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB6,0xB6,0xB8,0xB8,0xB9,0xBA,0xBB,0xBC,0xBE,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD1,0xD1,0xD3,0xD3,0xD5,0xD5,0xD7,0xD7,0xDD,0xD9,0xDA,0xDB,0xDC,0xDD,0xE0,0xDF, \
                0xE0,0xE2,0xE2,0xE4,0xE4,0xE6,0xE6,0xE8,0xE8,0xEA,0xEA,0xEC,0xEC,0xEE,0xEE,0xEF,0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF8,0xFA,0xFA,0xFC,0xFC,0xFD,0xFE,0xFF}

#elif _LIBFAT_CODE_PAGE == 857        /* Turkish (OEM) */
#define _DF1S        0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0x98,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x98,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9E, \
                0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA6,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xDE,0x59,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _LIBFAT_CODE_PAGE == 858        /* Multilingual Latin 1 + Euro (OEM) */
#define _DF1S        0
#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0xDE,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x59,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
                0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD1,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE7,0xE9,0xEA,0xEB,0xED,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _LIBFAT_CODE_PAGE == 862        /* Hebrew (OEM) */
#define _DF1S        0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
                0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _LIBFAT_CODE_PAGE == 866        /* Russian (OEM) */
#define _DF1S        0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
                0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0x90,0x91,0x92,0x93,0x9d,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,0xF0,0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _LIBFAT_CODE_PAGE == 874        /* Thai (OEM, Windows) */
#define _DF1S        0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
                0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _LIBFAT_CODE_PAGE == 1250       /* Central Europe (Windows) */
#define _DF1S        0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x8A,0x9B,0x8C,0x8D,0x8E,0x8F, \
                0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xA3,0xB4,0xB5,0xB6,0xB7,0xB8,0xA5,0xAA,0xBB,0xBC,0xBD,0xBC,0xAF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xFF}

#elif _LIBFAT_CODE_PAGE == 1251       /* Cyrillic (Windows) */
#define _DF1S        0
#define _EXCVT {0x80,0x81,0x82,0x82,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x80,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x8A,0x9B,0x8C,0x8D,0x8E,0x8F, \
                0xA0,0xA2,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB2,0xA5,0xB5,0xB6,0xB7,0xA8,0xB9,0xAA,0xBB,0xA3,0xBD,0xBD,0xAF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF}

#elif _LIBFAT_CODE_PAGE == 1252       /* Latin 1 (Windows) */
#define _DF1S        0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0xAd,0x9B,0x8C,0x9D,0xAE,0x9F, \
                0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0x9F}

#elif _LIBFAT_CODE_PAGE == 1253       /* Greek (Windows) */
#define _DF1S        0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
                0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xA2,0xB8,0xB9,0xBA, \
                0xE0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xF2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xFB,0xBC,0xFD,0xBF,0xFF}

#elif _LIBFAT_CODE_PAGE == 1254       /* Turkish (Windows) */
#define _DF1S        0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x8A,0x9B,0x8C,0x9D,0x9E,0x9F, \
                0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0x9F}

#elif _LIBFAT_CODE_PAGE == 1255       /* Hebrew (Windows) */
#define _DF1S        0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
                0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _LIBFAT_CODE_PAGE == 1256       /* Arabic (Windows) */
#define _DF1S        0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x8C,0x9D,0x9E,0x9F, \
                0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0x41,0xE1,0x41,0xE3,0xE4,0xE5,0xE6,0x43,0x45,0x45,0x45,0x45,0xEC,0xED,0x49,0x49,0xF0,0xF1,0xF2,0xF3,0x4F,0xF5,0xF6,0xF7,0xF8,0x55,0xFA,0x55,0x55,0xFD,0xFE,0xFF}

#elif _LIBFAT_CODE_PAGE == 1257       /* Baltic (Windows) */
#define _DF1S        0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
                0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xA8,0xB9,0xAA,0xBB,0xBC,0xBD,0xBE,0xAF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xFF}

#elif _LIBFAT_CODE_PAGE == 1258       /* Vietnam (OEM, Windows) */
#define _DF1S        0
#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0xAC,0x9D,0x9E,0x9F, \
                0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xEC,0xCD,0xCE,0xCF,0xD0,0xD1,0xF2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xFE,0x9F}

#elif _LIBFAT_CODE_PAGE == 1          /* ASCII (for only non-LFN cfg) */
#       if _LIBFAT_USE_LFN
#               error Cannot use LFN feature without valid code page.
#       endif
#define _DF1S        0

#else
#       error Unknown code page
#endif

/* Character code support macros */
#define IsUpper(c)                      (((c) >= 'A') && ((c) <= 'Z'))
#define IsLower(c)                      (((c) >= 'a') && ((c) <= 'z'))
#define IsDigit(c)                      (((c) >= '0') && ((c) <= '9'))

/* Code page is DBCS */
#if _DF1S
#       ifdef _DF2S /* Two 1st byte areas */
#               define IsDBCS1(c)       (((uint8_t)(c) >= _DF1S && (uint8_t)(c) <= _DF1E) || ((uint8_t)(c) >= _DF2S && (uint8_t)(c) <= _DF2E))
#       else                        /* One 1st byte area */
#               define IsDBCS1(c)       ((uint8_t)(c) >= _DF1S && (uint8_t)(c) <= _DF1E)
#       endif
#
#       ifdef _DS3S        /* Three 2nd byte areas */
#               define IsDBCS2(c)       (((uint8_t)(c) >= _DS1S && (uint8_t)(c) <= _DS1E) || ((uint8_t)(c) >= _DS2S && (uint8_t)(c) <= _DS2E) || ((uint8_t)(c) >= _DS3S && (uint8_t)(c) <= _DS3E))
#       else                        /* Two 2nd byte areas */
#               define IsDBCS2(c)       (((uint8_t)(c) >= _DS1S && (uint8_t)(c) <= _DS1E) || ((uint8_t)(c) >= _DS2S && (uint8_t)(c) <= _DS2E))
#       endif
#else /* Code page is SBCS */
#       define IsDBCS1(c)               0
#       define IsDBCS2(c)               0
#endif

/* Name status flags */
#define NS                              11          /* Index of name status byte in fn[] */
#define NS_LOSS                         0x01        /* Out of 8.3 format */
#define NS_LFN                          0x02        /* Force to create LFN entry */
#define NS_LAST                         0x04        /* Last segment */
#define NS_BODY                         0x08        /* Lower case flag (body) */
#define NS_EXT                          0x10        /* Lower case flag (ext) */
#define NS_DOT                          0x20        /* Dot entry */

/* FAT sub-type boundaries */
/* Note that the FAT spec by Microsoft says 4085 but Windows works with 4087! */
#define MIN_FAT16                       4086        /* Minimum number of clusters for FAT16 */
#define MIN_FAT32                       65526       /* Minimum number of clusters for FAT32 */

/* FatFs refers the members in the FAT structures as byte array instead of
/ structure member because the structure is not binary compatible between
/ different platforms */
#define BS_jmpBoot                      0           /* Jump instruction (3) */
#define BS_OEMName                      3           /* OEM name (8) */
#define BPB_BytsPerSec                  11          /* Sector size [byte] (2) */
#define BPB_SecPerClus                  13          /* Cluster size [sector] (1) */
#define BPB_RsvdSecCnt                  14          /* Size of reserved area [sector] (2) */
#define BPB_NumFATs                     16          /* Number of FAT copies (1) */
#define BPB_RootEntCnt                  17          /* Number of root dir entries for FAT12/16 (2) */
#define BPB_TotSec16                    19          /* Volume size [sector] (2) */
#define BPB_Media                       21          /* Media descriptor (1) */
#define BPB_FATSz16                     22          /* FAT size [sector] (2) */
#define BPB_SecPerTrk                   24          /* Track size [sector] (2) */
#define BPB_NumHeads                    26          /* Number of heads (2) */
#define BPB_HiddSec                     28          /* Number of special hidden sectors (4) */
#define BPB_TotSec32                    32          /* Volume size [sector] (4) */
#define BS_DrvNum                       36          /* Physical drive number (2) */
#define BS_BootSig                      38          /* Extended boot signature (1) */
#define BS_VolID                        39          /* Volume serial number (4) */
#define BS_VolLab                       43          /* Volume label (8) */
#define BS_FilSysType                   54          /* File system type (1) */
#define BPB_FATSz32                     36          /* FAT size [sector] (4) */
#define BPB_ExtFlags                    40          /* Extended flags (2) */
#define BPB_FSVer                       42          /* File system version (2) */
#define BPB_RootClus                    44          /* Root dir first cluster (4) */
#define BPB_FSInfo                      48          /* Offset of FSInfo sector (2) */
#define BPB_BkBootSec                   50          /* Offset of backup boot sector (2) */
#define BS_DrvNum32                     64          /* Physical drive number (2) */
#define BS_BootSig32                    66          /* Extended boot signature (1) */
#define BS_VolID32                      67          /* Volume serial number (4) */
#define BS_VolLab32                     71          /* Volume label (8) */
#define BS_FilSysType32                 82          /* File system type (1) */
#define FSI_LeadSig                     0           /* FSI: Leading signature (4) */
#define FSI_StrucSig                    484         /* FSI: Structure signature (4) */
#define FSI_Free_Count                  488         /* FSI: Number of free clusters (4) */
#define FSI_Nxt_Free                    492         /* FSI: Last allocated cluster (4) */
#define MBR_Table                       446         /* MBR: Partition table offset (2) */
#define SZ_PTE                          16          /* MBR: Size of a partition table entry */
#define BS_55AA                         510         /* Boot sector signature (2) */

#define DIR_Name                        0           /* Short file name (11) */
#define DIR_Attr                        11          /* Attribute (1) */
#define DIR_NTres                       12          /* NT flag (1) */
#define DIR_CrtTimeTenth                13          /* Created time sub-second (1) */
#define DIR_CrtTime                     14          /* Created time (2) */
#define DIR_CrtDate                     16          /* Created date (2) */
#define DIR_LstAccDate                  18          /* Last accessed date (2) */
#define DIR_FstClusHI                   20          /* Higher 16-bit of first cluster (2) */
#define DIR_WrtTime                     22          /* Modified time (2) */
#define DIR_WrtDate                     24          /* Modified date (2) */
#define DIR_FstClusLO                   26          /* Lower 16-bit of first cluster (2) */
#define DIR_FileSize                    28          /* File size (4) */
#define LDIR_Ord                        0           /* LFN entry order and LLE flag (1) */
#define LDIR_Attr                       11          /* LFN attribute (1) */
#define LDIR_Type                       12          /* LFN type (1) */
#define LDIR_Chksum                     13          /* Sum of corresponding SFN entry */
#define LDIR_FstClusLO                  26          /* Filled by zero (0) */
#define SZ_DIR                          32          /* Size of a directory entry */
#define LLE                             0x40        /* Last long entry flag in LDIR_Ord */
#define DDE                             0xE5        /* Deleted directory entry mark in DIR_Name[0] */
#define NDDE                            0x05        /* Replacement of the character collides with DDE */

#define LOAD_UINT16(ptr)                (uint16_t)(((uint16_t)*((uint8_t*)(ptr)+1)<<8)|(uint16_t)*(uint8_t*)(ptr))
#define LOAD_UINT32(ptr)                (uint32_t)(((uint32_t)*((uint8_t*)(ptr)+3)<<24)|((uint32_t)*((uint8_t*)(ptr)+2)<<16)|((uint16_t)*((uint8_t*)(ptr)+1)<<8)|*(uint8_t*)(ptr))
#define STORE_UINT16(ptr,val)           *(uint8_t*)(ptr)=(uint8_t)(val); *((uint8_t*)(ptr)+1)=(uint8_t)((uint16_t)(val)>>8)
#define STORE_UINT32(ptr,val)           *(uint8_t*)(ptr)=(uint8_t)(val); *((uint8_t*)(ptr)+1)=(uint8_t)((uint16_t)(val)>>8); *((uint8_t*)(ptr)+2)=(uint8_t)((uint32_t)(val)>>16); *((uint8_t*)(ptr)+3)=(uint8_t)((uint32_t)(val)>>24)

#if _LIBFAT_USE_LFN == 0
#       define DEF_NAMEBUF              uint8_t sfn[12]
#       define INIT_BUF(dobj)           (dobj).fn = sfn
#       define FREE_BUF()
#elif _LIBFAT_USE_LFN == 1
#       define DEF_NAMEBUF              uint8_t sfn[12]; wchar_t lbuf[_LIBFAT_MAX_LFN+1]
#       define INIT_BUF(dobj)           {(dobj).fn = sfn; (dobj).lfn = lbuf;}
#       define FREE_BUF()
#elif _LIBFAT_USE_LFN == 2
#       define DEF_NAMEBUF              uint8_t sfn[12]; wchar_t *lfn
#       define INIT_BUF(dobj)           {lfn = _libfat_malloc((_LIBFAT_MAX_LFN + 1) * 2); if (!lfn) {LEAVE_FF((dobj).fs, FR_NOT_ENOUGH_CORE);} (dobj).lfn = lfn; (dobj).fn = sfn;}
#       define FREE_BUF()               _libfat_free(lfn)
#else
#       error Wrong LFN configuration.
#endif

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
#if _LIBFAT_USE_LFN
extern wchar_t _libfat_convert  (wchar_t chr, uint dir);
extern wchar_t _libfat_wtoupper (wchar_t chr);
#endif
static int      lock_fs         (FATFS *fs);
static void     unlock_fs       (FATFS *fs, FRESULT res);
#if _LIBFAT_FS_LOCK
static FRESULT  chk_lock        (FATDIR *dj, int acc);
static int      enq_lock        (FATFS *fs);
static uint     inc_lock        (FATDIR *dj, int acc);
static FRESULT  dec_lock        (FATFS *fs, uint i);
static void     clear_lock      (FATFS *fs);
#endif
static FRESULT  sync_window     (FATFS *fs);
static FRESULT  move_window     (FATFS *fs, uint32_t sector);
static uint32_t clust2sect      (FATFS *fs, uint32_t clst);
static uint32_t get_fat         (FATFS *fs, uint32_t clst);
static FRESULT  put_fat         (FATFS *fs, uint32_t clst, uint32_t val);
static FRESULT  remove_chain    (FATFS *fs, uint32_t clst);
static uint32_t create_chain    (FATFS *fs, uint32_t clst);
static FRESULT  dir_sdi         (FATDIR *dj, uint16_t idx);
static FRESULT  dir_next        (FATDIR *dj, int stretch);
static FRESULT  dir_alloc       (FATDIR *dj, uint nent);
static uint32_t ld_clust        (FATFS *fs, uint8_t *dir);
static void     st_clust        (uint8_t *dir, uint32_t cl);
#if _LIBFAT_USE_LFN
static int      cmp_lfn         (wchar_t *lfnbuf, uint8_t *dir);
static int      pick_lfn        (wchar_t *lfnbuf, uint8_t *dir);
static void     fit_lfn         (const wchar_t *lfnbuf, uint8_t *dir, uint8_t ord, uint8_t sum);
static void     gen_numname     (uint8_t *dst, const uint8_t *src, const wchar_t *lfn, uint16_t seq);
static uint8_t  sum_sfn         (const uint8_t *dir);
#endif
static FRESULT  dir_find        (FATDIR *dj);
static FRESULT  dir_read        (FATDIR *dj, int vol);
static FRESULT  dir_register    (FATDIR *dj);
static FRESULT  dir_remove      (FATDIR *dj);
static FRESULT  create_name     (FATDIR *dj, const TCHAR **path);
static void     get_fileinfo    (FATDIR *dj, FILEINFO *fno);
static FRESULT  follow_path     (FATDIR *dj, const TCHAR *path);
static uint8_t  check_fs        (FATFS *fs, uint32_t sect);
static FRESULT  chk_mounted     (FATFS *fs);
static FRESULT  validate        (void *obj);

/*==============================================================================
  Local object definitions
==============================================================================*/
#ifdef _EXCVT
/* Upper conversion table for extended chars */
static const uint8_t ExCvt[] = _EXCVT;
#endif

#if _LIBFAT_USE_LFN
/* Offset of LFN chars in the directory entry */
static const uint8_t LfnOfs[] = {1,3,5,7,9,14,16,18,20,22,24,28,30};
#endif

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Request grant to access the volume
 *
 * @param[in] *fs       File system object
 *
 * @retval 1: Function succeeded
 * @retval 0: Could not create due to any error
 */
//==============================================================================
static int lock_fs(FATFS *fs)
{
        return _libfat_lock_access(fs->sobj);
}

//==============================================================================
/**
 * @brief Release grant to access the volume
 *
 * @param[in] *fs       File system object
 * @param[in]  res      Result code to be returned
 */
//==============================================================================
static void unlock_fs(FATFS *fs, FRESULT res)
{
        if (fs
           && res != FR_NOT_ENABLED
           && res != FR_INVALID_DRIVE
           && res != FR_INVALID_OBJECT
           && res != FR_TIMEOUT) {
                _libfat_unlock_access(fs->sobj);
        }
}

#if _LIBFAT_FS_LOCK
//==============================================================================
/**
 * @brief Check if the file can be accessed
 *
 * @param[in] *dj       Directory object pointing the file to be checked
 * @param[in]  acc      Desired access (0:Read, 1:Write, 2:Delete/Rename)
 *
 * @retval FR_OK
 * @retval FR_LOCKED
 */
//==============================================================================
static FRESULT chk_lock(FATDIR *dj, int acc)
{
        uint i, be;

        /* Search file semaphore table */
        for (i = be = 0; i < _LIBFAT_FS_LOCK; i++) {
                /* Existing entry */
                if (dj->fs->files[i].fs) {
                        /* Check if the file matched with an open file */
                        if (  dj->fs->files[i].fs  == dj->fs
                           && dj->fs->files[i].clu == dj->sclust
                           && dj->fs->files[i].idx == dj->index) {

                                break;
                        }
                } else { /* Blank entry */
                        be++;
                }
        }

        /* The file is not opened */
        if (i == _LIBFAT_FS_LOCK) {
                /* Is there a blank entry for new file? */
                return (be || acc == 2) ? FR_OK : FR_TOO_MANY_OPEN_FILES;
        }

        /* The file has been opened. Reject any open against writing file and all write mode open */
        return (acc || dj->fs->files[i].ctr == 0x100) ? FR_LOCKED : FR_OK;
}

//==============================================================================
/**
 * @brief Check if an entry is available for a new file
 *
 * @retval 0 unlocked
 * @retval 1 locked
 */
//==============================================================================
static int enq_lock(FATFS *fs)
{
        uint i;

        for (i = 0; i < _LIBFAT_FS_LOCK && fs->files[i].fs; i++);

        return (i == _LIBFAT_FS_LOCK) ? 0 : 1;
}

//==============================================================================
/**
 * @brief Increment file open counter and returns its index
 *
 * @param[in] *dj       Directory object pointing the file to be checked
 * @param[in]  acc      Desired access mode (0:Read, !0:Write)
 *
 * @retval 0 internal error
 */
//==============================================================================
static uint inc_lock(FATDIR *dj, int acc)
{
        uint i;

        /* Find the file */
        for (i = 0; i < _LIBFAT_FS_LOCK; i++) {
                if (  dj->fs->files[i].fs  == dj->fs
                   && dj->fs->files[i].clu == dj->sclust
                   && dj->fs->files[i].idx == dj->index) {

                        break;
                }
        }

        /* Not opened. Register it as new. */
        if (i == _LIBFAT_FS_LOCK) {
                for (i = 0; i < _LIBFAT_FS_LOCK && dj->fs->files[i].fs; i++);

                /* No space to register (int err) */
                if (i == _LIBFAT_FS_LOCK)
                        return 0;

                dj->fs->files[i].fs  = dj->fs;
                dj->fs->files[i].clu = dj->sclust;
                dj->fs->files[i].idx = dj->index;
                dj->fs->files[i].ctr = 0;
        }

        /* Access violation (int err) */
        if (acc && dj->fs->files[i].ctr)
                return 0;

        /* Set semaphore value */
        dj->fs->files[i].ctr = acc ? 0x100 : dj->fs->files[i].ctr + 1;

        return i + 1;
}

//==============================================================================
/**
 * @brief Decrement file open counter
 *
 * @param[in] i         Semaphore index
 *
 * @retval FR_OK
 * @retval FR_INT_ERR
 */
//==============================================================================
static FRESULT dec_lock(FATFS *fs, uint i)
{
        uint16_t n;
        FRESULT res;

        if (--i < _LIBFAT_FS_LOCK) {
                n = fs->files[i].ctr;

                if (n == 0x100)
                        n = 0;

                if (n)
                        n--;

                fs->files[i].ctr = n;

                if (!n) {
                        fs->files[i].fs = 0;
                }

                res = FR_OK;
        } else {
                res = FR_INT_ERR;
        }
        return res;
}

//==============================================================================
/**
 * @brief Clear lock entries of the volume
 *
 * @param[in] *fs       File system object
 */
//==============================================================================
static void clear_lock(FATFS *fs)
{
        uint i;

        for (i = 0; i < _LIBFAT_FS_LOCK; i++) {
                if (fs->files[i].fs == fs) {
                        fs->files[i].fs = 0;
                }
        }
}
#endif

//==============================================================================
/**
 * @brief Flush disk access window
 *
 * @param[in] *fs       File system object
 *
 * @retval FR_OK        success
 * @retval FR_DISK_ERR
 */
//==============================================================================
static FRESULT sync_window(FATFS *fs)
{
        /* Write back the sector if it is dirty */
        if (fs->wflag) {
                uint32_t wsect = fs->winsect;

                /* Current sector number */
                if (_libfat_disk_write(fs->srcfile, fs->win, wsect, 1) != RES_OK)
                        return FR_DISK_ERR;

                fs->wflag = 0;

                /* In FAT area? */
                if (wsect >= fs->fatbase && wsect < (fs->fatbase + fs->fsize)) {
                        /* Reflect the change to all FAT copies */
                        for (uint nf = fs->n_fats; nf >= 2; nf--) {
                                wsect += fs->fsize;
                                _libfat_disk_write(fs->srcfile, fs->win, wsect, 1);
                        }
                }
        }

        return FR_OK;
}

//==============================================================================
/**
 * @brief Move disk access window
 *
 * @param[in] *fs       File system object
 * @param[in]  sector   Sector number to make appearance in the fs->win[]
 *
 * @retval FR_OK        success
 */
//==============================================================================
static FRESULT move_window(FATFS *fs, uint32_t sector)
{
        /* Changed current window */
        if (sector != fs->winsect) {
                if (sync_window(fs) != FR_OK)
                        return FR_DISK_ERR;

                if (_libfat_disk_read(fs->srcfile, fs->win, sector, 1) != RES_OK)
                        return FR_DISK_ERR;

                fs->winsect = sector;
        }

        return FR_OK;
}

//==============================================================================
/**
 * @brief Synchronize file system and storage device
 *
 * @param[in] *fs       File system object
 *
 * @retval FR_OK
 * @retval FR_DISK_ERR
 */
//==============================================================================
static FRESULT sync_fs(FATFS *fs)
{
        FRESULT res;

        res = sync_window(fs);
        if (res == FR_OK) {
                /* Update FSInfo sector if needed */
                if (fs->fs_type == LIBFAT_FS_FAT32 && fs->fsi_flag) {
                        fs->winsect = 0;

                        /* Create FSInfo structure */
                        memset(fs->win, 0, 512);
                        STORE_UINT16(fs->win+BS_55AA, 0xAA55);
                        STORE_UINT32(fs->win+FSI_LeadSig, 0x41615252);
                        STORE_UINT32(fs->win+FSI_StrucSig, 0x61417272);
                        STORE_UINT32(fs->win+FSI_Free_Count, fs->free_clust);
                        STORE_UINT32(fs->win+FSI_Nxt_Free, fs->last_clust);

                        /* Write it into the FSInfo sector */
                        _libfat_disk_write(fs->srcfile, fs->win, fs->fsi_sector, 1);
                        fs->fsi_flag = 0;
                }

                /* Make sure that no pending write process in the physical drive */
                if (_libfat_disk_ioctl(fs->srcfile, CTRL_SYNC, 0) != RES_OK) {
                        res = FR_DISK_ERR;
                }
        }

        return res;
}

//==============================================================================
/**
 * @brief Get sector number from cluster number
 *
 * @param[in] *fs       File system object
 * @param[in]  clst     Cluster number to be converted
 *
 * @return !=0: Sector number, 0: Failed - invalid cluster number
 */
//==============================================================================
static uint32_t clust2sect(FATFS *fs, uint32_t clst)
{
        clst -= 2;

        if (clst >= (fs->n_fatent - 2))
                return 0;

        return clst * fs->csize + fs->database;
}

//==============================================================================
/**
 * @brief FAT access - Read value of a FAT entry
 *
 * @param[in] *fs       File system object
 * @param[in]  clst     Cluster number to get the link information
 *
 * @return 0xFFFFFFFF:Disk error, 1:Internal error, Else:Cluster status
 */
//==============================================================================
static uint32_t get_fat(FATFS *fs, uint32_t clst)
{
        uint wc, bc;
        uint8_t *p;

        if (clst < 2 || clst >= fs->n_fatent)
                return 1;

        switch (fs->fs_type) {
        case LIBFAT_FS_FAT12 :
                bc = (uint)clst; bc += bc / 2;

                if (move_window(fs, fs->fatbase + (bc / SS(fs))))
                        break;

                wc = fs->win[bc % SS(fs)]; bc++;

                if (move_window(fs, fs->fatbase + (bc / SS(fs))))
                        break;

                wc |= fs->win[bc % SS(fs)] << 8;

                return (clst & 1) ? (wc >> 4) : (wc & 0xFFF);

        case LIBFAT_FS_FAT16 :
                if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 2))))
                        break;

                p = &fs->win[clst * 2 % SS(fs)];
                return LOAD_UINT16(p);

        case LIBFAT_FS_FAT32 :
                if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 4))))
                        break;

                p = &fs->win[clst * 4 % SS(fs)];
                return LOAD_UINT32(p) & 0x0FFFFFFF;
        }

        return 0xFFFFFFFF;
}

//==============================================================================
/**
 * @brief FAT access - Change value of a FAT entry
 *
 * @param[in] *fs       File system object
 * @param[in]  clst     Cluster number to be changed in range of 2 to fs->n_fatent - 1
 * @param[in]  val      New value to mark the cluster
 *
 * @retval FR_OK
 * @retval FR_INT_ERR
 */
//==============================================================================
static FRESULT put_fat(FATFS *fs, uint32_t clst, uint32_t val)
{
        FRESULT res;

        if (clst < 2 || clst >= fs->n_fatent) {
                res = FR_INT_ERR;

        } else {
                switch (fs->fs_type) {
                case LIBFAT_FS_FAT12: {
                        uint bc  = (uint)clst;
                        bc += bc / 2;
                        res = move_window(fs, fs->fatbase + (bc / SS(fs)));
                        if (res != FR_OK)
                                break;

                        uint8_t *p = &fs->win[bc % SS(fs)];
                        *p = (clst & 1) ? ((*p & 0x0F) | ((uint8_t)val << 4)) : (uint8_t)val;
                        bc++;
                        fs->wflag = 1;

                        res = move_window(fs, fs->fatbase + (bc / SS(fs)));
                        if (res != FR_OK)
                                break;

                        p  = &fs->win[bc % SS(fs)];
                        *p = (clst & 1) ? (uint8_t)(val >> 4) : ((*p & 0xF0) | ((uint8_t)(val >> 8) & 0x0F));
                        break;
                }

                case LIBFAT_FS_FAT16:
                        res = move_window(fs, fs->fatbase + (clst / (SS(fs) / 2)));
                        if (res != FR_OK)
                                break;

                        uint8_t *p = &fs->win[clst * 2 % SS(fs)];
                        STORE_UINT16(p, (uint16_t)val);
                        break;

                case LIBFAT_FS_FAT32:
                        res = move_window(fs, fs->fatbase + (clst / (SS(fs) / 4)));
                        if (res != FR_OK)
                                break;

                        p = &fs->win[clst * 4 % SS(fs)];
                        val |= LOAD_UINT32(p) & 0xF0000000;
                        STORE_UINT32(p, val);
                        break;

                default:
                        res = FR_INT_ERR;
                }

                fs->wflag = 1;
        }

        return res;
}

//==============================================================================
/**
 * @brief FAT handling - Remove a cluster chain
 *
 * @param[in] *fs       File system object
 * @param[in]  clst     Cluster number to remove a chain from
 *
 * @retval FR_OK
 * @retval FR_DISK_ERR
 * @retval FR_INT_ERR
 */
//==============================================================================
static FRESULT remove_chain(FATFS *fs, uint32_t clst)
{
        FRESULT res;
        uint32_t nxt;
#if _LIBFAT_USE_ERASE
        uint32_t scl = clst, ecl = clst, rt[2];
#endif

        if (clst < 2 || clst >= fs->n_fatent) {
                res = FR_INT_ERR;

        } else {
                res = FR_OK;
                while (clst < fs->n_fatent) {
                        nxt = get_fat(fs, clst);
                        if (nxt == 0)
                                break;

                        if (nxt == 1) {
                                res = FR_INT_ERR;
                                break;
                        }        /* Internal error? */

                        if (nxt == 0xFFFFFFFF) {
                                res = FR_DISK_ERR;
                                break;
                        }        /* Disk error? */

                        res = put_fat(fs, clst, 0);
                        if (res != FR_OK)
                                break;

                        if (fs->free_clust != 0xFFFFFFFF) {
                                fs->free_clust++;
                                fs->fsi_flag = 1;
                        }
#if _LIBFAT_USE_ERASE
                        /* Is next cluster contiguous? */
                        if (ecl + 1 == nxt) {
                                ecl = nxt;
                        } else {
                                rt[0] = clust2sect(fs, scl);
                                rt[1] = clust2sect(fs, ecl) + fs->csize - 1;
                                _libfat_disk_ioctl(fs->drv, CTRL_ERASE_SECTOR, rt);
                                scl = ecl = nxt;
                        }
#endif
                        clst = nxt;
                }
        }

        return res;
}

//==============================================================================
/**
 * @brief FAT handling - Stretch or Create a cluster chain
 *
 * @param[in] *fs       File system object
 * @param[in]  clst     Cluster number to stretch. 0 means create a new chain
 *
 * @return 0:No free cluster, 1:Internal error, 0xFFFFFFFF:Disk error, >=2:New cluster number
 */
//==============================================================================
static uint32_t create_chain(FATFS *fs, uint32_t clst)
{
        uint32_t cs, ncl, scl;
        FRESULT res;

        if (clst == 0) {
                scl = fs->last_clust;
                if (!scl || scl >= fs->n_fatent) scl = 1;
        } else {
                cs = get_fat(fs, clst);
                if (cs < 2)
                        return 1;

                if (cs < fs->n_fatent)
                        return cs;

                scl = clst;
        }

        ncl = scl;
        for (;;) {
                ncl++;
                if (ncl >= fs->n_fatent) {
                        ncl = 2;
                        if (ncl > scl) return 0;
                }

                cs = get_fat(fs, ncl);
                if (cs == 0)
                        break;

                if (cs == 0xFFFFFFFF || cs == 1)
                        return cs;

                if (ncl == scl)
                        return 0;
        }

        res = put_fat(fs, ncl, 0x0FFFFFFF);
        if (res == FR_OK && clst != 0) {
                /* Link it to the previous one if needed */
                res = put_fat(fs, clst, ncl);
        }

        if (res == FR_OK) {
                /* Update FSINFO */
                fs->last_clust = ncl;
                if (fs->free_clust != 0xFFFFFFFF) {
                        fs->free_clust--;
                        fs->fsi_flag = 1;
                }
        } else {
                ncl = (res == FR_DISK_ERR) ? 0xFFFFFFFF : 1;
        }

        return ncl;
}

//==============================================================================
/**
 * @brief Directory handling - Set directory index
 *
 * @param[in] *dj       Pointer to directory object
 * @param[in]  idx      Index of directory table
 *
 * @retval FR_OK
 * @retval FR_INT_ERR
 * @retval FR_DISK_ERR
 */
//==============================================================================
static FRESULT dir_sdi(FATDIR *dj, uint16_t idx)
{
        uint32_t clst;
        uint16_t ic;

        dj->index = idx;
        clst = dj->sclust;
        if (clst == 1 || clst >= dj->fs->n_fatent)
                return FR_INT_ERR;

        /* Replace cluster number 0 with root cluster number if in FAT32 */
        if (!clst && dj->fs->fs_type == LIBFAT_FS_FAT32)
                clst = dj->fs->dirbase;

        /* Static table (root-dir in FAT12/16) */
        if (clst == 0) {
                dj->clust = clst;
                if (idx >= dj->fs->n_rootdir)
                        return FR_INT_ERR;

                dj->sect = dj->fs->dirbase + idx / (SS(dj->fs) / SZ_DIR);
        } else {
                /* Dynamic table (sub-dirs or root-dir in FAT32) */
                ic = SS(dj->fs) / SZ_DIR * dj->fs->csize;

                /* Follow cluster chain */
                while (idx >= ic) {
                        clst = get_fat(dj->fs, clst);
                        if (clst == 0xFFFFFFFF)
                                return FR_DISK_ERR;

                        /* Reached to end of table or int error */
                        if (clst < 2 || clst >= dj->fs->n_fatent)
                                return FR_INT_ERR;

                        idx -= ic;
                }

                dj->clust = clst;
                dj->sect  = clust2sect(dj->fs, clst) + idx / (SS(dj->fs) / SZ_DIR);
        }

        dj->dir = dj->fs->win + (idx % (SS(dj->fs) / SZ_DIR)) * SZ_DIR;

        return FR_OK;
}

//==============================================================================
/**
 * @brief Directory handling - Move directory table index next
 *
 * @param[in] *dj       Pointer to the directory object
 * @param[in]  stretch  0: Do not stretch table, 1: Stretch table if needed
 *
 * @retval FR_OK
 * @retval FR_NO_FILE
 * @retval FR_DISK_ERR
 */
//==============================================================================
static FRESULT dir_next(FATDIR *dj, int stretch)
{
        uint32_t clst;
        uint16_t i;

        i = dj->index + 1;
        if (!i || !dj->sect)
                return FR_NO_FILE;

        if (!(i % (SS(dj->fs) / SZ_DIR))) {
                dj->sect++;

                if (dj->clust == 0) {
                        if (i >= dj->fs->n_rootdir)
                                return FR_NO_FILE;
                } else {
                        /* Cluster changed? */
                        if (((i / (SS(dj->fs) / SZ_DIR)) & (dj->fs->csize - 1)) == 0) {
                                clst = get_fat(dj->fs, dj->clust);
                                if (clst <= 1)
                                        return FR_INT_ERR;

                                if (clst == 0xFFFFFFFF)
                                        return FR_DISK_ERR;

                                if (clst >= dj->fs->n_fatent) {
                                        uint8_t c;
                                        if (!stretch)
                                                return FR_NO_FILE;

                                        clst = create_chain(dj->fs, dj->clust);
                                        if (clst == 0)
                                                return FR_DENIED;

                                        if (clst == 1)
                                                return FR_INT_ERR;

                                        if (clst == 0xFFFFFFFF)
                                                return FR_DISK_ERR;

                                        /* Clean-up stretched table */
                                        if (sync_window(dj->fs))
                                                return FR_DISK_ERR;

                                        memset(dj->fs->win, 0, SS(dj->fs));
                                        dj->fs->winsect = clust2sect(dj->fs, clst);
                                        for (c = 0; c < dj->fs->csize; c++) {
                                                dj->fs->wflag = 1;
                                                if (sync_window(dj->fs))
                                                        return FR_DISK_ERR;

                                                dj->fs->winsect++;
                                        }
                                        dj->fs->winsect -= c;
                                }
                                dj->clust = clst;
                                dj->sect  = clust2sect(dj->fs, clst);
                        }
                }
        }

        dj->index = i;
        dj->dir = dj->fs->win + (i % (SS(dj->fs) / SZ_DIR)) * SZ_DIR;

        return FR_OK;
}

//==============================================================================
/**
 * @brief Directory handling - Reserve directory entry
 *
 * @param[in] *dj       Pointer to the directory object
 * @param[in]  nent     number of contiguous entries to allocate (1-21)
 *
 * @retval FR_OK
 * @retval FR_NO_FILE
 * @retval FR_DENIED
 */
//==============================================================================
static FRESULT dir_alloc(FATDIR *dj, uint nent)
{
        FRESULT res = dir_sdi(dj, 0);
        if (res == FR_OK) {
                uint n = 0;
                do {
                        res = move_window(dj->fs, dj->sect);
                        if (res != FR_OK)
                                break;

                        /* Is it a blank entry? */
                        if (dj->dir[0] == DDE || dj->dir[0] == 0) {
                                /* A block of contiguous entry is found */
                                if (++n == nent)
                                        break;
                        } else {
                                n = 0;
                        }

                        res = dir_next(dj, 1);
                } while (res == FR_OK);
        }

        if (res == FR_NO_FILE)
                res = FR_DENIED;

        return res;
}

//==============================================================================
/**
 * @brief Directory handling - Load start cluster number
 *
 * @param[in] *fs,      Pointer to the fs object
 * @param[in] *dir      Pointer to the directory entry
 *
 * @return cluster number
 */
//==============================================================================
static uint32_t ld_clust(FATFS *fs, uint8_t *dir)
{
        uint32_t cl;

        cl = LOAD_UINT16(dir + DIR_FstClusLO);

        if (fs->fs_type == LIBFAT_FS_FAT32) {
                cl |= (uint32_t)LOAD_UINT16(dir + DIR_FstClusHI) << 16;
        }

        return cl;
}

//==============================================================================
/**
 * @brief Directory handling - Store start cluster number
 *
 * @param[in] *dir      Pointer to the directory entry
 * @param[in]  cl       Value to be set
 */
//==============================================================================
static void st_clust(uint8_t *dir, uint32_t cl)
{
        STORE_UINT16(dir+DIR_FstClusLO, cl);
        STORE_UINT16(dir+DIR_FstClusHI, cl >> 16);
}

#if _LIBFAT_USE_LFN
//==============================================================================
/**
 * @brief LFN handling - Test an LFN segment from/to directory entry
 *
 * @param[in] *lfnbuf   Pointer to the LFN to be compared
 * @param[in] *dir      Pointer to the directory entry containing a part of LFN
 *
 * @return 1:Matched, 0:Not matched
 */
//==============================================================================
static int cmp_lfn(wchar_t *lfnbuf, uint8_t *dir)
{
        /* Get offset in the LFN buffer */
        uint i = ((dir[LDIR_Ord] & ~LLE) - 1) * 13;
        uint s = 0;
        wchar_t wc = 1;
        do {
                /* Pick an LFN character from the entry */
                wchar_t uc = LOAD_UINT16(dir + LfnOfs[s]);

                /* Last char has not been processed */
                if (wc) {
                        wc = _libfat_wtoupper(uc);
                        if (i >= _LIBFAT_MAX_LFN || wc != _libfat_wtoupper(lfnbuf[i++]))
                                return 0;
                } else {
                        if (uc != 0xFFFF)
                                return 0;
                }
        } while (++s < 13);

        /* Last segment matched but different length */
        if ((dir[LDIR_Ord] & LLE) && wc && lfnbuf[i])
                return 0;

        return 1;
}
#endif

#if _LIBFAT_USE_LFN
//==============================================================================
/**
 * @brief LFN handling - Pick an LFN segment from/to directory entry
 *
 * @param[in] *lfnbuf   Pointer to the Unicode-LFN buffer
 * @param[in] *dir      Pointer to the directory entry
 *
 * @return 1:Succeeded, 0:Buffer overflow
 */
//==============================================================================
static int pick_lfn(wchar_t *lfnbuf, uint8_t *dir)
{
        /* Offset in the LFN buffer */
        uint i = ((dir[LDIR_Ord] & 0x3F) - 1) * 13;

        uint s = 0;
        wchar_t wc = 1;
        do {
                /* Pick an LFN character from the entry */
                wchar_t uc = LOAD_UINT16(dir+LfnOfs[s]);
                if (wc) {
                        if (i >= _LIBFAT_MAX_LFN)
                                return 0;

                        lfnbuf[i++] = wc = uc;
                } else {
                        /* Check filler */
                        if (uc != 0xFFFF)
                                return 0;
                }
        } while (++s < 13);

        if (dir[LDIR_Ord] & LLE) {
                if (i >= _LIBFAT_MAX_LFN)
                        return 0;

                lfnbuf[i] = 0;
        }

        return 1;
}
#endif

#if _LIBFAT_USE_LFN
//==============================================================================
/**
 * @brief LFN handling - Fit an LFN segment from/to directory entry
 *
 * @param[in] *lfnbuf   Pointer to the LFN buffer
 * @param[in] *dir      Pointer to the directory entry
 * @param[in]  ord      LFN order (1-20)
 * @param[in]  sum      SFN sum
 */
//==============================================================================
static void fit_lfn(const wchar_t *lfnbuf, uint8_t *dir, uint8_t ord, uint8_t sum)
{
        uint i, s;
        wchar_t wc;

        /* Set check sum */
        dir[LDIR_Chksum] = sum;

        /* Set attribute. LFN entry */
        dir[LDIR_Attr] = LIBFAT_AM_LFN;

        dir[LDIR_Type] = 0;
        STORE_UINT16(dir+LDIR_FstClusLO, 0);

        /* Get offset in the LFN buffer */
        i = (ord - 1) * 13;
        s = wc = 0;
        do {
                /* Get an effective char */
                if (wc != 0xFFFF)
                        wc = lfnbuf[i++];

                STORE_UINT16(dir+LfnOfs[s], wc);
                if (!wc)
                        wc = 0xFFFF;
        } while (++s < 13);

        /* Bottom LFN part is the start of LFN sequence */
        if (wc == 0xFFFF || !lfnbuf[i])
                ord |= LLE;

        dir[LDIR_Ord] = ord;
}
#endif

#if _LIBFAT_USE_LFN
//==============================================================================
/**
 * @brief Create numbered name
 *
 * @param[in] *dst      Pointer to generated SFN
 * @param[in] *src      Pointer to source SFN to be modified
 * @param[in] *lfn      Pointer to LFN
 * @param[in]  seq      Sequence number
 */
//==============================================================================
static void gen_numname(uint8_t *dst, const uint8_t *src, const wchar_t *lfn, uint16_t seq)
{
        uint8_t ns[8], c;
        uint i, j;

        memcpy(dst, src, 11);

        /* On many collisions, generate a hash number instead of sequential number */
        if (seq > 5) {
                do {
                        seq = (seq >> 1) + (seq << 15) + (uint16_t)*lfn++;
                } while (*lfn);
        }

        /* itoa (hexdecimal) */
        i = 7;
        do {
                c = (seq % 16) + '0';
                if (c > '9')
                        c += 7;

                ns[i--] = c;
                seq /= 16;
        } while (seq);

        ns[i] = '~';

        /* Append the number */
        for (j = 0; j < i && dst[j] != ' '; j++) {
                if (IsDBCS1(dst[j])) {
                        if (j == i - 1)
                                break;
                        j++;
                }
        }
        do {
                dst[j++] = (i < 8) ? ns[i++] : ' ';
        } while (j < 8);
}
#endif

#if _LIBFAT_USE_LFN
//==============================================================================
/**
 * @brief Calculate sum of an SFN
 *
 * @param[in] *dir      Ptr to directory entry
 *
 * @return sum of an SFN
 */
//==============================================================================
static uint8_t sum_sfn(const uint8_t *dir)
{
        uint8_t sum = 0;
        uint    n   = 11;

        do {
                sum = (sum >> 1) + (sum << 7) + *dir++;
        } while (--n);

        return sum;
}
#endif

//==============================================================================
/**
 * @brief Directory handling - Find an object in the directory
 *
 * @param[in] *dj       Pointer to the directory object linked to the file name
 *
 * @retval FR_OK
 * @retval FR_NO_FILE
 */
//==============================================================================
static FRESULT dir_find(FATDIR *dj)
{
        FRESULT res;
        uint8_t c, *dir;
#if _LIBFAT_USE_LFN
        uint8_t a, ord, sum;
#endif
        /* Rewind directory object */
        res = dir_sdi(dj, 0);
        if (res != FR_OK)
                return res;

#if _LIBFAT_USE_LFN
        ord = sum = 0xFF;
#endif
        do {
                res = move_window(dj->fs, dj->sect);
                if (res != FR_OK)
                        break;

                /* Ptr to the directory entry of current index */
                dir = dj->dir;
                c = dir[DIR_Name];
                if (c == 0) {
                        res = FR_NO_FILE;
                        break;
                }

#if _LIBFAT_USE_LFN
                /* LFN configuration */
                a = dir[DIR_Attr] & LIBFAT_AM_MASK;

                /* An entry without valid data */
                if (c == DDE || ((a & LIBFAT_AM_VOL) && a != LIBFAT_AM_LFN)) {
                        ord = 0xFF;
                } else {
                        if (a == LIBFAT_AM_LFN) {
                                if (dj->lfn) {
                                        /* Is it start of LFN sequence? */
                                        if (c & LLE) {
                                                sum = dir[LDIR_Chksum];
                                                c &= ~LLE;
                                                ord = c;
                                                dj->lfn_idx = dj->index;
                                        }

                                        /* Check validity of the LFN entry and compare it with given name */
                                        ord = (c == ord && sum == dir[LDIR_Chksum] && cmp_lfn(dj->lfn, dir)) ? ord - 1 : 0xFF;
                                }
                        } else {                                        /* An SFN entry is found */
                                if (!ord && sum == sum_sfn(dir))
                                        break;

                                ord = 0xFF;
                                dj->lfn_idx = 0xFFFF;
                                if (!(dj->fn[NS] & NS_LOSS) && !memcmp(dir, dj->fn, 11))
                                        break;
                        }
                }
#else                /* Non LFN configuration */
                if (!(dir[DIR_Attr] & LIBFAT_AM_VOL) && !memcmp(dir, dj->fn, 11))
                        break;
#endif
                res = dir_next(dj, 0);
        } while (res == FR_OK);

        return res;
}

//==============================================================================
/**
 * @brief Read an object from the directory
 *
 * @param[in] *dj       Pointer to the directory object
 * @param[in]  vol      Filtered by 0:file/dir or 1:volume label
 *
 * @retval FR_OK
 * @retval FR_NO_FILE
 */
//==============================================================================
static FRESULT dir_read(FATDIR *dj, int vol)
{
        FRESULT res;
        uint8_t a, c, *dir;
#if _LIBFAT_USE_LFN
        uint8_t ord = 0xFF, sum = 0xFF;
#endif

        res = FR_NO_FILE;
        while (dj->sect) {
                res = move_window(dj->fs, dj->sect);
                if (res != FR_OK)
                        break;

                dir = dj->dir;
                c = dir[DIR_Name];
                if (c == 0) {
                        res = FR_NO_FILE;
                        break;
                }

                a = dir[DIR_Attr] & LIBFAT_AM_MASK;
#if _LIBFAT_USE_LFN
                /* An entry without valid data */
                if (c == DDE || (c == '.') || (a == LIBFAT_AM_VOL) != vol) {
                        ord = 0xFF;
                } else {
                        if (a == LIBFAT_AM_LFN) {
                                if (c & LLE) {
                                        sum = dir[LDIR_Chksum];
                                        c &= ~LLE;
                                        ord = c;
                                        dj->lfn_idx = dj->index;
                                }

                                /* Check LFN validity and capture it */
                                ord = (c == ord && sum == dir[LDIR_Chksum] && pick_lfn(dj->lfn, dir)) ? ord - 1 : 0xFF;
                        } else {
                                if (ord || sum != sum_sfn(dir))
                                        dj->lfn_idx = 0xFFFF;
                                break;
                        }
                }
#else
                /* Is it a valid entry? */
                if (c != DDE && (c != '.') && a != LIBFAT_AM_LFN && (a == LIBFAT_AM_VOL) == vol)
                        break;
#endif
                res = dir_next(dj, 0);
                if (res != FR_OK)
                        break;
        }

        if (res != FR_OK)
                dj->sect = 0;

        return res;
}

//==============================================================================
/**
 * @brief Register an object to the directory
 *
 * @param[in] *dj       Target directory with object name to be created
 *
 * @retval FR_OK:       Successful
 * @retval FR_DENIED:   No free entry or too many SFN collision
 * @retval FR_DISK_ERR: Disk error
 */
//==============================================================================
static FRESULT dir_register(FATDIR *dj)
{
        FRESULT res;
#if _LIBFAT_USE_LFN
        uint16_t n, ne;
        uint8_t sn[12], *fn, sum;
        wchar_t *lfn;

        fn = dj->fn; lfn = dj->lfn;
        memcpy(sn, fn, 12);

        /* When LFN is out of 8.3 format, generate a numbered name */
        if (sn[NS] & NS_LOSS) {
                fn[NS] = 0; dj->lfn = 0;

                /* Find only SFN */
                for (n = 1; n < 100; n++) {
                        gen_numname(fn, sn, lfn, n);
                        res = dir_find(dj);
                        if (res != FR_OK)
                                break;
                }

                if (n == 100)
                        return FR_DENIED;

                if (res != FR_NO_FILE)
                        return res;

                fn[NS] = sn[NS];
                dj->lfn = lfn;
        }

        /* When LFN is to be created, allocate entries for an SFN + LFNs */
        if (sn[NS] & NS_LFN) {
                for (n = 0; lfn[n]; n++);
                ne = (n + 25) / 13;
        } else {
                ne = 1;
        }

        res = dir_alloc(dj, ne);

        /* Set LFN entry if needed */
        if (res == FR_OK && --ne) {
                res = dir_sdi(dj, (uint16_t)(dj->index - ne));
                if (res == FR_OK) {
                        sum = sum_sfn(dj->fn);
                        do {
                                /* Store LFN entries in bottom first */
                                res = move_window(dj->fs, dj->sect);
                                if (res != FR_OK)
                                        break;

                                fit_lfn(dj->lfn, dj->dir, (uint8_t)ne, sum);
                                dj->fs->wflag = 1;
                                res = dir_next(dj, 0);
                        } while (res == FR_OK && --ne);
                }
        }
#else
        res = dir_alloc(dj, 1);
#endif

        /* Set SFN entry */
        if (res == FR_OK) {
                res = move_window(dj->fs, dj->sect);
                if (res == FR_OK) {
                        /* Clean the entry */
                        memset(dj->dir, 0, SZ_DIR);

                        /* Put SFN */
                        memcpy(dj->dir, dj->fn, 11);
#if _LIBFAT_USE_LFN
                        /* Put NT flag */
                        dj->dir[DIR_NTres] = *(dj->fn+NS) & (NS_BODY | NS_EXT);
#endif
                        dj->fs->wflag = 1;
                }
        }

        return res;
}

//==============================================================================
/**
 * @brief Remove an object from the directory
 *
 * @param[in] *dj       Directory object pointing the entry to be removed
 *
 * @retval FR_OK:       Successful
 * @retval FR_DISK_ERR: A disk error
 */
//==============================================================================
static FRESULT dir_remove(FATDIR *dj)
{
        FRESULT res;
#if _LIBFAT_USE_LFN
        uint16_t i;

        i = dj->index;

        /* Goto the SFN or top of the LFN entries */
        res = dir_sdi(dj, (uint16_t)((dj->lfn_idx == 0xFFFF) ? i : dj->lfn_idx));
        if (res == FR_OK) {
                do {
                        res = move_window(dj->fs, dj->sect);
                        if (res != FR_OK)
                                break;

                        /* Mark the entry "deleted" */
                        *dj->dir = DDE;
                        dj->fs->wflag = 1;

                        /* When reached SFN, all entries of the object has been deleted */
                        if (dj->index >= i)
                                break;

                        res = dir_next(dj, 0);
                } while (res == FR_OK);

                if (res == FR_NO_FILE)
                        res = FR_INT_ERR;
        }
#else
        res = dir_sdi(dj, dj->index);
        if (res == FR_OK) {
                res = move_window(dj->fs, dj->sect);
                if (res == FR_OK) {
                        /* Mark the entry "deleted" */
                        *dj->dir = DDE;
                        dj->fs->wflag = 1;
                }
        }
#endif

        return res;
}

//==============================================================================
/**
 * @brief Pick a segment and create the object name in directory form
 *
 * @param[in]  *dj      Pointer to the directory object
 * @param[in] **path    Pointer to pointer to the segment in the path string
 *
 * @retval FR_OK
 * @retval FR_INVALID_NAME
 */
//==============================================================================
static FRESULT create_name(FATDIR *dj, const TCHAR **path)
{
#if _LIBFAT_USE_LFN
        uint8_t b, cf;
        wchar_t w, *lfn;
        uint i, ni, si, di;
        const TCHAR *p;

        /* Create LFN in Unicode, strip duplicated separator */
        for (p = *path; *p == '/' || *p == '\\'; p++);
        lfn = dj->lfn;
        si = di = 0;
        for (;;) {
                /* Get a character */
                w = p[si++];

                /* Break on end of segment */
                if (w < ' ' || w == '/' || w == '\\')
                        break;

                /* Reject too long name */
                if (di >= _LIBFAT_MAX_LFN)
                        return FR_INVALID_NAME;
#if !_LIBFAT_LFN_UNICODE
                /* Check if it is a DBC 1st byte (always false on SBCS cfg) */
                w &= 0xFF;
                if (IsDBCS1(w)) {
                        /* Get 2nd byte */
                        b = (uint8_t)p[si++];

                        /* Reject invalid sequence */
                        if (!IsDBCS2(b))
                                return FR_INVALID_NAME;

                        /* Create a DBC */
                        w = (w << 8) + b;
                }

                /* Convert ANSI/OEM to Unicode */
                w = _libfat_convert(w, 1);
                if (!w)
                        return FR_INVALID_NAME;
#endif
                /* Reject illegal chars for LFN */
                if (w < 0x80 && strchr("\"*:<>\?|\x7F", w))
                        return FR_INVALID_NAME;

                lfn[di++] = w;
        }

        /* Return pointer to the next segment */
        *path = &p[si];

        /* Set last segment flag if end of path */
        cf = (w < ' ') ? NS_LAST : 0;

        /* Strip trailing spaces and dots */
        while (di) {
                w = lfn[di-1];
                if (w != ' ' && w != '.')
                        break;
                di--;
        }

        /* Reject null string */
        if (!di)
                return FR_INVALID_NAME;

        /* LFN is created */
        lfn[di] = 0;

        /* Create SFN in directory form */
        memset(dj->fn, ' ', 11);

        /* Strip leading spaces and dots */
        for (si = 0; lfn[si] == ' ' || lfn[si] == '.'; si++);

        if (si)
                cf |= NS_LOSS | NS_LFN;

        /* Find extension (di<=si: no extension) */
        while (di && lfn[di - 1] != '.') di--;

        b = i = 0;
        ni = 8;
        for (;;) {
                /* Get an LFN char */
                w = lfn[si++];
                if (!w)
                        break;

                /* Remove spaces and dots */
                if (w == ' ' || (w == '.' && si != di)) {
                        cf |= NS_LOSS | NS_LFN;
                        continue;
                }

                /* Extension or end of SFN */
                if (i >= ni || si == di) {
                        /* Long extension */
                        if (ni == 11) {
                                cf |= NS_LOSS | NS_LFN;
                                break;
                        }
                        /* Out of 8.3 format */
                        if (si != di) cf |= NS_LOSS | NS_LFN;

                        /* No extension */
                        if (si > di)
                                break;

                        /* Enter extension section */
                        si = di;
                        i  = 8;
                        ni = 11;
                        b <<= 2;
                        continue;
                }

                /* Non ASCII char */
                if (w >= 0x80) {
#ifdef _EXCVT
                        /* Unicode -> OEM code */
                        w = _libfat_convert(w, 0);

                        /* Convert extended char to upper (SBCS) */
                        if (w)
                                w = ExCvt[w - 0x80];
#else
                        /* Upper converted Unicode -> OEM code */
                        w = _libfat_convert(_libfat_wtoupper(w), 0);
#endif
                        /* Force create LFN entry */
                        cf |= NS_LFN;
                }

                /* Double byte char (always false on SBCS cfg) */
                if (_DF1S && w >= 0x100) {
                        if (i >= ni - 1) {
                                cf |= NS_LOSS | NS_LFN;
                                i   = ni;
                                continue;
                        }

                        dj->fn[i++] = (uint8_t)(w >> 8);
                } else {
                        /* Replace illegal chars for SFN */
                        if (!w || strchr("+,;=[]", w)) {
                                /* Lossy conversion */
                                w   = '_';
                                cf |= NS_LOSS | NS_LFN;
                        } else {
                                if (IsUpper(w)) {
                                        b |= 2;
                                } else {
                                        if (IsLower(w)) {
                                                b |= 1;
                                                w -= 0x20;
                                        }
                                }
                        }
                }

                dj->fn[i++] = (uint8_t)w;
        }

        /* If the first char collides with deleted mark, replace it with 0x05 */
        if (dj->fn[0] == DDE)
                dj->fn[0] = NDDE;

        if (ni == 8)
                b <<= 2;

        /* Create LFN entry when there are composite capitals */
        if ((b & 0x0C) == 0x0C || (b & 0x03) == 0x03)
                cf |= NS_LFN;

        /* When LFN is in 8.3 format without extended char, NT flags are created */
        if (!(cf & NS_LFN)) {
                if ((b & 0x03) == 0x01)
                        cf |= NS_EXT;

                if ((b & 0x0C) == 0x04)
                        cf |= NS_BODY;
        }

        /* SFN is created */
        dj->fn[NS] = cf;

        return FR_OK;
#else        /* Non-LFN configuration */
        uint8_t b, c, d, *sfn;
        uint ni, si, i;
        const char *p;

        /* Create file name in directory form, Strip duplicated separator */
        for (p = *path; *p == '/' || *p == '\\'; p++);
        sfn = dj->fn;
        memset(sfn, ' ', 11);
        si = i = b = 0; ni = 8;

        for (;;) {
                c = (uint8_t)p[si++];

                /* Break on end of segment */
                if (c <= ' ' || c == '/' || c == '\\')
                        break;

                if (c == '.' || i >= ni) {
                        if (ni != 8 || c != '.')
                                return FR_INVALID_NAME;

                        i = 8;
                        ni = 11;
                        b <<= 2;
                        continue;
                }

                if (c >= 0x80) {
                        /* Eliminate NT flag */
                        b |= 3;
#ifdef _EXCVT
                        /* To upper extended chars (SBCS cfg) */
                        c = ExCvt[c - 0x80];
#else
#if !_DF1S
                        /* Reject extended chars (ASCII cfg) */
                        return FR_INVALID_NAME;
#endif
#endif
                }

                /* Check if it is a DBC 1st byte (always false on SBCS cfg) */
                if (IsDBCS1(c)) {
                        /* Get 2nd byte */
                        d = (uint8_t)p[si++];

                        /* Reject invalid DBC */
                        if (!IsDBCS2(d) || i >= ni - 1)
                                return FR_INVALID_NAME;

                        sfn[i++] = c;
                        sfn[i++] = d;
                } else {
                        /* Reject illegal chrs for SFN */
                        if (strchr("\"*+,:;<=>\?[]|\x7F", c))
                                return FR_INVALID_NAME;

                        if (IsUpper(c)) {
                                b |= 2;
                        } else {
                                if (IsLower(c)) {
                                        b |= 1; c -= 0x20;
                                }
                        }

                        sfn[i++] = c;
                }
        }

        /* Return pointer to the next segment */
        *path = &p[si];

        /* Set last segment flag if end of path */
        c = (c <= ' ') ? NS_LAST : 0;

        /* Reject null string */
        if (!i)
                return FR_INVALID_NAME;

        /* When first char collides with DDE, replace it with 0x05 */
        if (sfn[0] == DDE)
                sfn[0] = NDDE;

        if (ni == 8)
                b <<= 2;

        /* NT flag (Name extension has only small capital) */
        if ((b & 0x03) == 0x01)
                c |= NS_EXT;

        /* NT flag (Name body has only small capital) */
        if ((b & 0x0C) == 0x04) c |= NS_BODY;

        /* Store NT flag, File name is created */
        sfn[NS] = c;

        return FR_OK;
#endif
}

//==============================================================================
/**
 * @brief Get file information from directory entry
 *
 * @param[in] *dj       Pointer to the directory object
 * @param[in] *fno      Pointer to the file information to be filled
 */
//==============================================================================
static void get_fileinfo(FATDIR *dj, FILEINFO *fno)
{
        uint i;
        uint8_t nt, *dir;

        TCHAR *p = fno->fname;
        if (dj->sect) {
                dir = dj->dir;

                /* NT flag */
                nt = dir[DIR_NTres];

                /* Copy name body */
                for (i = 0; i < 8; i++) {
                        TCHAR c = dir[i];
                        if (c == ' ')
                                break;

                        if (c == NDDE)
                                c = (TCHAR)DDE;

                        if (_LIBFAT_USE_LFN && (nt & NS_BODY) && IsUpper(c))
                                c += 0x20;
#if _LIBFAT_LFN_UNICODE
                        if (IsDBCS1(c) && i < 7 && IsDBCS2(dir[i+1]))
                                c = (c << 8) | dir[++i];

                        c = _libfat_convert(c, 1);
                        if (!c)
                                c = '?';
#endif
                        *p++ = c;
                }

                /* Copy name extension */
                if (dir[8] != ' ') {
                        *p++ = '.';
                        for (i = 8; i < 11; i++) {
                                TCHAR c = dir[i];
                                if (c == ' ')
                                        break;

                                if (_LIBFAT_USE_LFN && (nt & NS_EXT) && IsUpper(c))
                                        c += 0x20;
#if _LIBFAT_LFN_UNICODE
                                if (IsDBCS1(c) && i < 10 && IsDBCS2(dir[i+1]))
                                        c = (c << 8) | dir[++i];

                                c = _libfat_convert(c, 1);
                                if (!c)
                                        c = '?';
#endif
                                *p++ = c;
                        }
                }
                fno->fattrib = dir[DIR_Attr];                   /* Attribute */
                fno->fsize = LOAD_UINT32(dir+DIR_FileSize);     /* Size */
                fno->fdate = LOAD_UINT16(dir+DIR_WrtDate);      /* Date */
                fno->ftime = LOAD_UINT16(dir+DIR_WrtTime);      /* Time */
        }

        /* Terminate SFN str by a \0 */
        *p = 0;

#if _LIBFAT_USE_LFN
        if (fno->lfname && fno->lfsize) {
                TCHAR *tp = fno->lfname;

                i = 0;
                /* Get LFN if available */
                if (dj->sect && dj->lfn_idx != 0xFFFF) {
                        wchar_t *lfn = dj->lfn;

                        /* Get an LFN char */
                        wchar_t w;
                        while ((w = *lfn++) != 0) {
#if !_LIBFAT_LFN_UNICODE
                                w = _libfat_convert(w, 0);
                                if (!w) {
                                        /* Could not convert, no LFN */
                                        i = 0;
                                        break;
                                }

                                /* Put 1st byte if it is a DBC (always false on SBCS cfg) */
                                if (_DF1S && w >= 0x100)
                                        tp[i++] = (TCHAR)(w >> 8);
#endif
                                if (i >= fno->lfsize - 1) {
                                        /* Buffer overflow, no LFN */
                                        i = 0;
                                        break;
                                }

                                tp[i++] = (TCHAR)w;
                        }
                }

                /* Terminate the LFN str by a \0 */
                tp[i] = 0;
        }
#endif
}

//==============================================================================
/**
 * @brief Follow a file path
 *
 * @param[in] *dj       Directory object to return last directory and found object
 * @param[in] *path     Full-path string to find a file or directory
 *
 * @return FR_OK: successful, otherwise: error code
 */
//==============================================================================
static FRESULT follow_path(FATDIR *dj, const TCHAR *path)
{
        FRESULT res;

        /* Strip heading separator if exist */
        if (*path == '/' || *path == '\\')
                path++;

        /* Start from the root dir */
        dj->sclust = 0;

        /* Null path means the start directory itself */
        if ((uint)*path < ' ') {
                res = dir_sdi(dj, 0);
                dj->dir = 0;
        } else {
                /* Follow path */
                for (;;) {
                        res = create_name(dj, &path);
                        if (res != FR_OK)
                                break;

                        res = dir_find(dj);
                        uint8_t ns = *(dj->fn+NS);
                        if (res != FR_OK) {
                                if (res != FR_NO_FILE)
                                        break;

                                if (!(ns & NS_LAST))
                                        res = FR_NO_PATH;

                                break;
                        }

                        if (ns & NS_LAST)
                                break;

                        /* There is next segment. Follow the sub directory */
                        uint8_t *dir = dj->dir;
                        if (!(dir[DIR_Attr] & LIBFAT_AM_DIR)) {
                                /* Cannot follow because it is a file */
                                res = FR_NO_PATH;
                                break;
                        }

                        dj->sclust = ld_clust(dj->fs, dir);
                }
        }

        return res;
}

//==============================================================================
/**
 * @brief Load a sector and check if it is an FAT Volume Boot Record
 *
 * @param[in] *fs       File system object
 * @param[in]  sect     Sector number (lba) to check if it is an FAT boot record or not
 *
 * @return 0:FAT-VBR, 1:Any BR but not FAT, 2:Not a BR, 3:Disk error
 */
//==============================================================================
static uint8_t check_fs(FATFS *fs, uint32_t sect)
{
        /* Load boot record */
        if (_libfat_disk_read(fs->srcfile, fs->win, sect, 1) != RES_OK)
                return 3;

        /* Check record signature (always placed at offset 510 even if the sector size is >512) */
        if (LOAD_UINT16(&fs->win[BS_55AA]) != 0xAA55)
                return 2;

        /* Check "FAT" string */
        if ((LOAD_UINT32(&fs->win[BS_FilSysType]) & 0xFFFFFF) == 0x544146)
                return 0;

        if ((LOAD_UINT32(&fs->win[BS_FilSysType32]) & 0xFFFFFF) == 0x544146)
                return 0;

        return 1;
}

//==============================================================================
/**
 * @brief Check if the file system object is valid or not
 *
 * @param[in] *fs       File system object
 *
 * @return FR_OK: successful, otherwise: any error occurred
 */
//==============================================================================
static FRESULT chk_mounted(FATFS *fs)
{
        uint8_t  fmt;
        uint8_t  b;
        uint32_t bsect, fasize, tsect, sysect, nclst, szbfat;
        uint16_t nrsv;

#if _LIBFAT_MAX_SS != 512
        /* Get disk sector size (variable sector size cfg only) */
        if (_libfat_disk_ioctl(fs->drv, GET_SECTOR_SIZE, &fs->ssize) != RES_OK)
                return FR_DISK_ERR;
#endif

        /* Search FAT partition on the drive. Supports only generic partitions, FDISK and SFD. */

        /* Load sector 0 and check if it is an FAT-VBR (in SFD) */
        fmt = check_fs(fs, bsect = 0);

        if (fmt == 1) {
                /* Not an FAT-VBR, the physical drive can be partitioned */
                return FR_NO_FILESYSTEM;
        }

        if (fmt == 3) {
                return FR_DISK_ERR;
        }

        if (fmt) {
                /* No FAT volume is found */
                return FR_NO_FILESYSTEM;
        }

        /* An FAT volume is found. Following code initializes the file system object */

        /* (BPB_BytsPerSec must be equal to the physical sector size) */
        if (LOAD_UINT16(fs->win+BPB_BytsPerSec) != SS(fs)) {
                return FR_NO_FILESYSTEM;
        }

        /* Number of sectors per FAT */
        fasize = LOAD_UINT16(fs->win+BPB_FATSz16);
        if (!fasize)
                fasize = LOAD_UINT32(fs->win+BPB_FATSz32);
        fs->fsize = fasize;

        /* Number of FAT copies */
        fs->n_fats = b = fs->win[BPB_NumFATs];
        if (b != 1 && b != 2) {
                /* (Must be 1 or 2) */
                return FR_NO_FILESYSTEM;
        }

        /* Number of sectors for FAT area */
        fasize *= b;

        /* Number of sectors per cluster */
        fs->csize = b = fs->win[BPB_SecPerClus];
        if (!b || (b & (b - 1))) {
                /* (Must be power of 2) */
                return FR_NO_FILESYSTEM;
        }

        /* Number of root directory entries */
        fs->n_rootdir = LOAD_UINT16(fs->win+BPB_RootEntCnt);
        if (fs->n_rootdir % (SS(fs) / SZ_DIR)) {
                /* (BPB_RootEntCnt must be sector aligned) */
                return FR_NO_FILESYSTEM;
        }

        /* Number of sectors on the volume */
        tsect = LOAD_UINT16(fs->win+BPB_TotSec16);
        if (!tsect) {
                tsect = LOAD_UINT32(fs->win+BPB_TotSec32);
        }

        /* Number of reserved sectors */
        nrsv = LOAD_UINT16(fs->win+BPB_RsvdSecCnt);
        if (!nrsv) {
                /* (BPB_RsvdSecCnt must not be 0) */
                return FR_NO_FILESYSTEM;
        }

        /* Determine the FAT sub type */ /* RSV+FAT+DIR */
        sysect = nrsv + fasize + fs->n_rootdir / (SS(fs) / SZ_DIR);
        if (tsect < sysect) {
                /* (Invalid volume size) */
                return FR_NO_FILESYSTEM;
        }

        /* Number of clusters */
        nclst = (tsect - sysect) / fs->csize;
        if (!nclst) {
                /* (Invalid volume size) */
                return FR_NO_FILESYSTEM;
        }

        fmt = LIBFAT_FS_FAT12;
        if (nclst >= MIN_FAT16)
                fmt = LIBFAT_FS_FAT16;

        if (nclst >= MIN_FAT32)
                fmt = LIBFAT_FS_FAT32;

        /* Boundaries and Limits */
        fs->n_fatent = nclst + 2;       /* Number of FAT entries */
        fs->volbase  = bsect;           /* Volume start sector */
        fs->fatbase  = bsect + nrsv;    /* FAT start sector */
        fs->database = bsect + sysect;  /* Data start sector */
        if (fmt == LIBFAT_FS_FAT32) {
                if (fs->n_rootdir) {
                        /* (BPB_RootEntCnt must be 0) */
                        return FR_NO_FILESYSTEM;
                }

                /* Root directory start cluster */
                fs->dirbase = LOAD_UINT32(fs->win+BPB_RootClus);

                /* (Required FAT size) */
                szbfat = fs->n_fatent * 4;
        } else {
                if (!fs->n_rootdir) {
                        /* (BPB_RootEntCnt must not be 0) */
                        return FR_NO_FILESYSTEM;
                }

                /* Root directory start sector */
                fs->dirbase = fs->fatbase + fasize;

                /* (Required FAT size) */
                szbfat = (fmt == LIBFAT_FS_FAT16) ? fs->n_fatent * 2 : fs->n_fatent * 3 / 2 + (fs->n_fatent & 1);
        }

        /* (BPB_FATSz must not be less than required) */
        if (fs->fsize < (szbfat + (SS(fs) - 1)) / SS(fs))
                return FR_NO_FILESYSTEM;

        /* Initialize cluster allocation information */
        fs->free_clust = 0xFFFFFFFF;
        fs->last_clust = 0;

        /* Get fsinfo if available */
        if (fmt == LIBFAT_FS_FAT32) {
                fs->fsi_flag = 0;
                fs->fsi_sector = bsect + LOAD_UINT16(fs->win+BPB_FSInfo);

                if (  _libfat_disk_read(fs->srcfile, fs->win, fs->fsi_sector, 1) == RES_OK
                   && LOAD_UINT16(fs->win+BS_55AA)      == 0xAA55
                   && LOAD_UINT32(fs->win+FSI_LeadSig)  == 0x41615252
                   && LOAD_UINT32(fs->win+FSI_StrucSig) == 0x61417272) {

                                fs->last_clust = LOAD_UINT32(fs->win+FSI_Nxt_Free);
                                fs->free_clust = LOAD_UINT32(fs->win+FSI_Free_Count);
                }
        }

        /* FAT sub-type */
        fs->fs_type = fmt;

        /* Invalidate sector cache */
        fs->winsect = 0;
        fs->wflag = 0;

#if _LIBFAT_FS_LOCK
        /* Clear file lock semaphores */
        clear_lock(fs);
#endif

        return FR_OK;
}

//==============================================================================
/**
 * @brief Check if the file/dir object is valid or not
 *
 * @param[in] *obj      Pointer to the object FIL/DIR to check validity
 *
 * @retrun FR_OK(0): The object is valid, otherwise: Invalid
 */
//==============================================================================
static FRESULT validate(void *obj)
{
        /* Assuming offset of fs and id in the FATFILE/FATDIR are identical */
        FATFILE *fil = (FATFILE*)obj;

        if (!fil || !fil->fs || !fil->fs->fs_type || fil->fs->id != fil->id)
                return FR_INVALID_OBJECT;

        /* Lock file system */
        ENTER_FF(fil->fs);

        return FR_OK;
}

//==============================================================================
/**
 * @brief Mount a Logical Drive
 *
 * @param[in] *fsfile           pointer to opened source file
 * @param[in] *fs               pointer to existing library instance
 *
 * @retval FR_OK
 * @retval FR_DISK_ERR
 * @retval FR_INT_ERR
 */
//==============================================================================
FRESULT libfat_mount(FILE *fsfile, FATFS *fs)
{
        if (fs) {
                fs->srcfile = fsfile;

                /* Create sync object for the new volume */
                if (!_libfat_create_mutex(&fs->sobj))
                        return FR_INT_ERR;

                if (chk_mounted(fs) == FR_OK)
                        return FR_OK;
        }

        return FR_DISK_ERR;
}

//==============================================================================
/**
 * @brief Function unmount drive
 *
 * @param[in] *fs       pointer to existing library instance
 *
 * @retval FR_OK
 * @retval FR_DISK_ERR
 */
//==============================================================================
FRESULT libfat_umount(FATFS *fs)
{
        if (fs) {
                _libfat_delete_mutex(fs->sobj);
                return FR_OK;
        }

        return FR_DISK_ERR;
}

//==============================================================================
/**
 * @brief Open or create a File
 *
 * @param[in]  *fs       Pointer to existing library instance
 * @param[out] *fp       Pointer to the blank file object
 * @param[in]  *path     Pointer to the file name
 * @param[in]   mode     Access mode and file open mode flags
 *
 * @retval FR_OK
 * @retval FR_INVALID_OBJECT
 * @retval FR_INVALID_NAME
 * @retval FR_NO_FILE
 * @retval FR_DENIED
 * @retval FR_EXIST
 * @retval FR_DISK_ERR
 */
//==============================================================================
FRESULT libfat_open(FATFS *fs, FATFILE *fp, const TCHAR *path, uint8_t mode)
{
        FRESULT  res;
        FATDIR   dj;
        uint8_t *dir;
        DEF_NAMEBUF;

        if (!fp)
                return FR_INVALID_OBJECT;

        fp->fs = fs;

        mode &= LIBFAT_FA_READ | LIBFAT_FA_WRITE | LIBFAT_FA_CREATE_ALWAYS | LIBFAT_FA_OPEN_ALWAYS | LIBFAT_FA_CREATE_NEW;

        ENTER_FF(fs);
        dj.fs = fs;

        INIT_BUF(dj);

        /* Follow the file path */
        res = follow_path(&dj, path);
        dir = dj.dir;

        if (res == FR_OK) {
                if (!dir) {
                        /* Current dir itself */
                        res = FR_INVALID_NAME;
                }
#if _LIBFAT_FS_LOCK
                else {
                        res = chk_lock(&dj, (mode & ~LIBFAT_FA_READ) ? 1 : 0);
                }
#endif
        }

        /* Create or Open a file */
        if (mode & (LIBFAT_FA_CREATE_ALWAYS | LIBFAT_FA_OPEN_ALWAYS | LIBFAT_FA_CREATE_NEW)) {
                uint32_t dw, cl;

                /* No file, create new */
                if (res != FR_OK) {
                        if (res == FR_NO_FILE) {
#if _LIBFAT_FS_LOCK
                                res = enq_lock(fs) ? dir_register(&dj) : FR_TOO_MANY_OPEN_FILES;
                        }
#else
                                res = dir_register(&dj);
                        }
#endif
                        /* File is created */
                        mode |= LIBFAT_FA_CREATE_ALWAYS;
                        dir = dj.dir;
                } else {
                        if (dir[DIR_Attr] & (LIBFAT_AM_RDO | LIBFAT_AM_DIR)) {
                                /* Cannot overwrite it (R/O or DIR) */
                                res = FR_DENIED;
                        } else {
                                if (mode & LIBFAT_FA_CREATE_NEW) {
                                        /* Cannot create as new file */
                                        res = FR_EXIST;
                                }
                        }
                }

                /* Truncate it if overwrite mode */
                if (res == FR_OK && (mode & LIBFAT_FA_CREATE_ALWAYS)) {
                        /* Created time */
                        dw = _libfat_get_fattime();
                        STORE_UINT32(dir+DIR_CrtTime, dw);
                        dir[DIR_Attr] = 0;
                        STORE_UINT32(dir + DIR_FileSize, 0);
                        cl = ld_clust(dj.fs, dir);
                        st_clust(dir, 0);
                        dj.fs->wflag = 1;

                        /* Remove the cluster chain if exist */
                        if (cl) {
                                dw = dj.fs->winsect;
                                res = remove_chain(dj.fs, cl);
                                if (res == FR_OK) {
                                        /* Reuse the cluster hole */
                                        dj.fs->last_clust = cl - 1;
                                        res = move_window(dj.fs, dw);
                                }
                        }
                }
        } else {
                /* Open an existing file */
                if (res == FR_OK) {
                        if (dir[DIR_Attr] & LIBFAT_AM_DIR) {
                                /* It is a directory */
                                res = FR_NO_FILE;
                        } else {
                                if ((mode & LIBFAT_FA_WRITE) && (dir[DIR_Attr] & LIBFAT_AM_RDO)) {
                                        /* R/O violation */
                                        res = FR_DENIED;
                                }
                        }
                }
        }

        if (res == FR_OK) {
                if (mode & LIBFAT_FA_CREATE_ALWAYS) {
                        /* Set file change flag if created or overwritten */
                        mode |= LIBFAT_FA__WRITTEN;
                }

                /* Pointer to the directory entry */
                fp->dir_sect = dj.fs->winsect;
                fp->dir_ptr = dir;
#if _LIBFAT_FS_LOCK
                fp->lockid = inc_lock(&dj, (mode & ~LIBFAT_FA_READ) ? 1 : 0);
                if (!fp->lockid)
                        res = FR_INT_ERR;
#endif
        }

        FREE_BUF();

        if (res == FR_OK) {
                fp->flag   = mode;                              /* File access mode */
                fp->sclust = ld_clust(dj.fs, dir);              /* File start cluster */
                fp->fsize  = LOAD_UINT32(dir+DIR_FileSize);     /* File size */
                fp->fptr   = 0;                                 /* File pointer */
                fp->dsect  = 0;

                /* Validate file object */
                fp->fs = dj.fs;
                fp->id = dj.fs->id;
        }

        LEAVE_FF(dj.fs, res);
}

//==============================================================================
/**
 * @brief Read File
 *
 * @param[out] *fp       Pointer to the blank file object
 * @param[in]  *buff     Pointer to data buffer
 * @param[in]   btr      Number of bytes to read
 * @param[in]  *br       Pointer to number of bytes read
 *
 * @retval FR_OK
 * @retval FR_INVALID_OBJECT
 * @retval FR_INVALID_NAME
 * @retval FR_NO_FILE
 * @retval FR_DENIED
 * @retval FR_EXIST
 * @retval FR_DISK_ERR
 * @retval FR_INT_ERR
 */
//==============================================================================
FRESULT libfat_read(FATFILE *fp, void *buff, uint btr, uint *br)
{
        FRESULT  res;
        uint32_t clst, sect, remain;
        uint     rcnt, cc;
        uint8_t  csect, *rbuff = (uint8_t*)buff;

        /* Clear read byte counter */
        *br = 0;

        res = validate(fp);
        if (res != FR_OK) {
                LEAVE_FF(fp->fs, res);
        }

        if (fp->flag & LIBFAT_FA__ERROR) {
                LEAVE_FF(fp->fs, FR_INT_ERR);
        }

        /* Check access mode */
        if (!(fp->flag & LIBFAT_FA_READ)) {
                LEAVE_FF(fp->fs, FR_DENIED);
        }

        remain = fp->fsize - fp->fptr;

        if (btr > remain) {
                /* Truncate btr by remaining bytes */
                btr = (uint)remain;
        }

        /* Repeat until all data read */
        for ( ; btr; rbuff += rcnt, fp->fptr += rcnt, *br += rcnt, btr -= rcnt) {
                /* On the sector boundary? */
                if ((fp->fptr % SS(fp->fs)) == 0) {
                        /* Sector offset in the cluster */
                        csect = (uint8_t)(fp->fptr / SS(fp->fs) & (fp->fs->csize - 1));

                        /* On the cluster boundary? */
                        if (!csect) {
                                /* On the top of the file? */
                                if (fp->fptr == 0) {
                                        /* Follow from the origin */
                                        clst = fp->sclust;
                                } else {
                                        /* Follow cluster chain on the FAT */
                                        clst = get_fat(fp->fs, fp->clust);
                                }

                                if (clst < 2) {
                                        ABORT(fp->fs, FR_INT_ERR);
                                }

                                if (clst == 0xFFFFFFFF) {
                                        ABORT(fp->fs, FR_DISK_ERR);
                                }

                                /* Update current cluster */
                                fp->clust = clst;
                        }

                        /* Get current sector */
                        sect = clust2sect(fp->fs, fp->clust);
                        if (!sect) {
                                ABORT(fp->fs, FR_INT_ERR);
                        }

                        sect += csect;

                        /* When remaining bytes >= sector size */
                        cc = btr / SS(fp->fs);
                        if (cc) {
                                /* Read maximum contiguous sectors directly */
                                if (csect + cc > fp->fs->csize) {
                                        /* Clip at cluster boundary */
                                        cc = fp->fs->csize - csect;
                                }

                                if (_libfat_disk_read(fp->fs->srcfile, rbuff, sect, (uint8_t)cc) != RES_OK) {
                                        ABORT(fp->fs, FR_DISK_ERR);
                                }
#if _LIBFAT_FS_TINY
                                if (fp->fs->wflag && fp->fs->winsect - sect < cc) {
                                        memcpy(rbuff + ((fp->fs->winsect - sect) * SS(fp->fs)), fp->fs->win, SS(fp->fs));
                                }
#else
                                if ((fp->flag & LIBFAT_FA__DIRTY) && fp->dsect - sect < cc) {
                                        memcpy(rbuff + ((fp->dsect - sect) * SS(fp->fs)), fp->buf, SS(fp->fs));
                                }
#endif
                                /* Number of bytes transferred */
                                rcnt = SS(fp->fs) * cc;
                                continue;
                        }
#if !_LIBFAT_FS_TINY
                        /* Load data sector if not in cache */
                        if (fp->dsect != sect) {

                                /* Write-back dirty sector cache */
                                if (fp->flag & LIBFAT_FA__DIRTY) {
                                        if (_libfat_disk_write(fp->fs->srcfile, fp->buf, fp->dsect, 1) != RES_OK) {
                                                ABORT(fp->fs, FR_DISK_ERR);
                                        }

                                        fp->flag &= ~LIBFAT_FA__DIRTY;
                                }

                                /* Fill sector cache */
                                if (_libfat_disk_read(fp->fs->srcfile, fp->buf, sect, 1) != RES_OK) {
                                        ABORT(fp->fs, FR_DISK_ERR);
                                }
                        }
#endif
                        fp->dsect = sect;
                }

                /* Get partial sector data from sector buffer */
                rcnt = SS(fp->fs) - ((uint)fp->fptr % SS(fp->fs));
                if (rcnt > btr)
                        rcnt = btr;
#if _LIBFAT_FS_TINY
                /* Move sector window */
                if (move_window(fp->fs, fp->dsect)) {
                        ABORT(fp->fs, FR_DISK_ERR);
                }

                /* Pick partial sector */
                memcpy(rbuff, &fp->fs->win[fp->fptr % SS(fp->fs)], rcnt);
#else
                /* Pick partial sector */
                memcpy(rbuff, &fp->buf[fp->fptr % SS(fp->fs)], rcnt);
#endif
        }

        LEAVE_FF(fp->fs, FR_OK);
}

//==============================================================================
/**
 * @brief Write File
 *
 * @param[out] *fp       Pointer to the blank file object
 * @param[in]  *buff     Pointer to the data to be written
 * @param[in]   btr      Number of bytes to write
 * @param[in]  *br       Pointer to number of bytes written
 *
 * @retval FR_OK
 * @retval FR_INVALID_OBJECT
 * @retval FR_INVALID_NAME
 * @retval FR_NO_FILE
 * @retval FR_DENIED
 * @retval FR_EXIST
 * @retval FR_DISK_ERR
 * @retval FR_INT_ERR
 */
//==============================================================================
FRESULT libfat_write(FATFILE *fp, const void *buff, uint btw, uint *bw)
{
        FRESULT  res;
        uint32_t clst, sect;
        uint     wcnt, cc;
        const uint8_t *wbuff = (const uint8_t*)buff;
        uint8_t  csect;

        /* Clear write byte counter */
        *bw = 0;

        res = validate(fp);
        if (res != FR_OK) {
                LEAVE_FF(fp->fs, res);
        }

        if (fp->flag & LIBFAT_FA__ERROR) {
                LEAVE_FF(fp->fs, FR_INT_ERR);
        }

        if (!(fp->flag & LIBFAT_FA_WRITE)) {
                LEAVE_FF(fp->fs, FR_DENIED);
        }

        if ((uint32_t)(fp->fsize + btw) < fp->fsize) {
                /* File size cannot reach 4GB */
                btw = 0;
        }

        /* Repeat until all data written */
        for ( ; btw; wbuff += wcnt, fp->fptr += wcnt, *bw += wcnt, btw -= wcnt) {
                /* On the sector boundary? */
                if ((fp->fptr % SS(fp->fs)) == 0) {
                        /* Sector offset in the cluster */
                        csect = (uint8_t)(fp->fptr / SS(fp->fs) & (fp->fs->csize - 1));

                        if (!csect) {
                                /* On the top of the file? */
                                if (fp->fptr == 0) {
                                        clst = fp->sclust;
                                        if (clst == 0) {
                                                fp->sclust = clst = create_chain(fp->fs, 0);
                                        }
                                } else {
                                        /* Follow or stretch cluster chain on the FAT */
                                        clst = create_chain(fp->fs, fp->clust);
                                }

                                if (clst == 0) {
                                        /* Could not allocate a new cluster (disk full) */
                                        break;
                                }

                                if (clst == 1) {
                                        ABORT(fp->fs, FR_INT_ERR);
                                }

                                if (clst == 0xFFFFFFFF) {
                                        ABORT(fp->fs, FR_DISK_ERR);
                                }

                                /* Update current cluster */
                                fp->clust = clst;
                        }
#if _LIBFAT_FS_TINY
                        if (fp->fs->winsect == fp->dsect && sync_window(fp->fs)) {
                                /* Write-back sector cache */
                                ABORT(fp->fs, FR_DISK_ERR);
                        }
#else
                        if (fp->flag & LIBFAT_FA__DIRTY) {
                                /* Write-back sector cache */
                                if (_libfat_disk_write(fp->fs->srcfile, fp->buf, fp->dsect, 1) != RES_OK) {
                                        ABORT(fp->fs, FR_DISK_ERR);
                                }

                                fp->flag &= ~LIBFAT_FA__DIRTY;
                        }
#endif
                        /* Get current sector */
                        sect = clust2sect(fp->fs, fp->clust);
                        if (!sect) {
                                ABORT(fp->fs, FR_INT_ERR);
                        }

                        sect += csect;

                        /* When remaining bytes >= sector size */
                        cc = btw / SS(fp->fs);

                        /* Write maximum contiguous sectors directly */
                        if (cc) {
                                /* Clip at cluster boundary */
                                if (csect + cc > fp->fs->csize) {
                                        cc = fp->fs->csize - csect;
                                }

                                if (_libfat_disk_write(fp->fs->srcfile, wbuff, sect, (uint8_t)cc) != RES_OK) {
                                        ABORT(fp->fs, FR_DISK_ERR);
                                }
#if _LIBFAT_FS_TINY
                                /* Refill sector cache if it gets invalidated by the direct write */
                                if (fp->fs->winsect - sect < cc) {
                                        memcpy(fp->fs->win, wbuff + ((fp->fs->winsect - sect) * SS(fp->fs)), SS(fp->fs));
                                        fp->fs->wflag = 0;
                                }
#else
                                /* Refill sector cache if it gets invalidated by the direct write */
                                if (fp->dsect - sect < cc) {
                                        memcpy(fp->buf, wbuff + ((fp->dsect - sect) * SS(fp->fs)), SS(fp->fs));
                                        fp->flag &= ~LIBFAT_FA__DIRTY;
                                }
#endif
                                /* Number of bytes transferred */
                                wcnt = SS(fp->fs) * cc;
                                continue;
                        }
#if _LIBFAT_FS_TINY
                        /* Avoid silly cache filling at growing edge */
                        if (fp->fptr >= fp->fsize) {
                                if (sync_window(fp->fs)) {
                                        ABORT(fp->fs, FR_DISK_ERR);
                                }

                                fp->fs->winsect = sect;
                        }
#else
                        /* Fill sector cache with file data */
                        if (fp->dsect != sect) {
                                if (fp->fptr < fp->fsize && _libfat_disk_read(fp->fs->srcfile, fp->buf, sect, 1) != RES_OK) {
                                        ABORT(fp->fs, FR_DISK_ERR);
                                }
                        }
#endif
                        fp->dsect = sect;
                }

                /* Put partial sector into file I/O buffer */
                wcnt = SS(fp->fs) - ((uint)fp->fptr % SS(fp->fs));
                if (wcnt > btw)
                        wcnt = btw;
#if _LIBFAT_FS_TINY
                /* Move sector window */
                if (move_window(fp->fs, fp->dsect)) {
                        ABORT(fp->fs, FR_DISK_ERR);
                }

                /* Fit partial sector */
                memcpy(&fp->fs->win[fp->fptr % SS(fp->fs)], wbuff, wcnt);
                fp->fs->wflag = 1;
#else
                /* Fit partial sector */
                memcpy(&fp->buf[fp->fptr % SS(fp->fs)], wbuff, wcnt);
                fp->flag |= LIBFAT_FA__DIRTY;
#endif
        }

        /* Update file size if needed */
        if (fp->fptr > fp->fsize) {
                fp->fsize = fp->fptr;
        }

        /* Set file change flag */
        fp->flag |= LIBFAT_FA__WRITTEN;

        LEAVE_FF(fp->fs, FR_OK);
}

//==============================================================================
/**
 * @brief Synchronize the File Object
 *
 * @param[in] *fp       Pointer to the blank file object
 *
 * @retval FR_OK
 * @retval FR_INVALID_OBJECT
 * @retval FR_DISK_ERR
 * @retval FR_INT_ERR
 */
//==============================================================================
FRESULT libfat_sync(FATFILE *fp)
{
        FRESULT  res;
        uint32_t tm;
        uint8_t *dir;

        res = validate(fp);
        if (res == FR_OK) {
                /* Has the file been written? */
                if (fp->flag & LIBFAT_FA__WRITTEN) {
#if !_LIBFAT_FS_TINY        /* Write-back dirty buffer */
                        if (fp->flag & LIBFAT_FA__DIRTY) {
                                if (_libfat_disk_write(fp->fs->srcfile, fp->buf, fp->dsect, 1) != RES_OK) {
                                        LEAVE_FF(fp->fs, FR_DISK_ERR);
                                }

                                fp->flag &= ~LIBFAT_FA__DIRTY;
                        }
#endif
                        /* Update the directory entry */
                        res = move_window(fp->fs, fp->dir_sect);
                        if (res == FR_OK) {
                                dir = fp->dir_ptr;

                                /* Set archive bit */
                                dir[DIR_Attr] |= LIBFAT_AM_ARC;

                                /* Update file size */
                                STORE_UINT32(dir+DIR_FileSize, fp->fsize);

                                /* Update start cluster */
                                st_clust(dir, fp->sclust);

                                /* Update updated time */
                                tm = _libfat_get_fattime();
                                STORE_UINT32(dir+DIR_WrtTime, tm);
                                STORE_UINT16(dir+DIR_LstAccDate, 0);
                                fp->flag &= ~LIBFAT_FA__WRITTEN;
                                fp->fs->wflag = 1;
                                res = sync_fs(fp->fs);
                        }
                }
        }

        LEAVE_FF(fp->fs, res);
}

//==============================================================================
/**
 * @brief Close File
 *
 * @param[in] *fp       Pointer to the blank file object
 *
 * @retval FR_OK
 * @retval FR_INVALID_OBJECT
 * @retval FR_DISK_ERR
 * @retval FR_INT_ERR
 */
//==============================================================================
FRESULT libfat_close(FATFILE *fp)
{
        FRESULT res;

        /* Flush cached data */
        res = libfat_sync(fp);
#if _LIBFAT_FS_LOCK
        /* Decrement open counter */
        if (res == FR_OK) {
                FATFS *fs = fp->fs;;
                res = validate(fp);
                if (res == FR_OK) {
                        res = dec_lock(fp->fs, fp->lockid);
                        unlock_fs(fs, FR_OK);
                }
        }
#endif
        if (res == FR_OK) {
                /* Discard file object */
                fp->fs = 0;
        }

        return res;
}

//==============================================================================
/**
 * @brief Seek File R/W Pointer
 *
 * @param[in] *fp       Pointer to the blank file object
 * @param[in]  ofs      File pointer from top of file
 *
 * @retval FR_OK
 * @retval FR_INVALID_OBJECT
 * @retval FR_DISK_ERR
 * @retval FR_INT_ERR
 */
//==============================================================================
FRESULT libfat_lseek(FATFILE *fp, uint32_t ofs)
{
        FRESULT res;

        res = validate(fp);
        if (res != FR_OK) {
                LEAVE_FF(fp->fs, res);
        }

        if (fp->flag & LIBFAT_FA__ERROR) {
                LEAVE_FF(fp->fs, FR_INT_ERR);
        }

        uint32_t clst, bcs, nsect, ifptr;

        /* In read-only mode, clip offset with the file size */
        if (ofs > fp->fsize  && !(fp->flag & LIBFAT_FA_WRITE)) {
                ofs = fp->fsize;
        }

        ifptr = fp->fptr;
        fp->fptr = nsect = 0;
        if (ofs) {
                /* Cluster size (byte) */
                bcs = (uint32_t)fp->fs->csize * SS(fp->fs);

                /* When seek to same or following cluster, */
                if (ifptr > 0 && (ofs - 1) / bcs >= (ifptr - 1) / bcs) {
                        /* start from the current cluster */
                        fp->fptr = (ifptr - 1) & ~(bcs - 1);
                        ofs -= fp->fptr;
                        clst = fp->clust;
                } else {
                        /* start from the first cluster */
                        clst = fp->sclust;

                        /* If no cluster chain, create a new chain */
                        if (clst == 0) {
                                clst = create_chain(fp->fs, 0);
                                if (clst == 1) {
                                        ABORT(fp->fs, FR_INT_ERR);
                                }

                                if (clst == 0xFFFFFFFF) {
                                        ABORT(fp->fs, FR_DISK_ERR);
                                }

                                fp->sclust = clst;
                        }

                        fp->clust = clst;
                }

                if (clst != 0) {
                        /* Cluster following loop */
                        while (ofs > bcs) {

                                /* Check if in write mode or not */
                                if (fp->flag & LIBFAT_FA_WRITE) {
                                        /* Force stretch if in write mode */
                                        clst = create_chain(fp->fs, clst);
                                        if (clst == 0) {
                                                /* When disk gets full, clip file size */
                                                ofs = bcs;
                                                break;
                                        }
                                } else {
                                        /* Follow cluster chain if not in write mode */
                                        clst = get_fat(fp->fs, clst);
                                }

                                if (clst == 0xFFFFFFFF) {
                                        ABORT(fp->fs, FR_DISK_ERR);
                                }

                                if (clst <= 1 || clst >= fp->fs->n_fatent) {
                                        ABORT(fp->fs, FR_INT_ERR);
                                }

                                fp->clust = clst;
                                fp->fptr += bcs;
                                ofs      -= bcs;
                        }

                        fp->fptr += ofs;
                        if (ofs % SS(fp->fs)) {
                                /* Current sector */
                                nsect = clust2sect(fp->fs, clst);

                                if (!nsect) {
                                        ABORT(fp->fs, FR_INT_ERR);
                                }

                                nsect += ofs / SS(fp->fs);
                        }
                }
        }

        /* Fill sector cache if needed */
        if (fp->fptr % SS(fp->fs) && nsect != fp->dsect) {
#if !_LIBFAT_FS_TINY

                /* Write-back dirty sector cache */
                if (fp->flag & LIBFAT_FA__DIRTY) {
                        if (_libfat_disk_write(fp->fs->srcfile, fp->buf, fp->dsect, 1) != RES_OK) {
                                ABORT(fp->fs, FR_DISK_ERR);
                        }

                        fp->flag &= ~LIBFAT_FA__DIRTY;
                }

                /* Fill sector cache */
                if (_libfat_disk_read(fp->fs->srcfile, fp->buf, nsect, 1) != RES_OK) {
                        ABORT(fp->fs, FR_DISK_ERR);
                }
#endif
                fp->dsect = nsect;
        }

        /* Set file change flag if the file size is extended */
        if (fp->fptr > fp->fsize) {
                fp->fsize = fp->fptr;
                fp->flag |= LIBFAT_FA__WRITTEN;
        }

        LEAVE_FF(fp->fs, res);
}

//==============================================================================
/**
 * @brief Create a Directory Object
 *
 * @param[in] *fp       Pointer to the blank file object
 * @param[in] *dj,      Pointer to directory object to create
 * @param[in] *path     Pointer to the directory path
 *
 * @retval FR_OK
 * @retval FR_INVALID_OBJECT
 * @retval FR_DISK_ERR
 * @retval FR_NO_PATH
 * @retval FR_INT_ERR
 * @retval FR_NO_FILE
 */
//==============================================================================
FRESULT libfat_opendir(FATFS *fs, FATDIR *dj, const TCHAR *path)
{
        FRESULT res;

        DEF_NAMEBUF;

        if (!dj)
                return FR_INVALID_OBJECT;

        ENTER_FF(fs);
        dj->fs = fs;

        INIT_BUF(*dj);
        res = follow_path(dj, path);
        FREE_BUF();

        /* Follow completed */
        if (res == FR_OK) {
                if (dj->dir) {
                        /* It is not the root dir */
                        if (dj->dir[DIR_Attr] & LIBFAT_AM_DIR) {
                                /* The object is a directory */
                                dj->sclust = ld_clust(fs, dj->dir);
                        } else {
                                res = FR_NO_PATH;
                        }
                }

                if (res == FR_OK) {
                        dj->id = fs->id;

                        /* Rewind dir */
                        res = dir_sdi(dj, 0);
                }
        }

        if (res == FR_NO_FILE) {
                res = FR_NO_PATH;
        }

        if (res != FR_OK) {
                /* Invalidate the dir object if function faild */
                dj->fs = 0;
        }

        LEAVE_FF(fs, res);
}

//==============================================================================
/**
 * @brief Read Directory Entry in Sequence
 *
 * @param[in] *dj,      Pointer to the open directory object
 * @param[in] *fno      Pointer to file information to return
 *
 * @retval FR_OK
 * @retval FR_INVALID_OBJECT
 * @retval FR_DISK_ERR
 * @retval FR_NO_PATH
 * @retval FR_INT_ERR
 * @retval FR_NO_FILE
 */
//==============================================================================
FRESULT libfat_readdir(FATDIR *dj, FILEINFO *fno)
{
        FRESULT res;
        DEF_NAMEBUF;

        res = validate(dj);
        if (res == FR_OK) {
                if (!fno) {
                        /* Rewind the directory object */
                        res = dir_sdi(dj, 0);
                } else {
                        INIT_BUF(*dj);

                        res = dir_read(dj, 0);
                        if (res == FR_NO_FILE) {
                                /* Reached end of dir */
                                dj->sect = 0;
                                res = FR_OK;
                        }

                        if (res == FR_OK) {
                                get_fileinfo(dj, fno);
                                res = dir_next(dj, 0);
                                if (res == FR_NO_FILE) {
                                        dj->sect = 0;
                                        res = FR_OK;
                                }
                        }

                        FREE_BUF();
                }
        }

        LEAVE_FF(dj->fs, res);
}

//==============================================================================
/**
 * @brief Get File Status
 *
 * @param[in] *fs       Pointer to existing library instance
 * @param[in] *path     Pointer to the file path
 * @param[in] *fno      Pointer to file information to return
 *
 * @retval FR_OK
 * @retval FR_INVALID_OBJECT
 * @retval FR_DISK_ERR
 * @retval FR_NO_PATH
 * @retval FR_INT_ERR
 * @retval FR_INVALID_NAME
 * @retval FR_NO_FILE
 */
//==============================================================================
FRESULT libfat_stat(FATFS *fs, const TCHAR *path, FILEINFO *fno)
{
        FRESULT res;
        FATDIR  dj;
        DEF_NAMEBUF;

        ENTER_FF(fs);
        dj.fs = fs;

        INIT_BUF(dj);

        res = follow_path(&dj, path);
        if (res == FR_OK) {
                if (dj.dir) {
                        get_fileinfo(&dj, fno);
                } else {
                        res = FR_INVALID_NAME;
                }
        }

        FREE_BUF();

        LEAVE_FF(dj.fs, res);
}

//==============================================================================
/**
 * @brief Get Number of Free Clusters
 *
 * @param[in]  *fs      Pointer to return pointer to corresponding file system object
 * @param[out] *nclst   Pointer to a variable to return number of free clusters
 *
 * @retval FR_OK
 * @retval FR_INVALID_OBJECT
 * @retval FR_DISK_ERR
 * @retval FR_NO_PATH
 * @retval FR_INT_ERR
 * @retval FR_INVALID_NAME
 * @retval FR_NO_FILE
 */
//==============================================================================
FRESULT libfat_getfree(FATFS *fs, uint32_t *nclst)
{
        FRESULT  res = FR_OK;
        uint32_t n, clst, sect, stat;

        ENTER_FF(fs);

        /* If free_clust is valid, return it without full cluster scan */
        if (fs->free_clust <= fs->n_fatent - 2) {
                *nclst = fs->free_clust;
        } else {
                /* Get number of free clusters */
                uint8_t fat = fs->fs_type;
                n   = 0;
                if (fat == LIBFAT_FS_FAT12) {
                        clst = 2;
                        do {
                                stat = get_fat(fs, clst);
                                if (stat == 0xFFFFFFFF) {
                                        res = FR_DISK_ERR;
                                        break;
                                }

                                if (stat == 1) {
                                        res = FR_INT_ERR;
                                        break;
                                }

                                if (stat == 0)
                                        n++;
                        } while (++clst < fs->n_fatent);
                } else {
                        clst = fs->n_fatent;
                        sect = fs->fatbase;
                        uint i = 0;
                        uint8_t *p = 0;
                        do {
                                if (!i) {
                                        res = move_window(fs, sect++);
                                        if (res != FR_OK)
                                                break;

                                        p = fs->win;
                                        i = SS(fs);
                                }
                                if (fat == LIBFAT_FS_FAT16) {
                                        if (LOAD_UINT16(p) == 0)
                                                n++;

                                        p += 2;
                                        i -= 2;
                                } else {
                                        if ((LOAD_UINT32(p) & 0x0FFFFFFF) == 0)
                                                n++;

                                        p += 4;
                                        i -= 4;
                                }
                        } while (--clst);
                }

                fs->free_clust = n;

                if (fat == LIBFAT_FS_FAT32)
                        fs->fsi_flag = 1;

                *nclst = n;
        }

        LEAVE_FF(fs, res);
}

//==============================================================================
/**
 * @brief Truncate File
 *
 * @param[in] *fp       Pointer to the blank file object
 *
 * @retval FR_OK
 * @retval FR_INVALID_OBJECT
 * @retval FR_DISK_ERR
 * @retval FR_NO_PATH
 * @retval FR_INT_ERR
 * @retval FR_DENIED
 * @retval FR_NO_FILE
 */
//==============================================================================
FRESULT libfat_truncate(FATFILE *fp)
{
        FRESULT res;
        uint32_t ncl;

        res = validate(fp);
        if (res == FR_OK) {
                if (fp->flag & LIBFAT_FA__ERROR) {
                        res = FR_INT_ERR;
                } else {
                        if (!(fp->flag & LIBFAT_FA_WRITE)) {
                                res = FR_DENIED;
                        }
                }
        }
        if (res == FR_OK) {
                if (fp->fsize > fp->fptr) {
                        /* Set file size to current R/W point */
                        fp->fsize = fp->fptr;

                        fp->flag |= LIBFAT_FA__WRITTEN;
                        if (fp->fptr == 0) {
                                /* When set file size to zero, remove entire cluster chain */
                                res = remove_chain(fp->fs, fp->sclust);
                                fp->sclust = 0;
                        } else {
                                /* When truncate a part of the file, remove remaining clusters */
                                ncl = get_fat(fp->fs, fp->clust);
                                res = FR_OK;
                                if (ncl == 0xFFFFFFFF)
                                        res = FR_DISK_ERR;

                                if (ncl == 1)
                                        res = FR_INT_ERR;

                                if (res == FR_OK && ncl < fp->fs->n_fatent) {
                                        res = put_fat(fp->fs, fp->clust, 0x0FFFFFFF);

                                        if (res == FR_OK)
                                                res = remove_chain(fp->fs, ncl);
                                }
                        }
                }

                if (res != FR_OK)
                        fp->flag |= LIBFAT_FA__ERROR;
        }

        LEAVE_FF(fp->fs, res);
}

//==============================================================================
/**
 * @brief Delete a File or Directory
 *
 * @param[in] *fs       Pointer to existing library instance
 * @param[in] *path     Pointer to the file or directory path
 *
 * @retval FR_OK
 * @retval FR_INVALID_OBJECT
 * @retval FR_DISK_ERR
 * @retval FR_NO_PATH
 * @retval FR_INT_ERR
 * @retval FR_INVALID_NAME
 * @retval FR_NO_FILE
 */
//==============================================================================
FRESULT libfat_unlink(FATFS *fs, const TCHAR *path)
{
        FRESULT  res;
        FATDIR   dj, sdj;
        uint32_t dclst;
        DEF_NAMEBUF;

        ENTER_FF(fs);
        dj.fs = fs;

        INIT_BUF(dj);
        res = follow_path(&dj, path);
#if _LIBFAT_FS_LOCK
        if (res == FR_OK) {
                /* Cannot remove open file */
                res = chk_lock(&dj, 2);
        }
#endif
        if (res == FR_OK) {
                uint8_t *dir = dj.dir;
                if (!dir) {
                        /* Cannot remove the start directory */
                        res = FR_INVALID_NAME;
                } else {
                        if (dir[DIR_Attr] & LIBFAT_AM_RDO) {
                                /* Cannot remove R/O object */
                                res = FR_DENIED;
                        }
                }

                dclst = ld_clust(dj.fs, dir);

                /* Is it a sub-dir? */
                if (res == FR_OK && (dir[DIR_Attr] & LIBFAT_AM_DIR)) {
                        if (dclst < 2) {
                                res = FR_INT_ERR;
                        } else {
                                /* Check if the sub-dir is empty or not */
                                memcpy(&sdj, &dj, sizeof (FATDIR));
                                sdj.sclust = dclst;

                                /* Exclude dot entries */
                                res = dir_sdi(&sdj, 2);
                                if (res == FR_OK) {
                                        res = dir_read(&sdj, 0);
                                        if (res == FR_OK)
                                                res = FR_DENIED;

                                        if (res == FR_NO_FILE)
                                                res = FR_OK;
                                }
                        }
                }

                if (res == FR_OK) {
                        /* Remove the directory entry */
                        res = dir_remove(&dj);
                        if (res == FR_OK) {
                                /* Remove the cluster chain if exist */
                                if (dclst)
                                        res = remove_chain(dj.fs, dclst);

                                if (res == FR_OK)
                                        res = sync_fs(dj.fs);
                        }
                }
        }

        FREE_BUF();

        LEAVE_FF(dj.fs, res);
}

//==============================================================================
/**
 * @brief Create a Directory
 *
 * @param[in] *fs       Pointer to existing library instance
 * @param[in] *path     Pointer to the directory path
 *
 * @retval FR_OK
 * @retval FR_INVALID_OBJECT
 * @retval FR_DISK_ERR
 * @retval FR_NO_PATH
 * @retval FR_INT_ERR
 * @retval FR_INVALID_NAME
 * @retval FR_NO_FILE
 */
//==============================================================================
FRESULT libfat_mkdir(FATFS *fs, const TCHAR *path)
{
        FRESULT  res;
        FATDIR   dj;
        uint8_t  n;
        uint32_t dsc, dcl, pcl, tm = _libfat_get_fattime();
        DEF_NAMEBUF;

        ENTER_FF(fs);
        dj.fs = fs;

        INIT_BUF(dj);
        res = follow_path(&dj, path);
        if (res == FR_OK) {
                /* Any object with same name is already existing */
                res = FR_EXIST;
        }

        if (res == FR_NO_FILE) {
                /* Allocate a cluster for the new directory table */
                dcl = create_chain(dj.fs, 0);
                res = FR_OK;

                if (dcl == 0) {
                        /* No space to allocate a new cluster */
                        res = FR_DENIED;
                }

                if (dcl == 1) {
                        res = FR_INT_ERR;
                }

                if (dcl == 0xFFFFFFFF) {
                        res = FR_DISK_ERR;
                }


                if (res == FR_OK) {
                        /* Flush FAT */
                        res = sync_window(dj.fs);
                }

                uint8_t *dir;
                if (res == FR_OK) {
                        /* Initialize the new directory table */
                        dsc = clust2sect(dj.fs, dcl);
                        dir = dj.fs->win;
                        memset(dir, 0, SS(dj.fs));

                        /* Create "." entry */
                        memset(dir+DIR_Name, ' ', 11);
                        dir[DIR_Name] = '.';
                        dir[DIR_Attr] = LIBFAT_AM_DIR;
                        STORE_UINT32(dir+DIR_WrtTime, tm);
                        st_clust(dir, dcl);

                        /* Create ".." entry */
                        memcpy(dir+SZ_DIR, dir, SZ_DIR);

                        dir[33] = '.'; pcl = dj.sclust;
                        if (dj.fs->fs_type == LIBFAT_FS_FAT32 && pcl == dj.fs->dirbase) {
                                pcl = 0;
                        }

                        st_clust(dir+SZ_DIR, pcl);
                        for (n = dj.fs->csize; n; n--) {
                                /* Write dot entries and clear following sectors */
                                dj.fs->winsect = dsc++;
                                dj.fs->wflag = 1;

                                res = sync_window(dj.fs);
                                if (res != FR_OK)
                                        break;

                                memset(dir, 0, SS(dj.fs));
                        }
                }

                if (res == FR_OK) {
                        /* Register the object to the directory */
                        res = dir_register(&dj);
                }

                if (res != FR_OK) {
                        /* Could not register, remove cluster chain */
                        remove_chain(dj.fs, dcl);
                } else {
                        dir = dj.dir;
                        dir[DIR_Attr] = LIBFAT_AM_DIR;          /* Attribute */
                        STORE_UINT32(dir+DIR_WrtTime, tm);      /* Created time */
                        st_clust(dir, dcl);                     /* Table start cluster */
                        dj.fs->wflag = 1;
                        res = sync_fs(dj.fs);
                }
        }

        FREE_BUF();

        LEAVE_FF(dj.fs, res);
}

//==============================================================================
/**
 * @brief Change Attribute
 *
 * @param[in] *fs       Pointer to existing library instance
 * @param[in] *path     Pointer to the directory path
 * @param[in]  value    Attribute bits
 * @param[in]  mask     Attribute mask to change
 *
 * @retval FR_OK
 * @retval FR_INVALID_OBJECT
 * @retval FR_DISK_ERR
 * @retval FR_NO_PATH
 * @retval FR_INT_ERR
 * @retval FR_INVALID_NAME
 * @retval FR_NO_FILE
 */
//==============================================================================
FRESULT libfat_chmod(FATFS *fs, const TCHAR *path, uint8_t value, uint8_t mask)
{
        FRESULT  res;
        FATDIR   dj;
        DEF_NAMEBUF;

        ENTER_FF(fs);
        dj.fs = fs;

        INIT_BUF(dj);
        res = follow_path(&dj, path);
        FREE_BUF();

        if (res == FR_OK) {
                uint8_t *dir = dj.dir;
                if (!dir) {
                        res = FR_INVALID_NAME;
                } else {
                        /* Valid attribute mask */
                        mask &= LIBFAT_AM_RDO | LIBFAT_AM_HID | LIBFAT_AM_SYS | LIBFAT_AM_ARC;

                        /* Apply attribute change */
                        dir[DIR_Attr] = (value & mask) | (dir[DIR_Attr] & (uint8_t)~mask);
                        dj.fs->wflag = 1;
                        res = sync_fs(dj.fs);
                }
        }

        LEAVE_FF(dj.fs, res);
}

//==============================================================================
/**
 * @brief Change Timestamp
 *
 * @param[in] *fs       Pointer to existing library instance
 * @param[in] *path     Pointer to the directory path
 * @param[in] *fno      Pointer to the time stamp to be set
 *
 * @retval FR_OK
 * @retval FR_INVALID_OBJECT
 * @retval FR_DISK_ERR
 * @retval FR_NO_PATH
 * @retval FR_INT_ERR
 * @retval FR_INVALID_NAME
 * @retval FR_NO_FILE
 */
//==============================================================================
FRESULT libfat_utime(FATFS *fs, const TCHAR *path, const FILEINFO *fno)
{
        FRESULT  res;
        FATDIR   dj;
        DEF_NAMEBUF;

        ENTER_FF(fs);
        dj.fs = fs;

        INIT_BUF(dj);
        res = follow_path(&dj, path);
        FREE_BUF();

        if (res == FR_OK) {
                uint8_t *dir = dj.dir;
                if (!dir) {
                        res = FR_INVALID_NAME;
                } else {
                        /* File or sub-directory */
                        STORE_UINT16(dir+DIR_WrtTime, fno->ftime);
                        STORE_UINT16(dir+DIR_WrtDate, fno->fdate);
                        dj.fs->wflag = 1;
                        res = sync_fs(dj.fs);
                        }
                }

        LEAVE_FF(dj.fs, res);
}

//==============================================================================
/**
 * @brief Rename File/Directory
 *
 * @param[in] *fs       Pointer to existing library instance
 * @param[in] *path_old Pointer to the old name
 * @param[in] *path_new Pointer to the new name
 *
 * @retval FR_OK
 * @retval FR_INVALID_OBJECT
 * @retval FR_DISK_ERR
 * @retval FR_NO_PATH
 * @retval FR_INT_ERR
 * @retval FR_INVALID_NAME
 * @retval FR_NO_FILE
 */
//==============================================================================
FRESULT libfat_rename(FATFS *fs, const TCHAR *path_old, const TCHAR *path_new)
{
        FRESULT  res;
        FATDIR   djo, djn;
        uint8_t  buf[21];
        uint32_t dw;
        DEF_NAMEBUF;

        ENTER_FF(fs);
        djo.fs = fs;
        djn.fs = djo.fs;
        INIT_BUF(djo);
        res = follow_path(&djo, path_old);
#if _LIBFAT_FS_LOCK
        if (res == FR_OK)
                res = chk_lock(&djo, 2);
#endif
        if (res == FR_OK) {
                if (!djo.dir) {
                        res = FR_NO_FILE;
                } else {
                        /* Save the object information except for name */
                        memcpy(buf, djo.dir+DIR_Attr, 21);

                        /* Check new object */
                        memcpy(&djn, &djo, sizeof (FATDIR));

                        res = follow_path(&djn, path_new);
                        if (res == FR_OK) {
                                /* The new object name is already existing */
                                res = FR_EXIST;
                        }

                        if (res == FR_NO_FILE) {

/* Start critical section that any interruption can cause a cross-link */

                                /* Register the new entry */
                                res = dir_register(&djn);
                                if (res == FR_OK) {
                                        /* Copy object information except for name */
                                        uint8_t *dir = djn.dir;
                                        memcpy(dir+13, buf+2, 19);
                                        dir[DIR_Attr] = buf[0] | LIBFAT_AM_ARC;
                                        djo.fs->wflag = 1;
                                        if (djo.sclust != djn.sclust && (dir[DIR_Attr] & LIBFAT_AM_DIR)) {
                                                dw = clust2sect(djo.fs, ld_clust(djo.fs, dir));
                                                if (!dw) {
                                                        res = FR_INT_ERR;
                                                } else {
                                                        res = move_window(djo.fs, dw);

                                                        /* .. entry */
                                                        dir = djo.fs->win+SZ_DIR;
                                                        if (res == FR_OK && dir[1] == '.') {
                                                                dw = (djo.fs->fs_type == LIBFAT_FS_FAT32 && djn.sclust == djo.fs->dirbase) ? 0 : djn.sclust;
                                                                st_clust(dir, dw);
                                                                djo.fs->wflag = 1;
                                                        }
                                                }
                                        }

                                        if (res == FR_OK) {
                                                res = dir_remove(&djo);
                                                if (res == FR_OK)
                                                        res = sync_fs(djo.fs);
                                        }
                                }
/* End critical section */
                        }
                }
        }

        FREE_BUF();

        LEAVE_FF(djo.fs, res);
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
