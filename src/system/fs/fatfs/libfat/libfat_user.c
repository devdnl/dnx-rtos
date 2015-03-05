/*=========================================================================*//**
@file    libfat_user.c

@author  Daniel Zorychta

@brief   FAT file system library based on ChaN's code.

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
/*----------------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013             */
/*----------------------------------------------------------------------------*/
/* If a working storage control module is available, it should be             */
/* attached to the FatFs via a glue function rather than modifying it.        */
/* This is an example of glue functions to attach various existing            */
/* storage control module to the FatFs module with a defined API.             */
/*----------------------------------------------------------------------------*/

/*==============================================================================
  Include files
==============================================================================*/
#include "core/fs.h"
#include "libfat_user.h"
#include "libfat_conf.h"
#include <time.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Read Sector(s)
 *
 * @param[in]  *srcfile         source file object
 * @param[out] *buff            destination buffer
 * @param[in]   sector          sector number
 * @param[in]   count           sector to read
 *
 * @retval RES_OK read successful
 * @retval RES_ERROR read error
 */
//==============================================================================
DRESULT _libfat_disk_read(FILE *srcfile, uint8_t *buff, uint32_t sector, uint8_t count)
{
        _sys_fseek(srcfile, (u64_t)sector * _LIBFAT_MAX_SS, SEEK_SET);
        if (_sys_fread(buff, _LIBFAT_MAX_SS, count, srcfile) != count) {
                return RES_ERROR;
        } else {
                return RES_OK;
        }
}

//==============================================================================
/**
 * @brief Write Sector(s)
 *
 * @param[in]  *srcfile         source file object
 * @param[in]  *buff            source buffer
 * @param[in]   sector          sector number
 * @param[in]   count           sector to write
 *
 * @retval RES_OK write successful
 * @retval RES_ERROR write error
 */
//==============================================================================
DRESULT _libfat_disk_write(FILE *srcfile, const uint8_t *buff, uint32_t sector, uint8_t count)
{
        _sys_fseek(srcfile, (u64_t)sector * _LIBFAT_MAX_SS, SEEK_SET);
        if (_sys_fwrite(buff, _LIBFAT_MAX_SS, count, srcfile) != count) {
                return RES_ERROR;
        } else {
                return RES_OK;
        }
}

//==============================================================================
/**
 * @brief Miscellaneous Functions
 *
 * @param[in]     *srcfile      source file object
 * @param[in]      cmd          command
 * @param[in/out] *buff         input/output buffer
 *
 * @retval RES_OK write successful
 * @retval RES_PARERR argument error
 */
//==============================================================================
DRESULT _libfat_disk_ioctl(FILE *srcfile, uint8_t cmd, void *buff)
{
        (void) srcfile;
        (void) buff;

        switch (cmd) {
        case CTRL_SYNC:
                if (_sys_fflush(srcfile) == 0)
                        return RES_OK;
                else
                        return RES_ERROR;
        }

        return RES_PARERR;
}
//==============================================================================
/**
 * @brief Get time in FAT format
 *
 * @return Current time is returned packed. The bit field is as follows:
 *      bit31:25        Year from 1980 (0..127)
 *      bit24:21        Month (1..12)
 *      bit20:16        Day in month(1..31)
 *      bit15:11        Hour (0..23)
 *      bit10:5         Minute (0..59)
 *      bit4:0          Second / 2 (0..29)
 */
//==============================================================================
uint32_t _libfat_get_fattime(void)
{
        time_t     t  = time(NULL);
        struct tm *tm = gmtime(&t);

        uint32_t fattime = ((tm->tm_sec  / 2) & 0x1F)
                         | ((tm->tm_min  & 0x3F) << 5)
                         | ((tm->tm_hour & 0x1F) << 11)
                         | (((tm->tm_mon  + 1 ) & 0x1F) << 16)
                         | (((tm->tm_year - 80) & 0x7F) << 25);

        return fattime;
}

//==============================================================================
/**
 * @brief Create a sync object
 *
 * @param[out] *sobj    Pointer to return the created sync object
 *
 * @retval 1: Function succeeded
 * @retval 0: Could not create due to any error
 */
//==============================================================================
int _libfat_create_mutex(_LIBFAT_MUTEX_t *sobj)
{
        if (sobj) {
                _LIBFAT_MUTEX_t mtx = _sys_mutex_new(MUTEX_NORMAL);
                if (mtx) {
                        *sobj = mtx;
                        return 1;
                }
        }

        return 0;
}

//==============================================================================
/**
 * @brief Delete a sync object
 *
 * @param[in] sobj      Sync object tied to the logical drive to be deleted
 *
 * @retval 1: Function succeeded
 * @retval 0: Could not create due to any error
 */
//==============================================================================
int _libfat_delete_mutex (_LIBFAT_MUTEX_t sobj)
{
        if (sobj) {
                _sys_mutex_delete(sobj);
                return 1;
        }

        return 0;
}

//==============================================================================
/**
 * @brief Lock sync object
 *
 * @param[in] sobj      Sync object to wait
 *
 * @retval 1: Function succeeded
 * @retval 0: Could not create due to any error
 */
//==============================================================================
int _libfat_lock_access(_LIBFAT_MUTEX_t sobj)
{
        if (sobj) {
                if (_sys_mutex_lock(sobj, _LIBFAT_FS_TIMEOUT)) {
                        return 1;
                }
        }

        return 0;
}

//==============================================================================
/**
 * @brief Unlock sync object
 *
 * @param[in] sobj      Sync object to be signaled
 */
//==============================================================================
void _libfat_unlock_access(_LIBFAT_MUTEX_t sobj)
{
        if (sobj) {
                _sys_mutex_unlock(sobj);
        }
}

//==============================================================================
/**
 * @brief Memory allocation
 *
 * @param[in] msize     requested block size
 *
 * @return pointer to block, otherwise NULL
 */
//==============================================================================
void *_libfat_malloc(uint msize)
{
        return malloc(msize);
}

//==============================================================================
/**
 * @brief Memory freeing
 *
 * @param[in] mblock    block to free
 */
//==============================================================================
void _libfat_free(void *mblock)
{
        free(mblock);
}

/*==============================================================================
  End of file
==============================================================================*/
