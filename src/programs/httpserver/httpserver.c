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

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Include files
==============================================================================*/
#include <stdio.h>
#include <string.h>
#include "system/dnx.h"
#include "system/netapi.h"

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
/* put here global variables */
GLOBAL_VARIABLES_SECTION_END

static const char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
static const char http_index_html[] = "<html><head><title>Congrats!</title></head><body><h1>Welcome to our lwIP HTTP server!</h1><p>This is a small test page, served by httpserver-netconn.</body></html>";


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
static void serve(netapi_conn_t *conn)
{
        netapi_buf_t *inbuf;

        if (netapi_recv(conn, &inbuf) == NETAPI_ERR_OK) {
                puts("Buffer received");

                char *buf;
                u16_t buf_len;
                netapi_buf_data(inbuf, (void**)&buf, &buf_len);

                if (buf_len >= 5 && (strncmp("GET /", buf, 5) == 0)) {
                        puts("Sending page");
                        netapi_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1, NETAPI_CONN_FLAG_COPY);
                        netapi_write(conn, http_index_html, sizeof(http_index_html) - 1, NETAPI_CONN_FLAG_COPY);
                }

                netapi_close(conn);
                netapi_delete_buf(inbuf);
        }

        puts("serve(): exit");
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

        netapi_conn_t *conn = netapi_new_conn(NETAPI_CONN_TYPE_TCP);
        if (conn) {
                if (netapi_bind(conn, NULL, 80) == NETAPI_ERR_OK) {
                        if (netapi_listen(conn) == NETAPI_ERR_OK) {
                                puts("Listen connection");

                                netapi_err_t err;
                                do {
                                        netapi_conn_t *new_conn;
                                        err = netapi_accept(conn, &new_conn);
                                        if (err == NETAPI_ERR_OK) {
                                                puts("Accept connection");
                                                serve(new_conn);
                                                netapi_delete_conn(new_conn);
                                        }

                                } while (err == NETAPI_ERR_OK);
                        }
                }

                netapi_delete_conn(conn);
        }

        return 0;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
