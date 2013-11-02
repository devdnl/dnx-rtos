/*=========================================================================*//**
@file    telnet.c

@author  Daniel Zorychta

@brief   The simple example program

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

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/




//static void EchoRequest( struct netconn *pxNetCon ) {
//        struct  netbuf *pxRxBuffer;
//        portCHAR *pcRxString;
//        unsigned portSHORT usLength;
//
//        pxRxBuffer = netconn_recv( pxNetCon );< /FONT >
//        if ( pxRxBuffer != NULL ){
//                netbuf_data( pxRxBuffer, ( void  * ) &pcRxString, &usLength );
//                if (  pcRxString != NULL){
//                        netconn_write( pxNetCon, pcRxString, (u16_t) usLength, NETCONN_COPY );
//                }
//        netbuf_delete( pxRxBuffer );
//        }
//}
//
//portTASK_FUNCTION( vBasicTFTPServer, pvParameters ){
//        struct netconn *pxTCPListener, *pxNewConnection;
//        pxTCPListener = netconn_new( NETCONN_TCP );
//
//        netconn_bind(pxTCPListener, NULL, 23 );
//        netconn_listen( pxTCPListener );
//
//        for( ;; ){
//                pxNewConnection = netconn_accept(pxTCPListener);
//                if(pxNewConnection != NULL){
//                        EchoRequest(pxNewConnection);
//                }
//        }
//}


static void echo_request(netapi_conn_t *conn)
{
        netapi_buf_t *rx_buffer;

        if (netapi_recv(conn, &rx_buffer) == NETAPI_ERR_OK) {
                puts("New rx buffer created");

                char *data;
                u16_t len;
                if (netapi_buf_data(rx_buffer, (void *)&data, &len) == NETAPI_ERR_OK) {
                        puts("Data read from netapi_buf_t object");

                        puts(data);

                        if (netapi_write(conn, data, len, NETAPI_CONN_FLAG_COPY) == NETAPI_ERR_OK) {
                                puts("Data sent back");
                        } else {
                                puts("Unable to send data!");
                        }
                }
        } else {
                puts("Rx buffer creation error!");
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
PROGRAM_MAIN(telnet, int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        netapi_conn_t *listener = netapi_new_conn(NETAPI_CONN_TYPE_TCP);
        if (listener) {
                puts("New connection created");

                if (netapi_bind(listener, (netapi_ip_t *)&ip_addr_any, 23) == NETAPI_ERR_OK) {
                        puts("Binded successfully");

                        if (netapi_listen(listener) == NETAPI_ERR_OK) {
                                puts("Successfully goes to listen mode");

                                for (;;) {
                                        netapi_conn_t *new_connection;
                                        if (netapi_accept(listener, &new_connection) == NETAPI_ERR_OK) {
                                                echo_request(new_connection);
                                        } else {
                                                puts("Acceptation error!");
                                        }
                                }
                        } else {
                                puts("Unable to start in listen mode!");
                        }
                } else {
                        puts("Address and port bind error!");
                }
        } else {
                puts("Unable to create connection!");
        }

        return -1;
}

#ifdef __cplusplus
}
#endif

/*==============================================================================
  End of file
==============================================================================*/
