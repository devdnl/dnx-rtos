/*==============================================================================
File     dma.c

Author   Daniel Zorychta

Brief    General usage DMA driver.

         Copyright (C) 2020 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "dma_ddi.h"
#include "dma/dma_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define DMA_COUNT                       2
#define STREAM_COUNT                    8

#define DMAD(major, stream, ID)         (((ID) << 5) | (((stream) & 7) << 2) | ((major) & 3))
#define GETMAJOR(DMAD)                  (((DMAD) >> 0) & 3)
#define GETSTREAM(DMAD)                 (((DMAD) >> 2) & 7)

#define M2M_TRANSFER_TIMEOUT            5000

#if defined(ARCH_stm32f4) || defined(ARCH_stm32f7)
#define DMA_SxCR_CHSEL_SEL(c)           (((c) &7) << DMA_SxCR_CHSEL_Pos)
#endif

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        DMA_TypeDef             *DMA;
        DMA_Stream_TypeDef      *stream[STREAM_COUNT];
        #if defined(ARCH_stm32h7)
        DMAMUX_Channel_TypeDef  *channel[STREAM_COUNT];
        #endif
        IRQn_Type                IRQn[STREAM_COUNT];
} DMA_HW_t;

typedef struct {
        void           *user_ctx;
        _DMA_cb_t       cb_finish;
        _DMA_cb_t       cb_half;
        _DMA_cb_t       cb_next;
        u32_t           dmad;
        bool            release;
        bool            flush_cache;
} DMA_RT_stream_t;

typedef struct {
        DMA_RT_stream_t stream[STREAM_COUNT];
        queue_t       **m2m_queue;
        u32_t           ID_ctr;
        u8_t            major;
        u8_t            minor;
} DMA_RT_t;

typedef struct {
        DMA_RT_t DMA[DMA_COUNT];
} DMA_mem_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void clear_DMA_IRQ_flags(u8_t major, u8_t stream);
static bool M2M_callback(DMA_Stream_TypeDef *stream, u8_t SR, void *arg);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(DMA);

static const DMA_HW_t DMA_HW[] = {
        {
                .DMA        = DMA1,
                .stream[0]  = DMA1_Stream0,
                .stream[1]  = DMA1_Stream1,
                .stream[2]  = DMA1_Stream2,
                .stream[3]  = DMA1_Stream3,
                .stream[4]  = DMA1_Stream4,
                .stream[5]  = DMA1_Stream5,
                .stream[6]  = DMA1_Stream6,
                .stream[7]  = DMA1_Stream7,
#if defined(ARCH_stm32h7)
                .channel[0] = DMAMUX1_Channel0,
                .channel[1] = DMAMUX1_Channel1,
                .channel[2] = DMAMUX1_Channel2,
                .channel[3] = DMAMUX1_Channel3,
                .channel[4] = DMAMUX1_Channel4,
                .channel[5] = DMAMUX1_Channel5,
                .channel[6] = DMAMUX1_Channel6,
                .channel[7] = DMAMUX1_Channel7,
#endif
                .IRQn[0]    = DMA1_Stream0_IRQn,
                .IRQn[1]    = DMA1_Stream1_IRQn,
                .IRQn[2]    = DMA1_Stream2_IRQn,
                .IRQn[3]    = DMA1_Stream3_IRQn,
                .IRQn[4]    = DMA1_Stream4_IRQn,
                .IRQn[5]    = DMA1_Stream5_IRQn,
                .IRQn[6]    = DMA1_Stream6_IRQn,
                .IRQn[7]    = DMA1_Stream7_IRQn,
        },
        {
                .DMA        = DMA2,
                .stream[0]  = DMA2_Stream0,
                .stream[1]  = DMA2_Stream1,
                .stream[2]  = DMA2_Stream2,
                .stream[3]  = DMA2_Stream3,
                .stream[4]  = DMA2_Stream4,
                .stream[5]  = DMA2_Stream5,
                .stream[6]  = DMA2_Stream6,
                .stream[7]  = DMA2_Stream7,
#if defined(ARCH_stm32h7)
                .channel[0] = DMAMUX1_Channel8,
                .channel[1] = DMAMUX1_Channel9,
                .channel[2] = DMAMUX1_Channel10,
                .channel[3] = DMAMUX1_Channel11,
                .channel[4] = DMAMUX1_Channel12,
                .channel[5] = DMAMUX1_Channel13,
                .channel[6] = DMAMUX1_Channel14,
                .channel[7] = DMAMUX1_Channel15,
#endif
                .IRQn[0]    = DMA2_Stream0_IRQn,
                .IRQn[1]    = DMA2_Stream1_IRQn,
                .IRQn[2]    = DMA2_Stream2_IRQn,
                .IRQn[3]    = DMA2_Stream3_IRQn,
                .IRQn[4]    = DMA2_Stream4_IRQn,
                .IRQn[5]    = DMA2_Stream5_IRQn,
                .IRQn[6]    = DMA2_Stream6_IRQn,
                .IRQn[7]    = DMA2_Stream7_IRQn,
        },
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
                         * Only the DMA2 controller is able (F4,F7)/used (H7) to perform
                         * memory-to-memory transfers.
                         * Allocating memory for memory-to-memory queues.
                         */
                        DMA_RT_t *hdl = &DMA_mem->DMA[1];
                        err = sys_zalloc(sizeof(queue_t*) * STREAM_COUNT,
                                         cast(void*, &hdl->m2m_queue));
                        if (err) {
                                sys_free(device_handle);
                                DMA_mem = NULL;
                        }

                        if (!err) {
                                RCC->AHB1ENR |= (RCC_AHB1ENR_DMA1EN | RCC_AHB1ENR_DMA2EN);
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

#if defined(ARCH_stm32f4) || defined(ARCH_stm32f7)
//==============================================================================
/**
 * @brief Function allocate selected stream.
 *
 * @param [in]  major           DMA peripheral number.
 * @param [in]  stream          stream number.
 *
 * @return On success DMA descriptor number, otherwise 0.
 */
//==============================================================================
u32_t _DMA_DDI_reserve(u8_t major, u8_t stream)
{
        int dmad = 0;

        if (DMA_mem == NULL) {
                printk("DMA: driver not initialized");
                return dmad;
        }

        if ((major < DMA_COUNT) && (stream < STREAM_COUNT)) {

                sys_critical_section_begin();
                {
                        if (DMA_mem->DMA[major].stream[stream].dmad == 0) {
                                u32_t ID = DMA_mem->DMA[major].ID_ctr++;

                                dmad = DMAD(major, stream, ID);

                                DMA_mem->DMA[major].stream[stream].dmad = dmad;

                                if (DMA_mem->DMA[major].ID_ctr == 0) {
                                        DMA_mem->DMA[major].ID_ctr++;
                                }
                        }
                }
                sys_critical_section_end();
        }

        return dmad;
}
#endif

#if defined(ARCH_stm32h7)
//==============================================================================
/**
 * @brief Function allocate selected stream.
 *
 * @param [in]  major_mask    DMA peripheral number mask.
 *
 * @return On success DMA descriptor number, otherwise 0.
 */
//==============================================================================
u32_t _DMA_DDI_reserve(u8_t major_mask)
{
        int dmad = 0;

        if (DMA_mem == NULL) {
                printk("DMA: driver not initialized");
                return dmad;
        }

        for (u8_t major = 0; (major < DMA_COUNT) && (dmad == 0); major++) {

                if (major_mask & (1 << major)) {

                        sys_critical_section_begin();
                        {
                                for (u8_t stream = 0; stream < STREAM_COUNT; stream++) {

                                        if (DMA_mem->DMA[major].stream[stream].dmad == 0) {

                                                u32_t ID = DMA_mem->DMA[major].ID_ctr++;

                                                dmad = DMAD(major, stream, ID);

                                                DMA_mem->DMA[major].stream[stream].dmad = dmad;

                                                if (DMA_mem->DMA[major].ID_ctr == 0) {
                                                        DMA_mem->DMA[major].ID_ctr++;
                                                }

                                                break;
                                        }
                                }
                        }
                        sys_critical_section_end();
                }
        }

        return dmad;
}
#endif

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
                DMA_RT_stream_t *RT_stream = &DMA_mem->DMA[GETMAJOR(dmad)].stream[GETSTREAM(dmad)];

                if (RT_stream->dmad == dmad) {

                        DMA_Stream_TypeDef *DMA_Stream = DMA_HW[GETMAJOR(dmad)].stream[GETSTREAM(dmad)];

                        RT_stream->cb_finish = NULL;
                        RT_stream->cb_half   = NULL;
                        RT_stream->cb_next   = NULL;
                        DMA_Stream->CR       = 0;

                        clear_DMA_IRQ_flags(GETMAJOR(dmad), GETSTREAM(dmad));

                        DMA_Stream->M0AR = 0;
                        DMA_Stream->M1AR = 0;
                        DMA_Stream->NDTR = 0;
                        DMA_Stream->PAR  = 0;

                        memset(RT_stream, 0, sizeof(DMA_RT_stream_t));
                }
        }
}

//==============================================================================
/**
 * @brief Function reaturn DMA steram according to DMA descriptor.
 *
 * @param dmad                  DMA descriptor.
 * @param stream                stream
 *
 * @return One of errno value.
 */
//==============================================================================
int _DMA_DDI_get_stream(u32_t dmad, DMA_Stream_TypeDef **stream)
{
        int err = EINVAL;

        if (dmad && DMA_mem && stream) {
                DMA_RT_stream_t *RT_stream = &DMA_mem->DMA[GETMAJOR(dmad)].stream[GETSTREAM(dmad)];

                if (RT_stream->dmad == dmad) {

                        DMA_Stream_TypeDef *DMA_Stream = DMA_HW[GETMAJOR(dmad)].stream[GETSTREAM(dmad)];
                        *stream = DMA_Stream;
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
           && (config->memory_address[0] || config->memory_address[1])) {

                DMA_RT_stream_t    *RT_stream  = &DMA_mem->DMA[GETMAJOR(dmad)].stream[GETSTREAM(dmad)];
                DMA_Stream_TypeDef *DMA_Stream =  DMA_HW[GETMAJOR(dmad)].stream[GETSTREAM(dmad)];
                IRQn_Type           IRQn       =  DMA_HW[GETMAJOR(dmad)].IRQn[GETSTREAM(dmad)];

                if (RT_stream->dmad == dmad) {
                        DMA_Stream->CR = 0;
                        clear_DMA_IRQ_flags(GETMAJOR(dmad), GETSTREAM(dmad));

                        DMA_Stream->M0AR = config->memory_address[0];
                        DMA_Stream->M1AR = config->memory_address[1];
                        DMA_Stream->NDTR = config->data_number;
                        DMA_Stream->PAR  = config->peripheral_address;
                        DMA_Stream->CR   = ((config->memory_burst << DMA_SxCR_MBURST_Pos) & DMA_SxCR_MBURST_Msk)
                                         | ((config->peripheral_burst << DMA_SxCR_PBURST_Pos) & DMA_SxCR_PBURST_Msk)
                                        #if defined(ARCH_stm32f4) | defined(ARCH_stm32f7)
                                         | DMA_SxCR_CHSEL_SEL(config->channel)
                                        #endif
                                        #ifdef DMA_SxCR_TRBUFF_Msk
                                         | ((config->bufferable_transfer << DMA_SxCR_TRBUFF_Pos) & DMA_SxCR_TRBUFF_Msk)
                                        #endif
                                         | ((config->double_buffer_mode << DMA_SxCR_DBM_Pos) & DMA_SxCR_DBM_Msk)
                                         | ((config->priority_level << DMA_SxCR_PL_Pos) & DMA_SxCR_PL_Msk)
                                         | ((config->peripheral_increment_offset << DMA_SxCR_PINCOS_Pos) & DMA_SxCR_PINCOS_Msk)
                                         | ((config->memory_data_size << DMA_SxCR_MSIZE_Pos) & DMA_SxCR_MSIZE_Msk)
                                         | ((config->peripheral_data_size << DMA_SxCR_PSIZE_Pos) & DMA_SxCR_PSIZE_Msk)
                                         | ((config->memory_address_increment << DMA_SxCR_MINC_Pos) & DMA_SxCR_MINC_Msk)
                                         | ((config->peripheral_address_increment << DMA_SxCR_PINC_Pos) & DMA_SxCR_PINC_Msk)
                                         | ((config->circular_mode << DMA_SxCR_CIRC_Pos) & DMA_SxCR_CIRC_Msk)
                                         | ((config->transfer_direction << DMA_SxCR_DIR_Pos) & DMA_SxCR_DIR_Msk)
                                         | ((config->flow_controller << DMA_SxCR_PFCTRL_Pos) & DMA_SxCR_PFCTRL_Msk);

                        DMA_Stream->FCR  = ((config->mode << DMA_SxFCR_DMDIS_Pos) & DMA_SxFCR_DMDIS_Msk)
                                         | ((config->fifo_threshold << DMA_SxFCR_FTH_Pos) & DMA_SxFCR_FTH_Msk);

                        RT_stream->user_ctx  = config->user_ctx;
                        RT_stream->cb_finish = config->cb_finish;
                        RT_stream->cb_half   = config->cb_half;
                        RT_stream->cb_next   = config->cb_next;
                        RT_stream->release   = config->release;

                        clear_DMA_IRQ_flags(GETMAJOR(dmad), GETSTREAM(dmad));
                        NVIC_SetPriority(IRQn, config->IRQ_priority);
                        NVIC_EnableIRQ(IRQn);

                        RT_stream->flush_cache = (  _mm_is_cacheable((void*)config->memory_address[0])
                                                 || _mm_is_cacheable((void*)config->memory_address[1]));

                        if (RT_stream->flush_cache) {

                                if (  (config->transfer_direction == _DMA_DDI_TRANSFER_DIRECTION_MEMORY_TO_PERIPHERAL)
                                   || (config->transfer_direction == _DMA_DDI_TRANSFER_DIRECTION_MEMORY_TO_MEMORY) ) {

                                        _cpuctl_clean_dcache();
                                        RT_stream->flush_cache = false;
                                }
                        }

                        #if defined(ARCH_stm32h7)
                        DMAMUX_Channel_TypeDef *channel = DMA_HW[GETMAJOR(dmad)].channel[GETSTREAM(dmad)];
                        channel->CCR = (config->channel & DMAMUX_CxCR_DMAREQ_ID_Msk);
                        #endif

                        SET_BIT(DMA_Stream->CR, DMA_SxCR_TCIE | DMA_SxCR_TEIE | (config->cb_half ? DMA_SxCR_HTIE : 0));
                        SET_BIT(DMA_Stream->CR, DMA_SxCR_EN);

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
        /*
         * Only DMA2 is able to transfer data in memory-to-memory mode.
         */
        DMA_RT_t *hdl = &DMA_mem->DMA[1];
        if (!hdl || !dst || !src || !size) {
                return EINVAL;
        }

        if (!sys_is_mem_dma_capable(dst) || !sys_is_mem_dma_capable(src)) {
                return EIO;
        }

        int err = EBUSY;

        u32_t dmad   = 0;
        u8_t  stream = 0;

        for (; (stream < STREAM_COUNT) && (dmad == 0); stream++) {
                #if defined(ARCH_stm32h7)
                dmad = _DMA_DDI_reserve(_DMA_DDI_DMA2);
                #else
                dmad = _DMA_DDI_reserve(_DMA_DDI_DMA2, stream);
                #endif
        }

        if (dmad && (hdl->m2m_queue[stream] == NULL)) {
                err = sys_queue_create(1, sizeof(int), &hdl->m2m_queue[stream]);
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
                        config.user_ctx                     = hdl->m2m_queue[stream];
                        config.cb_finish                    = M2M_callback;
                        config.cb_half                      = NULL;
                        config.cb_next                      = NULL;
                        config.release                      = true;
                        config.peripheral_address           = cast(u32_t, src);
                        config.memory_address[0]            = cast(u32_t, dst);
                        config.IRQ_priority                 = __CPU_DEFAULT_IRQ_PRIORITY__;
                        config.mode                         = _DMA_DDI_MODE_DIRECT;
                        config.bufferable_transfer          = _DMA_DDI_BUFFERABLE_TRANSFER_DISABLED;
                        config.circular_mode                = _DMA_DDI_CIRCULAR_MODE_DISABLED;
                        config.double_buffer_mode           = _DMA_DDI_DOUBLE_BUFFER_MODE_DISABLED;
                        config.flow_controller              = _DMA_DDI_FLOW_CONTROLLER_DMA;
                        config.memory_burst                 = _DMA_DDI_MEMORY_BURST_SINGLE_TRANSFER;
                        config.memory_address_increment     = _DMA_DDI_MEMORY_ADDRESS_POINTER_INCREMENTED;
                        config.peripheral_burst             = _DMA_DDI_PERIPHERAL_BURST_SINGLE_TRANSFER;
                        config.peripheral_address_increment = _DMA_DDI_PERIPHERAL_ADDRESS_POINTER_INCREMENTED;
                        config.peripheral_increment_offset  = _DMA_DDI_PERIPHERAL_INCREMENT_OFFSET_ACCORDING_TO_PERIPHERAL_SIZE;
                        config.priority_level               = _DMA_DDI_PRIORITY_LEVEL_LOW;
                        config.transfer_direction           = _DMA_DDI_TRANSFER_DIRECTION_MEMORY_TO_MEMORY;

                        err = _DMA_DDI_transfer(dmad, &config);
                        if (!err) {
                                int ferr = EIO;
                                err = sys_queue_receive(hdl->m2m_queue[stream], &ferr,
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
 * @param  stream       stream number
 */
//==============================================================================
static void clear_DMA_IRQ_flags(u8_t major, u8_t stream)
{
        static const struct {
                char  reg;
                u32_t mask;
        } flag[] = {
                    {.reg = 'L', .mask = (DMA_LIFCR_CFEIF0 | DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CTEIF0 | DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTCIF0)},
                    {.reg = 'L', .mask = (DMA_LIFCR_CFEIF1 | DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CTEIF1 | DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTCIF1)},
                    {.reg = 'L', .mask = (DMA_LIFCR_CFEIF2 | DMA_LIFCR_CDMEIF2 | DMA_LIFCR_CTEIF2 | DMA_LIFCR_CHTIF2 | DMA_LIFCR_CTCIF2)},
                    {.reg = 'L', .mask = (DMA_LIFCR_CFEIF3 | DMA_LIFCR_CDMEIF3 | DMA_LIFCR_CTEIF3 | DMA_LIFCR_CHTIF3 | DMA_LIFCR_CTCIF3)},
                    {.reg = 'H', .mask = (DMA_HIFCR_CFEIF4 | DMA_HIFCR_CDMEIF4 | DMA_HIFCR_CTEIF4 | DMA_HIFCR_CHTIF4 | DMA_HIFCR_CTCIF4)},
                    {.reg = 'H', .mask = (DMA_HIFCR_CFEIF5 | DMA_HIFCR_CDMEIF5 | DMA_HIFCR_CTEIF5 | DMA_HIFCR_CHTIF5 | DMA_HIFCR_CTCIF5)},
                    {.reg = 'H', .mask = (DMA_HIFCR_CFEIF6 | DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CTEIF6 | DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTCIF6)},
                    {.reg = 'H', .mask = (DMA_HIFCR_CFEIF7 | DMA_HIFCR_CDMEIF7 | DMA_HIFCR_CTEIF7 | DMA_HIFCR_CHTIF7 | DMA_HIFCR_CTCIF7)},
        };

        if (flag[stream].reg == 'L') {
                DMA_HW[major].DMA->LIFCR = flag[stream].mask;
        } else {
                DMA_HW[major].DMA->HIFCR = flag[stream].mask;
        }
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
static bool M2M_callback(DMA_Stream_TypeDef *stream, u8_t SR, void *arg)
{
        UNUSED_ARG1(stream);

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
 * @param  stream       stream number
 */
//==============================================================================
static void IRQ_handle(u8_t major, u8_t stream)
{
        DMA_Stream_TypeDef *DMA_stream = DMA_HW[major].stream[stream];
        DMA_RT_stream_t    *RT_stream  = &DMA_mem->DMA[major].stream[stream];

        if (RT_stream->flush_cache) {
                _cpuctl_clean_invalidate_dcache();
        }

        bool  yield = false;
        u32_t LISR  = DMA_HW[major].DMA->LISR;
        u32_t HISR  = DMA_HW[major].DMA->HISR;

        u32_t SR = stream < 4 ? LISR : HISR;

        switch (stream % 4) {
        case 0: SR >>=  0; break;
        case 1: SR >>=  6; break;
        case 2: SR >>= 16; break;
        case 3: SR >>= 22; break;
        }

        if ((DMA_stream->CR & DMA_SxCR_HTIE) && (SR & DMA_SR_HTIF)) {
                if (RT_stream->cb_half) {
                        yield = RT_stream->cb_half(DMA_HW[major].stream[stream],
                                                   SR & 0x3F, RT_stream->user_ctx);
                }
        } else {
                if (RT_stream->cb_finish) {
                        yield = RT_stream->cb_finish(DMA_HW[major].stream[stream],
                                                     SR & 0x3F, RT_stream->user_ctx);
                }
        }

        if (!(DMA_stream->CR & DMA_SxCR_CIRC) && !(SR & DMA_SR_HTIF)) {
                DMA_stream->CR = 0;

                RT_stream->cb_finish = NULL;
                RT_stream->cb_half   = NULL;

                if (RT_stream->release) {
                        RT_stream->dmad    = 0;
                        RT_stream->release = false;
                }

                /*
                 * Writing EN bit to 0 is not immediately effective since it is
                 * actually written to 0 once all the current transfers have finished.
                 * When the EN bit is read as 0, this means that the stream is
                 * ready to be configured. It is therefore necessary to wait for
                 * the EN bit to be  cleared before starting any stream configuration.
                 */
                while (DMA_stream->CR & DMA_SxCR_EN);
        }

        clear_DMA_IRQ_flags(major, stream);

        if (RT_stream->cb_next) {
                yield |= RT_stream->cb_next(DMA_HW[major].stream[stream],
                                            SR & 0x3F, RT_stream->user_ctx);
        }

        sys_thread_yield_from_ISR(yield);
}

//==============================================================================
/**
 * @brief DMA1 Stream 0 IRQ.
 */
//==============================================================================
void DMA1_Stream0_IRQHandler(void)
{
        IRQ_handle(0, 0);
}

//==============================================================================
/**
 * @brief DMA1 Stream 1 IRQ.
 */
//==============================================================================
void DMA1_Stream1_IRQHandler(void)
{
        IRQ_handle(0, 1);
}

//==============================================================================
/**
 * @brief DMA1 Stream 2 IRQ.
 */
//==============================================================================
void DMA1_Stream2_IRQHandler(void)
{
        IRQ_handle(0, 2);
}

//==============================================================================
/**
 * @brief DMA1 Stream 3 IRQ.
 */
//==============================================================================
void DMA1_Stream3_IRQHandler(void)
{
        IRQ_handle(0, 3);
}

//==============================================================================
/**
 * @brief DMA1 Stream 4 IRQ.
 */
//==============================================================================
void DMA1_Stream4_IRQHandler(void)
{
        IRQ_handle(0, 4);
}

//==============================================================================
/**
 * @brief DMA1 Stream 5 IRQ.
 */
//==============================================================================
void DMA1_Stream5_IRQHandler(void)
{
        IRQ_handle(0, 5);
}

//==============================================================================
/**
 * @brief DMA1 Stream 6 IRQ.
 */
//==============================================================================
void DMA1_Stream6_IRQHandler(void)
{
        IRQ_handle(0, 6);
}

//==============================================================================
/**
 * @brief DMA1 Stream 7 IRQ.
 */
//==============================================================================
void DMA1_Stream7_IRQHandler(void)
{
        IRQ_handle(0, 7);
}

//==============================================================================
/**
 * @brief DMA2 Stream 0 IRQ.
 */
//==============================================================================
void DMA2_Stream0_IRQHandler(void)
{
        IRQ_handle(1, 0);
}

//==============================================================================
/**
 * @brief DMA2 Stream 1 IRQ.
 */
//==============================================================================
void DMA2_Stream1_IRQHandler(void)
{
        IRQ_handle(1, 1);
}

//==============================================================================
/**
 * @brief DMA2 Stream 2 IRQ.
 */
//==============================================================================
void DMA2_Stream2_IRQHandler(void)
{
        IRQ_handle(1, 2);
}

//==============================================================================
/**
 * @brief DMA2 Stream 3 IRQ.
 */
//==============================================================================
void DMA2_Stream3_IRQHandler(void)
{
        IRQ_handle(1, 3);
}

//==============================================================================
/**
 * @brief DMA2 Stream 4 IRQ.
 */
//==============================================================================
void DMA2_Stream4_IRQHandler(void)
{
        IRQ_handle(1, 4);
}

//==============================================================================
/**
 * @brief DMA2 Stream 5 IRQ.
 */
//==============================================================================
void DMA2_Stream5_IRQHandler(void)
{
        IRQ_handle(1, 5);
}

//==============================================================================
/**
 * @brief DMA2 Stream 6 IRQ.
 */
//==============================================================================
void DMA2_Stream6_IRQHandler(void)
{
        IRQ_handle(1, 6);
}

//==============================================================================
/**
 * @brief DMA2 Stream 7 IRQ.
 */
//==============================================================================
void DMA2_Stream7_IRQHandler(void)
{
        IRQ_handle(1, 7);
}

/*==============================================================================
  End of file
==============================================================================*/