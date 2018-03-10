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
Driver handles CAN controller peripheral existing in microcontroller.

\section drv-can-sup-arch Supported architectures
\li stm32f1

\section drv-can-ddesc Details
\subsection drv-can-ddesc-num Meaning of major and minor numbers
Only CAN1 is supported.

\subsubsection drv-can-ddesc-numres Numeration restrictions
Both major and minor number should be set to 0.

\subsection drv-can-ddesc-init Driver initialization
To initialize driver the following code can be used:

@code
driver_init("CAN", 0, 0, "/dev/can");
@endcode

\subsection drv-can-ddesc-release Driver release
To release driver the following code can be used:
@code
driver_release("CAN", 0, 0);
@endcode

\subsection drv-can-ddesc-cfg Driver configuration
To configure CAN controller one should do following steps:
1. CAN controller configuration (baud rate, transmission parameters),
2. Acceptance filter setup,
3. Enter CAN controller to normal mode.

Example code
@code
#include <sys/ioctl.h>

//...

FILE *f = fopen("/dev/can", "r+");
if (f) {
        // basic configuration
        CAN_config_t cfg;
        cfg.auto_bus_off_management = false;
        cfg.auto_retransmission = false;
        cfg.auto_wake_up = false;
        cfg.loopback = false;
        cfg.silent   = false;
        cfg.time_triggered_comm = false;
        cfg.SJW = 1;
        cfg.TS1 = 3;
        cfg.TS2 = 5;
        cfg.prescaler = 4;

        if (ioctl(fileno(f), IOCTL_CAN__CONFIGURE, &cfg) != 0) {
                perror("CAN configuration");
                ...
        }

        // filter 0 setup
        CAN_filter_t filter;
        filter.number = 0;
        filter.ID1 = 0xFF10;
        filter.ID2_mask = 0;
        filter.mask = false;

        if (ioctl(fileno(f), IOCTL_CAN__SET_FILTER, &filter) != 0) {
                perror("CAN filter error");
                ...
        }

        // filter 1 setup
        filter.number = 1;
        filter.ID1 = 0xFF20;
        filter.ID2_mask = 0xFFF0;
        filter.mask = mask;

        if (ioctl(fileno(f), IOCTL_CAN__SET_FILTER, &filter) != 0) {
                perror("CAN filter error");
                ...
        }

        // enabling normal mode
        CAN_mode_t mode = CAN_MODE__NORMAL;
        if (ioctl(fileno(f), IOCTL_CAN__SET_MODE, &mode) != 0) {
                perror("CAN normal mode error");
                ...
        }


        // configuration done
        ...

} else {
        perror("CAN");
}

//...
@endcode


\subsection drv-can-ddesc-write Data write
\todo Data write

\subsection drv-can-ddesc-read Data read
\todo Data read

\subsection drv-can-ddesc-baud Baud rate calculation
To calculate CAN baud rate following equation should be applied:

baud = (fPCLK [Hz]) / (prescaler * (1 + TS1 + TS2)) [bps]

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
 *  @brief  CAN controller configuration.
 *  @param  [WR] const @ref CAN_config_t * configuration container
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_CAN__CONFIGURE            _IOW(CAN, 0x00, const CAN_config_t*)

/**
 *  @brief  Acceptance filter setup.
 *  @param  [WR] const @ref CAN_filter_t * filter container
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_CAN__SET_FILTER           _IOW(CAN, 0x01, const CAN_filter_t*)

/**
 *  @brief  CAN controller mode selection.
 *  @param  [WR] const @ref CAN_mode_t * can mode
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_CAN__SET_MODE             _IOW(CAN, 0x02, const CAN_mode_t*)

/**
 *  @brief  Set send timeout in milliseconds. Used in write() family function.
 *  @param  [WR] const @ref u32_t * timeout in milliseconds
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_CAN__SET_SEND_TIMEOUT     _IOW(CAN, 0x03, const u32_t*)

/**
 *  @brief  Set receive timeout in milliseconds. Used in read() family function.
 *  @param  [WR] const @ref u32_t * timeout in milliseconds
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_CAN__SET_RECV_TIMEOUT     _IOW(CAN, 0x04, const u32_t*)

/**
 *  @brief  Send CAN message.
 *  @param  [WR] const @ref CAN_msg_t * CAN message source
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_CAN__SEND_MSG             _IOW(CAN, 0x05, const CAN_msg_t*)

/**
 *  @brief  Receive CAN message.
 *  @param  [RD] @ref CAN_msg_t * CAN message destination
 *  @return On success 0 is returned, otherwise -1.
 */
#define IOCTL_CAN__RECV_MSG             _IOR(CAN, 0x06, CAN_msg_t*)

/*==============================================================================
  Exported object types
==============================================================================*/
/**
 * Type represent CAN mode.
 */
typedef enum {
        CAN_MODE__INIT,                 /*!< CAN controller in initialization mode. */
        CAN_MODE__NORMAL,               /*!< CAN controller in normal mode. */
        CAN_MODE__SLEEP,                /*!< CAN controller in sleep mode. */
} CAN_mode_t;

/**
 * Type represent CAN acceptance filter.
 */
typedef struct {
        u32_t number;                   /*!< Filter number. */
        u32_t ID1;                      /*!< Identifier 1. */
        u32_t ID2_mask;                 /*!< Identifier 2 or mask. */
        bool  mask;                     /*!< Filter is a mask (ID1 + mask). */
} CAN_filter_t;

/**
 * Type represent configuration container.
 */
typedef struct {
        bool  loopback;                 /*!< Loopback mode. */
        bool  silent;                   /*!< Silent mode. */
        bool  time_triggered_comm;      /*!< Time triggered communication. */
        bool  auto_bus_off_management;  /*!< Automatic bus off management. */
        bool  auto_wake_up;             /*!< Automatic wake up. */
        bool  auto_retransmission;      /*!< Auto retransmission. */
        u32_t SJW;                      /*!< Resynchronization jump width <1,n> */
        u32_t TS1;                      /*!< Time segment 1 <1,n> */
        u32_t TS2;                      /*!< Time segment 2 <1,n> */
        u32_t prescaler;                /*!< Peripheral prescaler <1,n> */
} CAN_config_t;

/**
 * Type represent CAN message with timeout control.
 */
typedef struct {
        u32_t ID;                       /*!< Message identifier. */
        bool  extended_ID;              /*!< Extended identifier (true). */
        bool  remote_transmission;      /*!< Remote transmission (true). */
        u32_t data_length;              /*!< Data length. */
        u8_t  data[8];                  /*!< Data buffer (8 bytes). */
        u32_t timeout_ms;               /*!< Timeout in milliseconds. */
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
