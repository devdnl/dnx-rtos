/*=========================================================================*//**
@file    httpserver.c

@author  Daniel Zorychta

@brief   The httpserver example

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
#include <unistd.h>
#include <dnx/os.h>
#include <dnx/net.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define CACHE_LEN               2048
#define PATH_LEN                100

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

static const char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";


/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief HTTP server
 */
//==============================================================================
static void serve(net_conn_t *conn)
{
        errno = 0;

        net_buf_t *inbuf;
        if (net_recv(conn, &inbuf) == NET_ERR_OK) {

                char *buf;
                u16_t buf_len;
                net_buf_data(inbuf, (void**)&buf, &buf_len);

                if (buf_len >= 5 && (strncmp("GET /", buf, 5) == 0)) {

                        char *path = calloc(1, PATH_LEN);
                        if (path) {
                                getcwd(path, PATH_LEN);
                                uint cwd_len = strlen(path);

                                char *path_end = strchr(buf + 4, ' ');
                                if (path_end) {
                                        *path_end = '\0';

                                        strcat(path, buf + 4);

                                        printf("Requested: %s\n", path);

                                        if (strlen(path) - 1 == cwd_len) {
                                                strcat(path, "index.html");
                                        }

                                        FILE *file = fopen(path, "r");
                                        if (file) {
                                                net_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1, NET_CONN_FLAG_NOCOPY);

                                                char *cache = malloc(CACHE_LEN);
                                                if (cache) {
                                                        size_t n;
                                                        while ((n = fread(cache, 1, CACHE_LEN, file))) {
                                                                net_write(conn, cache, n, NET_CONN_FLAG_COPY);
                                                        }

                                                        if (ferror(file)) {
                                                                perror(path);
                                                        }

                                                        free(cache);
                                                } else {
                                                        perror(NULL);
                                                }

                                                fclose(file);
                                        } else {
                                                perror(path);
                                        }
                                }

                                free(path);
                        } else {
                                perror(NULL);
                        }
                }

                puts("Connection closed");

                net_close(conn);
                net_delete_buf(inbuf);
        }
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
PROGRAM_MAIN(httpserver, int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        net_conn_t *conn = net_new_conn(NET_CONN_TYPE_TCP);
        if (conn) {
                if (net_bind(conn, NULL, 80) == NET_ERR_OK) {
                        if (net_listen(conn) == NET_ERR_OK) {
                                puts("Listen connection");

                                net_err_t err;
                                do {
                                        net_conn_t *new_conn;
                                        err = net_accept(conn, &new_conn);
                                        if (err == NET_ERR_OK) {
                                                puts("Accept connection");
                                                serve(new_conn);
                                                net_delete_conn(new_conn);
                                        }

                                } while (err == NET_ERR_OK);
                        }
                }

                net_delete_conn(conn);
        }

        puts("Exit");

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
