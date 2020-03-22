/*==============================================================================
File    pwm_ioctl.h

Author  Daniel Zorychta

Brief   PWM driver

        Copyright (C) 2018 Daniel Zorychta <daniel.zorychta@gmail.com>

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
@defgroup drv-pwm PWM Driver

\section drv-pwm-desc Description
Driver handles timer in PWM mode. When timer is used by this driver then is not
possible to use timer as e.g. counter or in other usages.

\section drv-pwm-sup-arch Supported architectures
\li stm32f4

\section drv-pwm-ddesc Details
\subsection drv-pwm-ddesc-num Meaning of major and minor numbers
Major number select timer peripheral. Minor number select channel number.
@note In STM32Fx CPUs TIM1,2,... are accessible by major number 0,1,... . This
same situation is in case of channels.

\subsubsection drv-pwm-ddesc-numres Numeration restrictions
Major number limit depends on number of timers accessible in the microcontroller.
Maximum value of minor number depends on number of channels of timer.

\subsection drv-pwm-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("PWM", 0, 0, "/dev/pwm-tim1-ch1");
@endcode
@code
driver_init("PWM", 0, 1, "/dev/pwm-tim1-ch2");
@endcode

\subsection drv-pwm-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("PWM", 0, 0);
@endcode
@code
driver_release("PWM", 0, 1);
@endcode

\subsection drv-pwm-ddesc-cfg Driver configuration
Driver can be configured by using IOCTL_PWM__SET_CONFIGURATION or IOCTL_PWM__AUTO_ADJUST.
The first one set timer configuration defined by user. The second one automatically
set timer configuration to achieve output frequency and <i>top</i> value defined by user.

\subsection drv-pwm-ddesc-write Data write
Write data to set PWM output value.

\subsection drv-pwm-ddesc-read Data read
Read data to get PWM output value.

\subsection drv-pwm-ddesc-cfg Driver configuration
To configure driver the ioctl() function should be used:

@code
#include <stdio.h>
#include <sys/ioctl.h>

// ...

f = fopen("/dev/pwm", "r+");
if (f) {
        // PWM output frequency set to 50Hz, 0-1000 PWM duty range.
        // Configure entire timer not a channel!
        static const PWM_auto_adjust_t adj = {.frequency = 50, .reload = 999};
        ioctl(fileno(f), IOCTL_PWM__AUTO_ADJUST, &adj);
        fclose(f);
}

// ...
@endcode

or

@code
#include <stdio.h>
#include <sys/ioctl.h>

// ...

f = fopen("/dev/pwm", "r+");
if (f) {
        // PWM output frequency set to fpwm = ftim / prescaler / (reload + 1).
        // Configure entire timer not a channel!
        static const PWM_config_t cfg = {.reload = 999, .prescaler = 128};
        ioctl(fileno(f), IOCTL_PWM__SET_CONFIGURATION, &cfg);
        fclose(f);
}

// ...
@endcode

@{
*/

#ifndef _PWM_IOCTL_H_
#define _PWM_IOCTL_H_

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
 *  @brief  Set PWM configuration.
 *  @param  [WR] PWM_const_t*   configuration object
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_PWM__SET_CONFIGURATION    _IOW(PWM, 0x00, const PWM_config_t*)

/**
 *  @brief  Return base frequency of timer.
 *  @param  [RD] uint32_t*      frequency in Hz
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_PWM__GET_BASE_FREQUENCY   _IOR(PWM, 0x01, uint32_t*)

/**
 *  @brief  Set PWM value (duty).
 *  @param  [WR] uint16_t*      PWM value
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_PWM__SET_VALUE            _IOW(PWM, 0x02, uint16_t*)

/**
 *  @brief  Get PWM value (duty).
 *  @param  [RD] uint16_t*      PWM value
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_PWM__GET_VALUE            _IOR(PWM, 0x03, uint16_t*)

/**
 *  @brief  Get PWM reload (top).
 *  @param  [RD] uint16_t*      PWM reload
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_PWM__GET_RELOAD           _IOR(PWM, 0x04, uint16_t*)

/**
 *  @brief  Auto adjust of PWM output frequency.
 *  @param  [WR] PWM_auto_adjust_t*     auto adjust configuration object
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_PWM__AUTO_ADJUST          _IOW(PWM, 0x05, const PWM_auto_adjust_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type represent PWM configuration object.
 */
typedef struct {
        uint16_t reload;        /*!< Reload value (top) */
        uint16_t prescaler;     /*!< Prescaler (divider) */
} PWM_config_t;

/**
 * Type represent PWM auto adjust configuration object.
 */
typedef struct {
        uint32_t frequency;     /*!< Target PWM frequency in Hz */
        uint16_t reload;        /*!< PWM reload value (top) */
} PWM_auto_adjust_t;

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

#endif /* _PWM_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
