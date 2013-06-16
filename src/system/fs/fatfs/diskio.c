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

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read(FILE *srcfile, BYTE *buff, DWORD sector, BYTE count)
{
        fseek(srcfile, (u64_t)sector * _MAX_SS, SEEK_SET);
        if (fread(buff, _MAX_SS, count, srcfile) != count) {
                return RES_ERROR;
        } else {
                return RES_OK;
        }
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT disk_write(FILE *srcfile, const BYTE *buff, DWORD sector, BYTE count)
{
        fseek(srcfile, sector * _MAX_SS, SEEK_SET);
        if (fwrite(buff, _MAX_SS, count, srcfile) != count) {
                return RES_ERROR;
        } else {
                return RES_OK;
        }
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
#if _USE_IOCTL
DRESULT disk_ioctl(FILE *srcfile, BYTE cmd, void *buff)
{
        (void) srcfile;
        (void) buff;

        switch (cmd) {
        case CTRL_SYNC: return RES_OK;
        }

        return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Get time                                                              */
/*-----------------------------------------------------------------------*/
DWORD get_fattime(void)
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

