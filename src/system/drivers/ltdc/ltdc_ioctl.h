/*==============================================================================
File    ltdc_ioctl.h

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

/**
@defgroup drv-ltdc LTDC Driver

\section drv-ltdc-desc Description
Driver handles LTDC LCD-TFT Display Controller.

\section drv-ltdc-sup-arch Supported architectures
\li stm32h7

\section drv-ltdc-ddesc Details
\subsection drv-ltdc-ddesc-num Meaning of major and minor numbers
Only major-minor 0-0 numbers can be used.

\subsection drv-ltdc-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("LTDC", 0, 0, "/dev/LTDC0-0");
@endcode
@code
driver_init("LTDC", 0, 1, "/dev/LTDC0-1");
@endcode

\subsection drv-ltdc-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("LTDC", 0, 0);
@endcode
@code
driver_release("LTDC", 0, 1);
@endcode

\subsection drv-ltdc-ddesc-cfg Driver configuration
Driver can be configured by using driver_init2() or IOCTL_LTDC__CONFIGURE ioctl()
request.

\subsection drv-ltdc-ddesc-write Data write
Writting data to created device modyfi framebuffer pixels.

\subsection drv-ltdc-ddesc-read Data read
Device can be read as regular file.

@{
*/

#ifndef _LTDC_IOCTL_H_
#define _LTDC_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/ioctl_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 *  @brief  Configuration IOCTL request.
 *  @param  [WR] LTDC_config_t configuration
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_LTDC__CONFIGURE           _IOW(LTDC, 0x00, const LTDC_config_t*)

/**
 *  @brief  Get driver features.
 *  @param  [RD] LTDC_features_t features
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_LTDC__GET_FEATURES        _IOR(LTDC, 0x01, LTDC_features_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
typedef enum {
        LTDC_PIXEL_FORMAT__ARGB8888,
        LTDC_PIXEL_FORMAT__RGB888,
        LTDC_PIXEL_FORMAT__RGB565,
        LTDC_PIXEL_FORMAT__ARGB1555,
        LTDC_PIXEL_FORMAT__ARGB4444,
        LTDC_PIXEL_FORMAT__L8,
        LTDC_PIXEL_FORMAT__AL44,
        LTDC_PIXEL_FORMAT__AL88,
        _LTDC_PIXEL_FORMAT__AMOUNT,
} LTDC_pixel_format_t;

typedef struct {
        const char *mem_region_name;            //!< memory region used to create framebuffer (optional)
        void *framebuffer_addr;                 //!< framebuffer address given by user (optional)
        uint32_t framebuffer_size;              //!< framebuffer size in bytes (optional)
        LTDC_pixel_format_t pixel_format;       //!< pixel format
        uint8_t background_color_R;             //!< framebuffer background color red
        uint8_t background_color_G;             //!< framebuffer background color green
        uint8_t background_color_B;             //!< framebuffer background color blue
} LTDC_config_t;

typedef struct {
        uint16_t width;
        uint16_t height;
        void *framebuffer_addr;
        uint32_t framebuffer_size;
        LTDC_pixel_format_t pixel_format;
        uint8_t bits_per_pixel;
} LTDC_features_t;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _LTDC_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
