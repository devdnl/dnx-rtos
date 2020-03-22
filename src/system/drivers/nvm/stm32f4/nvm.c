/*==============================================================================
File    nvm.c

Author  Daniel Zorychta

Brief   NVM driver

        Copyright (C) 2018 Daniel Zorychta <daniel.zorychta@gmail.com>

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

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"
#include "stm32f4/nvm_cfg.h"
#include "stm32f4/stm32f4xx.h"
#include "../nvm_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define TIMEOUT_MS              100
#define RDP_KEY                 0x00A5
#define FLASH_KEY1              0x45670123
#define FLASH_KEY2              0xCDEF89AB
#define FLASH_OPT_KEY1          0x08192A3B
#define FLASH_OPT_KEY2          0x4C5D6E7F

#define FLASH_CR_PSIZE_BYTE     ((0 * FLASH_CR_PSIZE_1) | (0 * FLASH_CR_PSIZE_0))
#define FLASH_CR_PSIZE_HALFWORD ((0 * FLASH_CR_PSIZE_1) | (1 * FLASH_CR_PSIZE_0))
#define FLASH_CR_PSIZE_WORD     ((1 * FLASH_CR_PSIZE_1) | (0 * FLASH_CR_PSIZE_0))
#define FLASH_CR_PSIZE_DOUBLE   ((1 * FLASH_CR_PSIZE_1) | (1 * FLASH_CR_PSIZE_0))

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        mutex_t *mtx;
        uint32_t start_sector;
        uint32_t sector_count;
        uint32_t total_size;
} NVM_t;

typedef struct {
        uint32_t addr;
        uint32_t size;
} sector_info_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void FLASH_unlock(void);
static void FLASH_lock(void);
static int FLASH_erase_sector(uint32_t sector);
static int FLASH_program_byte(uint32_t address, uint8_t data);
static int FLASH_wait_for_operation_finish(void);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(NVM);

static const sector_info_t SECTOR_INFO[] = {
        /* BANK 0 */
        /*00:*/ {.addr = 0x08000000, .size =  16384},
        /*01:*/ {.addr = 0x08004000, .size =  16384},
        /*02:*/ {.addr = 0x08008000, .size =  16384},
        /*03:*/ {.addr = 0x0800C000, .size =  16384},
        /*04:*/ {.addr = 0x08010000, .size =  65536},
        /*05:*/ {.addr = 0x08020000, .size = 131072},
        /*06:*/ {.addr = 0x08040000, .size = 131072},
        /*07:*/ {.addr = 0x08060000, .size = 131072},
        /*08:*/ {.addr = 0x08080000, .size = 131072},
        /*09:*/ {.addr = 0x080A0000, .size = 131072},
        /*10:*/ {.addr = 0x080C0000, .size = 131072},
        /*11:*/ {.addr = 0x080E0000, .size = 131072},
        /* BANK 1 */
        /*12:*/ {.addr = 0x08100000, .size =  16384},
        /*13:*/ {.addr = 0x08102000, .size =  16384},
        /*14:*/ {.addr = 0x08104000, .size =  16384},
        /*15:*/ {.addr = 0x08106000, .size =  16384},
        /*16:*/ {.addr = 0x08108000, .size =  65536},
        /*17:*/ {.addr = 0x0810A000, .size = 131072},
        /*18:*/ {.addr = 0x0810C000, .size = 131072},
        /*19:*/ {.addr = 0x08110000, .size = 131072},
        /*20:*/ {.addr = 0x08120000, .size = 131072},
        /*21:*/ {.addr = 0x08140000, .size = 131072},
        /*22:*/ {.addr = 0x08160000, .size = 131072},
        /*23:*/ {.addr = 0x08180000, .size = 131072},
};

/*==============================================================================
  Exported object
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize device.
 *
 * @param[out]          **device_handle        device allocated memory
 * @param[in ]            major                major device number
 * @param[in ]            minor                minor device number
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_INIT(NVM, void **device_handle, u8_t major, u8_t minor)
{
        UNUSED_ARG1(major);

        int err = EFAULT;

        if (minor == 0) {
                err = sys_zalloc(sizeof(NVM_t), device_handle);
                if (!err) {
                        NVM_t *hdl = *device_handle;

                        err = sys_mutex_create(MUTEX_TYPE_NORMAL, &hdl->mtx);

                        if (err) {
                                sys_free(device_handle);
                        }
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Release device.
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_RELEASE(NVM, void *device_handle)
{
        NVM_t *hdl = device_handle;

        int err = sys_mutex_lock(hdl->mtx, TIMEOUT_MS);
        if (!err) {
                mutex_t *mtx = hdl->mtx;

                hdl->mtx = NULL;

                sys_free(&device_handle);

                sys_sleep_ms(TIMEOUT_MS + 100);

                sys_mutex_unlock(mtx);
                sys_mutex_destroy(mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief Open device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_OPEN(NVM, void *device_handle, u32_t flags)
{
        UNUSED_ARG2(device_handle, flags);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Close device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_CLOSE(NVM, void *device_handle, bool force)
{
        UNUSED_ARG2(device_handle, force);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Write data to device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_WRITE(NVM,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fattr);

        NVM_t *hdl = device_handle;

        if (hdl->sector_count == 0) {
                printk("%s: write at not configured device", GET_MODULE_NAME());
                return EINVAL;
        }

        if (*fpos >= hdl->total_size) {
                printk("%s: write pointer out of range", GET_MODULE_NAME());
                return ENOSPC;
        }

        int err = sys_mutex_lock(hdl->mtx, TIMEOUT_MS);
        if (!err) {

                uint32_t dst = SECTOR_INFO[hdl->start_sector].addr
                             + cast(uint32_t, *fpos);

                count = min(count, (hdl->total_size - *fpos));

                size_t n = 0;

                FLASH_unlock();

                while (!err && count--) {
                        err = FLASH_program_byte(dst++, *src++);
                        n++;
                }

                FLASH_lock();

                *wrcnt = n;

                sys_mutex_unlock(hdl->mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief Read data from device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_READ(NVM,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        NVM_t *hdl = device_handle;

        if (hdl->sector_count == 0) {
                printk("%s: read at not configured device", GET_MODULE_NAME());
                return EINVAL;
        }

        if (*fpos >= hdl->total_size) {
                printk("%s: read pointer out of range", GET_MODULE_NAME());
                return ENOSPC;
        }

        int err = sys_mutex_lock(hdl->mtx, TIMEOUT_MS);
        if (!err) {

                uint32_t src = SECTOR_INFO[hdl->start_sector].addr
                             + cast(uint32_t, *fpos);

                count = min(count, (hdl->total_size - *fpos));

                memcpy(dst, cast(uint8_t*, src), count);

                *rdcnt = count;

                sys_mutex_unlock(hdl->mtx);
        }

        return err;
}

//==============================================================================
/**
 * @brief IO control.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_IOCTL(NVM, void *device_handle, int request, void *arg)
{
        NVM_t *hdl = device_handle;

        int err = EINVAL;

        switch (request) {
        case IOCTL_NVM__CONFIGURE:
                if (arg) {
                        const NVM_config_t *conf = arg;

                        if (  (conf->number_of_sectors == 0)
                           || (conf->start_sector >= ARRAY_SIZE(SECTOR_INFO))
                           || ((conf->start_sector + conf->number_of_sectors - 1)
                              >= ARRAY_SIZE(SECTOR_INFO)) ) {

                                break;
                        }

                        err = sys_mutex_lock(hdl->mtx, TIMEOUT_MS);
                        if (!err) {
                                hdl->sector_count = conf->number_of_sectors;
                                hdl->start_sector = conf->start_sector;

                                hdl->total_size = 0;
                                size_t sector   = hdl->start_sector;
                                size_t count    = hdl->sector_count;

                                while (count--) {
                                        hdl->total_size += SECTOR_INFO[sector].size;
                                        sector++;
                                }

                                sys_mutex_unlock(hdl->mtx);
                        }
                }
                break;

        case IOCTL_NVM__SECTOR_ERASE:
                if (arg) {
                        err = sys_mutex_lock(hdl->mtx, TIMEOUT_MS);
                        if (!err) {
                                FLASH_unlock();

                                size_t sector = *cast(size_t*, arg) + hdl->start_sector;

                                if (sector < ARRAY_SIZE(SECTOR_INFO)) {
                                        err = FLASH_erase_sector(sector);
                                }

                                FLASH_lock();

                                sys_mutex_unlock(hdl->mtx);
                        }
                }
                break;

        case IOCTL_NVM__MASS_ERASE:
                err = sys_mutex_lock(hdl->mtx, TIMEOUT_MS);
                if (!err) {
                        FLASH_unlock();

                        for (size_t sector = hdl->start_sector;
                             sector < (hdl->start_sector + hdl->sector_count);
                             sector++) {

                                err = FLASH_erase_sector(sector);

                                if (err) {
                                        break;
                                }
                        }

                        FLASH_lock();

                        sys_mutex_unlock(hdl->mtx);
                }
                break;

        case IOCTL_NVM__GET_SECTOR_SIZE:
                if (arg) {
                        NVM_sector_size_t *secsz = arg;

                        size_t sector = secsz->sector + hdl->start_sector;

                        if (sector < (hdl->start_sector + hdl->sector_count)) {
                                secsz->size = SECTOR_INFO[sector].size;
                                err = ESUCC;
                        }
                }
                break;

        case IOCTL_NVM__GET_SECTOR_COUNT:
                if (arg) {
                        *cast(size_t*, arg) = hdl->sector_count;
                        err = ESUCC;
                }
                break;

        default:
                err = EBADRQC;
                break;
        }

        return err;
}

//==============================================================================
/**
 * @brief Flush device.
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_FLUSH(NVM, void *device_handle)
{
        UNUSED_ARG1(device_handle);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Device information.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_STAT(NVM, void *device_handle, struct vfs_dev_stat *device_stat)
{
        NVM_t *hdl = device_handle;

        device_stat->st_size = hdl->total_size;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Unlock for erase/program flash memory.
 */
//==============================================================================
static void FLASH_unlock(void)
{
        if (FLASH->CR & FLASH_CR_LOCK) {
                FLASH->KEYR = FLASH_KEY1;
                FLASH->KEYR = FLASH_KEY2;
        }
}

//==============================================================================
/**
 * @brief  Lock for erase/program flash memory.
 */
//==============================================================================
static void FLASH_lock(void)
{
        FLASH->CR |= FLASH_CR_LOCK;
}

//==============================================================================
/**
 * @brief  Erase selected sector.
 *
 * @param  sector       sector to erase
 *
 * @return One of errno value.
 */
//==============================================================================
static int FLASH_erase_sector(uint32_t sector)
{
        if (sector >= 12) {
                sector += 4;
        }

        int err = FLASH_wait_for_operation_finish();

        if (!err) {
                FLASH->CR &= FLASH_CR_PSIZE_Msk;
                FLASH->CR |= FLASH_CR_PSIZE_BYTE;
                FLASH->CR &= FLASH_CR_SNB_Msk;
                FLASH->CR |= ((sector << FLASH_CR_SNB_Pos) & FLASH_CR_SNB_Msk);
                FLASH->CR |= FLASH_CR_SER;
                FLASH->CR |= FLASH_CR_STRT;

                err = FLASH_wait_for_operation_finish();

                FLASH->CR &= (~FLASH_CR_SER);
                FLASH->CR &= FLASH_CR_SNB_Msk;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Program byte at selected address.
 *
 * @param  address      destination address
 * @param  data         data to program
 *
 * @return One of errno value.
 */
//==============================================================================
static int FLASH_program_byte(uint32_t address, uint8_t data)
{
        int err = FLASH_wait_for_operation_finish();

        if (!err) {
                FLASH->CR &= FLASH_CR_PSIZE_Msk;
                FLASH->CR |= FLASH_CR_PSIZE_BYTE;
                FLASH->CR |= FLASH_CR_PG;

                *cast(__IO uint8_t*, address) = data;

                err = FLASH_wait_for_operation_finish();

                FLASH->CR &= (~FLASH_CR_PG);

                if (*cast(__IO uint8_t*, address) != data) {
                        printk("%s: cell mismatch @ 0x%X", GET_MODULE_NAME(), address);
                        err = EIO;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Waits for operation finish.
 *
 * @return  One of errno value.
 */
//==============================================================================
static int FLASH_wait_for_operation_finish(void)
{
        int err = ETIME;

        u32_t tref = sys_get_uptime_ms();

        while (not sys_time_is_expired(tref, sys_get_uptime_ms())) {

                if (FLASH->SR & FLASH_SR_BSY) {
                        sys_sleep_ms(1);

                } else {
                        if (FLASH->SR & FLASH_SR_WRPERR) {
                                printk("%s: write protection error", GET_MODULE_NAME());
                                err = EIO;
                                break;

                        } else if (FLASH->SR & ( FLASH_SR_PGSERR | FLASH_SR_PGPERR
                                               | FLASH_SR_PGAERR)) {
                                printk("%s: program error", GET_MODULE_NAME());
                                err = EIO;
                                break;

                        } else if (FLASH->SR & FLASH_SR_SOP) {
                                printk("%s: operation error", GET_MODULE_NAME());
                                err = EIO;
                                break;
                        } else {
                                err = ESUCC;
                                break;
                        }
                }
        }

        SET_BIT(FLASH->SR, FLASH_SR_WRPERR | FLASH_SR_PGSERR | FLASH_SR_PGPERR
                         | FLASH_SR_PGAERR | FLASH_SR_SOP);

        if (err == ETIME) {
                printk("%s: operation timeout", GET_MODULE_NAME());
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
