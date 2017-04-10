/*=========================================================================*//**
@file    libfat_user.c

@author  Daniel Zorychta

@brief   FAT file system library based on ChaN's code.

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the Free Software Foundation and modified by the dnx RTOS exception.

         NOTE: The modification  to the GPL is  included to allow you to
               distribute a combined work that includes dnx RTOS without
               being obliged to provide the source  code for proprietary
               components outside of the dnx RTOS.

         The dnx RTOS  is  distributed  in the hope  that  it will be useful,
         but WITHOUT  ANY  WARRANTY;  without  even  the implied  warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         Full license text is available on the following file: doc/license.txt.


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
#include "fs/fs.h"
#include "libfat_user.h"
#include "libfat_conf.h"

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
        int err = sys_fseek(srcfile, (u64_t)sector * _LIBFAT_MAX_SS, SEEK_SET);
        if (err == ESUCC) {
                size_t rdcnt = 0;
                err = sys_fread(buff, _LIBFAT_MAX_SS * count, &rdcnt, srcfile);
                if (err == ESUCC && rdcnt == _LIBFAT_MAX_SS * count) {
                        return RES_OK;
                }
        }

        return RES_ERROR;
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
        int err = sys_fseek(srcfile, (u64_t)sector * _LIBFAT_MAX_SS, SEEK_SET);
        if (err == ESUCC) {
                size_t wrcnt = 0;
                err = sys_fwrite(buff, _LIBFAT_MAX_SS * count, &wrcnt, srcfile);
                if (err == ESUCC && wrcnt == _LIBFAT_MAX_SS * count) {
                        return RES_OK;
                }
        }

        return RES_ERROR;
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
                if (sys_fflush(srcfile) == 0)
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
        time_t t;
        sys_get_time(&t);

        struct tm tm;
        sys_gmtime_r(&t, &tm);

        uint32_t fattime = (tm.tm_sec >> 1)
                         | (tm.tm_min << 5)
                         | (tm.tm_hour << 11)
                         | (tm.tm_mday << 16)
                         | ((tm.tm_mon + 1) << 21)
                         | ((tm.tm_year - 80) << 25);

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
        return sys_mutex_create(MUTEX_TYPE_NORMAL, sobj) == ESUCC ? 1 : 0;
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
        return sys_mutex_destroy(sobj) == ESUCC ? 1 : 0;
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
        return sys_mutex_lock(sobj, _LIBFAT_FS_TIMEOUT) == ESUCC ? 1 : 0;
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
        sys_mutex_unlock(sobj);
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
        void *mem = NULL;
        sys_malloc(msize, &mem);
        return mem;
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
        sys_free(&mblock);
}

/*==============================================================================
  End of file
==============================================================================*/
