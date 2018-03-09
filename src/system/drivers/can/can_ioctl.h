/*==============================================================================
File    can_ioctl.h

Author  Daniel Zorychta

Brief   CAN driver

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
@defgroup drv-can CAN Driver

\section drv-can-desc Description
Driver handles ...

\section drv-can-sup-arch Supported architectures
\li stm32f1

\section drv-can-ddesc Details
\subsection drv-can-ddesc-num Meaning of major and minor numbers
\todo Meaning of major and minor numbers

\subsubsection drv-can-ddesc-numres Numeration restrictions
\todo Numeration restrictions

\subsection drv-can-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("CAN", 0, 0, "/dev/CAN0-0");
@endcode
@code
driver_init("CAN", 0, 1, "/dev/CAN0-1");
@endcode

\subsection drv-can-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("CAN", 0, 0);
@endcode
@code
driver_release("CAN", 0, 1);
@endcode

\subsection drv-can-ddesc-cfg Driver configuration
\todo Driver configuration

\subsection drv-can-ddesc-write Data write
\todo Data write

\subsection drv-can-ddesc-read Data read
\todo Data read

@{
*/

#ifndef _CAN_IOCTL_H_
#define _CAN_IOCTL_H_

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
#define IOCTL_CAN__CONFIGURE            _IOW(CAN, 0x00, const CAN_config_t*)

#define IOCTL_CAN__SET_FILTER           _IOW(CAN, 0x01, const CAN_filter_t*)

#define IOCTL_CAN__SET_MODE             _IOW(CAN, 0x02, const CAN_mode_t*)

#define IOCTL_CAN__SET_SEND_TIMEOUT     _IOW(CAN, 0x03, const u32_t*)

#define IOCTL_CAN__SET_RECV_TIMEOUT     _IOW(CAN, 0x04, const u32_t*)

#define IOCTL_CAN__SEND_MSG             _IOW(CAN, 0x05, const CAN_msg_t*)

#define IOCTL_CAN__RECV_MSG             _IOR(CAN, 0x06, CAN_msg_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
typedef enum {
        CAN_MODE__INIT,
        CAN_MODE__NORMAL,
        CAN_MODE__SLEEP,
} CAN_mode_t;

typedef struct {
        u32_t number;
        u32_t ID1;
        u32_t ID2_mask;
        bool  mask;
} CAN_filter_t;

typedef struct {
        bool  loopback;
        bool  silent;
        bool  time_triggered_communication;
        bool  auto_bus_off;
        bool  auto_wake_up;
        bool  auto_retransmission;
        u32_t SJW;
        u32_t TS1;
        u32_t TS2;
        u32_t prescaler;
} CAN_config_t;

typedef struct {
        u32_t ID;
        bool  extended_ID;
        bool  remote_tranmission;
        u32_t data_length;
        u8_t  data[8];
        u32_t timeout_ms;
} CAN_msg_t;

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

#endif /* _CAN_IOCTL_H_ */
/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
