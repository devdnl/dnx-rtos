/*=========================================================================*//**
@file    ls.c

@author  Daniel Zorychta

@brief   List files in specified directory

@note    Copyright (C) 2017 Daniel Zorychta <daniel.zorychta@gmail.com>

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

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <dnx/vt100.h>
#include <dnx/os.h>
#include <dnx/misc.h>
#include <sys/stat.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define CWD_MAX_LEN                     128

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
        char cwd[CWD_MAX_LEN];
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
        getcwd(global->cwd, CWD_MAX_LEN - 1);

        if (argc == 2) {
                char *path = argv[1];

                if (path[0] == '/') {
                        strcpy(global->cwd, path);

                } else {
                        if (LAST_CHARACTER(global->cwd) != '/') {
                                strcat(global->cwd, "/");
                        }

                        if (strncmp(path, "./", 2) == 0) {
                                path += 2;
                        }

                        strcat(global->cwd, path);
                }

                chdir(global->cwd);
        }

        DIR *dir = opendir(""); // path determined by CWD
        if (dir) {
                errno = 0;

                u16_t count = 0;

                dirent_t *dirent = readdir(dir);
                while (!errno && dirent) {

                        struct stat st;
                        if (stat(dirent->name, &st) == 0) {

                                const char *type;
                                switch (st.st_type) {
                                case FILE_TYPE_DIR:     type = VT100_FONT_COLOR_LIGHT_BLUE"d"; break;
                                case FILE_TYPE_DRV:     type = VT100_FONT_COLOR_MAGENTA"c";    break;
                                case FILE_TYPE_LINK:    type = VT100_FONT_COLOR_CYAN"l";       break;
                                case FILE_TYPE_REGULAR: type = VT100_FONT_COLOR_GREEN"-";      break;
                                case FILE_TYPE_PROGRAM: type = VT100_FONT_BOLD"*";             break;
                                case FILE_TYPE_PIPE:    type = VT100_FONT_COLOR_BROWN"p";      break;
                                default:                type = "?";                            break;
                                }

                                char mode[10];
                                mode[0] = (st.st_mode & S_IRUSR) ? 'r' : '-';
                                mode[1] = (st.st_mode & S_IWUSR) ? 'w' : '-';
                                mode[2] = (st.st_mode & S_IXUSR) ? 'x' : '-';
                                mode[3] = (st.st_mode & S_IRGRP) ? 'r' : '-';
                                mode[4] = (st.st_mode & S_IWGRP) ? 'w' : '-';
                                mode[5] = (st.st_mode & S_IXGRP) ? 'x' : '-';
                                mode[6] = (st.st_mode & S_IROTH) ? 'r' : '-';
                                mode[7] = (st.st_mode & S_IWOTH) ? 'w' : '-';
                                mode[8] = (st.st_mode & S_IXOTH) ? 'x' : '-';
                                mode[9] = '\0';

                                u32_t       size;
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

                                int mod_id    = get_module_ID2(st.st_dev);
                                int mod_major = get_module_major(st.st_dev);
                                int mod_minor = get_module_minor(st.st_dev);

                                char mod[12];
                                memset(mod, 0, sizeof(mod));

                                if (st.st_type == FILE_TYPE_DRV) {
                                        snprintf(mod, sizeof(mod), "%2d,%2d,%2d",
                                                 mod_id, mod_major, mod_minor);
                                }

                                struct tm tm;
                                localtime_r(&st.st_mtime, &tm);

                                char time[24];
                                strftime(time, sizeof(time), "%d-%m-%Y %H:%M", &tm);

                                printf("%s%s %9u %s"
                                       VT100_CURSOR_BACKWARD(999)VT100_CURSOR_FORWARD(24)"%s"
                                       VT100_CURSOR_BACKWARD(999)VT100_CURSOR_FORWARD(34)"%s"
                                       VT100_CURSOR_BACKWARD(999)VT100_CURSOR_FORWARD(51)"%s"
                                       VT100_RESET_ATTRIBUTES"\n",
                                       type, mode, size, unit, mod, time, dirent->name);

                                count++;
                        } else {
                                errno = EFAULT;
                                break;
                        }

                        dirent = readdir(dir);
                }

                if (!(errno == ESUCC || errno == ENOENT)) {
                        perror("Read dir");
                }

                printf("total %d\n", count);

                closedir(dir);
        } else {
                perror(global->cwd);
        }

        return errno ? EXIT_FAILURE : EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
