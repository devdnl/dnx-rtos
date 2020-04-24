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
#include "stm32f4/dma_ddi.h"
#include "stm32f4/stm32f4xx.h"
#include "../dma_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define DMA_COUNT                       2
#define STREAM_COUNT                    8

#define DMAD(major, stream, ID)         (((ID) << 4) | (((stream) & 7) << 1) | ((major) & 1))
#define GETMAJOR(DMAD)                  (((DMAD) >> 0) & 1)
#define GETSTREAM(DMAD)                 (((DMAD) >> 1) & 7)

#define M2M_TRANSFER_TIMEOUT            5000

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        DMA_TypeDef        *DMA;
        DMA_Stream_TypeDef *stream[STREAM_COUNT];
        IRQn_Type           IRQn[STREAM_COUNT];
} DMA_HW_t;

typedef struct {
        void           *arg;
        _DMA_cb_t       callback;
        _DMA_cb_t       cb_next;
        u32_t           dmad;
        bool            release;
} DMA_RT_stream_t;

typedef struct {
        DMA_RT_stream_t stream[STREAM_COUNT];
        queue_t       **queue;
        u32_t           ID_cnt;
        u8_t            major;
} DMA_RT_t;

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
                .DMA       = DMA1,
                .stream[0] = DMA1_Stream0,
                .stream[1] = DMA1_Stream1,
                .stream[2] = DMA1_Stream2,
                .stream[3] = DMA1_Stream3,
                .stream[4] = DMA1_Stream4,
                .stream[5] = DMA1_Stream5,
                .stream[6] = DMA1_Stream6,
                .stream[7] = DMA1_Stream7,
                .IRQn[0]   = DMA1_Stream0_IRQn,
                .IRQn[1]   = DMA1_Stream1_IRQn,
                .IRQn[2]   = DMA1_Stream2_IRQn,
                .IRQn[3]   = DMA1_Stream3_IRQn,
                .IRQn[4]   = DMA1_Stream4_IRQn,
                .IRQn[5]   = DMA1_Stream5_IRQn,
                .IRQn[6]   = DMA1_Stream6_IRQn,
                .IRQn[7]   = DMA1_Stream7_IRQn,
        },
        {
                .DMA       = DMA2,
                .stream[0] = DMA2_Stream0,
                .stream[1] = DMA2_Stream1,
                .stream[2] = DMA2_Stream2,
                .stream[3] = DMA2_Stream3,
                .stream[4] = DMA2_Stream4,
                .stream[5] = DMA2_Stream5,
                .stream[6] = DMA2_Stream6,
                .stream[7] = DMA2_Stream7,
                .IRQn[0]   = DMA2_Stream0_IRQn,
                .IRQn[1]   = DMA2_Stream1_IRQn,
                .IRQn[2]   = DMA2_Stream2_IRQn,
                .IRQn[3]   = DMA2_Stream3_IRQn,
                .IRQn[4]   = DMA2_Stream4_IRQn,
                .IRQn[5]   = DMA2_Stream5_IRQn,
                .IRQn[6]   = DMA2_Stream6_IRQn,
                .IRQn[7]   = DMA2_Stream7_IRQn,
        },
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
 * @param[in ]            config               optional module configuration
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_INIT(DMA, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG1(config);

        int err = EFAULT;

        if (major < DMA_COUNT && minor == 0) {
                err = sys_zalloc(sizeof(DMA_RT_t), device_handle);
                if (!err) {
                        /*
                         * Only the DMA2 controller is able to perform
                         * memory-to-memory transfers. It is a peripheral limitation.
                         * Allocating memory for memory-to-memory semaphores.
                         */
                        if (major == 1) {
                                DMA_RT_t *hdl = *device_handle;
                                err = sys_zalloc(sizeof(queue_t*) * STREAM_COUNT,
                                                 cast(void*, &hdl->queue));
                                if (err) {
                                        sys_free(device_handle);
                                }
                        }

                        if (!err) {
                                /*
                                 * NOTE: DMA1EN and DMA2EN are localized in this same
                                 *       register and DMA2EN is shifted by 1 bit relative
                                 *       to DMA1EN.
                                 */
                                RCC->AHB1ENR |= (RCC_AHB1ENR_DMA1EN << major);

                                DMA_RT[major] = *device_handle;
                                DMA_RT[major]->major = major;
                                DMA_RT[major]->ID_cnt++;
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
 * @param [in]  stream        stream number.
 *
 * @return On success DMA descriptor number, otherwise 0.
 */
//==============================================================================
u32_t _DMA_DDI_reserve(u8_t major, u8_t stream)
{
        int dmad = 0;

        if (DMA_RT[major] == NULL) {
                printk("DMA%d: not initialized", major + 1);
                return dmad;
        }

        if (major < DMA_COUNT && stream < STREAM_COUNT) {
                sys_critical_section_begin();
                {
                        if (DMA_RT[major]->stream[stream].dmad == 0) {
                                u32_t ID = DMA_RT[major]->ID_cnt++;

                                dmad = DMAD(major, stream, ID);

                                DMA_RT[major]->stream[stream].dmad = dmad;

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
                DMA_RT_stream_t *RT_stream = &DMA_RT[GETMAJOR(dmad)]->stream[GETSTREAM(dmad)];

                if (RT_stream->dmad == dmad) {

                        DMA_Stream_TypeDef *DMA_Stream = DMA_HW[GETMAJOR(dmad)].stream[GETSTREAM(dmad)];

                        RT_stream->callback = NULL;
                        DMA_Stream->CR      = 0;

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
           && (config->MA[0] || config->MA[1])) {

                DMA_RT_stream_t    *RT_stream  = &DMA_RT[GETMAJOR(dmad)]->stream[GETSTREAM(dmad)];
                DMA_Stream_TypeDef *DMA_Stream =  DMA_HW[GETMAJOR(dmad)].stream[GETSTREAM(dmad)];
                IRQn_Type           IRQn       =  DMA_HW[GETMAJOR(dmad)].IRQn[GETSTREAM(dmad)];

                if (RT_stream->dmad == dmad) {
                        DMA_Stream->CR = 0;
                        clear_DMA_IRQ_flags(GETMAJOR(dmad), GETSTREAM(dmad));

                        DMA_Stream->M0AR = config->MA[0];
                        DMA_Stream->M1AR = config->MA[1];
                        DMA_Stream->NDTR = config->NDT;
                        DMA_Stream->PAR  = config->PA;
                        DMA_Stream->CR   = config->CR & ~DMA_SxCR_EN;
                        DMA_Stream->FCR  = config->FC;

                        RT_stream->arg      = config->arg;
                        RT_stream->callback = config->callback;
                        RT_stream->cb_next  = config->cb_next;
                        RT_stream->release  = config->release;

                        clear_DMA_IRQ_flags(GETMAJOR(dmad), GETSTREAM(dmad));
                        NVIC_SetPriority(IRQn, config->IRQ_priority);
                        NVIC_EnableIRQ(IRQn);

                        SET_BIT(DMA_Stream->CR, DMA_SxCR_TCIE | DMA_SxCR_TEIE);
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
        DMA_RT_t *hdl = DMA_RT[1];
        if (!hdl || !dst || !src || !size) {
                return EINVAL;
        }

        int err = EBUSY;

        u32_t dmad   = 0;
        u8_t  stream = 0;

        for (; stream < STREAM_COUNT; stream++) {
                dmad = _DMA_DDI_reserve(hdl->major, stream);
                if (dmad) break;
        }

        if (dmad && (hdl->queue[stream] == NULL)) {
                err = sys_queue_create(1, sizeof(int), &hdl->queue[stream]);
                if (err) {
                        _DMA_DDI_release(dmad);
                        return err;
                }
        }

        if (dmad) {
                u32_t PMSIZE, NDT;

                if (  ((size & 3) == 0)
                   && ((cast(u32_t, src) & 3) == 0)
                   && ((cast(u32_t, dst) & 3) == 0) ) {

                        PMSIZE = (2 << DMA_SxCR_PSIZE_Pos)
                               | (2 << DMA_SxCR_MSIZE_Pos);

                        NDT = size / 4;

                } else if (  ((size & 1) == 0)
                          && ((cast(u32_t, src) & 1) == 0)
                          && ((cast(u32_t, dst) & 1) == 0) ) {

                        PMSIZE = (1 << DMA_SxCR_PSIZE_Pos)
                               | (1 << DMA_SxCR_MSIZE_Pos);

                        NDT = size / 2;

                } else {
                        PMSIZE = 0;
                        NDT    = size;
                }

                if (NDT <= UINT16_MAX) {
                        _DMA_DDI_config_t config;
                        config.arg      = hdl->queue[stream];
                        config.callback = M2M_callback;
                        config.cb_next  = NULL;
                        config.release  = true;
                        config.PA       = cast(u32_t, src);
                        config.MA[0]    = cast(u32_t, dst);
                        config.NDT      = NDT;
                        config.FC       = DMA_SxFCR_FTH_0 | DMA_SxFCR_FS_2;
                        config.CR       = PMSIZE | (2 << DMA_SxCR_DIR_Pos)
                                         | DMA_SxCR_MINC | DMA_SxCR_PINC;
                        config.IRQ_priority = __CPU_DEFAULT_IRQ_PRIORITY__;

                        err = _DMA_DDI_transfer(dmad, &config);
                        if (!err) {
                                int ferr = EIO;
                                err = sys_queue_receive(hdl->queue[stream], &ferr,
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
        DMA_Stream_TypeDef *DMA_Stream = DMA_HW[major].stream[stream];
        DMA_RT_stream_t    *RT_stream  = &DMA_RT[major]->stream[stream];

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

        if (RT_stream->callback) {
                yield = RT_stream->callback(DMA_HW[major].stream[stream],
                                            SR & 0x3F, RT_stream->arg);
        }

        if (!(DMA_Stream->CR & DMA_SxCR_CIRC)) {
                DMA_Stream->CR = 0;

                RT_stream->callback = NULL;

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
                while (DMA_Stream->CR & DMA_SxCR_EN);
        }

        clear_DMA_IRQ_flags(major, stream);

        if (RT_stream->cb_next) {
                yield |= RT_stream->cb_next(DMA_HW[major].stream[stream],
                                            SR & 0x3F, RT_stream->arg);
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
