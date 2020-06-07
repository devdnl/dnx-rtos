/*=========================================================================*//**
@file    irq_ioctl.h

@author  Daniel Zorychta

@brief   This driver support external interrupts.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==========================================================================*/

/**
@defgroup drv-irq IRQ Driver

\section drv-irq-desc Description
Driver handles external IRQ controller.

\section drv-irq-sup-arch Supported architectures
\li stm32f1
\li stm32f4

\section drv-irq-ddesc Details
\subsection drv-irq-ddesc-num Meaning of major and minor numbers
The major device number select IRQ controller. The most microcontrollers have
only one IRQ controller so the major number in the most cases should be set to 0
(e.g. stm32f1 architecture). The minor number selects IRQ line. For stm32f1
architecture range is from 0 to 15.

\subsubsection drv-irq-ddesc-numres Numeration restrictions
Number of IRQ controllers and IRQ lines determines range of major and minor
numbers.

\subsection drv-irq-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("IRQ", 0, 0, "/dev/button0");
@endcode
@code
driver_init("IRQ", 0, 1, "/dev/button1");
@endcode

\subsection drv-irq-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("IRQ", 0, 0);
@endcode
@code
driver_release("IRQ", 0, 1);
@endcode

\subsection drv-irq-ddesc-cfg Driver configuration
Driver can be configured by using Configtool or ioctl() function. Configuration
example:
\code
#include <stdio.h>
#include <sys/ioctl.h>

// ...

FILE *irq = fopen("/dev/button0", "r+");
if (irq) {
        static const IRQ_config_t cfg = IRQ_CONFIG__TRIGGER_ON_FALLING_EDGE;
        if (ioctl(fileno(irq), IOCTL_IRQ__CONFIGURE, &cfg) == 0) {
                puts("Configuration success");
        } else {
                perror("ioctl()");
        }

        fclose(irq);
} else {
        perror("/dev/button0");
}

// ...

\endcode

\subsection drv-irq-ddesc-write Data write
Operation not supported.

\subsection drv-irq-ddesc-read Data read
Operation not supported.

\subsection drv-irq-ddesc-catch IRQ catch
Each application can catch selected interrupt. In this case the ioctl() function
should be used. Example:
\code
#include <stdio.h>
#include <sys/ioctl.h>

// ...

FILE *irq = fopen("/dev/button0", "r+");
if (!irq) {
        abort();
}

// ...

// interrupt catch
u32_t timeout = 2000; // 2s timeout
if (ioctl(fileno(irq), IOCTL_IRQ__CATCH, &timeout) == 0) {
        puts("Interrupt caught!");

        // ...

} else {
        puts("Interrupt timeout!");

        // ...
}


// ...

\endcode

\subsection drv-irq-ddesc-trigger IRQ software trigger
Not all architectures are able to trigger external interrupt by software. If
selected architecture can do that the following example is valid:
\code
#include <stdio.h>
#include <sys/ioctl.h>

// ...

FILE *irq = fopen("/dev/button0", "r+");
if (!irq) {
        abort();
}

// ...

// interrupt trigger
if (ioctl(fileno(irq), IOCTL_IRQ__TRIGGER) == 0) {
        puts("Interrupt triggered!");

        // ...

} else {
        puts("Error occurred");
}

// ...

\endcode
@{
*/

#ifndef _IRQ_IOCTL_H_
#define _IRQ_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type represent IRQ trigger selecteion.
 */
typedef enum {
        IRQ_CONFIG__IRQ_DISABLED,           //!< IRQ disabled.
        IRQ_CONFIG__TRIGGER_ON_FALLING_EDGE,//!< IRQ triggered on falling edge.
        IRQ_CONFIG__TRIGGER_ON_RISING_EDGE, //!< IRQ triggered on rising edge.
        IRQ_CONFIG__TRIGGER_ON_BOTH_EDGES,  //!< IRQ triggered on both edges.
        IRQ_CONFIG__TRIGGER_ON_LOW_LEVEL,   //!< IRQ triggered on low level.
        IRQ_CONFIG__TRIGGER_ON_HIGH_LEVEL,  //!< IRQ triggered on high level.
} IRQ_config_t;

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief  Wait for selected interrupt number
 * @param  [WR] @ref u32_t*       timeout in milliseconds
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_IRQ__CATCH                _IOW(IRQ, 0, const u32_t*)

/**
 * @brief  Software interrupt trigger
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_IRQ__TRIGGER              _IO(IRQ, 1)

/**
 * @brief  Set IRQ configuration
 * @param  [WR] @ref IRQ_config_t * IRQ trigger configuration
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_IRQ__CONFIGURE            _IOW(IRQ, 2, const IRQ_config_t*)

/**
 * @brief  Get number of interrupt occurence
 * @param  [RD] @ref irq_counter_t*       number of interrupt occurence
 * @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_IRQ__GET_COUNTER          _IOR(IRQ, 3, u32_t*)

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

#endif /* _IRQ_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
