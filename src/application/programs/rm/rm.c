/*=========================================================================*//**
@file    rm.c

@author  Daniel Zorychta

@brief   Remove files

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dnx/misc.h>
#include <errno.h>
#include <llist.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define VERBOSE(...) if (global->verbose) printf(__VA_ARGS__)

/*==============================================================================
  Local types, enums definitions
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void print_help(char *argv[]);
static void list_nodes(const char *path);

/*==============================================================================
  Local object definitions
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        char path[256];
        bool opts;
        bool recursive;
        bool force;
        bool verbose;
        llist_t *rmlist;
};

/*==============================================================================
  Exported object definitions
==============================================================================*/
PROGRAM_PARAMS(rm, STACK_DEPTH_MEDIUM);

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Program main function
 */
//==============================================================================
int main(int argc, char *argv[])
{
        if (argc == 1) {
                print_help(argv);
                return EXIT_FAILURE;

        } else {
                if (isstreq(argv[1], "--help")) {
                        print_help(argv);
                        return EXIT_FAILURE;

                } else if (argv[1][0] == '-') {
                        global->opts = true;

                        if (argv[1][1] == '-') {
                                // no action
                        } else {
                                global->recursive =  (strchr(argv[1], 'r') != NULL)
                                                  or (strchr(argv[1], 'R') != NULL);

                                global->force = (strchr(argv[1], 'f') != NULL);

                                global->verbose = (strchr(argv[1], 'v') != NULL);
                        }
                }

                if (global->recursive) {
                        global->rmlist = llist_new(NULL, NULL);
                }
        }

        for (int i = 1; i < argc; i++) {

                if (global->opts and (i == 1)) {
                        continue;

                } else {
                        if (global->recursive) {
                                if (isstreq(argv[i], "./") or isstreq(argv[i], "*")) {
                                        printf("Skipped '%s'.\n", argv[i]);
                                        continue;
                                }

                                bool rmall = isstreq(argv[i], "./*");

                                llist_clear(global->rmlist);
                                list_nodes(rmall ? "./" : argv[i]);
                                if (rmall) llist_pop_back(global->rmlist);

                                char *path;
                                while ((path = llist_take_front(global->rmlist))) {

                                        VERBOSE("Removing: '%s'\n", path);

                                        errno = 0;
                                        int err = remove(path);
                                        if (err) {
                                                perror(path);
                                        }

                                        free(path);

                                        if (err and not global->force) {
                                                i = argc;
                                                break;
                                        }
                                }

                        } else {
                                errno = 0;
                                int err = remove(argv[i]);
                                if (err) {
                                        perror(argv[i]);
                                        if (not global->force) break;
                                }
                        }
                }
        }

        llist_delete(global->rmlist);

        return EXIT_SUCCESS;
}

//==============================================================================
/**
 * @brief  Print help message.
 *
 * @param  argc         arg count
 * @param  argv         arg values
 */
//==============================================================================
static void print_help(char *argv[])
{
        printf("Usage: %s [ARGS] [FILE]...\n", argv[0]);
        printf("\n");
        printf("  -r, -R   recursive delete\n");
        printf("  -f       ignore errors\n");
        printf("\n");
        printf("To remove a file whose name begins with a dash (-): %s -- -file.txt\n", argv[0]);
}

//==============================================================================
/**
 * @brief  List files to remove.
 *
 * @param  path         path to remove (file or dir)
 */
//==============================================================================
static void list_nodes(const char *path)
{
        strlcpy(global->path, path, sizeof(global->path));

        DIR *dir = opendir(path);
        if (dir) {

                if (LAST_CHARACTER(global->path) != '/') {
                        strlcat(global->path, "/", sizeof(global->path));
                }

                dirent_t *dirent;
                while ((dirent = readdir(dir))) {

                        if (  isstreq(dirent->d_name, ".")
                           or isstreq(dirent->d_name, "..") ) {
                                continue;
                        }

                        strlcat(global->path, dirent->d_name, sizeof(global->path));
                        list_nodes(global->path);
                        char *slash = strrchr(global->path, '/');
                        if (slash) *(slash + 1) = '\0';
                }

                char *slash = strrchr(global->path, '/');
                if (slash) *slash = '\0';
                else strcpy(global->path, "");

                closedir(dir);
        }

        llist_push_back(global->rmlist, strdup(path));
}

/*==============================================================================
  End of file
==============================================================================*/
