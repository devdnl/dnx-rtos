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
#include "stm32f1/nvm_cfg.h"
#include "stm32f1/stm32f10x.h"
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

#ifdef STM32F10X_XL
#define MAX_SECTORS             512
#else
#define MAX_SECTORS             256
#endif

#define MAX_SECTORS_IN_BANK     256

#if defined(STM32F10X_LD) || defined(STM32F10X_MD_VL) || defined(STM32F10X_MD)
#define SECTOR_SIZE 1024

#elif defined(STM32F10X_HD_VL) || defined(STM32F10X_HD) || defined(STM32F10X_XL) || defined(STM32F10X_CL)
#define SECTOR_SIZE 2048
#else
#error "Undefined processor family!"
#endif

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        mutex_t *mtx;
        uint32_t start_sector;
        uint32_t sector_count;
        uint32_t total_size;
} NVM_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void FLASH_unlock(void);
static void FLASH_lock(void);
static int  FLASH_erase_sector(uint32_t sector);
static int  FLASH_program_half_word(uint32_t address, uint16_t data);
static int  FLASH_wait_for_operation_finish(uint32_t address);
static int  configure(NVM_t *hdl, const NVM_config_t *conf);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(NVM);

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
 * @param[in ]            config               optional module configuration
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_INIT(NVM, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(major);

        int err = EFAULT;

        if (minor == 0) {
                err = sys_zalloc(sizeof(NVM_t), device_handle);
                if (!err) {
                        NVM_t *hdl = *device_handle;

                        err = sys_mutex_create(MUTEX_TYPE_NORMAL, &hdl->mtx);

                        if (!err && config) {
                                err = configure(hdl, config);
                        }

                        if (err) {
                                if (hdl->mtx) {
                                        sys_mutex_destroy(hdl->mtx);
                                }

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

        if (*fpos & 1) {
                printk("%s: address is not aligned to half words", GET_MODULE_NAME());
                return EINVAL;
        }

        int err = sys_mutex_lock(hdl->mtx, TIMEOUT_MS);
        if (!err) {

                uint32_t dst = FLASH_BASE + (hdl->start_sector * SECTOR_SIZE)
                             + cast(uint32_t, *fpos);

                count = min(count, (hdl->total_size - *fpos));

                size_t n = 0;

                FLASH_unlock();

                while (!err && count >= 2) {
                        err = FLASH_program_half_word(dst, *cast(u16_t*, src));
                        src   += 2;
                        count -= 2;
                        dst   += 2;
                        n     += 2;
                }

                if (!err && count == 1) {
                        err = FLASH_program_half_word(dst, 0xFF00 + *src++);
                        count -= 1;
                        dst   += 1;
                        n     += 1;
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
                printk("%s: read not configured device", GET_MODULE_NAME());
                return EINVAL;
        }

        if (*fpos >= hdl->total_size) {
                printk("%s: read pointer out of range", GET_MODULE_NAME());
                return ENOSPC;
        }

        int err = sys_mutex_lock(hdl->mtx, TIMEOUT_MS);
        if (!err) {

                uint32_t src = FLASH_BASE + (hdl->start_sector * SECTOR_SIZE)
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
                        err = configure(hdl, arg);
                }
                break;

        case IOCTL_NVM__SECTOR_ERASE:
                if (arg) {
                        err = sys_mutex_lock(hdl->mtx, TIMEOUT_MS);
                        if (!err) {
                                FLASH_unlock();

                                size_t sector = *cast(size_t*, arg) + hdl->start_sector;

                                if (sector < MAX_SECTORS) {
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
                                secsz->size = SECTOR_SIZE;
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
 * @brief  Function configure NVM.
 *
 * @param  hdl          driver handle
 * @param  config       driver configuration
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int configure(NVM_t *hdl, const NVM_config_t *conf)
{
        if (  (conf->number_of_sectors == 0)
           || (conf->start_sector >= MAX_SECTORS)
           || ((conf->start_sector + conf->number_of_sectors - 1)
              >= MAX_SECTORS) ) {

                return EINVAL;
        }

        int err = sys_mutex_lock(hdl->mtx, TIMEOUT_MS);
        if (!err) {
                hdl->sector_count = conf->number_of_sectors;
                hdl->start_sector = conf->start_sector;
                hdl->total_size   = SECTOR_SIZE * hdl->sector_count;

                sys_mutex_unlock(hdl->mtx);
        }

        return err;
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

#ifdef STM32F10X_XL
        if (FLASH->CR2 & FLASH_CR_LOCK) {
                FLASH->KEYR2 = FLASH_KEY1;
                FLASH->KEYR2 = FLASH_KEY2;
        }
#endif
}

//==============================================================================
/**
 * @brief  Lock for erase/program flash memory.
 */
//==============================================================================
static void FLASH_lock(void)
{
        FLASH->CR |= FLASH_CR_LOCK;

#ifdef STM32F10X_XL
        FLASH->CR2 |= FLASH_CR_LOCK;
#endif
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
        __IO uint32_t *FLASH_CR;
        __IO uint32_t *FLASH_AR;

        if (sector < MAX_SECTORS_IN_BANK) {
                FLASH_CR = &FLASH->CR;
                FLASH_AR = &FLASH->AR;
#ifdef STM32F10X_XL
        } else {
                FLASH_CR = &FLASH->CR2;
                FLASH_AR = &FLASH->AR2;
#endif
        }

        uint32_t addr = FLASH_BASE + (sector * SECTOR_SIZE);

        int err = FLASH_wait_for_operation_finish(addr);

        if (!err) {
                *FLASH_CR |= FLASH_CR_PER;
                *FLASH_AR  = addr;
                *FLASH_CR |= FLASH_CR_STRT;

                err = FLASH_wait_for_operation_finish(addr);

                *FLASH_CR &= (~FLASH_CR_PER);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Program half word at selected address.
 *
 * @param  address      destination address
 * @param  data         data to program
 *
 * @return One of errno value.
 */
//==============================================================================
static int FLASH_program_half_word(uint32_t address, uint16_t data)
{
        __IO uint32_t *FLASH_CR;

        if (address < FLASH_BASE + (SECTOR_SIZE * MAX_SECTORS_IN_BANK)) {
                FLASH_CR = &FLASH->CR;
#ifdef STM32F10X_XL
        } else {
                FLASH_CR = &FLASH->CR2;
#endif
        }

        int err = FLASH_wait_for_operation_finish(address);

        if (!err) {
                *FLASH_CR |= FLASH_CR_PG;

                *cast(__IO uint16_t*, address) = data;

                err = FLASH_wait_for_operation_finish(address);

                *FLASH_CR &= (~FLASH_CR_PG);

                if (*cast(__IO uint16_t*, address) != data) {
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
 * @param  address      address to detect current bank
 *
 * @return  One of errno value.
 */
//==============================================================================
static int FLASH_wait_for_operation_finish(uint32_t address)
{
        __IO uint32_t *FLASH_SR;

        if (address < FLASH_BASE + (SECTOR_SIZE * MAX_SECTORS_IN_BANK)) {
                FLASH_SR = &FLASH->SR;
#ifdef STM32F10X_XL
        } else {
                FLASH_SR = &FLASH->SR2;
#endif
        }

        int err = ETIME;

        u32_t tref = sys_get_uptime_ms();

        while (not sys_time_is_expired(tref, sys_get_uptime_ms())) {

                if (*FLASH_SR & FLASH_SR_BSY) {
                        sys_sleep_ms(1);

                } else {
                        if (*FLASH_SR & FLASH_SR_WRPRTERR) {
                                printk("%s: write protection error", GET_MODULE_NAME());
                                err = EIO;
                                break;

                        } else if (*FLASH_SR & FLASH_SR_PGERR) {
                                printk("%s: program error, cell @ 0x%X is not erased",
                                       GET_MODULE_NAME(), FLASH->AR | FLASH_BASE);
                                err = EIO;
                                break;
                        } else {
                                err = ESUCC;
                                break;
                        }
                }
        }

        // clear flags
        SET_BIT(*FLASH_SR, FLASH_SR_EOP | FLASH_SR_PGERR | FLASH_SR_WRPRTERR);

        if (err == ETIME) {
                printk("%s: operation timeout", GET_MODULE_NAME());
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
