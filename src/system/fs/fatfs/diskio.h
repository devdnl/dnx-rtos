/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file   (C)ChaN, 2013
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#include "ffconf.h"
#include "system/dnxfs.h"

/* Status of Disk Functions */
typedef uint8_t DSTATUS;

/* Results of Disk Functions */
typedef enum {
        RES_OK = 0,                     /* 0: Successful */
        RES_ERROR,                      /* 1: R/W Error */
        RES_WRPRT,                      /* 2: Write Protected */
        RES_NOTRDY,                     /* 3: Not Ready */
        RES_PARERR                      /* 4: Invalid Parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */


extern DSTATUS disk_initialize(FILE*);
extern DSTATUS disk_status(FILE*);
extern DRESULT disk_read(FILE*, uint8_t*, uint32_t, uint8_t);
extern DRESULT disk_write(FILE*, const uint8_t*, uint32_t, uint8_t);
extern DRESULT disk_ioctl(FILE*, uint8_t, void*);

#if _FS_REENTRANT
extern int  ff_cre_syncobj(_SYNC_t* sobj);             /* Create a sync object */
extern int  ff_req_grant(_SYNC_t sobj);                /* Lock sync object */
extern void ff_rel_grant(_SYNC_t sobj);                /* Unlock sync object */
extern int  ff_del_syncobj(_SYNC_t sobj);              /* Delete a sync object */
#endif

extern uint32_t get_fattime (void);


#if _USE_LFN == 3                               /* Memory functions */
extern void *ff_memalloc(uint msize);                 /* Allocate memory block */
extern void  ff_memfree(void *mblock);                 /* Free memory block */
#endif

/* Disk Status Bits (DSTATUS) */
#define STA_NOINIT              0x01    /* Drive not initialized */
#define STA_NODISK              0x02    /* No medium in the drive */
#define STA_PROTECT             0x04    /* Write protected */


/* Command code for disk_ioctrl function */
/* Generic command (used by FatFs) */
#define CTRL_SYNC               0        /* Flush disk cache (for write functions) */
#define GET_SECTOR_COUNT        1        /* Get media size (for only f_mkfs()) */
#define GET_SECTOR_SIZE         2        /* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define GET_BLOCK_SIZE          3        /* Get erase block size (for only f_mkfs()) */
#define CTRL_ERASE_SECTOR       4        /* Force erased a block of sectors (for only _USE_ERASE) */

/* Generic command (not used by FatFs) */
#define CTRL_POWER              5        /* Get/Set power status */
#define CTRL_LOCK               6        /* Lock/Unlock media removal */
#define CTRL_EJECT              7        /* Eject media */
#define CTRL_FORMAT             8        /* Create physical format on the media */

/* MMC/SDC specific ioctl command */
#define MMC_GET_TYPE            10        /* Get card type */
#define MMC_GET_CSD             11        /* Get CSD */
#define MMC_GET_CID             12        /* Get CID */
#define MMC_GET_OCR             13        /* Get OCR */
#define MMC_GET_SDSTAT          14        /* Get SD status */

/* ATA/CF specific ioctl command */
#define ATA_GET_REV             20        /* Get F/W revision */
#define ATA_GET_MODEL           21        /* Get model name */
#define ATA_GET_SN              22        /* Get serial number */

/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC                  0x01                /* MMC ver 3 */
#define CT_SD1                  0x02                /* SD ver 1 */
#define CT_SD2                  0x04                /* SD ver 2 */
#define CT_SDC                  (CT_SD1|CT_SD2)     /* SD */
#define CT_BLOCK                0x08                /* Block addressing */


#ifdef __cplusplus
}
#endif

#endif

/*==============================================================================
  End of file
==============================================================================*/
