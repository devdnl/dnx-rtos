/*==============================================================================
File    sd.h

Author  Daniel Zorychta

Brief   SD Card definitions.

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
@defgroup SD_H_ SD_H_

Detailed Doxygen description.
*/
/**@{*/

#ifndef _SD_H_
#define _SD_H_

/*==============================================================================
  Include files
==============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/

/*==============================================================================
  Exported object types
==============================================================================*/
/** card command definitions */
typedef enum {
        SD_CMD__CMD0   = (0x40+0 ),             /* GO_IDLE_STATE */
        SD_CMD__CMD1   = (0x40+1 ),             /* SEND_OP_COND (MMC) */
        SD_CMD__CMD2   = (0xC0+2 ),             /* GET_CID (SDC) */
        SD_CMD__CMD3   = (0x40+3 ),             /* GET_RCA (SDC) */
        SD_CMD__ACMD41 = (0xC0+41),             /* SEND_OP_COND (SDC) */
        SD_CMD__CMD8   = (0x40+8 ),             /* SEND_IF_COND */
        SD_CMD__CMD9   = (0x40+9 ),             /* SEND_CSD */
        SD_CMD__CMD10  = (0x40+10),             /* SEND_CID */
        SD_CMD__CMD12  = (0x40+12),             /* STOP_TRANSMISSION */
        SD_CMD__ACMD13 = (0xC0+13),             /* SD_STATUS (SDC) */
        SD_CMD__CMD16  = (0x40+16),             /* SET_BLOCKLEN */
        SD_CMD__CMD17  = (0x40+17),             /* READ_SINGLE_BLOCK */
        SD_CMD__CMD18  = (0x40+18),             /* READ_MULTIPLE_BLOCK */
        SD_CMD__CMD23  = (0x40+23),             /* SET_BLOCK_COUNT (MMC) */
        SD_CMD__ACMD23 = (0xC0+23),             /* SET_WR_BLK_ERASE_COUNT (SDC) */
        SD_CMD__CMD24  = (0x40+24),             /* WRITE_BLOCK */
        SD_CMD__CMD25  = (0x40+25),             /* WRITE_MULTIPLE_BLOCK */
        SD_CMD__CMD55  = (0x40+55),             /* APP_CMD */
        SD_CMD__CMD58  = (0x40+58)              /* READ_OCR */
} SD_cmd_t;

/** card types */
typedef struct {
        enum {
                SD_TYPE__UNKNOWN,
                SD_TYPE__MMC,
                SD_TYPE__SD1,
                SD_TYPE__SD2
        } type;

        bool block;
} SD_type_t;

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

#endif /* _SD_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
