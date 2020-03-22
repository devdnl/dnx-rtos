/*==============================================================================
File     dma.c

Author   Daniel Zorychta

Brief    General usage DMA driver.

         Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/driver.h"
#include "stm32f1/dma_ddi.h"
#include "stm32f1/stm32f10x.h"
#include "../dma_ioctl.h"
#include "kernel/khooks.h"

/*==============================================================================
  Local macros
==============================================================================*/
enum {
        _DMA1,
      #if defined(RCC_AHBENR_DMA2EN)
        _DMA2,
      #endif
        DMA_COUNT
};

#define DMA1_CHANNELS                   7
#define DMA2_CHANNELS                   5

#define DMAD(major, channel, ID)        (((ID) << 4) | (((channel) & 7) << 1) | ((major) & 1))
#define GETMAJOR(DMAD)                  (((DMAD) >> 0) & 1)
#define GETCHANNEL(DMAD)                (((DMAD) >> 1) & 7)

#define M2M_TRANSFER_TIMEOUT            5000

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        DMA_t         *DMA;
        DMA_Channel_t *channel[DMA1_CHANNELS];
        IRQn_Type      IRQn[DMA1_CHANNELS];
        u8_t           chcnt;
} DMA_HW_t;

typedef struct {
        void           *arg;
        _DMA_cb_t       callback;
        u32_t           dmad;
        bool            release;
} DMA_RT_channel_t;

typedef struct {
        DMA_RT_channel_t  *channel;
        queue_t          **queue;
        u32_t              ID_cnt;
        u8_t               major;
} DMA_RT_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void clear_DMA_IRQ_flags(u8_t major, u8_t channel);
static bool M2M_callback(DMA_Channel_t *channel, u8_t SR, void *arg);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(DMA);

static const DMA_HW_t DMA_HW[] = {
        {
                .DMA        = DMA1,
                .chcnt      = DMA1_CHANNELS,
                .channel[0] = DMA1_Channel1,
                .channel[1] = DMA1_Channel2,
                .channel[2] = DMA1_Channel3,
                .channel[3] = DMA1_Channel4,
                .channel[4] = DMA1_Channel5,
                .channel[5] = DMA1_Channel6,
                .channel[6] = DMA1_Channel7,
                .IRQn[0]    = DMA1_Channel1_IRQn,
                .IRQn[1]    = DMA1_Channel2_IRQn,
                .IRQn[2]    = DMA1_Channel3_IRQn,
                .IRQn[3]    = DMA1_Channel4_IRQn,
                .IRQn[4]    = DMA1_Channel5_IRQn,
                .IRQn[5]    = DMA1_Channel6_IRQn,
                .IRQn[6]    = DMA1_Channel7_IRQn,
        },
        #if defined(RCC_AHBENR_DMA2EN)
        {
                .DMA        = DMA2,
                .chcnt      = DMA2_CHANNELS,
                .channel[0] = DMA2_Channel1,
                .channel[1] = DMA2_Channel2,
                .channel[2] = DMA2_Channel3,
                .channel[3] = DMA2_Channel4,
                .channel[4] = DMA2_Channel5,
                .IRQn[0]    = DMA2_Channel1_IRQn,
                .IRQn[1]    = DMA2_Channel2_IRQn,
                .IRQn[2]    = DMA2_Channel3_IRQn,
                .IRQn[3]    = DMA2_Channel4_IRQn,
                .IRQn[4]    = DMA2_Channel5_IRQn,
        }
        #endif
};

static DMA_RT_t *DMA_RT[DMA_COUNT];

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
API_MOD_INIT(DMA, void **device_handle, u8_t major, u8_t minor)
{
        int err = EFAULT;

        if (major < DMA_COUNT && minor == 0) {
                err = sys_zalloc(sizeof(DMA_RT_t), device_handle);
                if (!err) {
                        DMA_RT_t *hdl = *device_handle;

                        err = sys_zalloc(sizeof(queue_t*) * DMA_HW[major].chcnt,
                                         cast(void*, &hdl->queue));
                        if (err) goto finish;

                        err = sys_zalloc(sizeof(DMA_RT_channel_t) * DMA_HW[major].chcnt,
                                         cast(void*, &hdl->channel));
                        if (err) goto finish;

                        /*
                         * NOTE: DMA1EN and DMA2EN are localized in this same
                         *       register and DMA2EN is shifted left by
                         *       1 bit relative to DMA1EN.
                         */
                        RCC->AHBENR |= (RCC_AHBENR_DMA1EN << major);

                        DMA_RT[major] = *device_handle;
                        DMA_RT[major]->major = major;
                        DMA_RT[major]->ID_cnt++;

                        finish:
                        if (err) {
                                if (hdl->queue) {
                                        sys_free(cast(void **, &hdl->queue));
                                }

                                if (hdl->channel) {
                                        sys_free(cast(void **, &hdl->channel));
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
API_MOD_RELEASE(DMA, void *device_handle)
{
        UNUSED_ARG1(device_handle);
        return EPERM;
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
API_MOD_OPEN(DMA, void *device_handle, u32_t flags)
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
API_MOD_CLOSE(DMA, void *device_handle, bool force)
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
API_MOD_WRITE(DMA,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG6(device_handle, src, count, fpos, wrcnt, fattr);

        return EPERM;
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
API_MOD_READ(DMA,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG6(device_handle, dst, count, fpos, rdcnt, fattr);

        return EPERM;
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
API_MOD_IOCTL(DMA, void *device_handle, int request, void *arg)
{
        DMA_RT_t *hdl = device_handle;

        int err = EPERM;

        if (arg) {
                switch (request) {
                case IOCTL_DMA__TRANSFER: {
                        err = EBUSY;

                        u32_t dmad    = 0;
                        u8_t  channel = 0;

                        for (; channel < DMA_HW[hdl->major].chcnt; channel++) {
                                dmad = _DMA_DDI_reserve(hdl->major, channel + 1);
                                if (dmad) break;
                        }

                        if (dmad && (hdl->queue[channel] == NULL)) {
                                err = sys_queue_create(1, sizeof(int), &hdl->queue[channel]);
                                if (err) {
                                        _DMA_DDI_release(dmad);
                                        break;
                                }
                        }

                        if (dmad) {
                                const DMA_transfer_t *transfer = arg;

                                u32_t PMSIZE, NDT;

                                if (  ((transfer->size & 3) == 0)
                                   && ((cast(u32_t, transfer->src) & 3) == 0)
                                   && ((cast(u32_t, transfer->dst) & 3) == 0) ) {

                                        PMSIZE = (2 << 10) | (2 << 8);

                                        NDT = transfer->size / 4;

                                } else if (  ((transfer->size & 1) == 0)
                                          && ((cast(u32_t, transfer->src) & 1) == 0)
                                          && ((cast(u32_t, transfer->dst) & 1) == 0) ) {

                                        PMSIZE = (1 << 10) | (1 << 8);

                                        NDT = transfer->size / 2;

                                } else {
                                        PMSIZE = 0;
                                        NDT   = transfer->size;
                                }

                                if (NDT <= UINT16_MAX) {
                                        _DMA_DDI_config_t config;
                                        config.arg      = hdl->queue[channel];
                                        config.callback = M2M_callback;
                                        config.release  = true;
                                        config.PA       = cast(u32_t, transfer->src);
                                        config.MA       = cast(u32_t, transfer->dst);
                                        config.NDT      = NDT;
                                        config.CR       = PMSIZE | DMA_CCR1_MEM2MEM
                                                         | DMA_CCR1_MINC | DMA_CCR1_PINC;
                                        config.IRQ_priority = __CPU_DEFAULT_IRQ_PRIORITY__;

                                        err = _DMA_DDI_transfer(dmad, &config);
                                        if (!err) {
                                                int ferr = EIO;
                                                err = sys_queue_receive(hdl->queue[channel],
                                                                        &ferr,
                                                                        M2M_TRANSFER_TIMEOUT);
                                                if (!err) {
                                                        err = ferr;
                                                }
                                        }
                                } else {
                                        err = EFBIG;
                                }

                                _DMA_DDI_release(dmad);
                        }
                        break;
                }

                default:
                        err = EBADRQC;
                        break;
                }
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
API_MOD_FLUSH(DMA, void *device_handle)
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
API_MOD_STAT(DMA, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);

        device_stat->st_size = 0;

        return ESUCC;
}

//==============================================================================
/**
 * @brief Function allocate selected stream.
 *
 * @param [in]  major         DMA peripheral number.
 * @param [in]  channel       channel number [1..7].
 *
 * @return On success DMA descriptor number, otherwise 0.
 */
//==============================================================================
u32_t _DMA_DDI_reserve(u8_t major, u8_t channel)
{
        int dmad = 0;
        channel--;

        if (DMA_RT[major] == NULL) {
                printk("DMA%d: not initialized", major + 1);
                return dmad;
        }

        if (major < DMA_COUNT && (channel < DMA_HW[major].chcnt)) {
                sys_critical_section_begin();
                {
                        if (DMA_RT[major]->channel[channel].dmad == 0) {
                                u32_t ID = DMA_RT[major]->ID_cnt++;

                                dmad = DMAD(major, channel, ID);

                                DMA_RT[major]->channel[channel].dmad = dmad;

                                if (DMA_RT[major]->ID_cnt == 0) {
                                        DMA_RT[major]->ID_cnt++;
                                }
                        }
                }
                sys_critical_section_end();
        }

        return dmad;
}

//==============================================================================
/**
 * @brief Function free allocated stream.
 *
 * @param dmad                  DMA descriptor.
 */
//==============================================================================
void _DMA_DDI_release(u32_t dmad)
{
        if (dmad && DMA_RT[GETMAJOR(dmad)]) {
                DMA_RT_channel_t *RT_channel = &DMA_RT[GETMAJOR(dmad)]->channel[GETCHANNEL(dmad)];

                if (RT_channel->dmad == dmad) {

                        DMA_Channel_t *DMA_channel = DMA_HW[GETMAJOR(dmad)].channel[GETCHANNEL(dmad)];

                        RT_channel->callback = NULL;
                        DMA_channel->CCR     = 0;

                        clear_DMA_IRQ_flags(GETMAJOR(dmad), GETCHANNEL(dmad));

                        DMA_channel->CMAR  = 0;
                        DMA_channel->CNDTR = 0;
                        DMA_channel->CPAR  = 0;

                        memset(RT_channel, 0, sizeof(DMA_RT_channel_t));
                }
        }
}

//==============================================================================
/**
 * @brief Function start transfer. The IRQ flags (TCIE, TEIE) are added
 *        automatically.
 *
 * @param dmad                  DMA descriptor.
 * @param config                DMA configuration.
 *
 * @return One of errno value.
 */
//==============================================================================
int _DMA_DDI_transfer(u32_t dmad, _DMA_DDI_config_t *config)
{
        int err = EINVAL;

        if (  dmad
           && DMA_RT[GETMAJOR(dmad)]
           && config
           && config->NDT
           && config->PA
           && config->MA) {

                DMA_RT_channel_t *RT_channel = &DMA_RT[GETMAJOR(dmad)]->channel[GETCHANNEL(dmad)];
                DMA_Channel_t    *DMA_Stream = DMA_HW[GETMAJOR(dmad)].channel[GETCHANNEL(dmad)];
                IRQn_Type         IRQn       = DMA_HW[GETMAJOR(dmad)].IRQn[GETCHANNEL(dmad)];

                if (RT_channel->dmad == dmad) {
                        DMA_Stream->CMAR  = config->MA;
                        DMA_Stream->CNDTR = config->NDT;
                        DMA_Stream->CPAR  = config->PA;
                        DMA_Stream->CCR   = config->CR & ~DMA_CCR1_EN;

                        RT_channel->arg      = config->arg;
                        RT_channel->callback = config->callback;
                        RT_channel->release  = config->release;

                        clear_DMA_IRQ_flags(GETMAJOR(dmad), GETCHANNEL(dmad));
                        NVIC_SetPriority(IRQn, config->IRQ_priority);
                        NVIC_ClearPendingIRQ(IRQn);
                        NVIC_EnableIRQ(IRQn);

                        SET_BIT(DMA_Stream->CCR, DMA_CCR1_TCIE | DMA_CCR1_TEIE);
                        SET_BIT(DMA_Stream->CCR, DMA_CCR1_EN);

                        err = ESUCC;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function clear flags for selected DMA and stream.
 *
 * @param  major        DMA number
 * @param  channel      stream number
 */
//==============================================================================
static void clear_DMA_IRQ_flags(u8_t major, u8_t channel)
{
        DMA_HW[major].DMA->IFCR = ( DMA_IFCR_CGIF1  | DMA_IFCR_CTCIF1
                                  | DMA_IFCR_CHTIF1 | DMA_IFCR_CTEIF1 )
                                  << (4 * channel);
}

//==============================================================================
/**
 * @brief  Memory to memory callback.
 *
 * @param  SR           status
 * @param  arg          argument
 *
 * @return True if yield needed, false otherwise.
 */
//==============================================================================
static bool M2M_callback(DMA_Channel_t *channel, u8_t SR, void *arg)
{
        UNUSED_ARG1(channel);

        bool yield = false;
        int  err   = (SR & DMA_SR_TCIF) ? ESUCC : EIO;

        sys_queue_send_from_ISR(arg, &err, &yield);

        return yield;
}

//==============================================================================
/**
 * @brief Function handle DMA IRQ.
 *
 * @param  major        DMA number
 * @param  channel      stream number
 */
//==============================================================================
static void IRQ_handle(u8_t major, u8_t channel)
{
        DMA_Channel_t    *DMA_channel = DMA_HW[major].channel[channel];
        DMA_RT_channel_t *RT_channel  = &DMA_RT[major]->channel[channel];

        bool  yield = false;

        u32_t SR = DMA_HW[major].DMA->ISR >> (4 * channel);

        if (RT_channel->callback) {
                yield = RT_channel->callback(DMA_channel, SR & 0xF, RT_channel->arg);
        }

        if (!(DMA_channel->CCR & DMA_CCR1_CIRC)) {
                CLEAR_BIT(DMA_channel->CCR, DMA_CCR1_EN);

                RT_channel->callback = NULL;
                RT_channel->arg      = NULL;

                if (RT_channel->release) {
                        RT_channel->dmad    = 0;
                        RT_channel->release = false;
                }

                /*
                 * Writing EN bit to 0 is not immediately effective since it is
                 * actually written to 0 once all the current transfers have finished.
                 * When the EN bit is read as 0, this means that the stream is
                 * ready to be configured. It is therefore necessary to wait for
                 * the EN bit to be  cleared before starting any stream configuration.
                 */
                while (DMA_channel->CCR & DMA_CCR1_EN);
        }

        clear_DMA_IRQ_flags(major, channel);

        sys_thread_yield_from_ISR(yield);
}

//==============================================================================
/**
 * @brief DMA1 Channel 1 IRQ.
 */
//==============================================================================
void DMA1_Channel1_IRQHandler(void)
{
        IRQ_handle(0, 0);
}

//==============================================================================
/**
 * @brief DMA1 Channel 2 IRQ.
 */
//==============================================================================
void DMA1_Channel2_IRQHandler(void)
{
        IRQ_handle(0, 1);
}

//==============================================================================
/**
 * @brief DMA1 Channel 3 IRQ.
 */
//==============================================================================
void DMA1_Channel3_IRQHandler(void)
{
        IRQ_handle(0, 2);
}

//==============================================================================
/**
 * @brief DMA1 Channel 4 IRQ.
 */
//==============================================================================
void DMA1_Channel4_IRQHandler(void)
{
        IRQ_handle(0, 3);
}

//==============================================================================
/**
 * @brief DMA1 Channel 5 IRQ.
 */
//==============================================================================
void DMA1_Channel5_IRQHandler(void)
{
        IRQ_handle(0, 4);
}

//==============================================================================
/**
 * @brief DMA1 Channel 6 IRQ.
 */
//==============================================================================
void DMA1_Channel6_IRQHandler(void)
{
        IRQ_handle(0, 5);
}

//==============================================================================
/**
 * @brief DMA1 Channel 7 IRQ.
 */
//==============================================================================
void DMA1_Channel7_IRQHandler(void)
{
        IRQ_handle(0, 6);
}

#if defined(RCC_AHBENR_DMA2EN)
//==============================================================================
/**
 * @brief DMA2 Channel 1 IRQ.
 */
//==============================================================================
void DMA2_Channel1_IRQHandler(void)
{
        IRQ_handle(1, 0);
}

//==============================================================================
/**
 * @brief DMA2 Channel 2 IRQ.
 */
//==============================================================================
void DMA2_Channel2_IRQHandler(void)
{
        IRQ_handle(1, 1);
}

//==============================================================================
/**
 * @brief DMA2 Channel 3 IRQ.
 */
//==============================================================================
void DMA2_Channel3_IRQHandler(void)
{
        IRQ_handle(1, 2);
}

//==============================================================================
/**
 * @brief DMA2 Channel 4 IRQ.
 */
//==============================================================================
void DMA2_Channel4_IRQHandler(void)
{
        IRQ_handle(1, 3);
}

//==============================================================================
/**
 * @brief DMA2 Channel 5 IRQ.
 */
//==============================================================================
void DMA2_Channel5_IRQHandler(void)
{
        IRQ_handle(1, 4);
}

//==============================================================================
/**
 * @brief DMA2 Channel 4 & 5 IRQ.
 */
//==============================================================================
void DMA2_Channel4_5_IRQHandler(void)
{
        if (DMA2->ISR & (DMA_ISR_GIF4 | DMA_ISR_TCIF4 | DMA_ISR_HTIF4 | DMA_ISR_TEIF4)) {
                IRQ_handle(1, 3);
        } else if (DMA2->ISR & (DMA_ISR_GIF5 | DMA_ISR_TCIF5 | DMA_ISR_HTIF5 | DMA_ISR_TEIF5)) {
                IRQ_handle(1, 4);
        } else {
                _assert(false);
        }
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
