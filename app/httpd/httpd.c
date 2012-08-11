/*=============================================================================================*//**
@file    httpd.c

@author  Daniel Zorychta

@brief   HTTP server

@note    Copyright (C) 2012 Daniel Zorychta <daniel.zorychta@gmail.com>

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


*//*==============================================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                            Include files
==================================================================================================*/
#include "httpd.h"
#include "net.h"
#include <string.h>


/*==================================================================================================
                                  Local symbolic constants/macros
==================================================================================================*/


/*==================================================================================================
                                   Local types, enums definitions
==================================================================================================*/


/*==================================================================================================
                                      Local function prototypes
==================================================================================================*/


/*==================================================================================================
                                      Local object definitions
==================================================================================================*/
static const ch_t data_hello_html[] =
{
"<!DOCTYPE html>\
<html>\
<body>\
<h1>Hello world MAIN</h1>\
<p>My first paragraph.</p>\
<a href=\"./STM32F107LED.html\">LED</a>\
<img src=\"./images/sunny.jpg\"/>\
</body>\
</html>"
};

/*==================================================================================================
                                     Shared object definitions
==================================================================================================*/


/*==================================================================================================
                                        Function definitions
==================================================================================================*/

//================================================================================================//
/**
 * @brief
 */
//================================================================================================//
APPLICATION(httpd)
{
      InitApp();

      (void) argv;

      AGAIN:
      for(;;)
      {
            netStatus_t status;

            netSoc_t socket = NET_NewTCPSocket(IP_ADDR_ANY, 600);

            if (!socket)
            {
                  print("Bad socket.\n");
                  Sleep(1000);
                  continue;
            }

            for (;;)
            {
                  if (NET_GetTCPStatus(socket, &status) != STD_STATUS_OK)
                  {
                        print("Get status error.\n");
                        Sleep(1000);
                        NET_CloseTCPSocket(socket);
                        goto AGAIN;
                  }

                  if (status.flag.BindAccepted)
                  {
                        print("Bind accepted\n");

                        for (;;)
                        {
                              NET_GetTCPStatus(socket, &status);

                              if (status.flag.ConnectionError)
                              {
                                    print("Closed TCP\n");
                                    NET_TCPClose(socket);
                              }

                              if (status.flag.DataPosted)
                              {
                                    print("data posted\n");
                                    NET_TCPClose(socket);
                              }

                              if (status.flag.DataReceived)
                              {
                                    print("Data received\n");

                                    if (NET_TCPAcceptReceived(socket) == STD_STATUS_ERROR)
                                    {
                                          print("TCPAcceptReceived error\n");
                                    }
                                    else
                                    {
                                          ch_t *data = NET_GetReceivedDataBuffer(socket);

                                          if (strncmp(data, "GET /hello", 10) == 0)
                                          {
                                                print("Sending page\n");

                                                NET_FreeReceivedBuffer(socket);

                                                ch_t *file = (ch_t*)&data_hello_html;
                                                u32_t size = sizeof(data_hello_html);

                                                NET_TCPWrite(socket, file, &size);
                                          }
                                    }
                              }

                              if (status.flag.Poll)
                              {
                                    print("Polling\n");
                                    NET_TCPAcceptPoll(socket);
                              }
                        }
                  }
            }
      }

      Exit(STD_STATUS_ERROR);
}

#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                            End of file
==================================================================================================*/
