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
#include "drivers/class/storage/sd.h"
#include "noarch/sdspi_cfg.h"
#include "sys/ioctl.h"
#include "../sdspi_ioctl.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/** driver instance */
typedef struct {
        u8_t       major;
        u8_t       minor;
        FILE      *SPI_file;
        mutex_t   *protect_mtx;
        u32_t      timeout_ms;
        SD_type_t  type;
        bool       initialized;
        u32_t      size_blocks;
} SDSPI_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int      configure                  (SDSPI_t *hdl, const SDSPI_config_t *sdspi_cfg);
static void     SPI_select_card            (SDSPI_t *hdl);
static void     SPI_deselect_card          (SDSPI_t *hdl);
static u8_t     SPI_transive               (SDSPI_t *hdl, u8_t out);
static int      SPI_transmit_block         (SDSPI_t *hdl, const u8_t *block, size_t count);
static int      SPI_receive_block          (SDSPI_t *hdl, u8_t *block, size_t count);
static u8_t     card_send_cmd              (SDSPI_t *hdl, SD_cmd_t cmd, u32_t arg);
static u8_t     card_wait_ready            (SDSPI_t *hdl);
static bool     card_receive_data_block    (SDSPI_t *hdl, u8_t *buff);
static bool     card_transmit_data_block   (SDSPI_t *hdl, const u8_t *buff, u8_t token);
static ssize_t  card_read_sectors          (SDSPI_t *hdl, u8_t *dst, size_t nsectors, u64_t lseek);
static ssize_t  card_write_sectors         (SDSPI_t *hdl, const u8_t *src, size_t nsectors, u64_t lseek);
static int      card_initialize            (SDSPI_t *hdl);
static int      card_read                  (SDSPI_t *hdl, u8_t *dst, size_t count, u64_t lseek, size_t *rdcnt);
static int      card_write                 (SDSPI_t *hdl, const u8_t *src, size_t count, u64_t lseek, size_t *wrcnt);

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
 * @param[in ]            config               optional module configuration
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(SDSPI, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        SDSPI_t *hdl = NULL;
        int err = sys_zalloc(sizeof(SDSPI_t), cast(void**, &hdl));
        if (!err) {
                hdl->major = major;
                hdl->minor = minor;

                err = sys_mutex_create(MUTEX_TYPE_NORMAL, &hdl->protect_mtx);
                if (!err) {
                        if (config) {
                                err = configure(hdl, config);
                        }
                }

                if (err) {
                        if (hdl->protect_mtx) {
                                sys_mutex_destroy(hdl->protect_mtx);
                        }

                        sys_free(cast(void**, &hdl));
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

        int err = sys_mutex_trylock(hdl->protect_mtx);
        if (!err) {
                sys_fclose(hdl->SPI_file);
                sys_mutex_destroy(hdl->protect_mtx);
                sys_free(cast(void**, &hdl));
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
        UNUSED_ARG2(device_handle, flags);
        return ESUCC;
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
        UNUSED_ARG2(device_handle, force);
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
        int      err = ENOMEDIUM;

        if (hdl->size_blocks > 0) {
                err = sys_mutex_lock(hdl->protect_mtx, MAX_DELAY_MS);
                if (!err) {
                        u64_t lseek = *fpos;
                        err = card_write(hdl, src, count, lseek, wrcnt);
                        sys_mutex_unlock(hdl->protect_mtx);
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
        int      err = ENOMEDIUM;

        if (hdl->size_blocks > 0) {
                err = sys_mutex_lock(hdl->protect_mtx, MAX_DELAY_MS);
                if (!err) {
                        u64_t lseek = *fpos;
                        err = card_read(hdl, dst, count, lseek, rdcnt);
                        sys_mutex_unlock(hdl->protect_mtx);
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
                err = configure(hdl, arg);
                break;
        }

        case IOCTL_SDSPI__INITIALIZE_CARD:
                err = sys_mutex_lock(hdl->protect_mtx, MAX_DELAY_MS);
                if (!err) {
                        err = card_initialize(hdl);
                        sys_mutex_unlock(hdl->protect_mtx);
                }
                break;

        default:
                err = EBADRQC;
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
 * @brief Function make basic configuration.
 *
 * @param[in]   hdl             driver's memory handle
 * @param[in]   sdspi_cfg       configuration
 *
 * @retval One of errno value (errno.h).
 */
//==============================================================================
static int configure(SDSPI_t *hdl, const SDSPI_config_t *sdspi_cfg)
{
        int err = EINVAL;

        if (hdl->SPI_file) {
                err = sys_fclose(hdl->SPI_file);
                if (err) {
                        return err;
                }
        }

        err = sys_fopen(sdspi_cfg->filepath, "r+", &hdl->SPI_file);
        if (!err) {

                hdl->timeout_ms = sdspi_cfg->timeout;

                SPI_config_t spi_cfg;
                err = sys_ioctl(hdl->SPI_file, IOCTL_SPI__GET_CONFIGURATION, &spi_cfg);
                if (!err) {

                        spi_cfg.flush_byte = 0xFF;
                        spi_cfg.mode       = SPI_MODE__0;
                        spi_cfg.msb_first  = true;

                        err = sys_ioctl(hdl->SPI_file, IOCTL_SPI__SET_CONFIGURATION, &spi_cfg);
                }

                if (err) {
                        sys_fclose(hdl->SPI_file);
                }
        }

        return err;
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

        if (hdl->initialized) {
                device_stat->st_size = cast(u64_t, hdl->size_blocks) * SECTOR_SIZE;
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
        sys_ioctl(hdl->SPI_file, IOCTL_SPI__SELECT);
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
        sys_ioctl(hdl->SPI_file, IOCTL_SPI__DESELECT);
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
        tr.separated = false;
        tr.next      = NULL;

        if (sys_ioctl(hdl->SPI_file, IOCTL_SPI__TRANSCEIVE, &tr) == ESUCC) {
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
        return sys_fwrite(block, count, &wrcnt, hdl->SPI_file);
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
        return sys_fread(block, count, &rdcnt, hdl->SPI_file);
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
              && !sys_time_is_expired(timer, hdl->timeout_ms));

        return response;
}

//==============================================================================
/**
 * @brief Function transmit command to card
 *
 * @param[in] hdl       partition handler
 * @param[in] cmd       card command
 * @param[in] arg       command's argument
 *
 * @return Response byte.
 */
//==============================================================================
static u8_t card_send_cmd(SDSPI_t *hdl, SD_cmd_t cmd, u32_t arg)
{
        u8_t response;

        /* ACMD<n> is the command sequence of CMD55-CMD<n> */
        if (cmd & 0x80) {
                cmd &= 0x7F;
                response = card_send_cmd(hdl, SD_CMD__CMD55, 0);
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
        case SD_CMD__CMD0: buf[len++] = 0x95; break;
        case SD_CMD__CMD8: buf[len++] = 0x87; break;
        default          : buf[len++] = 0x01; break;
        }

        if (cmd == SD_CMD__CMD12)
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
              && !sys_time_is_expired(timer, hdl->timeout_ms));

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
static ssize_t card_read_sectors(SDSPI_t *hdl, u8_t *dst, size_t nsectors, u64_t lseek)
{
        if (hdl->type.block) {
                lseek /= 512;
        }

        /* 1 sector to read */
        ssize_t n = -1;
        if (nsectors == 1) {
                if (card_send_cmd(hdl, SD_CMD__CMD17, cast(u32_t, lseek)) == 0) {
                        if (card_receive_data_block(hdl, dst)) {
                                n = 1;
                        }
                }
        } else {
                if (card_send_cmd(hdl, SD_CMD__CMD18, cast(u32_t, lseek)) == 0) {
                        n = 0;
                        do {
                                if (!card_receive_data_block(hdl, dst)) {
                                        break;
                                }

                                dst += SECTOR_SIZE;

                        } while (++n < (ssize_t)nsectors);

                        /* stop transmission */
                        card_send_cmd(hdl, SD_CMD__CMD12, 0);
                }
        }

        return n;
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
static ssize_t card_write_sectors(SDSPI_t *hdl, const u8_t *src, size_t nsectors, u64_t lseek)
{
        if (hdl->type.block) {
                lseek /= 512;
        }

        /* 1 sector to read */
        ssize_t n = -1;
        if (nsectors == 1) {
                if (card_send_cmd(hdl, SD_CMD__CMD24, cast(u32_t, lseek)) == 0) {
                        if (card_transmit_data_block(hdl, src, 0xFE)) {
                                n = 1;
                        }
                }
        } else {
                if (  hdl->type.type == SD_TYPE__SD1
                   || hdl->type.type == SD_TYPE__SD2) {

                        card_send_cmd(hdl, SD_CMD__ACMD23, nsectors);
                }

                if (card_send_cmd(hdl, SD_CMD__CMD25, cast(u32_t, lseek)) == 0) {
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
                sys_ioctl(hdl->SPI_file, IOCTL_SPI__TRANSMIT_NO_SELECT, &BYTE);
        }

        hdl->type.type   = SD_TYPE__UNKNOWN;
        hdl->type.block  = false;
        hdl->initialized = false;

        u32_t timer = sys_time_get_reference();

        if (card_send_cmd(hdl, SD_CMD__CMD0, 0) == 0x01) {
                if (card_send_cmd(hdl, SD_CMD__CMD8, 0x1AA) == 0x01) { /* check SDHC card */

                        u8_t OCR[4];
                        SPI_receive_block(hdl, OCR, sizeof(OCR));

                        if (OCR[2] == 0x01 && OCR[3] == 0xAA) {
                                while ( !sys_time_is_expired(timer, hdl->timeout_ms)
                                      && card_send_cmd(hdl, SD_CMD__ACMD41, 1UL << 30) ) {

                                        sys_sleep_ms(1);
                                }

                                if ( !sys_time_is_expired(timer, hdl->timeout_ms)
                                   && card_send_cmd(hdl, SD_CMD__CMD58, 0) == 0 ) {

                                        SPI_receive_block(hdl, OCR, sizeof(OCR));

                                        hdl->type.type   = SD_TYPE__SD2;
                                        hdl->type.block  = (OCR[0] & 0x40) ? true : false;
                                        hdl->initialized = true;
                                        err = ESUCC;
                                } else {
                                        err = ETIME;
                                }
                        }
                } else { /* SDSC or MMC */
                        u8_t cmd;
                        if (card_send_cmd(hdl, SD_CMD__ACMD41, 0) <= 0x01)   {
                                hdl->type.type = SD_TYPE__SD1;
                                cmd = SD_CMD__ACMD41;   /* SDSC */
                        } else {
                                hdl->type.type = SD_TYPE__MMC;
                                cmd = SD_CMD__CMD1;     /* MMC */
                        }

                        /* Wait for leaving idle state */
                        while ( !sys_time_is_expired(timer, hdl->timeout_ms)
                              && card_send_cmd(hdl, cmd, 0)) {

                                sys_sleep_ms(1);
                        }

                        /* set R/W block length to 512 */
                        if ( !sys_time_is_expired(timer, hdl->timeout_ms)
                           || card_send_cmd(hdl, SD_CMD__CMD16, SECTOR_SIZE) != 0) {

                                hdl->type.type   = SD_TYPE__UNKNOWN;
                                hdl->type.block  = false;
                                err = ETIME;
                        } else {
                                hdl->initialized = true;
                                err = ESUCC;
                        }
                }
        } else {
                err = ENOMEDIUM;
        }

        if (!err) {
                /* clear card info */
                hdl->size_blocks = 0;
                hdl->type.block = false;
                hdl->type.type = SD_TYPE__UNKNOWN;

                // read size
                if (card_send_cmd(hdl, SD_CMD__CMD9, 0) == 0) {
                        u8_t token;

                        u32_t timer = sys_time_get_reference();
                        while ( (token = SPI_transive(hdl, 0xFF)) == 0xFF
                              && !sys_time_is_expired(timer, hdl->timeout_ms));

                        if (token == 0xFE) {
                                u8_t CSD[16];
                                memset(CSD, 0, sizeof(CSD));

                                SPI_receive_block(hdl, CSD, sizeof(CSD));
                                SPI_transive(hdl, 0xFF);
                                SPI_transive(hdl, 0xFF);

                                /* SDC version 2.00 */
                                u32_t size;
                                if ((CSD[0] >> 6) == 1) {
                                        u32_t csize = CSD[9] + ((u16_t)CSD[8] << 8) + 1;
                                        size        = csize << 10;
                                } else { /* SDC version 1.XX or MMC*/
                                        u32_t n     = (CSD[5] & 15) + ((CSD[10] & 128) >> 7) + ((CSD[9] & 3) << 1) + 2;
                                        u32_t csize = (CSD[8] >> 6) + ((u16_t)CSD[7] << 2) + ((u16_t)(CSD[6] & 3) << 10) + 1;
                                        size        = csize << (n - 9);
                                }

                                printk("SDSPI: %d 512-byte logical blocks", size);
                                hdl->size_blocks = size;
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
static int card_read(SDSPI_t *hdl, u8_t *dst, size_t count, u64_t lseek, size_t *rdcnt)
{
        int err = EIO;

        if (hdl->initialized) {
                if ((count % SECTOR_SIZE == 0) && (lseek % SECTOR_SIZE == 0)) {

                        ssize_t n = card_read_sectors(hdl, dst, count / SECTOR_SIZE, lseek);
                        if (n >= 0) {
                                *rdcnt = n * SECTOR_SIZE;
                                err = ESUCC;
                        }

                } else {
                        err = ESPIPE;
                }

                SPI_deselect_card(hdl);
        }

        return err;
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
static int card_write(SDSPI_t *hdl, const u8_t *src, size_t count, u64_t lseek, size_t *wrcnt)
{
        int err = EIO;

        if (hdl->initialized) {
                if ((count % SECTOR_SIZE == 0) && (lseek % SECTOR_SIZE == 0)) {
                        ssize_t n  = card_write_sectors(hdl, src, count / SECTOR_SIZE, lseek);
                        if (n >= 0) {
                                *wrcnt = n * SECTOR_SIZE;
                                err = ESUCC;
                        }
                } else {
                        err = ESPIPE;
                }

                SPI_deselect_card(hdl);
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
