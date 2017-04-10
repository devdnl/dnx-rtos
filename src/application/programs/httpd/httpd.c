/*=========================================================================*//**
@file    httpd.c

@author  Daniel Zorychta

@brief   The HTTP server example

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dnx/net.h>
#include <dnx/misc.h>

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
        char buf[128];
};

static const char HTTP_HTML_HDR[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";

static const char INDEX_HTML[] =
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
 * @return None./t
 */
//==============================================================================
static void serve(SOCKET *socket)
{
        NET_INET_sockaddr_t addr;
        socket_get_address(socket, &addr);

        printf("New connection from: %d.%d.%d.%d\n",
               NET_INET_IPv4_a(addr.addr),
               NET_INET_IPv4_b(addr.addr),
               NET_INET_IPv4_c(addr.addr),
               NET_INET_IPv4_d(addr.addr));

        int sz = socket_recv(socket, global->buf, sizeof(global->buf), NET_FLAGS__FREEBUF);
        if (sz >= 5 && isstreqn("GET /", global->buf, 5)) {
                socket_send(socket, HTTP_HTML_HDR, sizeof(HTTP_HTML_HDR) - 1, NET_FLAGS__NOCOPY);
                socket_send(socket, INDEX_HTML, sizeof(INDEX_HTML) - 1, NET_FLAGS__NOCOPY);
        }

        socket_delete(socket);
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
int_main(httpd, STACK_DEPTH_LOW, int argc, char *argv[])
{
        UNUSED_ARG2(argc, argv);

        static const NET_INET_sockaddr_t ADDR_ANY = {
            .addr = NET_INET_IPv4_ANY,
            .port = 80
        };

        SOCKET *socket = socket_new(NET_FAMILY__INET, NET_PROTOCOL__TCP);
        if (socket) {
                if (socket_bind(socket, &ADDR_ANY) == 0) {
                        if (socket_listen(socket) == 0) {
                                int err;
                                do {
                                        SOCKET *new_socket;
                                        err = socket_accept(socket, &new_socket);
                                        if (!err) {
                                                serve(new_socket);
                                        }
                                } while (!err);
                        }
                }

                socket_delete(socket);
        }

        if (errno != 0) {
                perror("Socket error");
        }

        return 0;
}

/*==============================================================================
  End of file
==============================================================================*/
