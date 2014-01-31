/*=========================================================================*//**
@file    fsstack.c

@author  Daniel Zorychta

@brief   Program test how many file system use stack. Diagnose program.

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <dnx/os.h>
#include <dnx/thread.h>
#include <sys/stat.h>

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
char buffer[100];
GLOBAL_VARIABLES_SECTION_END

static const char *file_name = "__test_x__";
static const char *dir_name  = "__dir_x__";
static const char *fifo_name = "__fifo_x__";

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief Function test create-open-close file cycle
 */
//==============================================================================
static void test_create_open_close_file()
{
        FILE *file = fopen(file_name, "w");
        if (file) {
                fclose(file);
        }
}

//==============================================================================
/**
 * @brief Function test create-open-close fifo cycle
 */
//==============================================================================
static void test_create_open_close_fifo()
{
        if (mkfifo(fifo_name, 0666) != 0) {
                FILE *file = fopen(fifo_name, "r");
                if (file) {
                        fclose(file);
                }
        }
}

//==============================================================================
/**
 * @brief Function test open-write-close file
 */
//==============================================================================
static void test_open_write_close_file()
{
        FILE *file = fopen(file_name, "w");
        if (file) {
                fwrite(file_name, 1, strlen(file_name), file);
                fclose(file);
        }
}

//==============================================================================
/**
 * @brief Function test open-read-close file
 */
//==============================================================================
static void test_open_read_close_file()
{
        FILE *file = fopen(file_name, "r");
        if (file) {
                fread(global->buffer, 1, strlen(file_name), file);
                fclose(file);
        }
}

//==============================================================================
/**
 * @brief Function test open-write-close fifo
 */
//==============================================================================
static void test_open_write_close_fifo()
{
        FILE *file = fopen(fifo_name, "w");
        if (file) {
                fwrite(file_name, 1, strlen(file_name), file);
                fclose(file);
        }
}

//==============================================================================
/**
 * @brief Function test open-read-close fifo
 */
//==============================================================================
static void test_open_read_close_fifo()
{
        FILE *file = fopen(fifo_name, "r");
        if (file) {
                fread(global->buffer, 1, strlen(file_name), file);
                fclose(file);
        }
}

//==============================================================================
/**
 * @brief Function test flush on file and fifo
 */
//==============================================================================
static void test_flush()
{
        FILE *file = fopen(file_name, "r");
        if (file) {
                fflush(file);
                fclose(file);
        }

        file = fopen(fifo_name, "r");
        if (file) {
                fflush(file);
                fclose(file);
        }
}

//==============================================================================
/**
 * @brief Function test directory create
 */
//==============================================================================
static void test_create_dir()
{
        mkdir(dir_name, 0666);
}

//==============================================================================
/**
 * @brief Function test directory remove
 */
//==============================================================================
static void test_remove_dir()
{
        remove(dir_name);
}

//==============================================================================
/**
 * @brief Function test file remove
 */
//==============================================================================
static void test_remove_file()
{
        remove(file_name);
}

//==============================================================================
/**
 * @brief Function test fifo remove
 */
//==============================================================================
static void test_remove_fifo()
{
        remove(fifo_name);
}

//==============================================================================
/**
 * @brief Program main function
 */
//==============================================================================
PROGRAM_MAIN(fsstack, int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        int initial_stack = task_get_free_stack();

        test_create_open_close_file();
        test_create_open_close_fifo();
        test_open_write_close_file();
        test_open_read_close_file();
        test_open_write_close_fifo();
        test_open_read_close_fifo();
        test_flush();
        test_create_dir();
        test_remove_dir();
        test_remove_file();
        test_remove_fifo();

        printf("The highest usage of stack: %d levels\n", initial_stack - task_get_free_stack());

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/
