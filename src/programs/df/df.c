/*=========================================================================*//**
@file    df.c

@author  Daniel Zorychta

@brief   List mounted file systems

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

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mntent.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define KiB                             (u32_t)(1024)
#define MiB                             (u32_t)(1024*1024)
#define GiB                             (u64_t)(1024*1024*1024)
#define CONVERT_TO_KiB(_val)            (_val >> 10)
#define CONVERT_TO_MiB(_val)            (_val >> 20)
#define CONVERT_TO_GiB(_val)            (_val >> 30)

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
PROGRAM_MAIN(df, int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        struct vfs_mntent mnt;
        mnt.mnt_dir    = calloc(64, ARRAY_ITEM_SIZE(mnt.mnt_dir));
        mnt.mnt_fsname = calloc(64, ARRAY_ITEM_SIZE(mnt.mnt_fsname));
        mnt.free       = 0;
        mnt.total      = 0;

        if (mnt.mnt_dir && mnt.mnt_fsname) {
                printf("File system"CURSOR_FORWARD(5)"Total"CURSOR_FORWARD(5)
                       "Free"CURSOR_FORWARD(6)"%%Used  Mount point\n");

                for (u32_t i = 0;; i++) {
                        if (getmntentry(i, &mnt) == STD_RET_OK) {
                                u32_t dtotal;
                                u32_t dfree;
                                const char *unit;

                                if (mnt.total > 10*GiB) {
                                        dtotal = CONVERT_TO_GiB(mnt.total);
                                        dfree  = CONVERT_TO_GiB(mnt.free);
                                        unit   = "GiB";
                                } else if (mnt.total > 10*MiB) {
                                        dtotal = CONVERT_TO_MiB(mnt.total);
                                        dfree  = CONVERT_TO_MiB(mnt.free);
                                        unit   = "MiB";
                                } else if (mnt.total > 10*KiB) {
                                        dtotal = CONVERT_TO_KiB(mnt.total);
                                        dfree  = CONVERT_TO_KiB(mnt.free);
                                        unit   = "KiB";
                                } else {
                                        dtotal = mnt.total;
                                        dfree  = mnt.free;
                                        unit   = "B";
                                }

                                printf("%s"  CURSOR_BACKWARD(90)CURSOR_FORWARD(16)
                                       "%u%s"CURSOR_BACKWARD(90)CURSOR_FORWARD(26)
                                       "%u%s"CURSOR_BACKWARD(90)CURSOR_FORWARD(36)
                                       "%u%%"CURSOR_BACKWARD(90)CURSOR_FORWARD(43)
                                       "%s\n",
                                       mnt.mnt_fsname, dtotal, unit, dfree, unit,
                                       ((dtotal - dfree) * 100)/dtotal, mnt.mnt_dir);

                                memset(mnt.mnt_dir, 0, 64);
                                memset(mnt.mnt_fsname, 0, 64);
                        } else {
                                if (i == 0)
                                        perror(NULL);

                                break;
                        }
                }
        } else {
                perror(NULL);
        }

        if (mnt.mnt_dir)
                free(mnt.mnt_dir);

        if (mnt.mnt_fsname)
                free(mnt.mnt_fsname);

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
