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

/*==============================================================================
  Local objects
==============================================================================*/
GLOBAL_VARIABLES_SECTION {
        char buf[128];
        char bufline[256];
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
                printf("Usage: %s <line-to-edit> <file>\n", argv[0]);
                return EXIT_FAILURE;
        }

        size_t line = atoi(argv[1]);
        if (line < 1) {
                puts("Line must be higher than 0.");
                return EXIT_FAILURE;
        }

        bool rmfile = false;

        FILE  *file = fopen(argv[2], "r");
        if (file) {

                llist_t *content = llist_new(NULL, NULL);
                if (content) {

                        char *str;
                        while ((str = fgets_buffered(global->bufline, sizeof(global->bufline),
                                                     file, global->buf, sizeof(global->buf)))) {

                                size_t len = strlen(global->bufline);

                                if (global->bufline[len - 1]) {
                                        global->bufline[len - 1] = '\0';
                                }

                                // (A) buffer with \n (removed) + '\0'
                                llist_push_emplace_back(content, len + 1, global->bufline);
                        }

                        if (llist_size(content) > 0) {
                                str = llist_take(content, line - 1);
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

                                        llist_insert(content, line - 1, str);

                                        snprintf(global->buf, sizeof(global->buf), "%s~", argv[2]);
                                        snprintf(global->bufline, sizeof(global->bufline), "%s.~", argv[2]);

                                        FILE *out = fopen(global->buf, "w");
                                        if (out) {
                                                llist_foreach(char *, str, content) {
                                                        // Note: buffer is big enough to modify last byte.
                                                        // See (A). \n added because removed at read.
                                                        size_t len = strlen(str);
                                                        str[len] = '\n';
                                                        fwrite(str, 1, len + 1, out);
                                                }

                                                fclose(out);

                                                rename(argv[2], global->bufline);
                                                rename(global->buf, argv[2]);
                                                rmfile = true;
                                        } else {
                                                perror(argv[2]);
                                        }
                                } else {
                                        puts("No such line in file.");
                                }
                        }

                        llist_delete(content);
                }

                fclose(file);

                if (rmfile) {
                        remove(global->bufline);
                }
        } else {
                perror(argv[2]);
        }

        return EXIT_SUCCESS;
}

/*==============================================================================
  End of file
==============================================================================*/

