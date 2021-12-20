/*==============================================================================
File     dma.c

Author   Daniel Zorychta

Brief    General usage DMA driver.

         Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "dma/dma_ioctl.h"
#include "kernel/khooks.h"

#if defined(ARCH_stm32f1)
#include "stm32f1/stm32f10x.h"
#include "stm32f1/dma_ddi.h"
#elif defined(ARCH_stm32f3)
#include "stm32f3/stm32f3xx.h"
#include "stm32f3/dma_ddi.h"
#endif

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
        DMA_TypeDef          *DMA;
        DMA_Channel_TypeDef  *channel[max(DMA1_CHANNELS, DMA2_CHANNELS)];
        IRQn_Type             IRQn[max(DMA1_CHANNELS, DMA2_CHANNELS)];
        u8_t                  chcnt;
} DMA_HW_t;

typedef struct {
        void           *user_ctx;
        _DMA_cb_t       cb_finish;
        _DMA_cb_t       cb_half;
        _DMA_cb_t       cb_next;
        u32_t           dmad;
        bool            release;
} DMA_RT_channel_t;

typedef struct {
        DMA_RT_channel_t   channel[max(DMA1_CHANNELS, DMA2_CHANNELS)];
        kqueue_t          **m2m_queue;
        u32_t              ID_ctr;
        u8_t               major;
} DMA_RT_t;

typedef struct {
        DMA_RT_t DMA[DMA_COUNT];
} DMA_mem_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void clear_DMA_IRQ_flags(u8_t major, u8_t channel);
static bool M2M_callback(DMA_Channel_TypeDef *channel, u8_t SR, void *arg);

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

static DMA_mem_t *DMA_mem;

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
API_MOD_INIT(DMA, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(config);

        int err = EFAULT;

        if (major == 0 && minor == 0) {
                err = sys_zalloc(sizeof(DMA_mem_t), device_handle);
                if (!err) {
                        DMA_mem = *device_handle;

                        /*
                         * Only the DMA1 controller is used for memory-to-memory
                         * transfers.
                         */
                        DMA_RT_t *hdl = &DMA_mem->DMA[0];
                        err = sys_zalloc(sizeof(kqueue_t*) * DMA1_CHANNELS,
                                         cast(void*, &hdl->m2m_queue));
                        if (err) {
                                sys_free(device_handle);
                                DMA_mem = NULL;
                        }

                        if (!err) {
                                RCC->AHBENR |= (RCC_AHBENR_DMA1EN
                                #if defined(RCC_AHBENR_DMA2EN)
                                               | RCC_AHBENR_DMA2EN
                                #endif
                                );
                                DMA_mem->DMA[0].ID_ctr = 1;
                                DMA_mem->DMA[1].ID_ctr = 1;
                                DMA_mem->DMA[1].major  = 1;
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
        UNUSED_ARG1(device_handle);

        int err = EPERM;

        if (arg) {
                switch (request) {
                case IOCTL_DMA__TRANSFER: {
                        const DMA_transfer_t *transfer = arg;
                        err = _DMA_DDI_memcpy(transfer->dst, transfer->src, transfer->size);
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

        if (DMA_mem == NULL) {
                printk("DMA: driver not initialized");
                return dmad;
        }

        if (major < DMA_COUNT && (channel < DMA_HW[major].chcnt)) {
                sys_critical_section_begin();
                {
                        if (DMA_mem->DMA[major].channel[channel].dmad == 0) {
                                u32_t ID = DMA_mem->DMA[major].ID_ctr++;

                                dmad = DMAD(major, channel, ID);

                                DMA_mem->DMA[major].channel[channel].dmad = dmad;

                                if (DMA_mem->DMA[major].ID_ctr == 0) {
                                        DMA_mem->DMA[major].ID_ctr++;
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
        if (dmad && DMA_mem) {
                DMA_RT_channel_t *RT_channel = &DMA_mem->DMA[GETMAJOR(dmad)].channel[GETCHANNEL(dmad)];

                if (RT_channel->dmad == dmad) {

                        DMA_Channel_TypeDef *DMA_channel = DMA_HW[GETMAJOR(dmad)].channel[GETCHANNEL(dmad)];

                        RT_channel->cb_finish = NULL;
                        RT_channel->cb_half   = NULL;
                        RT_channel->cb_next   = NULL;
                        DMA_channel->CCR      = 0;

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
 * @brief Function reaturn DMA steram according to DMA descriptor.
 *
 * @param dmad                  DMA descriptor.
 * @param channel               channel
 *
 * @return One of errno value.
 */
//==============================================================================
int _DMA_DDI_get_channel(u32_t dmad, DMA_Channel_TypeDef **channel)
{
        int err = EINVAL;

        if (dmad && DMA_mem && channel) {
                DMA_RT_channel_t *RT_channel = &DMA_mem->DMA[GETMAJOR(dmad)].channel[GETCHANNEL(dmad)];

                if (RT_channel->dmad == dmad) {

                        DMA_Channel_TypeDef *DMA_channel = DMA_HW[GETMAJOR(dmad)].channel[GETCHANNEL(dmad)];
                        *channel = DMA_channel;
                        err = 0;

                } else {
                        err = ENODEV;
                }
        }

        return err;
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
           && DMA_mem
           && config
           && config->data_number
           && config->peripheral_address
           && config->memory_address) {

                DMA_RT_channel_t    *RT_channel = &DMA_mem->DMA[GETMAJOR(dmad)].channel[GETCHANNEL(dmad)];
                DMA_Channel_TypeDef *DMA_Stream = DMA_HW[GETMAJOR(dmad)].channel[GETCHANNEL(dmad)];
                IRQn_Type            IRQn       = DMA_HW[GETMAJOR(dmad)].IRQn[GETCHANNEL(dmad)];

                if (RT_channel->dmad == dmad) {
                        DMA_Stream->CMAR  = config->memory_address;
                        DMA_Stream->CNDTR = config->data_number;
                        DMA_Stream->CPAR  = config->peripheral_address;
                        DMA_Stream->CCR   = ((config->priority_level << DMA_CCR_PL_Pos) & DMA_CCR_PL_Msk)
                                          | ((config->memory_data_size << DMA_CCR_MSIZE_Pos) & DMA_CCR_MSIZE_Msk)
                                          | ((config->peripheral_data_size << DMA_CCR_PSIZE_Pos) & DMA_CCR_PSIZE_Msk)
                                          | ((config->memory_address_increment << DMA_CCR_MINC_Pos) & DMA_CCR_MINC_Msk)
                                          | ((config->peripheral_address_increment << DMA_CCR_PINC_Pos) & DMA_CCR_PINC_Msk)
                                          | ((config->circular_mode << DMA_CCR_CIRC_Pos) & DMA_CCR_CIRC_Msk);

                        switch (config->transfer_direction) {
                        case _DMA_DDI_TRANSFER_DIRECTION_PERIPHERAL_TO_MEMORY:
                                break;
                        case _DMA_DDI_TRANSFER_DIRECTION_MEMORY_TO_PERIPHERAL:
                                SET_BIT(DMA_Stream->CCR, DMA_CCR_DIR);
                                break;
                        case _DMA_DDI_TRANSFER_DIRECTION_MEMORY_TO_MEMORY:
                                SET_BIT(DMA_Stream->CCR, DMA_CCR_MEM2MEM);
                                break;
                        }

                        RT_channel->user_ctx  = config->user_ctx;
                        RT_channel->cb_finish = config->cb_finish;
                        RT_channel->cb_half   = config->cb_half;
                        RT_channel->cb_next   = config->cb_next;
                        RT_channel->release   = config->release;

                        clear_DMA_IRQ_flags(GETMAJOR(dmad), GETCHANNEL(dmad));
                        NVIC_SetPriority(IRQn, config->IRQ_priority);
                        NVIC_ClearPendingIRQ(IRQn);
                        NVIC_EnableIRQ(IRQn);

                        SET_BIT(DMA_Stream->CCR, DMA_CCR_TCIE | DMA_CCR_TEIE | (config->cb_half ? DMA_CCR_HTIE : 0));
                        SET_BIT(DMA_Stream->CCR, DMA_CCR_EN);

                        err = ESUCC;
                }
        }

        return err;
}

//==============================================================================
/**
 * @brief Function start memory-to-memory transfer by using free channel.
 *
 * @param dst                   destination address.
 * @param src                   source address.
 * @param size                  block size.
 *
 * @return One of errno value.
 */
//==============================================================================
int _DMA_DDI_memcpy(void *dst, const void *src, size_t size)
{
        if (!sys_is_mem_dma_capable(dst) || !sys_is_mem_dma_capable(src)) {
                return EIO;
        }

        DMA_RT_t *hdl = &DMA_mem->DMA[0];
        if (!hdl || !dst || !src || !size) {
                return EINVAL;
        }

        int err = EBUSY;

        u32_t dmad    = 0;
        u8_t  channel = 0;

        for (; channel < DMA_HW[hdl->major].chcnt; channel++) {
                dmad = _DMA_DDI_reserve(hdl->major, channel + 1);
                if (dmad) break;
        }

        if (dmad && (hdl->m2m_queue[channel] == NULL)) {
                err = sys_queue_create(1, sizeof(int), &hdl->m2m_queue[channel]);
                if (err) {
                        _DMA_DDI_release(dmad);
                        return err;
                }
        }

        if (dmad) {
                _DMA_DDI_config_t config = {0};

                if (  ((size & 3) == 0)
                   && ((cast(u32_t, src) & 3) == 0)
                   && ((cast(u32_t, dst) & 3) == 0) ) {

                        config.peripheral_data_size = _DMA_DDI_PERIPHERAL_DATA_SIZE_WORD;
                        config.memory_data_size     = _DMA_DDI_MEMORY_DATA_SIZE_WORD;
                        config.data_number                  = size / 4;

                } else if (  ((size & 1) == 0)
                          && ((cast(u32_t, src) & 1) == 0)
                          && ((cast(u32_t, dst) & 1) == 0) ) {

                        config.peripheral_data_size = _DMA_DDI_PERIPHERAL_DATA_SIZE_HALF_WORD;
                        config.memory_data_size     = _DMA_DDI_MEMORY_DATA_SIZE_HALF_WORD;
                        config.data_number                  = size / 2;

                } else {
                        config.peripheral_data_size = _DMA_DDI_PERIPHERAL_DATA_SIZE_BYTE;
                        config.memory_data_size     = _DMA_DDI_MEMORY_DATA_SIZE_BYTE;
                        config.data_number                  = size;
                }

                if (config.data_number <= UINT16_MAX) {
                        config.user_ctx                     = hdl->m2m_queue[channel];
                        config.cb_finish                    = M2M_callback;
                        config.cb_half                      = NULL;
                        config.cb_next                      = NULL;
                        config.release                      = true;
                        config.peripheral_address           = cast(u32_t, src);
                        config.memory_address               = cast(u32_t, dst);
                        config.IRQ_priority                 = __CPU_DEFAULT_IRQ_PRIORITY__;
                        config.circular_mode                = _DMA_DDI_CIRCULAR_MODE_DISABLED;
                        config.memory_address_increment     = _DMA_DDI_MEMORY_ADDRESS_POINTER_INCREMENTED;
                        config.peripheral_address_increment = _DMA_DDI_PERIPHERAL_ADDRESS_POINTER_INCREMENTED;
                        config.priority_level               = _DMA_DDI_PRIORITY_LEVEL_LOW;
                        config.transfer_direction           = _DMA_DDI_TRANSFER_DIRECTION_MEMORY_TO_MEMORY;

                        err = _DMA_DDI_transfer(dmad, &config);
                        if (!err) {
                                int ferr = EIO;
                                err = sys_queue_receive(hdl->m2m_queue[channel], &ferr,
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
static bool M2M_callback(DMA_Channel_TypeDef *channel, u8_t SR, void *arg)
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
        DMA_Channel_TypeDef *DMA_channel = DMA_HW[major].channel[channel];
        DMA_RT_channel_t    *RT_channel  = &DMA_mem->DMA[major].channel[channel];

        bool  yield = false;

        u32_t SR = DMA_HW[major].DMA->ISR >> (4 * channel);

        if ((DMA_channel->CCR & DMA_CCR_HTIE) && (SR & DMA_SR_HTIF)) {
                if (RT_channel->cb_half) {
                        yield = RT_channel->cb_half(DMA_channel, SR & 0xF, RT_channel->user_ctx);
                }
        } else {
                if (RT_channel->cb_finish) {
                        yield = RT_channel->cb_finish(DMA_channel, SR & 0xF, RT_channel->user_ctx);
                }
        }

        if (!(DMA_channel->CCR & DMA_CCR_CIRC)) {
                CLEAR_BIT(DMA_channel->CCR, DMA_CCR_EN);

                RT_channel->cb_half   = NULL;
                RT_channel->cb_finish = NULL;

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
                while (DMA_channel->CCR & DMA_CCR_EN);
        }

        clear_DMA_IRQ_flags(major, channel);

        if (RT_channel->cb_next) {
                yield |= RT_channel->cb_next(DMA_channel, SR & 0x3F,  RT_channel->user_ctx);
        }

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
