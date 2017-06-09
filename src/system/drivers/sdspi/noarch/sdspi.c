/*=========================================================================*//**
@file    sdspi.c

@author  Daniel Zorychta

@brief   This file support SD in SPI mode

@note    Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"
#include "noarch/sdspi_cfg.h"
#include "sys/ioctl.h"
#include "../sdspi_ioctl.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define MTX_BLOCK_TIME                                  0
#define MTX_BLOCK_TIME_LONG                             200
#define RELEASE_TIMEOUT_MS                              1000

/* MBR definitions */
#define MBR_BOOTSTRAP_CODE_OFFSET                       0x000
#define MBR_PARTITION_1_ENTRY_OFFSET                    0x1BE
#define MBR_PARTITION_2_ENTRY_OFFSET                    0x1CE
#define MBR_PARTITION_3_ENTRY_OFFSET                    0x1DE
#define MBR_PARTITION_4_ENTRY_OFFSET                    0x1EE
#define MBR_BOOT_SIGNATURE_OFFSET                       0x1FE

#define MBR_PARITION_ENTRY_STATUS_OFFSET                0x00
#define MBR_PARITION_ENTRY_CHS_FIRST_ADDR_OFFSET        0x01
#define MBR_PARITION_ENTRY_PARTITION_TYPE_OFFSET        0x04
#define MBR_PARITION_ENTRY_CHS_LAST_ADDR_OFFSET         0x05
#define MBR_PARITION_ENTRY_LBA_FIRST_ADDR_OFFSET        0x08
#define MBR_PARITION_ENTRY_NUM_OF_SECTORS_OFFSET        0x0C

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/* minor numbers */
enum {
        _SDSPI_VOLUME,
        _SDSPI_PARTITION_1,
        _SDSPI_PARTITION_2,
        _SDSPI_PARTITION_3,
        _SDSPI_PARTITION_4
};

/** card command definitions */
typedef enum {
        CMD0   = (0x40+0 ),             /* GO_IDLE_STATE */
        CMD1   = (0x40+1 ),             /* SEND_OP_COND (MMC) */
        ACMD41 = (0xC0+41),             /* SEND_OP_COND (SDC) */
        CMD8   = (0x40+8 ),             /* SEND_IF_COND */
        CMD9   = (0x40+9 ),             /* SEND_CSD */
        CMD10  = (0x40+10),             /* SEND_CID */
        CMD12  = (0x40+12),             /* STOP_TRANSMISSION */
        ACMD13 = (0xC0+13),             /* SD_STATUS (SDC) */
        CMD16  = (0x40+16),             /* SET_BLOCKLEN */
        CMD17  = (0x40+17),             /* READ_SINGLE_BLOCK */
        CMD18  = (0x40+18),             /* READ_MULTIPLE_BLOCK */
        CMD23  = (0x40+23),             /* SET_BLOCK_COUNT (MMC) */
        ACMD23 = (0xC0+23),             /* SET_WR_BLK_ERASE_COUNT (SDC) */
        CMD24  = (0x40+24),             /* WRITE_BLOCK */
        CMD25  = (0x40+25),             /* WRITE_MULTIPLE_BLOCK */
        CMD55  = (0x40+55),             /* APP_CMD */
        CMD58  = (0x40+58)              /* READ_OCR */
} card_cmd_t;

/** card types */
typedef struct {
        enum {CT_UNKNOWN, CT_MMC, CT_SD1, CT_SD2} type : 2;
        bool block : 1;
} card_type;

typedef struct {
        u32_t      offset;              /* partition offset sector    */
        u32_t      size;                /* partition size in sectors  */
        bool       used;                /* true if part used          */
} part_t;

/** main control structure */
typedef struct {
        FILE      *SPI_file;
        mutex_t   *protect_mtx;
        u32_t      timeout_ms;
        card_type  type;
        bool       initialized;
        u8_t       part_init;
        part_t     part[_SDSPI_PARTITION_4 + 1];
} SDSPI_ctrl_t;

/** driver instance associated with parition */
typedef struct {
        SDSPI_ctrl_t *stg;              /* module storage. */
        u8_t          minor;            /* minor number. */
} SDSPI_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void     SPI_select_card                    (SDSPI_t *hdl);
static void     SPI_deselect_card                  (SDSPI_t *hdl);
static u8_t     SPI_transive                       (SDSPI_t *hdl, u8_t out);
static int      SPI_transmit_block                 (SDSPI_t *hdl, const u8_t *block, size_t count);
static int      SPI_receive_block                  (SDSPI_t *hdl, u8_t *block, size_t count);
static u8_t     card_send_cmd                      (SDSPI_t *hdl, card_cmd_t cmd, u32_t arg);
static u8_t     card_wait_ready                    (SDSPI_t *hdl);
static bool     card_receive_data_block            (SDSPI_t *hdl, u8_t *buff);
static bool     card_transmit_data_block           (SDSPI_t *hdl, const u8_t *buff, u8_t token);
static ssize_t  card_read_entire_sectors           (SDSPI_t *hdl, u8_t *dst, size_t nsectors, u64_t lseek);
static ssize_t  card_read_partial_sectors          (SDSPI_t *hdl, u8_t *dst, size_t size, u64_t lseek);
static ssize_t  card_write_entire_sectors          (SDSPI_t *hdl, const u8_t *src, size_t nsectors, u64_t lseek);
static ssize_t  card_write_partial_sectors         (SDSPI_t *hdl, const u8_t *src, size_t size, u64_t lseek);
static int      card_initialize                    (SDSPI_t *hdl);
static ssize_t  card_read                          (SDSPI_t *hdl, u8_t *dst, size_t count, u64_t lseek, size_t *rdcnt);
static ssize_t  card_write                         (SDSPI_t *hdl, const u8_t *src, size_t count, u64_t lseek, size_t *wrcnt);
static u16_t    MBR_get_boot_signature             (u8_t *sector);
static u32_t    MBR_get_partition_first_LBA_sector (int partition, u8_t *sector);
static u32_t    MBR_get_partition_number_of_sectors(int partition, u8_t *sector);
static int      MBR_detect_partitions              (SDSPI_t *hdl);

/*==============================================================================
  Local object definitions
==============================================================================*/
MODULE_NAME(SDSPI);

static const u16_t SECTOR_SIZE = 512;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize device
 *
 * @param[out]          **device_handle        device allocated memory
 * @param[in ]            major                major device number
 * @param[in ]            minor                minor device number
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(SDSPI, void **device_handle, u8_t major, u8_t minor)
{
        int err = ENODEV;

        if (minor > _SDSPI_PARTITION_4) {
                return err;
        }

        SDSPI_t *hdl = NULL;

        if (minor == 0) {
                err = sys_zalloc(sizeof(SDSPI_t), cast(void**, &hdl));
                if (err) goto finish;

                err = sys_zalloc(sizeof(SDSPI_ctrl_t), cast(void**, &hdl->stg));
                if (err) goto finish;

                err = sys_mutex_create(MUTEX_TYPE_NORMAL, &hdl->stg->protect_mtx);
                if (err) goto finish;

                finish:
                if (err) {
                        if (hdl) {
                                if (hdl->stg) {
                                        if (hdl->stg->protect_mtx) {
                                                sys_mutex_destroy(hdl->stg->protect_mtx);
                                        }

                                        sys_free(cast(void**, &hdl->stg));
                                }

                                sys_free(cast(void**, &hdl));
                        }
                }

        } else {
                err = sys_zalloc(sizeof(SDSPI_t), cast(void**, &hdl));
                if (!err) {
                        SDSPI_t *hdl0 = NULL;
                        err = sys_module_get_instance(major, 0, cast(void**, &hdl0));
                        if (err) {
                                sys_free(cast(void**, &hdl));
                        } else {
                                hdl->minor = minor;
                                hdl->stg   = hdl0->stg;
                                hdl->stg->part_init++;
                        }
                }
        }

        *device_handle = hdl;

        return err;
}

//==============================================================================
/**
 * @brief Release device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_RELEASE(SDSPI, void *device_handle)
{
        SDSPI_t *hdl = device_handle;

        int err = EBUSY;

        if (!hdl->stg->part[hdl->minor].used) {
                if (hdl->minor == 0) {
                        if (hdl->stg->part_init == 0) {
                                sys_fclose(hdl->stg->SPI_file);
                                sys_mutex_destroy(hdl->stg->protect_mtx);
                                sys_free(cast(void**, &hdl->stg));
                                sys_free(cast(void**, &hdl));
                                err = ESUCC;
                        }

                } else {
                        sys_free(cast(void**, &hdl));
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Open device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_OPEN(SDSPI, void *device_handle, u32_t flags)
{
        UNUSED_ARG1(flags);

        SDSPI_t *hdl = device_handle;

        return hdl->stg->part[hdl->minor].used ? EBUSY : ESUCC;
}

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_CLOSE(SDSPI, void *device_handle, bool force)
{
        UNUSED_ARG1(force);

        SDSPI_t *hdl = device_handle;

        hdl->stg->part[hdl->minor].used = false;

        return ESUCC;
}

//==============================================================================
/**
 * @brief Write data to device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_WRITE(SDSPI,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fattr);

        SDSPI_t *hdl = device_handle;
        part_t *part = &hdl->stg->part[hdl->minor];
        int      err = ENOMEDIUM;

        if (part->size > 0) {
                err = sys_mutex_lock(hdl->stg->protect_mtx, MAX_DELAY_MS);
                if (!err) {
                        u64_t lseek = *fpos + (cast(u64_t, part->offset) * SECTOR_SIZE);
                        err = card_write(hdl, src, count, lseek, wrcnt);
                        sys_mutex_unlock(hdl->stg->protect_mtx);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Read data from device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_READ(SDSPI,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        SDSPI_t *hdl = device_handle;
        part_t *part = &hdl->stg->part[hdl->minor];
        int      err = ENOMEDIUM;

        if (part->size > 0) {
                err = sys_mutex_lock(hdl->stg->protect_mtx, MAX_DELAY_MS);
                if (!err) {
                        u64_t lseek = *fpos + (cast(u64_t, part->offset) * SECTOR_SIZE);
                        err = card_read(hdl, dst, count, lseek, rdcnt);
                        sys_mutex_unlock(hdl->stg->protect_mtx);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_IOCTL(SDSPI, void *device_handle, int request, void *arg)
{
        SDSPI_t *hdl = device_handle;

        int err = EBADRQC;

        switch (request) {
        case IOCTL_SDSPI__CONFIGURE: {
                SDSPI_config_t *sdspi_cfg = arg;

                if (hdl->stg->SPI_file) {
                        err = sys_fclose(hdl->stg->SPI_file);
                        if (err) {
                                break;
                        }
                }

                err = sys_fopen(sdspi_cfg->filepath, "r+", &hdl->stg->SPI_file);
                if (!err) {

                        hdl->stg->timeout_ms = sdspi_cfg->timeout;

                        SPI_config_t spi_cfg;
                        err = sys_ioctl(hdl->stg->SPI_file,
                                        IOCTL_SPI__GET_CONFIGURATION, &spi_cfg);
                        if (!err) {

                                spi_cfg.flush_byte = 0xFF;
                                spi_cfg.mode       = SPI_MODE__0;
                                spi_cfg.msb_first  = true;

                                err = sys_ioctl(hdl->stg->SPI_file,
                                                IOCTL_SPI__SET_CONFIGURATION, &spi_cfg);
                        }

                        if (err) {
                                sys_fclose(hdl->stg->SPI_file);
                        }
                }
                break;
        }

        case IOCTL_SDSPI__INITIALIZE_CARD: {
                err = sys_mutex_lock(hdl->stg->protect_mtx, MAX_DELAY_MS);
                if (!err) {
                        err = card_initialize(hdl);
                        sys_mutex_unlock(hdl->stg->protect_mtx);
                }
                break;
        }

        case IOCTL_SDSPI__READ_MBR: {
                err = sys_mutex_lock(hdl->stg->protect_mtx, MAX_DELAY_MS);
                if (!err) {
                        if (hdl->stg->initialized) {
                                err = MBR_detect_partitions(hdl);
                        } else {
                                err = ENOMEDIUM;
                        }
                        sys_mutex_unlock(hdl->stg->protect_mtx);
                }
                break;
        }

        default:
                return EBADRQC;
        }

        return err;
}

//==============================================================================
/**
 * @brief Flush device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_FLUSH(SDSPI, void *device_handle)
{
        UNUSED_ARG1(device_handle);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Device information
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_STAT(SDSPI, void *device_handle, struct vfs_dev_stat *device_stat)
{
        SDSPI_t *hdl = device_handle;

        if (hdl->stg->initialized) {
                device_stat->st_size = cast(u64_t, hdl->stg->part[hdl->minor].size)
                                                 * SECTOR_SIZE;
        } else {
                device_stat->st_size = 0;
        }

        return ESUCC;
}

//==============================================================================
/**
 * @brief Function select card
 *
 * @param[in] hdl       partition handler
 *
 * @return None
 */
//==============================================================================
static void SPI_select_card(SDSPI_t *hdl)
{
        sys_ioctl(hdl->stg->SPI_file, IOCTL_SPI__SELECT);
}

//==============================================================================
/**
 * @brief Function deselect card
 *
 * @param[in] hdl       partition handler
 *
 * @return None
 */
//==============================================================================
static void SPI_deselect_card(SDSPI_t *hdl)
{
        sys_ioctl(hdl->stg->SPI_file, IOCTL_SPI__DESELECT);
}

//==============================================================================
/**
 * @brief Function send byte by SPI peripheral
 *
 * @param[in] hdl       partition handler
 * @param[in] out       data to send
 *
 * @return received byte
 */
//==============================================================================
static u8_t SPI_transive(SDSPI_t *hdl, u8_t out)
{
        SPI_transceive_t tr;
        tr.count     = 1;
        tr.rx_buffer = &out;
        tr.tx_buffer = &out;

        if (sys_ioctl(hdl->stg->SPI_file, IOCTL_SPI__TRANSCEIVE, &tr) == ESUCC) {
                return out;
        } else {
                return 0x00;
        }
}

//==============================================================================
/**
 * @brief Transmit block by using SPI
 *
 * @param[in]  hdl      partition handler
 * @param[in]  block    block address
 * @param[in]  count    block size
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int SPI_transmit_block(SDSPI_t *hdl, const u8_t *block, size_t count)
{
        size_t wrcnt;
        return sys_fwrite(block, count, &wrcnt, hdl->stg->SPI_file);
}

//==============================================================================
/**
 * @brief Receive block by usig SPI
 *
 * @param[in]  hdl      partition handler
 * @param[out] block    block address
 * @param[in]  count    block size
 *
 * @return  One of errno value (errno.h).
 */
//==============================================================================
static int SPI_receive_block(SDSPI_t *hdl, u8_t *block, size_t count)
{
        size_t rdcnt;
        return sys_fread(block, count, &rdcnt, hdl->stg->SPI_file);
}

//==============================================================================
/**
 * @brief Function wait for card ready
 *
 * @param[in] hdl       partition handler
 *
 * @return card response
 */
//==============================================================================
static u8_t card_wait_ready(SDSPI_t *hdl)
{
        u8_t response;
        u32_t timer = sys_time_get_reference();

        while ((response = SPI_transive(hdl, 0xFF)) != 0xFF
              && !sys_time_is_expired(timer, hdl->stg->timeout_ms));

        return response;
}

//==============================================================================
/**
 * @brief Function transmit command to card
 *
 * @param[in] hdl       partition handler
 * @param[in] cmd       card command
 * @param[in] arg       command's argument
 */
//==============================================================================
static u8_t card_send_cmd(SDSPI_t *hdl, card_cmd_t cmd, u32_t arg)
{
        u8_t response;

        /* ACMD<n> is the command sequence of CMD55-CMD<n> */
        if (cmd & 0x80) {
                cmd &= 0x7F;
                response = card_send_cmd(hdl, CMD55, 0);
                if (response > 1) {
                        return response;
                }
        }

        /* select the card and wait for ready */
        SPI_deselect_card(hdl);
        SPI_select_card(hdl);

        if (card_wait_ready(hdl) != 0xFF) {
                return 0xFF;
        }

        /* send command packet */
        u8_t buf[8], len = 0;
        buf[len++] = cmd;
        buf[len++] = arg >> 24;
        buf[len++] = arg >> 16;
        buf[len++] = arg >> 8;
        buf[len++] = arg;

        switch (cmd) {
        case CMD0: buf[len++] = 0x95; break;
        case CMD8: buf[len++] = 0x87; break;
        default  : buf[len++] = 0x01; break;
        }

        if (cmd == CMD12)
                buf[len++] = 0xFF;           /* Skip a stuff byte when stop reading */

        SPI_transmit_block(hdl, buf, len);

        /* wait for a valid response in timeout of 10 attempts */
        int n = 10;
        do {
                response = SPI_transive(hdl, 0xFF);

        } while ((response & 0x80) && --n);

        return response;
}

//==============================================================================
/**
 * @brief Function receive data block
 *
 * @param[in]   hdl             partition handler
 * @param[out]  buff            data buffer (sector size)
 *
 * @retval true if success
 * @retval false if error
 */
//==============================================================================
static bool card_receive_data_block(SDSPI_t *hdl, u8_t *buff)
{
        u8_t token;
        u32_t timer = sys_time_get_reference();
        while ((token = SPI_transive(hdl, 0xFF)) == 0xFF
              && !sys_time_is_expired(timer, hdl->stg->timeout_ms));

        if (token != 0xFE) {
                return false;
        }

        /* block send */
        SPI_receive_block(hdl, buff, SECTOR_SIZE);

        /* discard CRC */
        u8_t crc[2];
        SPI_receive_block(hdl, crc, 2);

        return true;
}

//==============================================================================
/**
 * @brief Function transmit data block
 *
 * @param[in]  hdl              partition handler
 * @param[in]  buff             source buffer (sector size)
 * @param[in]  toke             token to send
 *
 * @retval true if success
 * @retval false if error
 */
//==============================================================================
static bool card_transmit_data_block(SDSPI_t *hdl, const u8_t *buff, u8_t token)
{
        if (card_wait_ready(hdl) != 0xFF) {
                return false;
        }

        SPI_transive(hdl, token);

        if (token != 0xFD) {
                u8_t dummy_crc_and_response[3];

                SPI_transmit_block(hdl, buff, SECTOR_SIZE);
                SPI_receive_block(hdl, dummy_crc_and_response, 3);

                if ((dummy_crc_and_response[2] & 0x1F) != 0x05) {
                        return false;
                }
        }

        return true;
}

//==============================================================================
/**
 * @brief Function read whole sectors
 *
 * @param[in]   hdl             driver's memory handle
 * @param[out]  dst             destination
 * @param[in]   nsectors        sectors to read
 * @param[in]   lseek           file index
 *
 * @retval number of read sectors
 */
//==============================================================================
static ssize_t card_read_entire_sectors(SDSPI_t *hdl, u8_t *dst, size_t nsectors, u64_t lseek)
{
        if (hdl->stg->type.block) {
                lseek >>= 9;    /* divide by 512 */
        }

        /* 1 sector to read */
        ssize_t n = -1;
        if (nsectors == 1) {
                if (card_send_cmd(hdl, CMD17, cast(u32_t, lseek)) == 0) {
                        if (card_receive_data_block(hdl, dst)) {
                                n = 1;
                        }
                }
        } else {
                if (card_send_cmd(hdl, CMD18, cast(u32_t, lseek)) == 0) {
                        n = 0;
                        do {
                                if (!card_receive_data_block(hdl, dst)) {
                                        break;
                                }

                                dst += SECTOR_SIZE;

                        } while (++n < (ssize_t)nsectors);

                        /* stop transmission */
                        card_send_cmd(hdl, CMD12, 0);
                }
        }

        return n;
}

//==============================================================================
/**
 * @brief Function read only selected data from sectors
 *
 * @param[in]   hdl             driver's memory handle
 * @param[out]  dst             destination
 * @param[in]   size            number of bytes to read
 * @param[in]   lseek           file index
 *
 * @retval number of read bytes
 */
//==============================================================================
static ssize_t card_read_partial_sectors(SDSPI_t *hdl, u8_t *dst, size_t size, u64_t lseek)
{
        u8_t *buffer;
        int err = sys_malloc(SECTOR_SIZE, cast(void**, &buffer));
        if (err != ESUCC)
                return -1;

        u32_t recv_data = 0;
        while (recv_data < size) {
                if (lseek % SECTOR_SIZE == 0 && (size - recv_data) / SECTOR_SIZE > 0) {
                        ssize_t n = card_read_entire_sectors(hdl, dst, size / SECTOR_SIZE, lseek);
                        if (n == -1) {
                                recv_data = -1;
                                goto exit;

                        } else if (n != cast(ssize_t, size) / SECTOR_SIZE) {
                                break;
                        }

                        dst       += n * SECTOR_SIZE;
                        lseek     += n * SECTOR_SIZE;
                        recv_data += n * SECTOR_SIZE;
                } else {
                        ssize_t n = card_read_entire_sectors(hdl, buffer, 1, lseek & ~(SECTOR_SIZE - 1));
                        if (n == -1) {
                                recv_data = -1;
                                goto exit;

                        } else if (n != 1) {
                                break;
                        }

                        u32_t rest;
                        if ((SECTOR_SIZE - (lseek % SECTOR_SIZE)) > (size - recv_data))
                                rest = size - recv_data;
                        else
                                rest = SECTOR_SIZE - (lseek % SECTOR_SIZE);

                        memcpy(dst, buffer + (lseek % SECTOR_SIZE), rest);
                        dst       += rest;
                        recv_data += rest;
                        lseek     += rest;
                }
        }

        exit:
        sys_free(cast(void**, &buffer));

        return recv_data;
}

//==============================================================================
/**
 * @brief Function write whole sectors
 *
 * @param[in]   hdl             driver's memory handle
 * @param[out]  src             source
 * @param[in]   nsectors        sectors to read
 * @param[in]   lseek           file index
 *
 * @retval number of written sectors
 */
//==============================================================================
static ssize_t card_write_entire_sectors(SDSPI_t *hdl, const u8_t *src, size_t nsectors, u64_t lseek)
{
        if (hdl->stg->type.block) {
                lseek >>= 9;    /* divide by 512 */
        }

        /* 1 sector to read */
        ssize_t n = -1;
        if (nsectors == 1) {
                if (card_send_cmd(hdl, CMD24, cast(u32_t, lseek)) == 0) {
                        if (card_transmit_data_block(hdl, src, 0xFE)) {
                                n = 1;
                        }
                }
        } else {
                if (  hdl->stg->type.type == CT_SD1
                   || hdl->stg->type.type == CT_SD2) {

                        card_send_cmd(hdl, ACMD23, nsectors);
                }

                if (card_send_cmd(hdl, CMD25, cast(u32_t, lseek)) == 0) {
                        n = 0;
                        do {
                                if (!card_transmit_data_block(hdl, src, 0xFC)) {
                                        break;
                                }

                                src += SECTOR_SIZE;
                        } while (++n < cast(ssize_t, nsectors));

                        /* stop transmission */
                        if (!card_transmit_data_block(hdl, NULL, 0xFD)) {
                                n = 0;
                        }
                }
        }

        return n;
}

//==============================================================================
/**
 * @brief Function write only selected data to sectors
 *
 * @param[in]   hdl             driver's memory handle
 * @param[out]  src             source
 * @param[in]   size            number of bytes to read
 * @param[in]   lseek           file index
 *
 * @retval number of written bytes
 */
//==============================================================================
static ssize_t card_write_partial_sectors(SDSPI_t *hdl, const u8_t *src, size_t size, u64_t lseek)
{
        u8_t *buffer = NULL;
        int err = sys_malloc(SECTOR_SIZE, cast(void**, &buffer));
        if (err)
                return -1;

        u32_t transmit_data = 0;
        while (transmit_data < size) {
                if (lseek % SECTOR_SIZE == 0 && (size - transmit_data) / SECTOR_SIZE > 0) {
                        ssize_t n = card_write_entire_sectors(hdl, src, size / SECTOR_SIZE, lseek);
                        if (n == -1) {
                                transmit_data = -1;
                                goto exit;

                        } else if (n != cast(ssize_t, size) / SECTOR_SIZE) {
                                break;
                        }

                        src           += n * SECTOR_SIZE;
                        lseek         += n * SECTOR_SIZE;
                        transmit_data += n * SECTOR_SIZE;
                } else {
                        ssize_t n = card_read_entire_sectors(hdl, buffer, 1, lseek & ~(SECTOR_SIZE - 1));
                        if (n == -1) {
                                transmit_data = -1;
                                goto exit;

                        } else if (n != 1) {
                                break;
                        }

                        u32_t rest;
                        if ((SECTOR_SIZE - (lseek % SECTOR_SIZE)) > (size - transmit_data))
                                rest = size - transmit_data;
                        else
                                rest = SECTOR_SIZE - (lseek % SECTOR_SIZE);

                        memcpy(buffer + (lseek % SECTOR_SIZE), src, rest);

                        n = card_write_entire_sectors(hdl, buffer, 1, lseek & ~(SECTOR_SIZE - 1));
                        if (n == -1) {
                                transmit_data = -1;
                                goto exit;

                        } else if (n != 1) {
                                break;
                        }

                        src           += rest;
                        transmit_data += rest;
                        lseek         += rest;
                }
        }

        exit:
        sys_free(cast(void**, &buffer));

        return transmit_data;
}

//==============================================================================
/**
 * @brief Function initialize card
 *
 * @param[in]  hdl      SD module data
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int card_initialize(SDSPI_t *hdl)
{
        int err = EIO;

        SPI_deselect_card(hdl);
        for (int n = 0; n < 50; n++) {
                static const u8_t BYTE = 0xFF;
                sys_ioctl(hdl->stg->SPI_file, IOCTL_SPI__TRANSMIT_NO_SELECT, &BYTE);
        }

        hdl->stg->type.type   = CT_UNKNOWN;
        hdl->stg->type.block  = false;
        hdl->stg->initialized = false;

        u32_t timer = sys_time_get_reference();

        if (card_send_cmd(hdl, CMD0, 0) == 0x01) {
                if (card_send_cmd(hdl, CMD8, 0x1AA) == 0x01) { /* check SDHC card */

                        u8_t OCR[4];
                        SPI_receive_block(hdl, OCR, sizeof(OCR));

                        if (OCR[2] == 0x01 && OCR[3] == 0xAA) {
                                while ( !sys_time_is_expired(timer, hdl->stg->timeout_ms)
                                      && card_send_cmd(hdl, ACMD41, 1UL << 30) ) {

                                        sys_sleep_ms(1);
                                }

                                if ( !sys_time_is_expired(timer, hdl->stg->timeout_ms)
                                   && card_send_cmd(hdl, CMD58, 0) == 0 ) {

                                        SPI_receive_block(hdl, OCR, sizeof(OCR));

                                        hdl->stg->type.type   = CT_SD2;
                                        hdl->stg->type.block  = (OCR[0] & 0x40) ? true : false;
                                        hdl->stg->initialized = true;
                                        err = ESUCC;
                                } else {
                                        err = ETIME;
                                }
                        }
                } else { /* SDSC or MMC */
                        u8_t cmd;
                        if (card_send_cmd(hdl, ACMD41, 0) <= 0x01)   {
                                hdl->stg->type.type = CT_SD1;
                                cmd = ACMD41;   /* SDSC */
                        } else {
                                hdl->stg->type.type = CT_MMC;
                                cmd = CMD1;     /* MMC */
                        }

                        /* Wait for leaving idle state */
                        while ( !sys_time_is_expired(timer, hdl->stg->timeout_ms)
                              && card_send_cmd(hdl, cmd, 0)) {

                                sys_sleep_ms(1);
                        }

                        /* set R/W block length to 512 */
                        if ( !sys_time_is_expired(timer, hdl->stg->timeout_ms)
                           || card_send_cmd(hdl, CMD16, SECTOR_SIZE) != 0) {

                                hdl->stg->type.type   = CT_UNKNOWN;
                                hdl->stg->type.block  = false;
                                err = ETIME;
                        } else {
                                hdl->stg->initialized = true;
                                err = ESUCC;
                        }
                }
        } else {
                err = ENOMEDIUM;
        }

        if (!err) {
                /* clear partition info */
                memset(hdl->stg->part, 0, sizeof(hdl->stg->part));

                // read size
                if (card_send_cmd(hdl, CMD9, 0) == 0) {
                        u8_t csd[16];
                        u8_t token;

                        memset(csd, 0, sizeof(csd));

                        uint timer = sys_time_get_reference();
                        while ( (token = SPI_transive(hdl, 0xFF)) == 0xFF
                              && !sys_time_is_expired(timer, hdl->stg->timeout_ms));

                        if (token == 0xFE) {
                                SPI_receive_block(hdl, csd, sizeof(csd));
                                SPI_transive(hdl, 0xFF);
                                SPI_transive(hdl, 0xFF);

                                /* SDC version 2.00 */
                                u32_t size;
                                if ((csd[0] >> 6) == 1) {
                                        u32_t csize = csd[9] + ((u16_t)csd[8] << 8) + 1;
                                        size        = csize << 10;
                                } else { /* SDC version 1.XX or MMC*/
                                        u32_t n     = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                                        u32_t csize = (csd[8] >> 6) + ((u16_t)csd[7] << 2) + ((u16_t)(csd[6] & 3) << 10) + 1;
                                        size        = csize << (n - 9);
                                }

                                hdl->stg->part[_SDSPI_VOLUME].size = size;
                        }
                }
        }

        SPI_deselect_card(hdl);

        return err;
}

//==============================================================================
/**
 * @brief Read data from card
 *
 * @param[in]   hdl             driver's memory handle
 * @param[out]  dst             destination
 * @param[in]   count           bytes to read
 * @param[in]   lseek           file index
 * @param[out]  rdcnt           number of read bytes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static ssize_t card_read(SDSPI_t *hdl, u8_t *dst, size_t count, u64_t lseek, size_t *rdcnt)
{
        ssize_t n = 0;

        if (hdl->stg->initialized == false) {
                return EIO;

        } else {
                if ((count % SECTOR_SIZE == 0) && (lseek % SECTOR_SIZE == 0)) {

                        n  = card_read_entire_sectors(hdl, dst, count / SECTOR_SIZE, lseek);
                        n *= SECTOR_SIZE;

                } else {
                        n  = card_read_partial_sectors(hdl, dst, count, lseek);
                }

                SPI_deselect_card(hdl);
        }

        if (n >= 0) {
                *rdcnt = n;
                return ESUCC;
        } else {
                return EIO;
        }
}

//==============================================================================
/**
 * @brief Write data to card
 *
 * @param[in]  hdl              driver's memory handle
 * @param[in]  src              source
 * @param[in]  count            bytes to write
 * @param[in]  lseek            file index
 * @param[out] wrcnt            number of written bytes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static ssize_t card_write(SDSPI_t *hdl, const u8_t *src, size_t count, u64_t lseek, size_t *wrcnt)
{
        ssize_t n = 0;

        if (hdl->stg->initialized == false) {
                return EIO;

        } else {
                if ((count % SECTOR_SIZE == 0) && (lseek % SECTOR_SIZE == 0)) {
                        n  = card_write_entire_sectors(hdl, src, count / SECTOR_SIZE, lseek);
                        n *= SECTOR_SIZE;
                } else {
                        n  = card_write_partial_sectors(hdl, src, count, lseek);
                }

                SPI_deselect_card(hdl);
        }

        if (n >= 0) {
                *wrcnt = n;
                return ESUCC;
        } else {
                return EIO;
        }
}

//==============================================================================
/**
 * @brief Function load 32b data from selected buffer and offset
 *
 * @param buff                  buffer
 * @param offset                variable offset
 *
 * @return 32-bit value
 */
//==============================================================================
static u32_t load_u32(u8_t *buff, u16_t offset)
{
        return cast(u32_t, ( (cast(u32_t, buff[offset + 0] <<  0))
                                  | (cast(u32_t, buff[offset + 1] <<  8))
                                  | (cast(u32_t, buff[offset + 2] << 16))
                                  | (cast(u32_t, buff[offset + 3] << 24)) ) );
}

//==============================================================================
/**
 * @brief Function load 16b data from selected buffer and offset
 *
 * @param buff                  buffer
 * @param offset                variable offset
 *
 * @return 16-bit value
 */
//==============================================================================
static u16_t load_u16(u8_t *buff, u16_t offset)
{
        return cast(u16_t, ( (cast(u16_t, buff[offset + 0] << 0))
                                  | (cast(u16_t, buff[offset + 1] << 8)) ) );
}

//==============================================================================
/**
 * @brief Function gets boot signature
 *
 * @param sector                buffer with sector
 *
 * @return boot signature
 */
//==============================================================================
static u16_t MBR_get_boot_signature(u8_t *sector)
{
     return load_u16(sector, MBR_BOOT_SIGNATURE_OFFSET);
}

//==============================================================================
/**
 * @brief Function return partition first LBA sector number
 *
 * @param partition             partition number (1 - 4)
 * @param sector                buffer with sector
 *
 * @return first LBA sector number
 */
//==============================================================================
static u32_t MBR_get_partition_first_LBA_sector(int partition, u8_t *sector)
{
        switch (partition) {
        case 1 : return load_u32(sector, MBR_PARTITION_1_ENTRY_OFFSET + MBR_PARITION_ENTRY_LBA_FIRST_ADDR_OFFSET);
        case 2 : return load_u32(sector, MBR_PARTITION_2_ENTRY_OFFSET + MBR_PARITION_ENTRY_LBA_FIRST_ADDR_OFFSET);
        case 3 : return load_u32(sector, MBR_PARTITION_3_ENTRY_OFFSET + MBR_PARITION_ENTRY_LBA_FIRST_ADDR_OFFSET);
        case 4 : return load_u32(sector, MBR_PARTITION_4_ENTRY_OFFSET + MBR_PARITION_ENTRY_LBA_FIRST_ADDR_OFFSET);
        default: return 0;
        }
}

//==============================================================================
/**
 * @brief Function return partition number of sectors
 *
 * @param partition             partition number (1 - 4)
 * @param sector                buffer with sector
 *
 * @return number of partition sectors
 */
//==============================================================================
static u32_t MBR_get_partition_number_of_sectors(int partition, u8_t *sector)
{
        switch (partition) {
        case 1 : return load_u32(sector, MBR_PARTITION_1_ENTRY_OFFSET + MBR_PARITION_ENTRY_NUM_OF_SECTORS_OFFSET);
        case 2 : return load_u32(sector, MBR_PARTITION_2_ENTRY_OFFSET + MBR_PARITION_ENTRY_NUM_OF_SECTORS_OFFSET);
        case 3 : return load_u32(sector, MBR_PARTITION_3_ENTRY_OFFSET + MBR_PARITION_ENTRY_NUM_OF_SECTORS_OFFSET);
        case 4 : return load_u32(sector, MBR_PARTITION_4_ENTRY_OFFSET + MBR_PARITION_ENTRY_NUM_OF_SECTORS_OFFSET);
        default: return 0;
        }


}

//==============================================================================
/**
 * @brief Function detect partitions
 *
 * @param[in]  hdl      SD module data
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
static int MBR_detect_partitions(SDSPI_t *hdl)
{
        int err = EIO;

        u8_t *MBR;
        err = sys_malloc(SECTOR_SIZE, cast(void**, &MBR));
        if (err == ESUCC) {
                size_t rdcnt;
                err = card_read(hdl, MBR, SECTOR_SIZE, 0, &rdcnt);
                if (!err && (rdcnt != SECTOR_SIZE)) {
                        goto error;
                }

                if (MBR_get_boot_signature(MBR) != 0xAA55) {
                        err = EMEDIUMTYPE;
                        goto error;
                }

                for (int i = _SDSPI_PARTITION_1; i <= _SDSPI_PARTITION_4; i++) {
                        u32_t size = MBR_get_partition_number_of_sectors(i, MBR);
                        hdl->stg->part[i].size   = size;
                        hdl->stg->part[i].offset = MBR_get_partition_first_LBA_sector(i, MBR);
                }

                error:
                sys_free(cast(void**, &MBR));
        } else {
                err = ENOMEM;
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
