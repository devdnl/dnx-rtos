/*==============================================================================
File    crc.h

Author  Daniel Zorychta

Brief   CRC library.

        Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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
@defgroup _CRC_H_ _CRC_H_

Detailed Doxygen description.
*/
/**@{*/

#pragma once

/*==============================================================================
  Include files
==============================================================================*/
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/
extern const uint16_t crc16_ccitt_init;
extern const uint32_t crc32_ethernet_init;

/*==============================================================================
  Exported functions
==============================================================================*/
//==============================================================================
/**
 * @brief  Calculate CCITT CRC16.
 *
 * @param  buf          buffer to calculate
 * @param  len          buffer length
 * @param  crc_in       init value or last calculated CRC
 *
 * @return CRC16.
 */
//==============================================================================
extern uint16_t crc16_udpate_ccitt(const void *buf, uint16_t len, uint16_t crc_in);

//==============================================================================
/**
 * @brief  Calculate CRC32.
 *
 * @param  buf          buffer
 * @param  buflen       buffer length
 * @param  init         initial value
 *
 * @return CRC32 result.
 */
//==============================================================================
extern uint32_t crc32_update(const void *buf, uint32_t buflen, uint32_t init);

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
