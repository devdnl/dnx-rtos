/*==============================================================================
File    mbr.c

Author  Daniel Zorychta

Brief   MBR handling functions.

        Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "drivers/class/storage/mbr.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Function load 16b data from selected buffer and offset
 *
 * @param buff                  buffer
 * @param offset                variable offset
 *
 * @return 16-bit value
 */
//==============================================================================
static u16_t load_u16(u8_t *buff, u16_t offset)
{
        return cast(u16_t, ( (cast(u16_t, buff[offset + 0] << 0))
                           | (cast(u16_t, buff[offset + 1] << 8)) ) );
}

//==============================================================================
/**
 * @brief Function load 32b data from selected buffer and offset
 *
 * @param buff                  buffer
 * @param offset                variable offset
 *
 * @return 32-bit value
 */
//==============================================================================
static u32_t load_u32(u8_t *buff, u16_t offset)
{
        return cast(u32_t, ( (cast(u32_t, buff[offset + 0] <<  0))
                           | (cast(u32_t, buff[offset + 1] <<  8))
                           | (cast(u32_t, buff[offset + 2] << 16))
                           | (cast(u32_t, buff[offset + 3] << 24)) ) );
}

//==============================================================================
/**
 * @brief Function gets boot signature
 *
 * @param sector                buffer with sector
 *
 * @return boot signature
 */
//==============================================================================
u16_t MBR_get_boot_signature(u8_t *sector)
{
     return load_u16(sector, MBR_BOOT_SIGNATURE_OFFSET);
}

//==============================================================================
/**
 * @brief Function return partition first LBA sector number
 *
 * @param partition             partition number (1 - 4)
 * @param sector                buffer with sector
 *
 * @return first LBA sector number
 */
//==============================================================================
u32_t MBR_get_partition_first_LBA_sector(int partition, u8_t *sector)
{
        if ((partition >= 1) && (partition <= 4) && sector) {

                return load_u32(sector, ((partition - 1) * MBR_PARTITION_ENTRY_SIZE)
                                        + MBR_PARTITION_1_ENTRY_OFFSET
                                        + MBR_PARTITION_ENTRY_LBA_FIRST_ADDR_OFFSET);
        } else {
                return 0;
        }
}

//==============================================================================
/**
 * @brief Function return partition number of sectors
 *
 * @param partition             partition number (1 - 4)
 * @param sector                buffer with sector
 *
 * @return number of partition sectors
 */
//==============================================================================
u32_t MBR_get_partition_number_of_sectors(int partition, u8_t *sector)
{
        if ((partition >= 1) && (partition <= 4) && sector) {

                return load_u32(sector, ((partition - 1) * MBR_PARTITION_ENTRY_SIZE)
                                        + MBR_PARTITION_1_ENTRY_OFFSET
                                        + MBR_PARTITION_ENTRY_NUM_OF_SECTORS_OFFSET);
        } else {
                return 0;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
