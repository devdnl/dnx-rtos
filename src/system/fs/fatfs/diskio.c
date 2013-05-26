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
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(FILE *srcfile)
{
        (void) srcfile;
        return 0;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(FILE *srcfile)
{
        (void) srcfile;
        return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (FILE *srcfile, BYTE *buff, DWORD sector, BYTE count)
{
        fseek(srcfile, sector * _MAX_SS, SEEK_SET);
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
        switch (cmd) {
        case CTRL_SYNC: return RES_OK;
        }

        return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Get time                                                              */
/*-----------------------------------------------------------------------*/

DWORD get_fattime (void)
{
        return 0;
}
