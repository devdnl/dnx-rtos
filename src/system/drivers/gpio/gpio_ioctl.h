/*=========================================================================*//**
@file    gpio_ioctl.h

@author  Daniel Zorychta

@brief   This driver support GPIO ioctl request codes.

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
@defgroup drv-gpio GPIO Driver

\section drv-gpio-desc Description
Driver handles GPIO peripheral.

\section drv-gpio-sup-arch Supported architectures
\li stm32f1
\li stm32f4
\li stm32f7
\li efr32

\section drv-gpio-ddesc Details
\subsection drv-gpio-ddesc-num Meaning of major and minor numbers
Some manufactures enumerate devices starting from 1 instead of 0 (e.g. ST).
In this case major number starts from 0 and is connected to the first device
e.g. GPIOA. Major number selects GPIO peripheral.

\subsubsection drv-gpio-ddesc-numres Numeration restrictions
Number of peripherals determines how big the major number can be. If there is
only one GPIO peripheral then the major number is always 0.

\subsection drv-gpio-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("GPIO", 0, 0, "/dev/GPIOA");
@endcode
@code
driver_init("GPIO", 1, 0, "/dev/GPIOB");
@endcode

\subsection drv-gpio-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("GPIO", 0, 0);
@endcode
@code
driver_release("GPIO", 1, 0);
@endcode

\subsection drv-gpio-ddesc-cfg Driver configuration
Driver configuration can be done by using Configtool or configuration files.

\subsection drv-gpio-ddesc-write Data write
Data to the GPIO device can be wrote as regular file. The entire wide of port
is used to send bytes. If port is 16-bit then entire word will set output
pins to selected state.

\subsection drv-gpio-ddesc-read Data read
Data to the GPIO device can be read as regular file. The entire word of port
is read to buffer in read operation.

\subsection drv-gpio-ddesc-pinctr Pin control
Each bit can be controlled by using ioctl() function. There is possibility to
control selected pin on not opened port for example: user opened GPIOA and
by using IOCTL_GPIO__SET_PIN_IN_PORT ioctl()'s request can control pin in
GPIOB. This is possible only by port index.

@code
#include <stdio.h>
#include <sys/ioctl.h>

FILE *f = fopen("/dev/GPIOA", "r+);
if (f) {

     // control pin on port A...
     static const u8_t LED = IOCTL_GPIO_PIN_IDX__LED;
     ioctl(fileno(f), IOCTL_GPIO__CLEAR_PIN, &LED);

     // ... this same can be done by using alternative interface:
     static const GPIO_pin_in_port_t LED_PORT = {
            .pin_idx  = IOCTL_GPIO_PIN_IDX__LED,
            .port_idx = IOCTL_GPIO_PORT_IDX__LED
     };
     ioctl(fileno(f), IOCTL_GPIO__CLEAR_PIN_IN_PORT, &LED_PORT);


     // there is possiblity to control pin on other port
     static const GPIO_pin_in_port_t OTHER_PIN = {
            .pin_idx  = IOCTL_GPIO_PIN_IDX__OTHER_PIN,
            .port_idx = IOCTL_GPIO_PORT_IDX__OTHER_PIN
     };
     ioctl(fileno(f), IOCTL_GPIO__SET_PIN_IN_PORT, &OTHER_PIN);

     fclose(f);

} else {
     perror(NULL);
}
@endcode

There are restrictions that allow user to control pins by using other port
file: the port that is controlled should be earlier initialized. If port is
not initialized then pin behavior is undefined (depends on microcontroller).

@{
*/

#ifndef _GPIO_IOCTL_H_
#define _GPIO_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "drivers/ioctl_macros.h"

// PIN names definitions (IOCTL_GPIO_PIN__name and IOCTL_GPIO_PIN_MASK__name)
#if defined(ARCH_stm32f1)
        #include "stm32f1/gpio_cfg.h"
#elif defined(ARCH_stm32f4)
        #include "stm32f4/gpio_cfg.h"
#elif defined(ARCH_efr32)
        #include "efr32/gpio_cfg.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type used to read pin state
 */
typedef struct {
        u8_t pin_idx;           /*!< Pin index.*/
        int  state;             /*!< Pin state.*/
} GPIO_pin_state_t;

/**
 * Type represent pin on port selected by index.
 */
typedef struct {
        u8_t port_idx;          /*!< Port index.*/
        u8_t pin_idx;           /*!< Pin index.*/
} GPIO_pin_in_port_t;

/**
 * Type represent pin state on port selected by index.
 */
typedef struct {
        u8_t port_idx;          /*!< Port index.*/
        u8_t pin_idx;           /*!< Pin index.*/
        int  state;             /*!< Pin state.*/
} GPIO_pin_in_port_state_t;

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief  Not existing pin number.
 */
#define IOCTL_GPIO_PIN_IDX__NULL                0xFF

/**
 * @brief  Not existing port number.
 */
#define IOCTL_GPIO_PORT_IDX__NULL               0xFF

/**
 *  @brief  Set selected pin (set pin to Hi state).
 *  @param  [WR] @ref u8_t*                     pin index
 *  @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_GPIO__SET_PIN                     _IOW(GPIO, 0, const u8_t*)

/**
 *  @brief  Clear selected pin (set pin to Low state).
 *  @param  [WR] @ref u8_t*                     pin index
 *  @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_GPIO__CLEAR_PIN                   _IOW(GPIO, 1, const u8_t*)

/**
 *  @brief  Toggle pin state.
 *  @param  [WR] @ref u8_t*                     pin index
 *  @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_GPIO__TOGGLE_PIN                  _IOW(GPIO, 2, const u8_t*)

/**
 *  @brief  Set pin state.
 *  @param  [WR] @ref GPIO_pin_state_t*         pin index and state to set
 *  @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_GPIO__SET_PIN_STATE               _IOW(GPIO, 3, const GPIO_pin_state_t*)

/**
 *  @brief  Gets pin state.
 *  @param  [RD] @ref GPIO_pin_state_t*         pin index and read state
 *  @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_GPIO__GET_PIN_STATE               _IOR(GPIO, 4, GPIO_pin_state_t*)

/**
 *  @brief  Set selected pin in port (set pin to Hi state).
 *  @param  [WR] @ref GPIO_pin_in_port_t*       pin and port index
 *  @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_GPIO__SET_PIN_IN_PORT             _IOW(GPIO, 5, GPIO_pin_in_port_t*)

/**
 *  @brief  Set selected pin in port (set pin to Low state).
 *  @param  [WR] @ref GPIO_pin_in_port_t*       pin and port index
 *  @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_GPIO__CLEAR_PIN_IN_PORT           _IOW(GPIO, 6, GPIO_pin_in_port_t*)

/**
 *  @brief  Toggle selected pin in port.
 *  @param  [WR] @ref GPIO_pin_in_port_t*       pin and port index
 *  @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_GPIO__TOGGLE_PIN_IN_PORT          _IOW(GPIO, 7, GPIO_pin_in_port_t*)

/**
 *  @brief  Set pin state in selected port.
 *  @param  [WR] @ref GPIO_pin_in_port_state_t* pin and port index and state to set
 *  @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_GPIO__SET_PIN_STATE_IN_PORT       _IOW(GPIO, 8, GPIO_pin_in_port_state_t*)

/**
 *  @brief  Gets pin state in selected port.
 *  @param  [RD] @ref GPIO_pin_in_port_state_t* pin and port index and read state
 *  @return On success 0 is returned, otherwise -1 and @ref errno code is set
 */
#define IOCTL_GPIO__GET_PIN_STATE_IN_PORT       _IOR(GPIO, 9, GPIO_pin_in_port_state_t*)


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

#endif /* _GPIO_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
