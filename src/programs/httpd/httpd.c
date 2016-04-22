/*=========================================================================*//**
@file    httpd.c

@author  Daniel Zorychta

@brief   The HTTP server example

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <stdlib.h>
#include <unistd.h>
#include <dnx/net.h>

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
GLOBAL_VARIABLES_SECTION {
};

static const char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";

static const char index_html[] =
        "<!DOCTYPE html>"
        "<html>"
                "<head>"
                        "<meta charset=\"utf-8\">"
                        "<title>Welcome to dnx RTOS HTTP server!</title>"

                        "<style>"
                        "body {"
                                "background-image: url(\"http://www.socwall.com/images/wallpapers/10022-1920x1200.jpg\");"
                                "background-repeat: no-repeat;"
                                "background-attachment: fixed;}"
                        "</style>"
                "</head>"
                "<body>"
                        "<img src=\"http://www.dnx-rtos.org/wp-content/uploads/2014/02/dnx_logo.png\""
                        "style=\"opacity: 0.5; position: absolute; right: 20px; bottom: 20px;\">"

                        "<div style=\"width:800px;"
                                     "height:500px;"
                                     "position:absolute;"
                                     "background: white;"
                                     "opacity: 0.8;"
                                     "padding:10px;"
                                     "margin-top:-250px;"
                                     "margin-left:-400px;"
                                     "border-top-left-radius: 10px;"
                                     "border-top-right-radius: 10px;"
                                     "border-bottom-left-radius: 10px;"
                                     "border-bottom-right-radius: 10px;"
                                     "top:50%;"
                                     "left:50%;\">"

                                "<font size=\"5\">"
                                "<p><center>Welcome to dnx RTOS HTTP server!</center><p>"
                                "</font>"

                                "Visit dnx RTOS pages:<p>"
                                "<ul>"
                                        "<li><a href=\"http://www.dnx-rtos.org\">dnx RTOS Homepage</a></li>"
                                        "<li><a href=\"http://www.dnx-rtos.org/forum\">dnx RTOS Forum</a></li>"
                                        "<li><a href=\"https://www.youtube.com/channel/UCRLYrTTz0OUfRp_eaCgXlvA/feed\">dnx RTOS YouTube Channel</a></li>"
                                "</ul>"
                        "</div>"
                "</body>"
        "</html>";

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  HTTP server
 * @param  conn  connection
 * @return None
 */
//==============================================================================
//static void serve(net_conn_t *conn)
//{
//        net_buf_t *inbuf;
//        if (net_conn_receive(conn, &inbuf) == NET_ERR_OK) {
//
//                char *buf;
//                u16_t buf_len;
//                net_buf_data(inbuf, (void**)&buf, &buf_len);
//
//                if (buf_len >= 5 && (strncmp("GET /", buf, 5) == 0)) {
//                        net_conn_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1, NET_CONN_FLAG_NOCOPY);
//                        net_conn_write(conn, index_html, sizeof(index_html) - 1, NET_CONN_FLAG_NOCOPY);
//                }
//
//                puts("Connection closed");
//
//                net_buf_delete(inbuf);
//        }
//
//        net_conn_close(conn);
//        net_conn_delete(conn);
//}

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
int_main(httpd, STACK_DEPTH_LOW, int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        SOCKET *socket = socket_new(NET_FAMILY__INET, NET_PROTOCOL__TCP);
        if (socket) {
                static const NET_INET_addr_t addr = {
                    .addr = NET_INET_IP(0,0,0,0),
                    .port = 80
                };

                socket_bind(socket, &addr, sizeof(NET_INET_addr_t));


                socket_delete(socket);
        }

        puts("Exit");

//        net_conn_t *conn = net_conn_new(NET_CONN_TYPE_TCP);
//        if (conn) {
//                if (net_conn_bind(conn, NULL, 80) == NET_ERR_OK) {
//                        if (net_conn_listen(conn) == NET_ERR_OK) {
//                                puts("Listen connection");
//
//                                net_err_t err;
//                                do {
//                                        net_conn_t *new_conn;
//                                        err = net_conn_accept(conn, &new_conn);
//                                        if (err == NET_ERR_OK) {
//                                                puts("Accept connection");
//                                                serve(new_conn);
//                                        }
//
//                                } while (err == NET_ERR_OK);
//                        }
//                }
//
//                net_conn_delete(conn);
//        }
//
//        puts("Exit");

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
