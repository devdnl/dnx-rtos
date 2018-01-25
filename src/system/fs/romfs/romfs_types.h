/*==============================================================================
File    romfs_types.h

Author  Daniel Zorychta

Brief   ROM File System

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
@defgroup ROMFS_TYPES_H_ ROMFS_TYPES_H_

Detailed Doxygen description.
*/
/**@{*/

#ifndef _ROMFS_TYPES_H_
#define _ROMFS_TYPES_H_

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
typedef enum {
        ROMFS_FILE_TYPE__NONE,
        ROMFS_FILE_TYPE__DIR,
        ROMFS_FILE_TYPE__FILE,
} romfs_file_type_t;

typedef struct {
        romfs_file_type_t type;
        size_t size;
        const void *data;
        const char *name;
} romfs_entry_t;

typedef struct {
        size_t items;
        romfs_entry_t entry[];
} romfs_dir_t;

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

#endif /* _ROMFS_TYPES_H_ */

/**@}*/
/*==============================================================================
  End of file
==============================================================================*/
