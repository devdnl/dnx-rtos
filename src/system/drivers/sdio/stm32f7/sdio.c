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
#define MTX_TIMEOUT                     2500

#define _SDIO_MODE_POLLING              0
#define _SDIO_MODE_IRQ                  1
#define _SDIO_MODE_DMA                  2

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
        u32_t mode;
        u8_t major;
        u8_t minor;
        SD_HandleTypeDef hsd;
        u64_t card_size;
        kmtx_t *mtx;
        ksem_t *sem_xfer_complete;
        bool irq_yield;
} SDIO_t;

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
        u8_t           DMA_channel;
        u8_t           DMA_major;
        u8_t           DMA_stream_pri;
        u8_t           DMA_stream_alt;
} sd_info_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int card_initialize(SDIO_t *hdl);
static int card_wait_ready(SDIO_t *hdl);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(SDIO);

/*
 * For IRQ purpose only.
 */
static SDIO_t *sdio[SD_COUNT];

/*
 * SDMMCx configuration.
 */
static const sd_info_t SDMMC[] = {
        #if defined(SDMMC1)
        {
                .instance = SDMMC1,
                .RCC_APBENR = &RCC->APB2ENR,
                .RCC_APBENR_SDMMCEN = RCC_APB2ENR_SDMMC1EN,
                .IRQn = SDMMC1_IRQn,
                .IRQ_priority = __SDIO_CFG_SDMMC1_IRQ_PRIORITY__,
                .card_timeout = __SDIO_CFG_SDMMC1_CARD_TIMEOUT__,
                .clock_edge = __SDIO_CFG_SDMMC1_CLOCK_EDGE__,
                .clock_power_save = __SDIO_CFG_SDMMC1_PWRSAVE__,
                .clock_div = __SDIO_CFG_SDMMC1_CKDIV__ - 1,
                .bus_wide = __SDIO_CFG_SDMMC1_BUS_WIDE__,
                .hardware_flow_ctrl = __SDIO_CFG_SDMMC1_HW_FLOW_CTRL__,
                .mode = __SDIO_CFG_SDMMC1_MODE__,
                .DMA_channel = 4,
                .DMA_major = 1,
                .DMA_stream_pri = 3,
                .DMA_stream_alt = 6,
        },
        #endif
        #if defined(SDMMC2)
        {
                .instance = SDMMC2,
                .RCC_APBENR = &RCC->APB2ENR,
                .RCC_APBENR_SDMMCEN = RCC_APB2ENR_SDMMC2EN,
                .IRQn = SDMMC2_IRQn,
                .IRQ_priority = __SDIO_CFG_SDMMC2_IRQ_PRIORITY__,
                .card_timeout = __SDIO_CFG_SDMMC2_CARD_TIMEOUT__,
                .clock_edge = __SDIO_CFG_SDMMC2_CLOCK_EDGE__,
                .clock_power_save = __SDIO_CFG_SDMMC2_PWRSAVE__,
                .clock_div = __SDIO_CFG_SDMMC2_CKDIV__ - 1,
                .bus_wide = __SDIO_CFG_SDMMC2_BUS_WIDE__,
                .hardware_flow_ctrl = __SDIO_CFG_SDMMC2_HW_FLOW_CTRL__,
                .mode = __SDIO_CFG_SDMMC2_MODE__,
                .DMA_channel = 11,
                .DMA_major = 1,
                .DMA_stream_pri = 0,
                .DMA_stream_alt = 5,
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

                        err = sys_semaphore_create(1, 0, &hdl->sem_xfer_complete);
                        if (!err) {

                                err = sys_mutex_create(KMTX_TYPE_RECURSIVE, &hdl->mtx);
                                if (!err) {
                                        err = card_initialize(hdl);
                                        if (!err) {
                                                sdio[hdl->major] = hdl;
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
API_MOD_RELEASE(SDIO, void *device_handle)
{
        SDIO_t *hdl = device_handle;

        kmtx_t *mtx = hdl->mtx;

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
                if (((*fpos & (BLOCKSIZE - 1)) == 0) and ((count & (BLOCKSIZE - 1)) == 0)) {

                        u32_t block_address = *fpos / BLOCKSIZE;
                        u32_t blocks        = count / BLOCKSIZE;

                        for (int i = 0; i < 5; i++) {

                                if (hdl->mode == _SDIO_MODE_POLLING) {

                                        err = HAL_SD_WriteBlocks(&hdl->hsd, src, block_address, blocks,
                                                                 SDMMC[hdl->major].card_timeout);

                                } else if (hdl->mode == _SDIO_MODE_IRQ) {

                                        err = HAL_SD_WriteBlocks_IT(&hdl->hsd, src, block_address, blocks);
                                        if (!err) {
                                                err = sys_semaphore_wait(hdl->sem_xfer_complete,
                                                                         SDMMC[hdl->major].card_timeout);
                                        }

                                } else if (hdl->mode == _SDIO_MODE_DMA) {

                                        err = HAL_SD_WriteBlocks_DMA(&hdl->hsd, src, block_address, blocks);
                                        if (!err) {
                                                err = sys_semaphore_wait(hdl->sem_xfer_complete,
                                                                         SDMMC[hdl->major].card_timeout);
                                        }

                                } else {
                                        err = EFAULT;
                                        dev_dbg(hdl, "unknown card mode!", 0);
                                }

                                if (!err) {
                                        err = card_wait_ready(hdl);
                                        if (!err) {
                                                *wrcnt += count;
                                        } else {
                                                dev_dbg(hdl, "card busy at write transfer", 0);
                                        }
                                }

                                if (!err) {
                                        break;
                                } else {
                                        dev_dbg(hdl, "write error %d @ block %u", err, block_address);
                                        sys_sleep_ms(50);
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

        int err = sys_mutex_lock(hdl->mtx, MTX_TIMEOUT);
        if (!err) {
                if (((*fpos & (BLOCKSIZE - 1)) == 0) and ((count & (BLOCKSIZE - 1)) == 0)) {

                        u32_t block_address = *fpos / BLOCKSIZE;
                        u32_t blocks        = count / BLOCKSIZE;

                        for (int i = 0; i < 5; i++) {

                                if (hdl->mode == _SDIO_MODE_POLLING) {

                                        err = HAL_SD_ReadBlocks(&hdl->hsd, dst, block_address, blocks,
                                                                SDMMC[hdl->major].card_timeout);

                                } else if (hdl->mode == _SDIO_MODE_IRQ) {

                                        err = HAL_SD_ReadBlocks_IT(&hdl->hsd, dst, block_address, blocks);
                                        if (!err) {
                                                err = sys_semaphore_wait(hdl->sem_xfer_complete,
                                                                         SDMMC[hdl->major].card_timeout);
                                        }

                                } else if (hdl->mode == _SDIO_MODE_DMA) {

                                        err = HAL_SD_ReadBlocks_DMA(&hdl->hsd, dst, block_address, blocks);
                                        if (!err) {
                                                err = sys_semaphore_wait(hdl->sem_xfer_complete,
                                                                         SDMMC[hdl->major].card_timeout);
                                        }

                                } else {
                                        err = EFAULT;
                                        dev_dbg(hdl, "unknown card mode!", 0);
                                }

                                if (!err) {
                                        err = card_wait_ready(hdl);
                                        if (!err) {
                                                *rdcnt += count;
                                        } else {
                                                dev_dbg(hdl, "card busy at read transfer", 0);
                                        }
                                }

                                if (!err) {
                                        break;
                                } else {
                                        dev_dbg(hdl, "read error %d @ block %u", err, block_address);
                                        sys_sleep_ms(50);
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
API_MOD_IOCTL(SDIO, void *device_handle, int request, void *arg)
{
        UNUSED_ARG1(arg);

        SDIO_t *hdl = device_handle;

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

        if (hdl->mode != _SDIO_MODE_POLLING) {
                NVIC_SetPriority(SDMMC[major].IRQn, SDMMC[major].IRQ_priority);
                NVIC_ClearPendingIRQ(SDMMC[major].IRQn);
                NVIC_EnableIRQ(SDMMC[major].IRQn);
        }
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
        int err = EFAULT;

        hdl->hsd.CtxPtr = hdl;

        hdl->mode = SDMMC[hdl->major].mode;

        if (SDMMC[hdl->major].mode == _SDIO_MODE_DMA) {
//                DMA_HandleTypeDef *dmah;
//                err = sys_zalloc(sizeof(*dmah), cast(void**, &dmah));
//                if (!err) {
//                        dmah->dmad = _DMA_DDI_reserve(SDMMC[hdl->major].DMA_major,
//                                                      SDMMC[hdl->major].DMA_stream_pri);
//                        if (!dmah->dmad) {
//                                dmah->dmad = _DMA_DDI_reserve(SDMMC[hdl->major].DMA_major,
//                                                              SDMMC[hdl->major].DMA_stream_alt);
//                        }
//
//                        if (!dmah->dmad) {
//                                sys_free(cast(void**, &dmah));
//                                err = EFAULT;
//                        }
//                }
//
//                if (!err) {
//                        dmah->channel      = SDMMC[hdl->major].DMA_channel;
//                        dmah->irq_priority = SDMMC[hdl->major].IRQ_priority;
//                        dmah->parent       = &hdl->hsd;
//
//                        hdl->mode = _SDIO_MODE_DMA;
//                        hdl->hsd.hdma = dmah;
//
//                } else {
                        dev_dbg(hdl, "DMA not accessible, using IRQ mode", 0);
                        hdl->mode = _SDIO_MODE_IRQ;
                        err = 0;
//                }
        }

        hdl->hsd.Instance = SDMMC[hdl->major].instance;
        hdl->hsd.Init.ClockEdge = SDMMC[hdl->major].clock_edge;
        hdl->hsd.Init.ClockBypass = SDMMC_CLOCK_BYPASS_DISABLE;
        hdl->hsd.Init.ClockPowerSave = SDMMC[hdl->major].clock_power_save;
        hdl->hsd.Init.BusWide = SDMMC_BUS_WIDE_1B;
        hdl->hsd.Init.HardwareFlowControl = SDMMC[hdl->major].hardware_flow_ctrl;
        hdl->hsd.Init.ClockDiv = SDMMC[hdl->major].clock_div;

        for (int i = 0; i < 3; i++) {
                HAL_SD_DeInit(&hdl->hsd);

                err = HAL_SD_Init(&hdl->hsd);
                if (!err) {

                        const char *mode = "?";
                        switch (hdl->mode) {
                        case _SDIO_MODE_POLLING:
                                mode = "polling";
                                break;
                        case _SDIO_MODE_IRQ:
                                mode = "IRQ";
                                break;
                        case _SDIO_MODE_DMA:
                                mode = "DMA";
                                break;
                        }
                        dev_dbg(hdl, "using %s mode", mode);

                        if (SDMMC[hdl->major].bus_wide != SDMMC_BUS_WIDE_1B) {
                                if (HAL_SD_ConfigWideBusOperation(&hdl->hsd, SDMMC_BUS_WIDE_4B) == SD_HAL_OK) {
                                        dev_dbg(hdl, "switched to 4-bit bus", 0);
                                } else {
                                        dev_dbg(hdl, "4-bit bus not supported", 0);
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
                        }
                }

                if (err) {
                        HAL_SD_DeInit(&hdl->hsd);
                } else {
                        break;
                }
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
static int card_wait_ready(SDIO_t *hdl)
{
        int err = 0;

        size_t timeout = SDMMC[hdl->major].card_timeout / 10;

        HAL_SD_CardStateTypeDef state;
        while ((state = HAL_SD_GetCardState(&hdl->hsd)) != HAL_SD_CARD_TRANSFER) {

                sys_sleep_ms(1);

                if (--timeout == 0) {
                        err = ETIME;
                        break;
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
        SDIO_t *hdl = hsd->CtxPtr;
        sys_semaphore_signal_from_ISR(hdl->sem_xfer_complete, &hdl->irq_yield);
}

//==============================================================================
/**
 * @brief  Tx complete callback.
 *
 * @param  hsd          SD handle
 */
//==============================================================================
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
        SDIO_t *hdl = hsd->CtxPtr;
        sys_semaphore_signal_from_ISR(hdl->sem_xfer_complete, &hdl->irq_yield);
}

//==============================================================================
/**
 * @brief  SDMMC1 IRQ handler.
 */
//==============================================================================
#if defined(SDMMC1) && (__SDIO_CFG_SDMMC1_MODE__ != _SDIO_MODE_POLLING)
void SDMMC1_IRQHandler(void)
{
        if (sdio[SD1]) {
                HAL_SD_IRQHandler(&sdio[SD1]->hsd);
        }

        sys_thread_yield_from_ISR(&sdio[SD1]->irq_yield);
}
#endif

//==============================================================================
/**
 * @brief  SDMMC2 IRQ handler.
 */
//==============================================================================
#if defined(SDMMC2) && (__SDIO_CFG_SDMMC2_MODE__ != _SDIO_MODE_POLLING)
void SDMMC2_IRQHandler(void)
{
        if (sdio[SD2]) {
                HAL_SD_IRQHandler(&sdio[SD2]->hsd);
        }

        sys_thread_yield_from_ISR(&sdio[SD2]->irq_yield);
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
