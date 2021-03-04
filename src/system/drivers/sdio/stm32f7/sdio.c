/*==============================================================================
File    sdio.c

Author  Daniel Zorychta

Brief   SD Card Interface Driver.

        Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "stm32f7/dma_ddi.h"
#include "sys/ioctl.h"
#include "../sdio_ioctl.h"
#include "stm32f7xx_hal_sd.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define MTX_TIMEOUT     2500

/*==============================================================================
  Local object types
==============================================================================*/
enum {
        #if defined(SDMMC1)
        SD1,
        #endif
        #if defined(SDMMC2)
        SD2,
        #endif
        SD_COUNT
};

/** driver instance associated with partition */
typedef struct {
        u8_t major;
        u8_t minor;
        SD_HandleTypeDef hsd;
        u64_t card_size;
        mutex_t *mtx;
} SDIO_t;

typedef struct {
        SDMMC_TypeDef *instance;
        __IO u32_t    *RCC_APBENR;
        u32_t          RCC_APBENR_SDMMCEN;
        IRQn_Type      IRQn;
        u32_t          IRQ_priority;
} sd_info_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int card_initialize(SDIO_t *hdl);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(SDIO);

static SDIO_t *sdio[SD_COUNT];

static const sd_info_t SDMMC[] = {
        #if defined(SDMMC1)
        {
                .instance = SDMMC1,
                .RCC_APBENR = &RCC->APB2ENR,
                .RCC_APBENR_SDMMCEN = RCC_APB2ENR_SDMMC1EN,
                .IRQn = SDMMC1_IRQn,
                .IRQ_priority = __SDIO_IRQ_PRIORITY__
        },
        #endif
        #if defined(SDMMC2)
        {
                .instance = SDMMC2,
                .RCC_APBENR = &RCC->APB2ENR,
                .RCC_APBENR_SDMMCEN = RCC_APB2ENR_SDMMC2EN,
                .IRQn = SDMMC2_IRQn,
                .IRQ_priority = __SDIO_IRQ_PRIORITY__
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
API_MOD_INIT(SDIO, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(config);

        int err = ENODEV;

        if ((major < SD_COUNT) && (minor == 0)) {
                err = sys_zalloc(sizeof(SDIO_t), device_handle);
                if (!err) {
                        SDIO_t *hdl = *device_handle;
                        hdl->major = major;
                        hdl->minor = minor;

                        err = sys_mutex_create(MUTEX_TYPE_RECURSIVE, &hdl->mtx);
                        if (!err) {
                                err = card_initialize(hdl);
                                if (!err) {
                                        sdio[hdl->major] = hdl;
                                        return err;
                                }

                                sys_mutex_destroy(hdl->mtx);
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
API_MOD_RELEASE(SDIO, void *device_handle)
{
        SDIO_t *hdl = device_handle;

        mutex_t *mtx = hdl->mtx;

        int err = sys_mutex_lock(mtx, MTX_TIMEOUT);
        if (!err) {
                hdl->mtx = NULL;
                sys_sleep_ms(MTX_TIMEOUT + 500);
                HAL_SD_DeInit(&hdl->hsd);
                sys_mutex_unlock(mtx);
                sys_mutex_destroy(hdl->mtx);
                sys_free(device_handle);
                sdio[hdl->major] = NULL;
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
API_MOD_OPEN(SDIO, void *device_handle, u32_t flags)
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
API_MOD_CLOSE(SDIO, void *device_handle, bool force)
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
API_MOD_WRITE(SDIO,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fattr);

        SDIO_t *hdl = device_handle;

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {
                if (((*fpos & 0x1FF) == 0) and ((count & 0x1FF) == 0)) {

                        u32_t block_address = *fpos / 512;
                        u32_t blocks        = count / 512;

                        err = HAL_SD_WriteBlocks(&hdl->hsd, src, block_address, blocks, __SDIO_CFG_CARD_TIMEOUT__);
                        if (!err) {
                                *wrcnt += count;
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
API_MOD_READ(SDIO,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        SDIO_t *hdl  = device_handle;

        // rozwiązanie problemu z odbiorem DMA: Posted on August 29, 2017 at 15:11
        // https://community.st.com/s/question/0D50X00009XkeZ4SAJ/stm32l4-sdmmc-command-timeout

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {
                if (((*fpos & 0x1FF) == 0) and ((count & 0x1FF) == 0)) {

                        u32_t block_address = *fpos / 512;
                        u32_t blocks        = count / 512;

                        err = HAL_SD_ReadBlocks(&hdl->hsd, dst, block_address, blocks, __SDIO_CFG_CARD_TIMEOUT__);
                        if (!err) {
                                *rdcnt += count;
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
API_MOD_IOCTL(SDIO, void *device_handle, int request, void *arg)
{
        UNUSED_ARG1(arg);

        SDIO_t *hdl = device_handle;

        int err = EBADRQC;

        switch (request) {
        case IOCTL_SDIO__INITIALIZE_CARD: {
                err = sys_mutex_lock(hdl->mtx, MAX_DELAY_MS);
                if (!err) {
                        err = card_initialize(hdl);
                        sys_mutex_unlock(hdl->mtx);
                }
                break;
        }

        case IOCTL_SDIO__READ_MBR: {
//                err = sys_mutex_lock(hdl->ctrl->protect, MAX_DELAY_MS);
//                if (!err) {
//                        if (hdl->ctrl->initialized) {
//                                err = MBR_detect_partitions(hdl);
//                        } else {
//                                err = ENOMEDIUM;
//                        }
//                        sys_mutex_unlock(hdl->ctrl->protect);
//                }
                break;
        }

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
API_MOD_FLUSH(SDIO, void *device_handle)
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
API_MOD_STAT(SDIO, void *device_handle, struct vfs_dev_stat *device_stat)
{
        SDIO_t *hdl = device_handle;

        device_stat->st_size = hdl->card_size;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Basic interface initialization.
 *
 * @param  hsd          SD handle
 */
//==============================================================================
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
        SDIO_t *hdl = hsd->CtxPtr;

        u8_t major = 0;

        if (hsd->Instance == SDMMC1) {
                major = 0;
#ifdef SDMMC2
        } else if (hsd->Instance == SDMMC2) {
                major = 1;
#endif
        } else {
                dev_dbg(hdl, "init: unknown SDMMC instance", 0);
                return;
        }

        SET_BIT(*SDMMC[major].RCC_APBENR, SDMMC[major].RCC_APBENR_SDMMCEN);

        NVIC_SetPriority(SDMMC[major].IRQn, SDMMC[major].IRQ_priority);
        NVIC_ClearPendingIRQ(SDMMC[major].IRQn);
        NVIC_EnableIRQ(SDMMC[major].IRQn);
}

//==============================================================================
/**
 * @brief  Basic interface de-initialization.
 *
 * @param  hsd          SD handle
 */
//==============================================================================
void HAL_SD_MspDeInit(SD_HandleTypeDef *hsd)
{
        SDIO_t *hdl = hsd->CtxPtr;

        u8_t major = 0;

        if (hsd->Instance == SDMMC1) {
                major = 0;
#ifdef SDMMC2
        } else if (hsd->Instance == SDMMC2) {
                major = 1;
#endif
        } else {
                dev_dbg(hdl, "deinit: unknown SDMMC instance", 0);
                return;
        }

        NVIC_ClearPendingIRQ(SDMMC[major].IRQn);
        NVIC_DisableIRQ(SDMMC[major].IRQn);

        CLEAR_BIT(*SDMMC[major].RCC_APBENR, SDMMC[major].RCC_APBENR_SDMMCEN);
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
static int card_initialize(SDIO_t *hdl)
{
        hdl->hsd.CtxPtr = hdl;

        hdl->hsd.Instance = SDMMC[hdl->major].instance;
        hdl->hsd.Init.ClockEdge = __SDIO_CFG_CLOCK_EDGE__;
        hdl->hsd.Init.ClockBypass = SDMMC_CLOCK_BYPASS_DISABLE;
        hdl->hsd.Init.ClockPowerSave = __SDIO_CFG_PWRSAVE__;
        hdl->hsd.Init.BusWide = __SDIO_CFG_BUS_WIDE__;
        hdl->hsd.Init.HardwareFlowControl = __SDIO_CFG_HW_FLOW_CTRL__;
        hdl->hsd.Init.ClockDiv = __SDIO_CFG_CKDIV__ - 1;

        HAL_SD_DeInit(&hdl->hsd);

        int err = HAL_SD_Init(&hdl->hsd);
        if (!err) {

                if (__SDIO_CFG_BUS_WIDE__ != SDMMC_BUS_WIDE_1B) {
                        if (HAL_SD_ConfigWideBusOperation(&hdl->hsd, SDMMC_BUS_WIDE_4B) == SD_HAL_OK) {
                                dev_dbg(hdl, "switched to 4-bit mode", 0);
                        } else {
                                dev_dbg(hdl, "4-bit mode not supported", 0);
                        }
                }

                HAL_SD_CardInfoTypeDef info;
                err = HAL_SD_GetCardInfo(&hdl->hsd, &info);
                if (!err) {

                        dev_dbg(hdl, "found SD%s card",
                                info.CardType == CARD_SDSC ? "SC" : "HC/XC");

                        dev_dbg(hdl, "%d %u-byte logical blocks",
                               info.BlockNbr, info.BlockSize);

                        hdl->card_size = cast(u64_t, info.BlockSize)
                                       * cast(u64_t, info.BlockNbr);

                } else {
                        HAL_SD_DeInit(&hdl->hsd);
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Rx complete callback.
 *
 * @param  hsd          SD handle
 */
//==============================================================================
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
        UNUSED(hsd);
//        SDIO_t *hdl = hsd->CtxPtr;
}

//==============================================================================
/**
 * @brief  SDMMC1 IRQ handler.
 */
//==============================================================================
void SDMMC1_IRQHandler(void)
{
        HAL_SD_IRQHandler(&sdio[SD1]->hsd);
}

//==============================================================================
/**
 * @brief  SDMMC2 IRQ handler.
 */
//==============================================================================
#if defined(SDMMC2)
void SDMMC2_IRQHandler(void)
{
        HAL_SD_IRQHandler(&sdio[SD2]->hsd);
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
