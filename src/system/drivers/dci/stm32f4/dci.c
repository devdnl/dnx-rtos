/*==============================================================================
File     dcmi.c

Author   Daniel Zorychta

Brief    Digital Camera Interface Driver

         Copyright (C) 2017 Daniel Zorychta <daniel.zorychta>

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
#include "stm32f4/dci_cfg.h"
#include "stm32f4/stm32f4xx.h"
#include "stm32f4/dma_ddi.h"
#include "../dci_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define DMA_CHANNEL             1
#define DMA_STREAM_PRI          1
#define DMA_STREAM_AUX          7
#define DMA_MAX_TRANSFER        65535
#define DMA_TIMEOUT             2000

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        sem_t     *event;       // event
        dev_lock_t lock;        // device lock
        u16_t      lines;       // number of lines to capture
        u16_t      tleft;       // number of DMA transfers to do
        u16_t      TSIZEW;      // DMA transfer size (in Words)
        u16_t      TCOUNT;      // DMA transfers count
} DCI_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static bool DMA_callback(DMA_Stream_TypeDef *stream, u8_t SR, void *arg);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(DCI);

static DCI_t *DCI;

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
API_MOD_INIT(DCI, void **device_handle, u8_t major, u8_t minor)
{
        int err = EFAULT;

        if (major == 0 && minor == 0) {
                err = sys_zalloc(sizeof(DCI_t), device_handle);
                if (!err) {
                        DCI = *device_handle;
                        sys_device_unlock(&DCI->lock, true);

                        err = sys_semaphore_create(1, 0, &DCI->event);
                        if (!err) {

                                SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_DCMIEN);

                                DCMI->CR = _DCI_EDM
                                         | _DCI_FCRC
                                         | _DCI_VSPOL
                                         | _DCI_HSPOL
                                         | _DCI_PCKPOL
                                         | _DCI_ESS
                                         | _DCI_JPEG
                                         | _DCI_CROP
                                         | DCMI_CR_CM;

                                DCMI->ICR = 0xFF;

                                DCMI->ESCR = (_DCI_FEC <<  0)
                                           | (_DCI_LEC <<  8)
                                           | (_DCI_LSC << 16)
                                           | (_DCI_FSC << 24);

                                DCMI->ESUR = (_DCI_FSU <<  0)
                                           | (_DCI_LSU <<  8)
                                           | (_DCI_LEU << 16)
                                           | (_DCI_FEU << 24);

                                DCMI->CWSTRTR = (_DCI_CROP_START_X <<  0)
                                              | (_DCI_CROP_START_Y << 16);

                                DCMI->CWSIZER = (_DCI_CROP_HEIGHT <<  0)
                                              | (_DCI_CROP_WIDTH  << 16);

                                SET_BIT(DCMI->CR, DCMI_CR_ENABLE);

                                // calculate max DMA transfer size and number of transfers
                                size_t tc   = (sizeof(u32_t) * (DMA_MAX_TRANSFER + 1)
                                            / (_DCI_CAM_RES_X * _DCI_BYTES_PER_PIXEL));

                                DCI->TSIZEW = (_DCI_CAM_RES_X * _DCI_BYTES_PER_PIXEL)
                                            / sizeof(u32_t);

                                DCI->TCOUNT = _DCI_CAM_RES_Y;

                                for (int i = tc - 1; i > 0; i--) {
                                        if (_DCI_CAM_RES_Y % i == 0) {
                                                DCI->TSIZEW = ((_DCI_CAM_RES_X * _DCI_BYTES_PER_PIXEL) * i)
                                                            / sizeof(u32_t);

                                                DCI->TCOUNT = _DCI_CAM_RES_Y / i;
                                                break;
                                        }
                                }
                        } else {
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
API_MOD_RELEASE(DCI, void *device_handle)
{
        DCI_t *hdl = device_handle;

        int err = sys_device_lock(&hdl->lock);
        if (!err) {
                SET_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_DCMIRST);
                CLEAR_BIT(RCC->AHB2RSTR, RCC_AHB2RSTR_DCMIRST);
                CLEAR_BIT(RCC->AHB2ENR, RCC_AHB2ENR_DCMIEN);

                sys_free(&device_handle);
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
API_MOD_OPEN(DCI, void *device_handle, u32_t flags)
{
        UNUSED_ARG1(flags);

        DCI_t *hdl = device_handle;

        return sys_device_lock(&hdl->lock);
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
API_MOD_CLOSE(DCI, void *device_handle, bool force)
{
        DCI_t *hdl = device_handle;

        return sys_device_unlock(&hdl->lock, force);
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
API_MOD_WRITE(DCI,
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
API_MOD_READ(DCI,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        DCI_t *hdl = device_handle;

        size_t frame_size = _DCI_CROP
                          ? (_DCI_CROP_WIDTH * _DCI_CROP_HEIGHT * _DCI_BYTES_PER_PIXEL)
                          : (_DCI_CAM_RES_X  * _DCI_CAM_RES_Y   * _DCI_BYTES_PER_PIXEL);

        if (count != frame_size) {
                return ENOTSUP;
        }

        if (*fpos != 0) {
                return ESPIPE;
        }

        u32_t dmad = _DMA_DDI_reserve(1, DMA_STREAM_PRI);
        if (dmad == 0) {
                dmad = _DMA_DDI_reserve(1, DMA_STREAM_AUX);
                if (dmad == 0) {
                        return EBUSY;
                }
        }

        _DMA_DDI_config_t config;
        config.MA[0]    = cast(u32_t, dst);
        config.MA[1]    = cast(u32_t, dst) + (hdl->TSIZEW * sizeof(u32_t));
        config.PA       = cast(u32_t, &DCMI->DR);
        config.NDT      = hdl->TSIZEW;
        config.arg      = DCI;
        config.callback = DMA_callback;
        config.release  = false;
        config.FC       = DMA_SxFCR_FTH_1 | DMA_SxFCR_FTH_0 | DMA_SxFCR_FS_2 | DMA_SxFCR_DMDIS;
        config.CR       = DMA_CHANNEL << DMA_SxCR_CHSEL_Pos
                        | DMA_SxCR_PL_1
                        | DMA_SxCR_MSIZE_0
                        | DMA_SxCR_PSIZE_1
                        | DMA_SxCR_MINC | DMA_SxCR_DBM
                        | DMA_SxCR_CIRC;

        hdl->tleft = hdl->TCOUNT - 1;

        int err = _DMA_DDI_transfer(dmad, &config);
        if (!err) {
                SET_BIT(DCMI->CR, DCMI_CR_CAPTURE);

                err = sys_semaphore_wait(hdl->event, DMA_TIMEOUT);
                if (!err) {
                        *rdcnt = frame_size;
                } else {
                        CLEAR_BIT(DCMI->CR, DCMI_CR_CAPTURE | DCMI_CR_ENABLE);
                        SET_BIT(DCMI->CR, DCMI_CR_ENABLE);
                }
                _DMA_DDI_release(dmad);
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
API_MOD_IOCTL(DCI, void *device_handle, int request, void *arg)
{
        UNUSED_ARG1(device_handle);

        int err = EINVAL;

        switch (request) {
        case IOCTL_DCI__GET_PARAMS:
                cast(DCI_params_t*, arg)->x_resolution    = _DCI_CAM_RES_X;
                cast(DCI_params_t*, arg)->y_resolution    = _DCI_CAM_RES_Y;
                cast(DCI_params_t*, arg)->bytes_per_pixel = _DCI_BYTES_PER_PIXEL;
                cast(DCI_params_t*, arg)->JPEG_mode       = _DCI_JPEG;
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
API_MOD_FLUSH(DCI, void *device_handle)
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
API_MOD_STAT(DCI, void *device_handle, struct vfs_dev_stat *device_stat)
{
        UNUSED_ARG1(device_handle);

        if (_DCI_CROP) {
                device_stat->st_size = _DCI_CROP_WIDTH * _DCI_CROP_HEIGHT * _DCI_BYTES_PER_PIXEL;

        } else {
                device_stat->st_size = _DCI_CAM_RES_X * _DCI_CAM_RES_Y * _DCI_BYTES_PER_PIXEL;
        }

        return ESUCC;
}

//==============================================================================
/**
 * @brief DCMI DMA finished callback.
 */
//==============================================================================
static bool DMA_callback(DMA_Stream_TypeDef *stream, u8_t SR, void *arg)
{
        DCI_t *hdl = arg;

        bool yield = false;

        if (SR & DMA_SR_TCIF) {
                if (hdl->tleft > 0) {
                        if (stream->CR & DMA_SxCR_CT) {
                                stream->M0AR += 2 * (hdl->TSIZEW * sizeof(u32_t));
                        } else {
                                stream->M1AR += 2 * (hdl->TSIZEW * sizeof(u32_t));
                        }

                        hdl->tleft--;
                } else {
                        stream->CR = 0;
                        sys_semaphore_signal_from_ISR(hdl->event, &yield);
                }
        }

        if (SR & DMA_SR_TEIF) {
                sys_semaphore_signal_from_ISR(hdl->event, &yield);
        }

        return yield;
}

//==============================================================================
/**
 * @brief DCMI IRQ.
 */
//==============================================================================
void DCMI_IRQHandler(void)
{
        /* not used yet */
}

/*==============================================================================
  End of file
==============================================================================*/
