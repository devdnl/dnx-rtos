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

#ifdef __cplusplus
extern "C" {
#endif

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
        netapi_conn_t *conn = arg;
        netapi_err_t   err  = NETAPI_ERR_OK;

        while (err == NETAPI_ERR_OK) {
                netapi_buf_t *rx_buffer;
                err = netapi_recv(conn, &rx_buffer);
                if (err == NETAPI_ERR_OK) {
                        char *string;
                        u16_t len;
                        if (netapi_buf_data(rx_buffer, (void *)&string, &len) == NETAPI_ERR_OK) {

                                if (strncmp(string, "test\r\n", len) == 0) {
                                        const char *str = "::Connection test::\n";
                                        netapi_write(conn, str, strlen(str), NETAPI_CONN_FLAG_NOCOPY);

                                        char *test = calloc(1, 50);
                                        strcpy(test, "Test buffer...\n");
                                        netapi_write(conn, test, strlen(test), NETAPI_CONN_FLAG_COPY);
                                        free(test);

                                } else if (strncmp(string, "exit\r\n", len) == 0) {
                                        const char *str = "Exiting...\n";
                                        netapi_write(conn, str, strlen(str), NETAPI_CONN_FLAG_NOCOPY);
                                        err = NETAPI_ERR_CONNECTION_CLOSED;
                                }
                        }

                        netapi_delete_buf(rx_buffer);
                }
        }

        netapi_delete_conn(conn);
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
PROGRAM_MAIN(telnet, int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        errno = 0;

        thread_t *thread[NUMBER_OF_CONNECTIONS] = {NULL};

        netapi_conn_t *listener = netapi_new_conn(NETAPI_CONN_TYPE_TCP);
        if (!listener) {
                puts("Connection failed");
                goto exit;
        }

        if (netapi_bind(listener, (netapi_ip_t *)&ip_addr_any, 23) != NETAPI_ERR_OK) {
                puts("Bind failed");
                goto exit;
        }

        if (netapi_listen(listener) != NETAPI_ERR_OK) {
                puts("Listen error");
                goto exit;
        }

        for (;;) {
                netapi_conn_t *new_conn ;
                if (netapi_accept(listener, &new_conn) != NETAPI_ERR_OK) {
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
                                        netapi_delete_conn(new_conn);
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
                        netapi_delete_conn(new_conn);
                }
        }

exit:
        if (listener) {
                netapi_delete_conn(listener);
        }


        return EXIT_FAILURE;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
