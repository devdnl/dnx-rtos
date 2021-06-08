/*==============================================================================
File    ed.c

Author  Daniel Zorychta

Brief   Simple line editor

        Copyright (C) 3DGence 2021 Daniel Zorychta <daniel.zorychta@gmail.com>

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <llist/llist.h>
#include <dnx/misc.h>

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int load_file_content(llist_t **content, const char *path);
static int save_file_content(llist_t **content, const char *path);
static int insert_line(llist_t **content, int line);
static int delete_line(llist_t **content, int line);
static int edit_line(llist_t **content, int line);

/*==============================================================================
  Local objects
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        char buf[128];
        char bufline[4096];
};

/*==============================================================================
  Exported objects
==============================================================================*/
PROGRAM_PARAMS(ed, STACK_DEPTH_LOW);

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * Main program function.
 *
 * @param argc      argument count
 * @param argv      arguments
 *
 * @return On succes 0.
 */
//==============================================================================
int main(int argc, char *argv[])
{
        if (argc < 3) {
                printf("Usage: %s [arg] <line-to-edit> <file>\n", argv[0]);
                printf("Arguments:\n");
                printf("  -i  insert line\n");
                printf("  -d  delete line\n");
                return EXIT_FAILURE;
        }

        bool insert = false;
        bool delete = false;
        int  line   = 0;
        const char *path = "";

        if (argc == 4) {
                insert = isstreq(argv[1], "-i");
                delete = isstreq(argv[1], "-d");
                line   = atoi(argv[2]);
                path   = argv[3];
        } else {
                line = atoi(argv[1]);
                path = argv[2];
        }

        if (line < 1) {
                puts("Line must be higher than 0.");
                return EXIT_FAILURE;
        }

        llist_t *content = llist_new(NULL, NULL);
        if (content) {

                int err = load_file_content(&content, path);

                if (!err) {
                        if (insert) {
                                insert_line(&content, line);

                        } else if (delete) {
                                delete_line(&content, line);

                        } else {
                                edit_line(&content, line);
                        }

                        if (!err) {
                                err = save_file_content(&content, path);
                        }
                }

                llist_delete(content);
        }

        return EXIT_SUCCESS;
}

//==============================================================================
/**
 * @brief  Function load file content.
 *
 * @param  content      content list
 * @param  path         file to read
 *
 * @return On success 0 is returned.
 */
//==============================================================================
static int load_file_content(llist_t **content, const char *path)
{
        int err = -1;

        FILE *file = fopen(path, "r");
        if (file) {
                char *str;
                while ((str = fgets_buffered(global->bufline, sizeof(global->bufline),
                                             file, global->buf, sizeof(global->buf)))) {

                        size_t len = strlen(global->bufline);

                        if (global->bufline[len - 1]) {
                                global->bufline[len - 1] = '\0';
                        }

                        // (A) buffer with \n (removed) + '\0'
                        llist_push_emplace_back(*content, len + 1, global->bufline);
                }

                err = 0;

                fclose(file);
        } else {
                perror(path);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function save file content.
 *
 * @param  content      content list
 * @param  path         file to read
 *
 * @return On success 0 is returned.
 */
//==============================================================================
static int save_file_content(llist_t **content, const char *path)
{
        int err = -1;

        snprintf(global->buf, sizeof(global->buf), "%s~", path);
        snprintf(global->bufline, sizeof(global->bufline), "%s.~", path);

        FILE *out = fopen(global->buf, "w");
        if (out) {
                llist_foreach(char *, str, *content) {
                        // Note: buffer is big enough to modify last byte.
                        // See (A). \n added because removed at read.
                        size_t len = strlen(str);
                        str[len] = '\n';
                        fwrite(str, 1, len + 1, out);
                }

                fclose(out);

                rename(path, global->bufline);
                rename(global->buf, path);
                remove(global->bufline);

                err = 0;
        } else {
                perror(path);
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function insert line to content.
 *
 * @param  content      content list
 * @param  line         line
 *
 * @return On success 0 is returned.
 */
//==============================================================================
static int insert_line(llist_t **content, int line)
{
        int err = -1;

        if (fgets(global->bufline, sizeof(global->buf), stdin)) {

                size_t new_size = strsize(global->bufline);

                global->bufline[new_size - 2] = '\0';

                if (line <= 1) {
                        llist_push_emplace_front(*content, new_size, global->bufline);
                } else if (llist_size(*content) < line) {
                        llist_push_emplace_back(*content, new_size, global->bufline);
                } else {
                        llist_emplace(*content, line - 1, new_size, global->bufline);
                }

                err = 0;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function delete line in content.
 *
 * @param  content      content list
 * @param  line         line
 *
 * @return On success 0 is returned.
 */
//==============================================================================
static int delete_line(llist_t **content, int line)
{
        int lines = llist_size(*content);

        if (line < 1) {
                line = 0;
        } else if (lines < line) {
                line = lines - 1;
        } else {
                line--;
        }

        return llist_erase(*content, line);
}

//==============================================================================
/**
 * @brief  Function edit line in content.
 *
 * @param  content      content list
 * @param  line         line
 *
 * @return On success 0 is returned.
 */
//==============================================================================
static int edit_line(llist_t **content, int line)
{
        int err = -1;

        if (llist_size(*content) > 0) {
                char *str = llist_take(*content, line - 1);
                if (str) {

                        ioctl(fileno(stdout), IOCTL_TTY__SET_EDITLINE, str);

                        fgets(global->bufline, sizeof(global->buf), stdin);

                        size_t old_size = strsize(str);
                        size_t new_size = strsize(global->bufline);

                        global->bufline[new_size - 2] = '\0';

                        if (new_size <= old_size) {
                                strcpy(str, global->bufline);
                        } else {
                                str = realloc(str, new_size);
                                strlcpy(str, global->bufline, new_size);
                        }

                        llist_insert(*content, line - 1, str);
                } else {
                        puts("No such line in file.");
                }
        } else {
                puts("File is empty.");
        }

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/

