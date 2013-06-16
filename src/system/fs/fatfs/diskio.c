/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various existing       */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "ffconf.h"

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
DRESULT disk_read(FILE *srcfile, uint8_t *buff, uint32_t sector, uint8_t count)
{
        fseek(srcfile, (u64_t)sector * _MAX_SS, SEEK_SET);
        if (fread(buff, _MAX_SS, count, srcfile) != count) {
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
DRESULT disk_write(FILE *srcfile, const uint8_t *buff, uint32_t sector, uint8_t count)
{
        fseek(srcfile, sector * _MAX_SS, SEEK_SET);
        if (fwrite(buff, _MAX_SS, count, srcfile) != count) {
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
DRESULT disk_ioctl(FILE *srcfile, uint8_t cmd, void *buff)
{
        (void) srcfile;
        (void) buff;

        switch (cmd) {
        case CTRL_SYNC:
                return RES_OK;
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
uint32_t get_fattime(void)
{
        return 0;
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
int ff_cre_syncobj(_SYNC_t* sobj)
{
        if (sobj) {
                _SYNC_t mtx = new_mutex();
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
int ff_del_syncobj (_SYNC_t sobj)
{
        if (sobj) {
                delete_mutex(sobj);
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
int ff_req_grant(_SYNC_t sobj)
{
        if (sobj) {
                if (lock_mutex(sobj, _FS_TIMEOUT) == MUTEX_LOCKED) {
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
void ff_rel_grant(_SYNC_t sobj)
{
        if (sobj) {
                unlock_mutex(sobj);
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
void *ff_memalloc(uint msize)
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
void ff_memfree(void *mblock)
{
        free(mblock);
}

/*==============================================================================
  End of file
==============================================================================*/

