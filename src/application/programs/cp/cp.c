/*=========================================================================*//**
@file    cp.c

@author  Daniel Zorychta

@brief   Program to copy files

@note    Copyright (C) 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <dirent.h>
#include <dnx/misc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <dnx/os.h>
#include <sys/stat.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define BUFFER_MAX_SIZE                 32768

#define VERBOSE(...) if (global->verbose) printf(__VA_ARGS__)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void print_help(char *argv[]);
static int copy_file(const char *src, const char *dst);
static int copy_recursive(const char *src_path, const char *dst_path);

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        size_t buffer_size;
        u8_t *buffer;
        bool opts;
        bool recursive;
        bool force;
        bool verbose;
        char src_path[256];
        char dst_path[256];
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(cp, STACK_DEPTH_MEDIUM);

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Cat main function
 */
//==============================================================================
int main(int argc, char *argv[])
{
        if (argc < 3) {
                print_help(argv);
                return EXIT_FAILURE;

        } else {
                if (isstreq(argv[1], "--help")) {
                        print_help(argv);
                        return EXIT_FAILURE;

                } else if (argv[1][0] == '-') {
                        global->opts = true;

                        global->recursive =  (strchr(argv[1], 'r') != NULL)
                                          or (strchr(argv[1], 'R') != NULL);

                        global->force = (strchr(argv[1], 'f') != NULL);

                        global->verbose = (strchr(argv[1], 'v') != NULL);
                }
        }

        int err = ENOMEM;

        // allocate operational buffer
        global->buffer_size = BUFFER_MAX_SIZE;
        while ((global->buffer = malloc(global->buffer_size)) == NULL) {
                global->buffer_size /= 2;

                if (global->buffer_size < 512) {
                        perror(NULL);
                        return EXIT_FAILURE;
                }
        }

        if (global->buffer) {
                if (global->opts) {
                        err = copy_recursive(argv[2], argv[3]);

                } else {
                        err = copy_file(argv[1], argv[2]);
                }

                free(global->buffer);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Print help message.
 *
 * @param  argv
 */
//==============================================================================
static void print_help(char *argv[])
{
        printf("Usage: %s [-Rvf] <source file> <destination file>\n", argv[0]);
        printf(" -R    recursive copy\n");
        printf(" -v    verbose\n");
        printf(" -f    skip erros\n");
}

//==============================================================================
/**
 * @brief  Copy selected file.
 *
 * @param  src          source path
 * @param  dst          destination path
 *
 * @return On success 0 is returned.
 */
//==============================================================================
static int copy_file(const char *src, const char *dst)
{
        int   err      = 0;
        FILE *src_file = NULL;
        FILE *dst_file = NULL;

        errno = 0;
        src_file = fopen(src, "r");
        if (!src_file) {
                err = errno;
                perror(src);
        }

        errno = 0;
        dst_file = fopen(dst, "w");
        if (!dst_file) {
                err = errno;
                perror(dst);
        }

        if (src_file and dst_file) {
                int n;
                errno = 0;
                while ((n = fread(global->buffer, 1, global->buffer_size, src_file)) > 0) {
                        if (ferror(src_file)) {
                                err = errno;
                                perror(src);
                                break;
                        }

                        errno = 0;
                        fwrite(global->buffer, 1, n, dst_file);
                        if (ferror(dst_file)) {
                                err = errno;
                                perror(dst);
                                break;
                        }
                }
        }

        fclose(src_file);
        fclose(dst_file);

        return err;
}

//==============================================================================
/**
 * @brief  Copy files/dirs recursively
 *
 * @param  src_path     source path
 * @param  dst_path     destination path
 *
 * @return On success 0 is returned.
 */
//==============================================================================
static int copy_recursive(const char *src_path, const char *dst_path)
{
        int err = 0;

        strlcpy(global->src_path, src_path, sizeof(global->src_path));
        strlcpy(global->dst_path, dst_path, sizeof(global->dst_path));

        DIR *dir = opendir(src_path);
        if (dir) {

                char *slash = NULL;

                if (LAST_CHARACTER(global->src_path) != '/') {
                        strlcat(global->src_path, "/", sizeof(global->src_path));
                }

                if (LAST_CHARACTER(global->dst_path) != '/') {
                        strlcat(global->dst_path, "/", sizeof(global->dst_path));
                }

                errno = 0;
                err = mkdir(global->dst_path, 0666);
                if (!err) {
                        VERBOSE("New directory: '%s'\n", global->dst_path);
                } else {
                        err = 0;
                }

                dirent_t *dirent;
                while (not err and (dirent = readdir(dir))) {

                        if (  isstreq(dirent->d_name, ".")
                           or isstreq(dirent->d_name, "..") ) {
                                continue;
                        }

                        strlcat(global->src_path, dirent->d_name, sizeof(global->src_path));
                        strlcat(global->dst_path, dirent->d_name, sizeof(global->dst_path));

                        int err = copy_recursive(global->src_path, global->dst_path);
                        if (err and not global->force) {
                                break;
                        } else {
                                err = 0;
                        }

                        slash = strrchr(global->src_path, '/');
                        if (slash) *(slash + 1) = '\0';

                        slash = strrchr(global->dst_path, '/');
                        if (slash) *(slash + 1) = '\0';
                }

                slash = strrchr(global->src_path, '/');
                if (slash) *(slash) = '\0';
                else strcpy(global->src_path, "");

                slash = strrchr(global->dst_path, '/');
                if (slash) *(slash) = '\0';
                else strcpy(global->dst_path, "");

                closedir(dir);

        } else {
                VERBOSE("Coping: '%s' => '%s'\n", src_path, dst_path);
                err = copy_file(src_path, dst_path);
                if (err) {
                        if (global->force) err = 0;
                }
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
