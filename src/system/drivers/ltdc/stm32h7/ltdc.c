/*==============================================================================
File    ltdc.c

Author  Daniel Zorychta

Brief   LCD-TFT Display Controller module

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
#include "stm32h7/ltdc_cfg.h"
#include "../ltdc_ioctl.h"
#include "stm32h7xx_hal_ltdc.h"

_Static_assert(LTDC_PIXEL_FORMAT__ARGB8888 == LTDC_PIXEL_FORMAT_ARGB8888, "");
_Static_assert(LTDC_PIXEL_FORMAT__RGB888 == LTDC_PIXEL_FORMAT_RGB888, "");
_Static_assert(LTDC_PIXEL_FORMAT__RGB565 == LTDC_PIXEL_FORMAT_RGB565, "");
_Static_assert(LTDC_PIXEL_FORMAT__ARGB1555 == LTDC_PIXEL_FORMAT_ARGB1555, "");
_Static_assert(LTDC_PIXEL_FORMAT__ARGB4444 == LTDC_PIXEL_FORMAT_ARGB4444, "");
_Static_assert(LTDC_PIXEL_FORMAT__L8 == LTDC_PIXEL_FORMAT_L8, "");
_Static_assert(LTDC_PIXEL_FORMAT__AL44 == LTDC_PIXEL_FORMAT_AL44, "");
_Static_assert(LTDC_PIXEL_FORMAT__AL88 == LTDC_PIXEL_FORMAT_AL88, "");

/*==============================================================================
  Local macros
==============================================================================*/
#define BITS_IN_BYTE            8

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        u8_t major;
        u8_t minor;
        LTDC_config_t config;
        LTDC_HandleTypeDef ltdc;
        bool user_framebuffer;
        size_t framebuffer_size;
        void *framebuffer;
} LTDC_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int configure(LTDC_t *hdl, const LTDC_config_t *config);

/*==============================================================================
  Local object
==============================================================================*/
MODULE_NAME(LTDC);

static const u8_t PIXEL_FORMAT_TO_PIXEL_SIZE[] = {
        [LTDC_PIXEL_FORMAT__ARGB8888] = 4,
        [LTDC_PIXEL_FORMAT__RGB888] = 3,
        [LTDC_PIXEL_FORMAT__RGB565] = 2,
        [LTDC_PIXEL_FORMAT__ARGB1555] = 2,
        [LTDC_PIXEL_FORMAT__ARGB4444] = 2,
        [LTDC_PIXEL_FORMAT__L8] = 1,
        [LTDC_PIXEL_FORMAT__AL44] = 1,
        [LTDC_PIXEL_FORMAT__AL88] = 2,
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
API_MOD_INIT(LTDC, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        int err = EFAULT;

        if (major == 0 && minor == 0) {
                err = sys_zalloc(sizeof(LTDC_t), device_handle);
                if (!err) {
                        LTDC_t *hdl = *device_handle;
                        hdl->major = major;
                        hdl->minor = minor;

                        if (config) {
                                err = configure(hdl, config);
                        }

                        if (err) {
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
API_MOD_RELEASE(LTDC, void *device_handle)
{
        LTDC_t *hdl = device_handle;

        HAL_LTDC_DeInit(&hdl->ltdc);

        if (hdl->framebuffer and not hdl->user_framebuffer) {
                sys_free(&hdl->framebuffer);
        }

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
API_MOD_OPEN(LTDC, void *device_handle, u32_t flags)
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
API_MOD_CLOSE(LTDC, void *device_handle, bool force)
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
 * @param[out]          *wrctr                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_WRITE(LTDC,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrctr,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fattr);

        LTDC_t *hdl = device_handle;

        if (*fpos < hdl->framebuffer_size) {
                size_t to_write = min(count, hdl->framebuffer_size - *fpos);
                *wrctr = to_write;
                memcpy(hdl->framebuffer + *fpos, src, to_write);
        } else {
                *wrctr = 0;
        }

        return ESUCC;
}

//==============================================================================
/**
 * @brief Read data from device.
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *rdctr                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_MOD_READ(LTDC,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdctr,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        LTDC_t *hdl = device_handle;

        if (*fpos < hdl->framebuffer_size) {
                size_t to_read = min(count, hdl->framebuffer_size - *fpos);
                *rdctr = to_read;
                memcpy(dst, hdl->framebuffer + *fpos, to_read);
        } else {
                *rdctr = 0;
        }

        return ESUCC;
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
API_MOD_IOCTL(LTDC, void *device_handle, int request, void *arg)
{
        LTDC_t *hdl = device_handle;

        int err = EINVAL;

        switch (request) {
        case IOCTL_LTDC__CONFIGURE:
                err = configure(hdl, arg);
                break;

        case IOCTL_LTDC__GET_FEATURES: {
                LTDC_features_t *feat = arg;
                feat->bits_per_pixel = PIXEL_FORMAT_TO_PIXEL_SIZE[hdl->config.pixel_format] * BITS_IN_BYTE;
                feat->framebuffer_addr = hdl->framebuffer;
                feat->framebuffer_size = hdl->framebuffer_size;
                feat->pixel_format = hdl->config.pixel_format;
                feat->width = __LTDC_WIDTH__;
                feat->height = __LTDC_HEIGHT__;
                break;
        }

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
API_MOD_FLUSH(LTDC, void *device_handle)
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
API_MOD_STAT(LTDC, void *device_handle, struct vfs_dev_stat *device_stat)
{
        LTDC_t *hdl = device_handle;

        device_stat->st_size = hdl->framebuffer_size;

        return ESUCC;
}

//==============================================================================
/**
 * @brief  LTDC configuration.
 *
 * @param  hdl          device handle
 * @param  config       device configuration
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
static int configure(LTDC_t *hdl, const LTDC_config_t *config)
{
        int err = EINVAL;

        if (config->pixel_format >= _LTDC_PIXEL_FORMAT__AMOUNT) {
                dev_dbg(hdl, "invalid pixel format");
                return EINVAL;
        }

        hdl->framebuffer_size = __LTDC_WIDTH__ * __LTDC_HEIGHT__
                              * PIXEL_FORMAT_TO_PIXEL_SIZE[config->pixel_format];

        if (config->framebuffer_addr) {
                if (config->framebuffer_size == 0) {
                        printk("zero framebuffer size");
                        return EINVAL;

                } else if (config->framebuffer_size < hdl->framebuffer_size) {
                        printk("too small framebuffer");
                        return EINVAL;
                }
        }

        HAL_LTDC_DeInit(&hdl->ltdc);

        if (hdl->framebuffer and not hdl->user_framebuffer) {
                sys_free(&hdl->framebuffer);
        }

        if (config->framebuffer_addr) {
                hdl->framebuffer = config->framebuffer_addr;
                hdl->user_framebuffer = true;

        } else {
                err = sys_zalloc2(sizeof(LTDC_t), config->mem_region_name,
                                  _MM_FLAG__DMA_CAPABLE, _MM_FLAG__DMA_CAPABLE,
                                  &hdl->framebuffer);
                if (err) {
                        return err;
                }

                hdl->user_framebuffer = false;

                dev_dbg(hdl, "allocated framebuffer @ %p-%p",
                        hdl->framebuffer, cast(uint32_t, hdl->framebuffer) + hdl->framebuffer_size);
        }

        hdl->ltdc.Instance = LTDC;
        hdl->ltdc.Init.HSPolarity = __LTDC_HSPOLARITY__;
        hdl->ltdc.Init.VSPolarity = __LTDC_VSPOLARITY__;
        hdl->ltdc.Init.DEPolarity = __LTDC_DEPOLARITY__;
        hdl->ltdc.Init.PCPolarity = __LTDC_PCPOLARITY__;

        hdl->ltdc.Init.HorizontalSync     = __LTDC_HSYNC__ - 1U;
        hdl->ltdc.Init.AccumulatedHBP     = (__LTDC_HSYNC__ + (__LTDC_HBP__ - 0U) - 1U);
        hdl->ltdc.Init.AccumulatedActiveW = __LTDC_HSYNC__ + __LTDC_WIDTH__ + __LTDC_HBP__ - 1U;
        hdl->ltdc.Init.TotalWidth         = __LTDC_HSYNC__ + __LTDC_WIDTH__ + (__LTDC_HBP__ - 0U) + __LTDC_HFP__ - 1U;
        hdl->ltdc.Init.VerticalSync       = __LTDC_VSYNC__ - 1U;
        hdl->ltdc.Init.AccumulatedVBP     = __LTDC_VSYNC__ + __LTDC_VBP__ - 1U;
        hdl->ltdc.Init.AccumulatedActiveH = __LTDC_VSYNC__ + __LTDC_HEIGHT__ + __LTDC_VBP__ - 1U;
        hdl->ltdc.Init.TotalHeigh         = __LTDC_VSYNC__ + __LTDC_HEIGHT__ + __LTDC_VBP__ + __LTDC_VFP__ - 1U;

        hdl->ltdc.Init.Backcolor.Red   = config->background_color_R;
        hdl->ltdc.Init.Backcolor.Green = config->background_color_G;
        hdl->ltdc.Init.Backcolor.Blue  = config->background_color_B;

        err = HAL_LTDC_Init(&hdl->ltdc);
        if (!err) {
                LTDC_LayerCfgTypeDef layer_cfg;
                layer_cfg.WindowX0 = 0;
                layer_cfg.WindowX1 = __LTDC_WIDTH__;
                layer_cfg.WindowY0 = 0;
                layer_cfg.WindowY1 = __LTDC_HEIGHT__;
                layer_cfg.PixelFormat = config->pixel_format;
                layer_cfg.Alpha = 255;
                layer_cfg.Alpha0 = 0;
                layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
                layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
                layer_cfg.FBStartAdress = cast(u32_t, hdl->framebuffer);
                layer_cfg.ImageWidth = layer_cfg.WindowX1 - layer_cfg.WindowX0;
                layer_cfg.ImageHeight = layer_cfg.WindowY1 - layer_cfg.WindowY0;
                layer_cfg.Backcolor.Blue = 0;
                layer_cfg.Backcolor.Green = 0;
                layer_cfg.Backcolor.Red = 0;
                err = HAL_LTDC_ConfigLayer(&hdl->ltdc, &layer_cfg, 0);
        }

        if (err) {
                if (not hdl->user_framebuffer) {
                        sys_free(&hdl->framebuffer);
                }
        } else {
                hdl->config = *config;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Initialize LCD peripheral.
 *
 * @param  ltdc         lld driver instance
 */
//==============================================================================
void HAL_LTDC_MspInit(LTDC_HandleTypeDef *ltdc)
{
        UNUSED_ARG1(ltdc);

        SET_BIT(RCC->APB3ENR, RCC_APB3ENR_LTDCEN);
        SET_BIT(RCC->APB3RSTR, RCC_APB3RSTR_LTDCRST);
        CLEAR_BIT(RCC->APB3RSTR, RCC_APB3RSTR_LTDCRST);
}

//==============================================================================
/**
 * @brief  Deinitialize LCD peripheral.
 *
 * @param  ltdc         lld driver instance
 */
//==============================================================================
void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef *ltdc)
{
        UNUSED_ARG1(ltdc);

        SET_BIT(RCC->APB3RSTR, RCC_APB3RSTR_LTDCRST);
        CLEAR_BIT(RCC->APB3RSTR, RCC_APB3RSTR_LTDCRST);
        CLEAR_BIT(RCC->APB3ENR, RCC_APB3ENR_LTDCEN);
}

/*==============================================================================
  End of file
==============================================================================*/
