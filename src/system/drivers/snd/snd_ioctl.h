/*==============================================================================
File    snd_ioctl.h

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

/**
@defgroup drv-snd SND Driver

\section drv-snd-desc Description
Driver handles ...

\section drv-snd-sup-arch Supported architectures
\li stm32f4

\section drv-snd-ddesc Details
\subsection drv-snd-ddesc-num Meaning of major and minor numbers
\todo Meaning of major and minor numbers

\subsubsection drv-snd-ddesc-numres Numeration restrictions
\todo Numeration restrictions

\subsection drv-snd-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("SND", 0, 0, "/dev/SND0-0");
@endcode
@code
driver_init("SND", 0, 1, "/dev/SND0-1");
@endcode

\subsection drv-snd-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("SND", 0, 0);
@endcode
@code
driver_release("SND", 0, 1);
@endcode

\subsection drv-snd-ddesc-cfg Driver configuration
\todo Driver configuration

\subsection drv-snd-ddesc-write Data write
\todo Data write

\subsection drv-snd-ddesc-read Data read
\todo Data read

@{
*/

#ifndef _SND_IOCTL_H_
#define _SND_IOCTL_H_

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
 *  @brief  Example IOCTL request.
 *  @param  [WR,RD] ioctl() params...
 *  @return On success 0 is returned, otherwise -1.
 *
 *  @b Example
 *  @code
    #include <sys/ioctl.h>

    //...



    //...
    @endcode
 */
#define IOCTL_SND__SET_CONFIGURATION            _IOW(SND, 0x00, SND_config_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
typedef enum {
        SND_CHANNELS__MONO,
        SND_CHANNELS__STEREO,
        _SND_CHANNELS_AMOUNT,
} SND_channels_t;

typedef enum {
        SND_SAMPLE_RATE__44100,
        SND_SAMPLE_RATE__22050,
        SND_SAMPLE_RATE__11025,
        _SND_SAMPLE_RATE_AMOUNT,
} SND_sample_rate_t;

typedef enum {
        SND_BITS_PER_SAMPLE__8,
        SND_BITS_PER_SAMPLE__16,
        SND_BITS_PER_SAMPLE__32,
        _SND_BITS_PER_SAMPLE_AMOUNT,
} SND_bits_per_sample_t;

typedef struct {
        SND_channels_t        channels;
        SND_sample_rate_t     sample_rate;
        SND_bits_per_sample_t bits_per_sample;
} SND_config_t;

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

#endif /* _SND_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
