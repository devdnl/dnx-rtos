/*==============================================================================
File    sdmmc.c

Author  Daniel Zorychta

Brief   SD/MMC/eMMC Card Driver.

        Copyright (C) 2022 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "stm32h7/dma_ddi.h"
#include "sys/ioctl.h"
#include "../sdmmc_ioctl.h"
#include "stm32h7xx_hal_sdmmc.h"
#include "stm32h7xx_hal_sd.h"
#include "stm32h7xx_hal_mmc.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define MTX_TIMEOUT                     2500

#define _SDMMC_MODE_POLLING             0
#define _SDMMC_MODE_IRQ                 1
#define _SDMMC_MODE_DMA                 2
#define _SDMMC_TYPE_SDCARD              0
#define _SDMMC_TYPE_MMC                 1

#define IS_SD_USING                     ((__SDMMC_CFG_SDMMC1_TYPE__ == _SDMMC_TYPE_SDCARD) || (__SDMMC_CFG_SDMMC2_TYPE__ == _SDMMC_TYPE_SDCARD))
#define IS_MMC_USING                    ((__SDMMC_CFG_SDMMC1_TYPE__ == _SDMMC_TYPE_MMC) || (__SDMMC_CFG_SDMMC2_TYPE__ == _SDMMC_TYPE_MMC))

/*==============================================================================
  Local object types
==============================================================================*/
enum {
        #if defined(SDMMC1)
        SDMMC_1,
        #endif
        #if defined(SDMMC2)
        SDMMC_2,
        #endif
        SDMMC_COUNT
};

/** driver instance associated with partition */
typedef struct {
        u32_t mode;
        u8_t major;
        u8_t minor;
        u64_t card_size;
        kmtx_t *mtx;
        ksem_t *sem_xfer_complete;
        bool irq_yield;

        union {
                #if IS_SD_USING
                SD_HandleTypeDef sd;
                #endif
                #if IS_MMC_USING
                MMC_HandleTypeDef mmc;
                #endif
        } handle;
} SDMMC_t;

typedef struct {
        SDMMC_TypeDef *instance;
        __IO u32_t    *RCC_APBENR;
        u32_t          RCC_APBENR_SDMMCEN;
        IRQn_Type      IRQn;
        u32_t          IRQ_priority;
        u32_t          card_timeout;
        u32_t          clock_edge;
        u32_t          clock_power_save;
        u32_t          clock_div;
        u32_t          bus_wide;
        u32_t          hardware_flow_ctrl;
        u32_t          mode;
        u32_t          speed_mode;
        u8_t           card_type;
} sdmmc_info_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int card_initialize(SDMMC_t *hdl);
static int card_wait_ready(SDMMC_t *hdl);
static int card_write_blocks(SDMMC_t *hdl, const u8_t *src, uint32_t block_address, uint32_t blocks, uint32_t timeout);
static int card_write_blocks_IT(SDMMC_t *hdl, const u8_t *src, uint32_t block_address, uint32_t blocks);
static int card_write_blocks_DMA(SDMMC_t *hdl, const u8_t *src, uint32_t block_address, uint32_t blocks);
static int card_read_blocks(SDMMC_t *hdl, u8_t *dst, uint32_t block_address, uint32_t blocks, uint32_t timeout);
static int card_read_blocks_IT(SDMMC_t *hdl, u8_t *dst, uint32_t block_address, uint32_t blocks);
static int card_read_blocks_DMA(SDMMC_t *hdl, u8_t *dst, uint32_t block_address, uint32_t blocks);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(SDMMC);

/*
 * For IRQ purpose only.
 */
static SDMMC_t *sdmmc[SDMMC_COUNT];

/*
 * SDMMCx configuration.
 */
static const sdmmc_info_t SDMMC[] = {
        #if defined(SDMMC1)
        {
                .instance = SDMMC1,
                .RCC_APBENR = &RCC->AHB3ENR,
                .RCC_APBENR_SDMMCEN = RCC_AHB3ENR_SDMMC1EN,
                .IRQn = SDMMC1_IRQn,
                .IRQ_priority = __SDMMC_CFG_SDMMC1_IRQ_PRIORITY__,
                .card_timeout = __SDMMC_CFG_SDMMC1_TIMEOUT__,
                .clock_edge = __SDMMC_CFG_SDMMC1_CLOCK_EDGE__,
                .clock_power_save = __SDMMC_CFG_SDMMC1_PWRSAVE__,
                .clock_div = __SDMMC_CFG_SDMMC1_CKDIV__ - 1,
                .bus_wide = __SDMMC_CFG_SDMMC1_BUS_WIDE__,
                .hardware_flow_ctrl = __SDMMC_CFG_SDMMC1_HW_FLOW_CTRL__,
                .mode = __SDMMC_CFG_SDMMC1_MODE__,
                .speed_mode = __SDMMC_CFG_SDMMC1_SPEED_MODE__,
                .card_type = __SDMMC_CFG_SDMMC1_TYPE__,
        },
        #endif
        #if defined(SDMMC2)
        {
                .instance = SDMMC2,
                .RCC_APBENR = &RCC->AHB2ENR,
                .RCC_APBENR_SDMMCEN = RCC_AHB2ENR_SDMMC2EN,
                .IRQn = SDMMC2_IRQn,
                .IRQ_priority = __SDMMC_CFG_SDMMC2_IRQ_PRIORITY__,
                .card_timeout = __SDMMC_CFG_SDMMC2_TIMEOUT__,
                .clock_edge = __SDMMC_CFG_SDMMC2_CLOCK_EDGE__,
                .clock_power_save = __SDMMC_CFG_SDMMC2_PWRSAVE__,
                .clock_div = __SDMMC_CFG_SDMMC2_CKDIV__ - 1,
                .bus_wide = __SDMMC_CFG_SDMMC2_BUS_WIDE__,
                .hardware_flow_ctrl = __SDMMC_CFG_SDMMC2_HW_FLOW_CTRL__,
                .mode = __SDMMC_CFG_SDMMC2_MODE__,
                .speed_mode = __SDMMC_CFG_SDMMC2_SPEED_MODE__,
                .card_type = __SDMMC_CFG_SDMMC2_TYPE__,
        }
        #endif
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
 * @param[in ]            config               optional module configuration
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_INIT(SDMMC, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(config);

        int err = ENODEV;

        if ((major < SDMMC_COUNT) && (minor == 0)) {
                err = sys_zalloc(sizeof(SDMMC_t), device_handle);
                if (!err) {
                        SDMMC_t *hdl = *device_handle;
                        hdl->major = major;
                        hdl->minor = minor;

                        err = sys_semaphore_create(1, 0, &hdl->sem_xfer_complete);
                        if (!err) {

                                err = sys_mutex_create(KMTX_TYPE_RECURSIVE, &hdl->mtx);
                                if (!err) {
                                        err = card_initialize(hdl);
                                        if (!err) {
                                                sdmmc[hdl->major] = hdl;
                                                return err;
                                        }

                                        sys_mutex_destroy(hdl->mtx);
                                }

                                sys_semaphore_destroy(hdl->sem_xfer_complete);
                        }

                        sys_free(*device_handle);
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
API_MOD_RELEASE(SDMMC, void *device_handle)
{
        SDMMC_t *hdl = device_handle;

        kmtx_t *mtx = hdl->mtx;

        int err = sys_mutex_lock(mtx, MTX_TIMEOUT);
        if (!err) {
                hdl->mtx = NULL;
                sys_sleep_ms(MTX_TIMEOUT + 500);

                if (SDMMC[hdl->major].card_type == _SDMMC_TYPE_SDCARD) {
                        #if IS_SD_USING
                        HAL_SD_DeInit(&hdl->handle.sd);
                        #endif
                } else {
                        #if IS_MMC_USING
                        HAL_MMC_DeInit(&hdl->handle.mmc);
                        #endif
                }

                sys_mutex_unlock(mtx);
                sys_mutex_destroy(hdl->mtx);
                sys_free(device_handle);
                sdmmc[hdl->major] = NULL;
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
API_MOD_OPEN(SDMMC, void *device_handle, u32_t flags)
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
API_MOD_CLOSE(SDMMC, void *device_handle, bool force)
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
API_MOD_WRITE(SDMMC,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fattr);

        SDMMC_t *hdl = device_handle;

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {
                if (((*fpos & (BLOCKSIZE - 1)) == 0) and ((count & (BLOCKSIZE - 1)) == 0)) {

                        u32_t block_address = *fpos / BLOCKSIZE;
                        u32_t blocks        = count / BLOCKSIZE;

                        for (int i = 0; i < 5; i++) {

                                err = card_wait_ready(hdl);
                                if (err) {
                                        dev_dbg(hdl, "card busy at write transfer");
                                }

                                if (hdl->mode == _SDMMC_MODE_POLLING) {

                                        err = card_write_blocks(hdl, src, block_address, blocks,
                                                                SDMMC[hdl->major].card_timeout);

                                } else if (hdl->mode == _SDMMC_MODE_IRQ) {

                                        err = card_write_blocks_IT(hdl, src, block_address, blocks);

                                } else if (hdl->mode == _SDMMC_MODE_DMA) {

                                        err = card_write_blocks_DMA(hdl, src, block_address, blocks);

                                } else {
                                        err = EFAULT;
                                        dev_dbg(hdl, "unknown card mode!");
                                }

                                if (!err) {
                                        *wrcnt += count;
                                        break;
                                } else {
                                        dev_dbg(hdl, "write error %d @ block %u", err, block_address);
                                        sys_sleep_ms(25);

                                        if (i >= 3) {
                                                card_initialize(hdl);
                                        }
                                }
                        }

                } else {
                        err = EINVAL;
                }

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
API_MOD_READ(SDMMC,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        SDMMC_t *hdl  = device_handle;

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {
                if (((*fpos & (BLOCKSIZE - 1)) == 0) and ((count & (BLOCKSIZE - 1)) == 0)) {

                        u32_t block_address = *fpos / BLOCKSIZE;
                        u32_t blocks        = count / BLOCKSIZE;

                        for (int i = 0; i < 5; i++) {

                                err = card_wait_ready(hdl);
                                if (err) {
                                        dev_dbg(hdl, "card busy at read transfer");
                                }

                                if (hdl->mode == _SDMMC_MODE_POLLING) {

                                        err = card_read_blocks(hdl, dst, block_address, blocks,
                                                               SDMMC[hdl->major].card_timeout);

                                } else if (hdl->mode == _SDMMC_MODE_IRQ) {

                                        err = card_read_blocks_IT(hdl, dst, block_address, blocks);

                                } else if (hdl->mode == _SDMMC_MODE_DMA) {

                                        err = card_read_blocks_DMA(hdl, dst, block_address, blocks);

                                } else {
                                        err = EFAULT;
                                        dev_dbg(hdl, "unknown card mode!");
                                }

                                if (!err) {
                                        *rdcnt += count;
                                        break;
                                } else {
                                        dev_dbg(hdl, "read error %d @ block %u", err, block_address);
                                        sys_sleep_ms(25);

                                        if (i >= 3) {
                                                card_initialize(hdl);
                                        }
                                }
                        }

                } else {
                        err = EINVAL;
                }

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
API_MOD_IOCTL(SDMMC, void *device_handle, int request, void *arg)
{
        UNUSED_ARG1(arg);

        SDMMC_t *hdl = device_handle;

        int err = EBADRQC;

        switch (request) {
        case IOCTL_SDIO__INITIALIZE_CARD:
                err = sys_mutex_lock(hdl->mtx, _MAX_DELAY_MS);
                if (!err) {
                        err = card_initialize(hdl);
                        sys_mutex_unlock(hdl->mtx);
                }
                break;

        default:
                return EBADRQC;
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
API_MOD_FLUSH(SDMMC, void *device_handle)
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
API_MOD_STAT(SDMMC, void *device_handle, struct vfs_dev_stat *device_stat)
{
        SDMMC_t *hdl = device_handle;

        device_stat->st_size = hdl->card_size;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Write blocks to SD/MMC by using polling method.
 *
 * @param  hdl                  driver handle
 * @param  src                  source buffer
 * @param  block_address        block address
 * @param  blocks               blocks to write
 * @param  timeout              timeout in milliseconds
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int card_write_blocks(SDMMC_t *hdl, const u8_t *src, uint32_t block_address, uint32_t blocks, uint32_t timeout)
{
        if (SDMMC[SDMMC_1].card_type == _SDMMC_TYPE_SDCARD) {
                #if IS_SD_USING
                return HAL_SD_WriteBlocks(&hdl->handle.sd, src, block_address, blocks, timeout);
                #endif
        } else {
                #if IS_MMC_USING
                return HAL_MMC_WriteBlocks(&hdl->handle.mmc, src, block_address, blocks, timeout);
                #endif
        }

        return EFAULT;
}

//==============================================================================
/**
 * @brief  Write blocks to SD/MMC by using interrupts.
 *
 * @param  hdl                  driver handle
 * @param  src                  source buffer
 * @param  block_address        block address
 * @param  blocks               blocks to write
 * @param  timeout              timeout in milliseconds
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int card_write_blocks_IT(SDMMC_t *hdl, const u8_t *src, uint32_t block_address, uint32_t blocks)
{
        int err = EFAULT;

        if (SDMMC[SDMMC_1].card_type == _SDMMC_TYPE_SDCARD) {
                #if IS_SD_USING
                err = HAL_SD_WriteBlocks_IT(&hdl->handle.sd, src, block_address, blocks);
                #endif
        } else {
                #if IS_MMC_USING
                err = HAL_MMC_WriteBlocks_IT(&hdl->handle.mmc, src, block_address, blocks);
                #endif
        }

        if (!err) {
                err = sys_semaphore_wait(hdl->sem_xfer_complete,
                                         SDMMC[hdl->major].card_timeout);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Write blocks to SD/MMC by using DMA.
 *
 * @param  hdl                  driver handle
 * @param  src                  source buffer
 * @param  block_address        block address
 * @param  blocks               blocks to write
 * @param  timeout              timeout in milliseconds
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int card_write_blocks_DMA(SDMMC_t *hdl, const u8_t *src, uint32_t block_address, uint32_t blocks)
{
        int err = EFAULT;

        _cpuctl_clean_invalidate_dcache_by_addr(const_cast(src), blocks * BLOCKSIZE);

        if (SDMMC[SDMMC_1].card_type == _SDMMC_TYPE_SDCARD) {
                #if IS_SD_USING
                err = HAL_SD_WriteBlocks_DMA(&hdl->handle.sd, src, block_address, blocks);
                #endif
        } else {
                #if IS_MMC_USING
                err = HAL_MMC_WriteBlocks_DMA(&hdl->handle.mmc, src, block_address, blocks);
                #endif
        }

        if (!err) {
                err = sys_semaphore_wait(hdl->sem_xfer_complete,
                                         SDMMC[hdl->major].card_timeout);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Read blocks to SD/MMC by using polling method.
 *
 * @param  hdl                  driver handle
 * @param  dst                  destination buffer
 * @param  block_address        block address
 * @param  blocks               blocks to write
 * @param  timeout              timeout in milliseconds
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int card_read_blocks(SDMMC_t *hdl, u8_t *dst, uint32_t block_address, uint32_t blocks, uint32_t timeout)
{
        if (SDMMC[SDMMC_1].card_type == _SDMMC_TYPE_SDCARD) {
                #if IS_SD_USING
                return HAL_SD_ReadBlocks(&hdl->handle.sd, dst, block_address, blocks, timeout);
                #endif
        } else {
                #if IS_MMC_USING
                return HAL_MMC_ReadBlocks(&hdl->handle.mmc, dst, block_address, blocks, timeout);
                #endif
        }

        return EFAULT;
}

//==============================================================================
/**
 * @brief  Read blocks to SD/MMC by using interrupts.
 *
 * @param  hdl                  driver handle
 * @param  dst                  destination buffer
 * @param  block_address        block address
 * @param  blocks               blocks to write
 * @param  timeout              timeout in milliseconds
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int card_read_blocks_IT(SDMMC_t *hdl, u8_t *dst, uint32_t block_address, uint32_t blocks)
{
        int err = EFAULT;

        if (SDMMC[SDMMC_1].card_type == _SDMMC_TYPE_SDCARD) {
                #if IS_SD_USING
                err = HAL_SD_ReadBlocks_IT(&hdl->handle.sd, dst, block_address, blocks);
                #endif
        } else {
                #if IS_MMC_USING
                err = HAL_MMC_ReadBlocks_IT(&hdl->handle.mmc, dst, block_address, blocks);
                #endif
        }

        if (!err) {
                err = sys_semaphore_wait(hdl->sem_xfer_complete,
                                         SDMMC[hdl->major].card_timeout);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Read blocks to SD/MMC by using DMA.
 *
 * @param  hdl                  driver handle
 * @param  dst                  destination buffer
 * @param  block_address        block address
 * @param  blocks               blocks to write
 * @param  timeout              timeout in milliseconds
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int card_read_blocks_DMA(SDMMC_t *hdl, u8_t *dst, uint32_t block_address, uint32_t blocks)
{
        int err = EFAULT;

        _cpuctl_clean_invalidate_dcache_by_addr(dst, blocks * BLOCKSIZE);

        if (SDMMC[SDMMC_1].card_type == _SDMMC_TYPE_SDCARD) {
                #if IS_SD_USING
                err = HAL_SD_ReadBlocks_DMA(&hdl->handle.sd, dst, block_address, blocks);
                #endif
        } else {
                #if IS_MMC_USING
                err = HAL_MMC_ReadBlocks_DMA(&hdl->handle.mmc, dst, block_address, blocks);
                #endif
        }

        if (!err) {
                err = sys_semaphore_wait(hdl->sem_xfer_complete,
                                         SDMMC[hdl->major].card_timeout);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Basic interface initialization.
 *
 * @param  hdl          driver instance
 */
//==============================================================================
static void HAL_SDMMC_MspInit(SDMMC_t *hdl)
{
        SET_BIT(*SDMMC[hdl->major].RCC_APBENR, SDMMC[hdl->major].RCC_APBENR_SDMMCEN);

        if (hdl->mode != _SDMMC_MODE_POLLING) {
                NVIC_SetPriority(SDMMC[hdl->major].IRQn, SDMMC[hdl->major].IRQ_priority);
                NVIC_ClearPendingIRQ(SDMMC[hdl->major].IRQn);
                NVIC_EnableIRQ(SDMMC[hdl->major].IRQn);
        }
}

//==============================================================================
/**
 * @brief  Basic interface de-initialization.
 *
 * @param  hdl          driver instance
 */
//==============================================================================
static void HAL_SDMMC_MspDeInit(SDMMC_t *hdl)
{
        NVIC_ClearPendingIRQ(SDMMC[hdl->major].IRQn);
        NVIC_DisableIRQ(SDMMC[hdl->major].IRQn);

        CLEAR_BIT(*SDMMC[hdl->major].RCC_APBENR, SDMMC[hdl->major].RCC_APBENR_SDMMCEN);
}

//==============================================================================
/**
 * @brief  Basic MMC interface initialization.
 *
 * @param  hsd          SD handle
 */
//==============================================================================
void HAL_MMC_MspInit(MMC_HandleTypeDef *hmmc)
{
        HAL_SDMMC_MspInit(hmmc->CtxPtr);
}

//==============================================================================
/**
 * @brief  Basic SD interface initialization.
 *
 * @param  hsd          SD handle
 */
//==============================================================================
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
        HAL_SDMMC_MspInit(hsd->CtxPtr);
}

//==============================================================================
/**
 * @brief  Basic SD interface de-initialization.
 *
 * @param  hsd          SD handle
 */
//==============================================================================
void HAL_SD_MspDeInit(SD_HandleTypeDef *hsd)
{
        HAL_SDMMC_MspDeInit(hsd->CtxPtr);
}

//==============================================================================
/**
 * @brief  Basic MMC interface de-initialization.
 *
 * @param  hsd          SD handle
 */
//==============================================================================
void HAL_MMC_MspDeInit(MMC_HandleTypeDef *hmmc)
{
        HAL_SDMMC_MspDeInit(hmmc->CtxPtr);
}

//==============================================================================
/**
 * @brief  Card initialization.
 *
 * @param  hdl          device handle
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int card_initialize(SDMMC_t *hdl)
{
        int err = EFAULT;

        hdl->mode = SDMMC[hdl->major].mode;

        if (SDMMC[hdl->major].card_type == _SDMMC_TYPE_SDCARD) {
                #if IS_SD_USING
                hdl->handle.sd.CtxPtr = hdl;

                hdl->handle.sd.Instance = SDMMC[hdl->major].instance;
                hdl->handle.sd.Init.ClockEdge = SDMMC[hdl->major].clock_edge;
                hdl->handle.sd.Init.ClockPowerSave = SDMMC[hdl->major].clock_power_save;
                hdl->handle.sd.Init.BusWide = SDMMC_BUS_WIDE_1B;
                hdl->handle.sd.Init.HardwareFlowControl = SDMMC[hdl->major].hardware_flow_ctrl;
                hdl->handle.sd.Init.ClockDiv = SDMMC[hdl->major].clock_div;

                for (int i = 0; i < 3; i++) {
                        HAL_SD_DeInit(&hdl->handle.sd);

                        err = HAL_SD_Init(&hdl->handle.sd);
                        if (!err) {

                                const char *mode = "?";
                                switch (hdl->mode) {
                                case _SDMMC_MODE_POLLING: mode = "polling"; break;
                                case _SDMMC_MODE_IRQ: mode = "IRQ"; break;
                                case _SDMMC_MODE_DMA: mode = "DMA"; break;
                                }
                                dev_dbg(hdl, "using %s mode", mode);

                                if (SDMMC[hdl->major].bus_wide != SDMMC_BUS_WIDE_1B) {
                                        if (HAL_SD_ConfigWideBusOperation(&hdl->handle.sd, SDMMC_BUS_WIDE_4B) == HAL_OK) {
                                                dev_dbg(hdl, "switched to 4-bit bus");
                                        } else {
                                                dev_dbg(hdl, "4-bit bus not supported");
                                        }
                                }

                                if (SDMMC[hdl->major].speed_mode != SDMMC_SPEED_MODE_DEFAULT) {

                                        const char *speed_mode = "?";
                                        switch (SDMMC[hdl->major].speed_mode) {
                                        case SDMMC_SPEED_MODE_DEFAULT: speed_mode = "default"; break;
                                        case SDMMC_SPEED_MODE_AUTO: speed_mode = "auto"; break;
                                        case SDMMC_SPEED_MODE_HIGH: speed_mode = "high"; break;
                                        case SDMMC_SPEED_MODE_ULTRA: speed_mode = "ultra"; break;
                                        }

                                        err = HAL_SD_ConfigSpeedBusOperation(&hdl->handle.sd, SDMMC[hdl->major].speed_mode);
                                        if (not err) {
                                                dev_dbg(hdl, "switched to %s speed mode", speed_mode);
                                        } else {
                                                dev_dbg(hdl, "%s speed mode error: %d", speed_mode, err);
                                                HAL_SD_ConfigSpeedBusOperation(&hdl->handle.sd, SDMMC_SPEED_MODE_DEFAULT);
                                        }
                                }

                                HAL_SD_CardInfoTypeDef info;
                                err = HAL_SD_GetCardInfo(&hdl->handle.sd, &info);
                                if (!err) {

                                        dev_dbg(hdl, "found SD%s card",
                                                info.CardType == CARD_SDSC ? "SC" : "HC/XC");

                                        dev_dbg(hdl, "%d %u-byte logical blocks",
                                                info.BlockNbr, info.BlockSize);

                                        hdl->card_size = cast(u64_t, info.BlockSize)
                                                       * cast(u64_t, info.BlockNbr);
                                }
                        }

                        if (err) {
                                HAL_SD_DeInit(&hdl->handle.sd);
                        } else {
                                break;
                        }
                }
                #endif

        } else {
                #if IS_MMC_USING
                hdl->handle.mmc.CtxPtr = hdl;

                hdl->handle.mmc.Instance = SDMMC[hdl->major].instance;
                hdl->handle.mmc.Init.ClockEdge = SDMMC[hdl->major].clock_edge;
                hdl->handle.mmc.Init.ClockPowerSave = SDMMC[hdl->major].clock_power_save;
                hdl->handle.mmc.Init.BusWide = SDMMC_BUS_WIDE_1B;
                hdl->handle.mmc.Init.HardwareFlowControl = SDMMC[hdl->major].hardware_flow_ctrl;
                hdl->handle.mmc.Init.ClockDiv = SDMMC[hdl->major].clock_div;

                for (int i = 0; i < 3; i++) {
                        HAL_MMC_DeInit(&hdl->handle.mmc);

                        err = HAL_MMC_Init(&hdl->handle.mmc);
                        if (!err) {

                                const char *mode = "?";
                                switch (hdl->mode) {
                                case _SDMMC_MODE_POLLING: mode = "polling"; break;
                                case _SDMMC_MODE_IRQ: mode = "IRQ"; break;
                                case _SDMMC_MODE_DMA: mode = "DMA"; break;
                                }
                                dev_dbg(hdl, "using %s mode", mode);

                                if (SDMMC[hdl->major].bus_wide != SDMMC_BUS_WIDE_1B) {
                                        u8_t bits = SDMMC[hdl->major].bus_wide == SDMMC_BUS_WIDE_4B ? 4 : 8;

                                        if (HAL_MMC_ConfigWideBusOperation(&hdl->handle.mmc, SDMMC[hdl->major].bus_wide) == HAL_OK) {
                                                dev_dbg(hdl, "switched to %u-bit bus", bits);
                                        } else {
                                                dev_dbg(hdl, "%u-bit bus not supported", bits);
                                        }
                                }

                                if (SDMMC[hdl->major].speed_mode != SDMMC_SPEED_MODE_DEFAULT) {

                                        const char *speed_mode = "?";
                                        switch (SDMMC[hdl->major].speed_mode) {
                                        case SDMMC_SPEED_MODE_DEFAULT: speed_mode = "default"; break;
                                        case SDMMC_SPEED_MODE_AUTO: speed_mode = "auto"; break;
                                        case SDMMC_SPEED_MODE_HIGH: speed_mode = "high"; break;
                                        case SDMMC_SPEED_MODE_DDR: speed_mode = "DDR"; break;
                                        }

                                        err = HAL_MMC_ConfigSpeedBusOperation(&hdl->handle.mmc, SDMMC[hdl->major].speed_mode);
                                        if (not err) {
                                                dev_dbg(hdl, "switched to %s speed mode", speed_mode);
                                        } else {
                                                dev_dbg(hdl, "%s speed mode error: %d", speed_mode, err);
                                                HAL_MMC_ConfigSpeedBusOperation(&hdl->handle.mmc, SDMMC_SPEED_MODE_DEFAULT);
                                        }
                                }

                                HAL_MMC_CardInfoTypeDef info;
                                err = HAL_MMC_GetCardInfo(&hdl->handle.mmc, &info);
                                if (!err) {

                                        dev_dbg(hdl, "found %s capacity MMC card",
                                                info.CardType == MMC_LOW_CAPACITY_CARD ? "low" : "high");

                                        dev_dbg(hdl, "%d %u-byte logical blocks",
                                                info.BlockNbr, info.BlockSize);

                                        hdl->card_size = cast(u64_t, info.BlockSize)
                                                       * cast(u64_t, info.BlockNbr);
                                }
                        }

                        if (err) {
                                HAL_MMC_DeInit(&hdl->handle.mmc);
                        } else {
                                break;
                        }
                }
                #endif
        }

        return err;
}

//==============================================================================
/**
 * @brief  Wait for card to be ready.
 *
 * @param  hdl          device handle
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int card_wait_ready(SDMMC_t *hdl)
{
        size_t timeout = SDMMC[hdl->major].card_timeout / 10;

        if (SDMMC[hdl->major].card_type == _SDMMC_TYPE_SDCARD) {
                #if IS_SD_USING
                HAL_SD_CardStateTypeDef state;
                while ((state = HAL_SD_GetCardState(&hdl->handle.sd)) != HAL_SD_CARD_TRANSFER) {

                        sys_sleep_ms(1);

                        if (--timeout == 0) {
                                return ETIME;
                        }
                }
                #else
                return EFAULT;
                #endif

        } else {
                #if IS_MMC_USING
                HAL_MMC_CardStateTypeDef state;
                while ((state = HAL_MMC_GetCardState(&hdl->handle.mmc)) != HAL_MMC_CARD_TRANSFER) {

                        sys_sleep_ms(1);

                        if (--timeout == 0) {
                                return ETIME;
                        }
                }
                #else
                return EFAULT;
                #endif
        }

        return 0;
}

#if IS_SD_USING
//==============================================================================
/**
 * @brief  SD Rx complete callback.
 *
 * @param  hsd          SD handle
 */
//==============================================================================
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
        SDMMC_t *hdl = hsd->CtxPtr;
        sys_semaphore_signal_from_ISR(hdl->sem_xfer_complete, &hdl->irq_yield);
}

//==============================================================================
/**
 * @brief  SD Tx complete callback.
 *
 * @param  hsd          SD handle
 */
//==============================================================================
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
        SDMMC_t *hdl = hsd->CtxPtr;
        sys_semaphore_signal_from_ISR(hdl->sem_xfer_complete, &hdl->irq_yield);
}
#endif

#if IS_MMC_USING
//==============================================================================
/**
 * @brief  MMC Rx complete callback.
 *
 * @param  hsd          SD handle
 */
//==============================================================================
void HAL_MMC_RxCpltCallback(MMC_HandleTypeDef *hsd)
{
        SDMMC_t *hdl = hsd->CtxPtr;
        sys_semaphore_signal_from_ISR(hdl->sem_xfer_complete, &hdl->irq_yield);
}

//==============================================================================
/**
 * @brief  MMC Tx complete callback.
 *
 * @param  hsd          SD handle
 */
//==============================================================================
void HAL_MMC_TxCpltCallback(MMC_HandleTypeDef *hsd)
{
        SDMMC_t *hdl = hsd->CtxPtr;
        sys_semaphore_signal_from_ISR(hdl->sem_xfer_complete, &hdl->irq_yield);
}
#endif

//==============================================================================
/**
 * @brief  SDMMC1 IRQ handler.
 */
//==============================================================================
#if defined(SDMMC1) && (__SDMMC_CFG_SDMMC1_MODE__ != _SDMMC_MODE_POLLING)
void SDMMC1_IRQHandler(void)
{
        if (sdmmc[SDMMC_1]) {
                if (SDMMC[SDMMC_1].card_type == _SDMMC_TYPE_SDCARD) {
                        #if IS_SD_USING
                        HAL_SD_IRQHandler(&sdmmc[SDMMC_1]->handle.sd);
                        #endif
                } else {
                        #if IS_MMC_USING
                        HAL_MMC_IRQHandler(&sdmmc[SDMMC_1]->handle.mmc);
                        #endif
                }
        }

        sys_thread_yield_from_ISR(&sdmmc[SDMMC_1]->irq_yield);
}
#endif

//==============================================================================
/**
 * @brief  SDMMC2 IRQ handler.
 */
//==============================================================================
#if defined(SDMMC2) && (__SDMMC_CFG_SDMMC2_MODE__ != _SDMMC_MODE_POLLING)
void SDMMC2_IRQHandler(void)
{
        if (sdmmc[SDMMC_2]) {
                if (SDMMC[SDMMC_2].card_type == _SDMMC_TYPE_SDCARD) {
                        #if IS_SD_USING
                        HAL_SD_IRQHandler(&sdmmc[SDMMC_2]->handle.sd);
                        #endif
                } else {
                        #if IS_MMC_USING
                        HAL_MMC_IRQHandler(&sdmmc[SDMMC_2]->handle.mmc);
                        #endif
                }
        }

        sys_thread_yield_from_ISR(&sdmmc[SDMMC_2]->irq_yield);
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
