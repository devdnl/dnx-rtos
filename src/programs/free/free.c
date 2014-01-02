/*=========================================================================*//**
@file    free.c

@author  Daniel Zorychta

@brief   Show used memory

@note    Copyright (C) 2013 Daniel Zorychta <daniel.zorychta@gmail.com>

         This program is free software; you can redistribute it and/or modify
         it under the terms of the GNU General Public License as published by
         the  Free Software  Foundation;  either version 2 of the License, or
         any later version.

         This  program  is  distributed  in the hope that  it will be useful,
         but  WITHOUT  ANY  WARRANTY;  without  even  the implied warranty of
         MERCHANTABILITY  or  FITNESS  FOR  A  PARTICULAR  PURPOSE.  See  the
         GNU General Public License for more details.

         You  should  have received a copy  of the GNU General Public License
         along  with  this  program;  if not,  write  to  the  Free  Software
         Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


*//*==========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "system/dnx.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION_BEGIN

GLOBAL_VARIABLES_SECTION_END

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Cat main function
 */
//==============================================================================
PROGRAM_MAIN(free, int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        uint  drv_count = get_number_of_modules();
        int *modmem = malloc(drv_count * sizeof(int));
        if (!modmem) {
                perror(NULL);
                return EXIT_FAILURE;
        }

        struct sysmoni_used_memory sysmem;
        get_detailed_memory_usage(&sysmem);

        for (uint module = 0; module < drv_count; module++) {
                modmem[module] = get_module_memory_usage(module);
        }

        u32_t m_free = get_free_memory();
        u32_t m_used = get_used_memory();

        printf("Total: %d\n", get_memory_size());
        printf("Free : %d\n", m_free);
        printf("Used : %d\n", m_used);
        printf("Memory usage: %d%%\n\n", (m_used * 100)/get_memory_size());

        printf("Detailed memory usage:\n");
        printf("  Kernel  : %d\n", sysmem.used_kernel_memory);
        printf("  System  : %d\n", sysmem.used_system_memory);
        printf("  Modules : %d\n", sysmem.used_modules_memory);
        printf("  Network : %d\n", sysmem.used_network_memory);
        printf("  Programs: %d\n\n", sysmem.used_programs_memory);

        printf("Detailed modules memory usage:\n");
        for (uint module = 0; module < drv_count; module++) {
                printf("  %s"CURSOR_BACKWARD(99)CURSOR_FORWARD(14)": %d\n", get_module_name(module), modmem[module]);
        }

        free(modmem);

        return EXIT_SUCCESS;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
