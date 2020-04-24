/*==============================================================================
File    snd.c

Author  Daniel Zorychta

Brief   Sound interface driver

        Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "stm32f4/snd_cfg.h"
#include "../snd_ioctl.h"
#include "stm32f4/stm32f4xx.h"
#include "stm32f4/dma_ddi.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define DMA_CHANNEL                     0
#define DMA_STREAM_PRI                  1
#define DMA_STREAM_ALT                  3

/*==============================================================================
  Local object types
==============================================================================*/
enum {
        #if defined(RCC_APB2ENR_SAI1EN)
        _SAI1,
        #endif
        #if defined(RCC_APB1ENR_SAI2EN)
        _SAI2,
        #endif
        SAI_PERIPHERALS,
        SAI_BLOCKS = 2,
};

/* SAI peripheral info */
struct SAI_info {
        SAI_TypeDef       *const BASE;                  //!< SAI peripheral address
        SAI_Block_TypeDef *const BLOCK[SAI_BLOCKS];     //!< SAI BLOCK address
        __IO u32_t        *APBRSTR;                     //!< APB reset register address
        __IO u32_t        *APBENR;                      //!< APB enable register
        u32_t              APBRSTRENR;                  //!< APB reset/enable bit
        u8_t               DMA_major[SAI_BLOCKS];       //!< DMA peripheral number
        u8_t               DMA_channel_pri[SAI_BLOCKS]; //!< DMA peripheral request channel number
        u8_t               DMA_channel_alt[SAI_BLOCKS]; //!< DMA peripheral request channel number
        u8_t               DMA_stream_pri[SAI_BLOCKS];  //!< primary stream number
        u8_t               DMA_stream_alt[SAI_BLOCKS];  //!< alternative stream number
        u32_t              CR1_CONF[SAI_BLOCKS];
        u32_t              CR2_CONF[SAI_BLOCKS];
        u32_t              FRCR_CONF[SAI_BLOCKS];
        u32_t              SLOTR_CONF[SAI_BLOCKS];
};

typedef struct {
        const u8_t *ptr;
        size_t size;
} buf_t;

typedef struct {
        const struct SAI_info *SAI;
        sem_t       *sem;
        u32_t        dmad;
        SND_config_t conf;
        u8_t         major;
        u8_t         minor;
        u8_t         DMA_channel;
        bool         configured;
        bool         active;
        buf_t        next;
} SND_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int  configure(SND_t *hdl, const SND_config_t *config);
static int  DMA_start(SND_t *hdl, const buf_t *buf);
static bool DMA_next(DMA_Stream_TypeDef *stream, u8_t SR, void *arg);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(SND);

static const struct SAI_info SAI_INFO[SAI_PERIPHERALS] = {
#if defined(RCC_APB2ENR_SAI1EN)
        {
                .BASE            = SAI1,
                .BLOCK           = {SAI1_Block_A, SAI1_Block_B},
                .APBRSTR         = &RCC->APB2RSTR,
                .APBENR          = &RCC->APB2ENR,
                .APBRSTRENR      = RCC_APB2ENR_SAI1EN,
                .DMA_major       = {1, 1},
                .DMA_channel_pri = {0, 0},
                .DMA_channel_alt = {0, 1},
                .DMA_stream_pri  = {1, 5},
                .DMA_stream_alt  = {3, 4},
                .CR1_CONF        = {_SND_CFG_SAI1_BLKA_CR1, _SND_CFG_SAI1_BLKB_CR1},
                .CR2_CONF        = {_SND_CFG_SAI1_BLKA_CR2, _SND_CFG_SAI1_BLKB_CR2},
                .FRCR_CONF       = {_SND_CFG_SAI1_BLKA_FRCR, _SND_CFG_SAI1_BLKB_FRCR},
                .SLOTR_CONF      = {_SND_CFG_SAI1_BLKA_SLOTR, _SND_CFG_SAI1_BLKB_SLOTR},
        },
#endif
#if defined(RCC_APB2ENR_SAI2EN)
        {
                .BASE            = SAI2,
                .BLOCK           = {SAI2_Block_A, SAI2_Block_B},
                .APBRSTR         = &RCC->APB2RSTR,
                .APBENR          = &RCC->APB2ENR,
                .APBRSTRENR      = RCC_APB2ENR_SAI2EN,
                .DMA_major       = {1, 1},
                .DMA_channel_pri = {3, 0},
                .DMA_channel_alt = {3, 3},
                .DMA_stream_pri  = {4, 7},
                .DMA_stream_alt  = {4, 6},
                .CR1_CONF        = {_SND_CFG_SAI2_BLKA_CR1, _SND_CFG_SAI2_BLKB_CR1},
                .CR2_CONF        = {_SND_CFG_SAI2_BLKA_CR2, _SND_CFG_SAI2_BLKB_CR2},
                .FRCR_CONF       = {_SND_CFG_SAI2_BLKA_FRCR, _SND_CFG_SAI2_BLKB_FRCR},
                .SLOTR_CONF      = {_SND_CFG_SAI2_BLKA_SLOTR, _SND_CFG_SAI2_BLKB_SLOTR},
        },
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
API_MOD_INIT(SND, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(config);

        int err = EFAULT;

        if (major < SAI_PERIPHERALS && minor < SAI_BLOCKS) {

                err = sys_zalloc(sizeof(SND_t), device_handle);
                if (!err) {

                        SND_t *hdl = *device_handle;
                        hdl->major = major;
                        hdl->minor = minor;
                        hdl->SAI   = &SAI_INFO[major];

                        SET_BIT(*hdl->SAI->APBENR, hdl->SAI->APBRSTRENR);
                        SET_BIT(*hdl->SAI->APBRSTR, hdl->SAI->APBRSTRENR);
                        CLEAR_BIT(*hdl->SAI->APBRSTR, hdl->SAI->APBRSTRENR);

                        hdl->DMA_channel = hdl->SAI->DMA_channel_pri[minor];
                        hdl->dmad = _DMA_DDI_reserve(hdl->SAI->DMA_major[minor],
                                                     hdl->SAI->DMA_stream_pri[minor]);

                        if (!hdl->dmad) {
                                hdl->DMA_channel = hdl->SAI->DMA_channel_alt[minor];
                                hdl->dmad = _DMA_DDI_reserve(hdl->SAI->DMA_major[minor],
                                                             hdl->SAI->DMA_stream_alt[minor]);
                        }

                        if (!hdl->dmad) {
                                printk("SND: could not reserve DMA stream");
                                err = EIO;
                        }

                        if (!err) {
                                err = sys_semaphore_create(1, 0, &hdl->sem);
                        }

                        // release resources on error
                        if (err) {
                                if (hdl->dmad) {
                                        _DMA_DDI_release(hdl->dmad);
                                }

                                if (hdl->sem) {
                                        sys_semaphore_destroy(hdl->sem);
                                }

                                sys_free(*device_handle);
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
API_MOD_RELEASE(SND, void *device_handle)
{
        SND_t *hdl = device_handle;

        _DMA_DDI_release(hdl->dmad);
        sys_semaphore_destroy(hdl->sem);

        SET_BIT(*hdl->SAI->APBRSTR, hdl->SAI->APBRSTRENR);
        CLEAR_BIT(*hdl->SAI->APBRSTR, hdl->SAI->APBRSTRENR);
        CLEAR_BIT(*hdl->SAI->APBENR, hdl->SAI->APBRSTRENR);

        return sys_free(&device_handle);
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
API_MOD_OPEN(SND, void *device_handle, u32_t flags)
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
API_MOD_CLOSE(SND, void *device_handle, bool force)
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
API_MOD_WRITE(SND,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fattr);

        SND_t *hdl = device_handle;

        int err = (not hdl->configured) ? EIO : ESUCC;

        if (!err) {
                buf_t buf;
                buf.ptr  = src;
                buf.size = count;

                if (not hdl->active) {
                        err = DMA_start(hdl, &buf);

                } else {
                        hdl->next.ptr  = src;
                        hdl->next.size = count;
                        err = sys_semaphore_wait(hdl->sem, MAX_DELAY_MS);
                }
        }

        *fpos  = 0;
        *wrcnt = !err ? count : 0;

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
API_MOD_READ(SND,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG6(device_handle, dst, count, fpos, rdcnt, fattr);

        int err = ENOTSUP;

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
API_MOD_IOCTL(SND, void *device_handle, int request, void *arg)
{
        SND_t *hdl = device_handle;

        int err = EBADRQC;

        switch (request) {
        case IOCTL_SND__SET_CONFIGURATION: {
                const SND_config_t *conf = cast(const SND_config_t*, arg);
                if (conf) {
                        err = configure(hdl, conf);
                } else {
                        err = EINVAL;
                }
                break;
        }

        default:
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
API_MOD_FLUSH(SND, void *device_handle)
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
API_MOD_STAT(SND, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);

        device_stat->st_size = 0;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  Function configure SAI peripheral.
 *
 * @param  hdl          driver instance
 * @param  config       configuration
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int configure(SND_t *hdl, const SND_config_t *config)
{
        if (  (config->bits_per_sample > _SND_BITS_PER_SAMPLE_AMOUNT)
           or (config->channels > _SND_CHANNELS_AMOUNT)
           or (config->sample_rate > _SND_SAMPLE_RATE_AMOUNT) ) {

                hdl->configured = false;
                return EINVAL;
        }

        static u8_t MCKDIV[_SND_SAMPLE_RATE_AMOUNT] = {
                [SND_SAMPLE_RATE__11025] = 2,
                [SND_SAMPLE_RATE__22050] = 1,
                [SND_SAMPLE_RATE__44100] = 0,
        };

        u8_t minor = hdl->minor;
        SAI_Block_TypeDef *SAI_BLK = hdl->SAI->BLOCK[minor];

        hdl->conf = *config;

        int err = ESUCC;

        u32_t mckdiv = 0;
        if (config->sample_rate < _SND_SAMPLE_RATE_AMOUNT) {
                mckdiv = MCKDIV[config->sample_rate];
        } else {
                printk("SND: set default frequency to 44100 Hz");
        }

        u32_t mono = 0;
        if (config->channels == SND_CHANNELS__MONO) {
                mono = SAI_xCR1_MONO;
        }


        SET_BIT(*hdl->SAI->APBRSTR, hdl->SAI->APBRSTRENR);
        CLEAR_BIT(*hdl->SAI->APBRSTR, hdl->SAI->APBRSTRENR);

        hdl->SAI->BASE->GCR = 0;

        SAI_BLK->CR1 = ((mckdiv << SAI_xCR1_MCKDIV_Pos) & SAI_xCR1_MCKDIV_Msk)
                       | (1 * SAI_xCR1_DMAEN)
                       | mono
                       | hdl->SAI->CR1_CONF[minor];

        SAI_BLK->CR2 = ((3 << SAI_xCR2_FTH_Pos ) & SAI_xCR2_FTH_Msk)
                       | hdl->SAI->CR2_CONF[minor];

        SAI_BLK->FRCR = hdl->SAI->FRCR_CONF[minor];

        SAI_BLK->SLOTR = hdl->SAI->SLOTR_CONF[minor];

        SET_BIT(SAI_BLK->CR1, SAI_xCR1_SAIEN);

        hdl->configured = (err == 0);

        return err;
}

//==============================================================================
/**
 * @brief
 *
 * @param  ?
 *
 * @return ?
 */
//==============================================================================
static int DMA_start(SND_t *hdl, const buf_t *buf)
{
        const u32_t PSIZE[_SND_BITS_PER_SAMPLE_AMOUNT] = {
                [SND_BITS_PER_SAMPLE__8]  = DMA_SxCR_PSIZE_BYTE,
                [SND_BITS_PER_SAMPLE__16] = DMA_SxCR_PSIZE_HALFWORD,
                [SND_BITS_PER_SAMPLE__32] = DMA_SxCR_PSIZE_WORD,
        };

        const u32_t WLEN[_SND_BITS_PER_SAMPLE_AMOUNT] = {
                [SND_BITS_PER_SAMPLE__8]  = 1,
                [SND_BITS_PER_SAMPLE__16] = 2,
                [SND_BITS_PER_SAMPLE__32] = 4,
        };

        _DMA_DDI_config_t config;
        config.callback = NULL;
        config.cb_next  = DMA_next;
        config.arg      = hdl;
        config.release  = false;
        config.PA       = cast(u32_t, &hdl->SAI->BLOCK[hdl->minor]->DR);
        config.MA[0]    = cast(u32_t, buf->ptr);
        config.MA[1]    = 0;
        config.NDT      = buf->size / WLEN[hdl->conf.bits_per_sample];
        config.CR       = DMA_SxCR_DIR_M2P
                        | DMA_SxCR_CHSEL_SEL(hdl->DMA_channel)
                        | DMA_SxCR_MBURST_SINGLE
                        | DMA_SxCR_PBURST_SINGLE
                        | DMA_SxCR_DBM_DISABLE
                        | DMA_SxCR_PL_MEDIUM
                        | DMA_SxCR_PINCOS_PSIZE
                        | DMA_SxCR_PINC_FIXED
                        | PSIZE[hdl->conf.bits_per_sample]
                        | DMA_SxCR_MINC_ENABLE
                        | DMA_SxCR_MSIZE_BYTE
                        | DMA_SxCR_CIRC_DISABLE
                        | DMA_SxCR_PFCTRL_DMA;
        config.FC       = DMA_SxFCR_FEIE_DISABLE
                        | DMA_SxFCR_DMDIS_YES
                        | DMA_SxFCR_FTH_FULL;
        config.IRQ_priority = __CPU_DEFAULT_IRQ_PRIORITY__;

        int err = _DMA_DDI_transfer(hdl->dmad, &config);
        if (!err) {
                hdl->active = true;
        }

        return err;
}

//==============================================================================
/**
 * @brief DMA next callback.
 *
 * @param stream        DMA stream
 * @param SR            status register
 * @param arg           argument
 *
 * @return Return true if IRQ should yield, false otherwise.
 */
//==============================================================================
static bool DMA_next(DMA_Stream_TypeDef *stream, u8_t SR, void *arg)
{
        UNUSED_ARG2(stream, SR);

        SND_t *hdl = arg;

        bool yield = false;

        if (hdl->next.ptr && hdl->next.size) {

                if (DMA_start(hdl, &hdl->next) != 0) {
                        hdl->active = false;
                }

                hdl->next.ptr  = NULL;
                hdl->next.size = 0;

                sys_semaphore_signal_from_ISR(hdl->sem, &yield);

        } else {
                hdl->active = false;
        }

        return yield;
}

/*==============================================================================
  End of file
==============================================================================*/
