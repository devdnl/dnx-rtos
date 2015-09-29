/*=========================================================================*//**
@file    ls.c

@author  Daniel Zorychta

@brief   List files in specified directory

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <dnx/vt100.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define PATH_LEN                        100

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
GLOBAL_VARIABLES_SECTION {
};

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
int_main(ls, STACK_DEPTH_LOW, int argc, char *argv[])
{
        char *path = malloc(100);
        if (!path) {
                perror(NULL);
                return EXIT_FAILURE;
        }

        if (argc == 1 || (argc == 2 && strcmp(argv[1], ".") == 0)) {
                getcwd(path, PATH_LEN);
        } else {
                strcpy(path, argv[1]);
        }

        DIR *dir = opendir(path);
        if (dir) {
                errno = 0;
                dirent_t *dirent = readdir(dir);
                while (dirent) {

                        const char *type;

                        switch (dirent->filetype) {
                        case FILE_TYPE_DIR:     type = VT100_FONT_COLOR_LIGHT_BLUE"d";  break;
                        case FILE_TYPE_DRV:     type = VT100_FONT_COLOR_MAGENTA"c";     break;
                        case FILE_TYPE_LINK:    type = VT100_FONT_COLOR_CYAN"l";        break;
                        case FILE_TYPE_REGULAR: type = VT100_FONT_COLOR_GREEN" ";       break;
                        case FILE_TYPE_PROGRAM: type = VT100_FONT_BOLD"x";              break;
                        case FILE_TYPE_PIPE:    type = VT100_FONT_COLOR_BROWN"p";       break;
                        default: type = "?";
                        }

                        u32_t size;
                        const char *unit;
                        if (dirent->size >= (u64_t)(10*GiB)) {
                                size = CONVERT_TO_GiB(dirent->size);
                                unit = "GiB";
                        } else if (dirent->size >= 10*MiB) {
                                size = CONVERT_TO_MiB(dirent->size);
                                unit = "MiB";
                        } else if (dirent->size >= 10*KiB) {
                                size = CONVERT_TO_KiB(dirent->size);
                                unit = "KiB";
                        } else {
                                size = dirent->size;
                                unit = "B";
                        }

                        if (dirent->filetype == FILE_TYPE_DRV) {
                                printf("%s %u%s"VT100_CURSOR_BACKWARD(100)VT100_CURSOR_FORWARD(11)"%i"
                                       VT100_CURSOR_BACKWARD(100)VT100_CURSOR_FORWARD(15)"%s"VT100_RESET_ATTRIBUTES"\n",
                                       type, size, unit, dirent->dev, dirent->name);
                        } else {
                                printf("%s %u%s"VT100_CURSOR_BACKWARD(100)VT100_CURSOR_FORWARD(15)"%s"VT100_RESET_ATTRIBUTES"\n",
                                       type, size, unit, dirent->name);
                        }

                        errno  = 0;
                        dirent = readdir(dir);
                }

                if (errno != ENOENT && errno != ESUCC) {
                        perror(argv[0]);
                }

                closedir(dir);
        } else {
                perror(path);
        }

        free(path);

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
