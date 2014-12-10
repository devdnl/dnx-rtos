/*=========================================================================*//**
@file    telnet.c

@author  Daniel Zorychta

@brief   Telnet example program

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
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <dnx/net.h>
#include <dnx/thread.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define NUMBER_OF_CONNECTIONS           4

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
/* put here global variables */
GLOBAL_VARIABLES_SECTION_END

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Telnet thread
 */
//==============================================================================
static void telnet_thread(void *arg)
{
        net_conn_t *conn = arg;
        net_err_t   err  = NET_ERR_OK;

        while (err == NET_ERR_OK) {
                net_buf_t *rx_buffer;
                err = net_conn_receive(conn, &rx_buffer);
                if (err == NET_ERR_OK) {
                        char *string;
                        u16_t len;
                        if (net_buf_data(rx_buffer, (void *)&string, &len) == NET_ERR_OK) {

                                if (strncmp(string, "test\r\n", len) == 0) {
                                        const char *str = "::Connection test::\n";
                                        net_conn_write(conn, str, strlen(str), NET_CONN_FLAG_NOCOPY);

                                        char *test = calloc(1, 50);
                                        strcpy(test, "Test buffer...\n");
                                        net_conn_write(conn, test, strlen(test), NET_CONN_FLAG_COPY);
                                        free(test);

                                } else if (strncmp(string, "exit\r\n", len) == 0) {
                                        const char *str = "Exiting...\n";
                                        net_conn_write(conn, str, strlen(str), NET_CONN_FLAG_NOCOPY);
                                        err = NET_ERR_CONNECTION_CLOSED;
                                }
                        }

                        net_buf_delete(rx_buffer);
                }
        }

        net_conn_delete(conn);
        puts("Thread closed");
}

//==============================================================================
/**
 * @brief Program main function
 *
 * @param  argc         count of arguments
 * @param *argv[]       argument table
 *
 * @return program status
 */
//==============================================================================
PROGRAM_MAIN(telnet, STACK_DEPTH_LOW, int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        errno = 0;

        thread_t *thread[NUMBER_OF_CONNECTIONS] = {NULL};

        net_conn_t *listener = net_conn_new(NET_CONN_TYPE_TCP);
        if (!listener) {
                puts("Connection failed");
                goto exit;
        }

        if (net_conn_bind(listener, (net_ip_t *)&ip_addr_any, 23) != NET_ERR_OK) {
                puts("Bind failed");
                goto exit;
        }

        if (net_conn_listen(listener) != NET_ERR_OK) {
                puts("Listen error");
                goto exit;
        }

        for (;;) {
                net_conn_t *new_conn ;
                if (net_conn_accept(listener, &new_conn) != NET_ERR_OK) {
                        puts("Connection accept error");
                        continue;
                }

                bool thread_created = false;
                for (int i = 0; i < NUMBER_OF_CONNECTIONS; i++) {
                        if (thread[i] == NULL) {
                                errno = 0;
                                thread[i] = thread_new(telnet_thread, STACK_DEPTH_LOW, new_conn);
                                if (thread[i] == NULL) {
                                        perror("New thread");
                                        net_conn_delete(new_conn);
                                } else {
                                        puts("Started new thread");
                                        thread_created = true;
                                        break;
                                }
                        } else {
                                if (thread_is_finished(thread[i])) {
                                        thread_delete(thread[i]);
                                        thread[i] = NULL;
                                        i--;
                                }
                        }
                }

                if (!thread_created) {
                        puts("Reached maximum number of connections");
                        net_conn_delete(new_conn);
                }
        }

exit:
        if (listener) {
                net_conn_delete(listener);
        }


        return EXIT_FAILURE;
}

/*==============================================================================
  End of file
==============================================================================*/
