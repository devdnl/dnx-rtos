/*=========================================================================*//**
@file    telnetd.c

@author  Daniel Zorychta

@brief   Telnet example program

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
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dnx/net.h>
#include <dnx/thread.h>
#include <sys/ioctl.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define NUMBER_OF_CONNECTIONS           4
#define BUF_SIZE                        100
#define PIPE_NAME_LEN                   24
#define TELNET_CFG_BYTE                 0xFF
#define PROGRAM_NAME                    "dsh"
#define RECEIVE_TIMOUT                  100
#define SEND_TIMEOUT                    3000

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

/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/
//==============================================================================
/**
 * @brief  Create pipe with individual name and open created file
 * @param[in]  conn         connection pointer used to create pipe name
 * @param[in]  c            additional character used to create pipe name
 * @param[out] f            opened pipe
 * @return On success pointer to name buffer is returned, otherwise NULL.
 */
//==============================================================================
//static char *create_and_open_pipe(net_conn_t *conn, char c, FILE **f)
//{
//        char *name = calloc(1, PIPE_NAME_LEN);
//        if (name) {
//                snprintf(name, PIPE_NAME_LEN, "/tmp/tn%x%c", conn, c);
//
//                if (mkfifo(name, 0666) == 0) {
//                        *f = fopen(name, "r+");
//                        if (*f) {
//                                return name;
//                        }
//
//                        remove(name);
//                }
//
//                free(name);
//                name = NULL;
//        }
//
//        return name;
//}
//
////==============================================================================
///**
// * @brief  Telnet thread
// * @param  arg   thread argument - connection
// * @return None
// */
////==============================================================================
//static void telnet_thread(void *arg)
//{
//        net_err_t   err           = NET_ERR_OK;
//        net_conn_t *conn          = arg;
//        FILE       *fin           = NULL;
//        FILE       *fout          = NULL;
//        char       *strout        = NULL;
//        prog_t     *prog          = NULL;
//        char       *pipe_in_name  = NULL;
//        char       *pipe_out_name = NULL;
//        net_ip_t    ip;
//        u16_t       port;
//
//        // create buffer for program output data
//        strout = malloc(BUF_SIZE);
//        if (!strout)
//                goto exit;
//
//        // create program input pipe
//        pipe_in_name = create_and_open_pipe(conn, 'i', &fin);
//        if (!pipe_in_name)
//                goto exit;
//
//        // create program output pipe
//        pipe_out_name = create_and_open_pipe(conn, 'o', &fout);
//        if (!pipe_out_name)
//                goto exit;
//
//        // start program
//        prog = program_new(PROGRAM_NAME, "/", fin, fout, fout);
//        if (!prog)
//                goto exit;
//
//        // handle telnet connection
//        while (err != NET_ERR_CONNECTION_CLOSED) {
//                net_conn_set_receive_timeout(conn, RECEIVE_TIMOUT);
//                net_conn_set_send_timeout(conn, SEND_TIMEOUT);
//
//                // receive input packet from telnet client
//                net_buf_t *rx_buffer;
//                err = net_conn_receive(conn, &rx_buffer);
//                if (err == NET_ERR_OK) {
//                        char *string; u16_t len;
//                        if (net_buf_data(rx_buffer, (void *)&string, &len) == NET_ERR_OK) {
//
//                                if (string[0] != TELNET_CFG_BYTE) {
//                                        // repleace \r\n to \n
//                                        if (string[len - 1] == '\n' && string[len - 2] == '\r') {
//                                                string[len - 2] = '\n';
//                                                string[len - 1] = '\0';
//                                        }
//
//                                        len = strnlen(string, len);
//                                        fwrite(string, 1, len, fin);
//                                }
//                        }
//
//                        net_buf_delete(rx_buffer);
//                }
//
//                // send data from started program
//                int n;
//                do {
//                        ioctl(fout, IOCTL_VFS__NON_BLOCKING_RD_MODE);
//                        n = fread(strout, 1, BUF_SIZE, fout);
//                        if (n) {
//                                err = net_conn_write(conn, strout, n, NET_CONN_FLAG_COPY);
//                        }
//                } while (n);
//
//                // check if program is finished
//                if (program_is_closed(prog)) {
//                        err = NET_ERR_CONNECTION_CLOSED;
//                }
//        }
//
//        exit:
//        net_conn_get_address(conn, &ip, &port, false);
//        net_conn_delete(conn);
//
//        if (strout)
//                free(strout);
//
//        if (fin)
//                fclose(fin);
//
//        if (fout)
//                fclose(fout);
//
//        if (prog) {
//                program_kill(prog);
//                program_delete(prog);
//        }
//
//        if (pipe_in_name) {
//                remove(pipe_in_name);
//                free(pipe_in_name);
//        }
//
//        if (pipe_out_name) {
//                remove(pipe_out_name);
//                free(pipe_out_name);
//        }
//
//        printf("Connection closed: %d.%d.%d.%d\n",
//               net_IP_get_part_a(&ip),
//               net_IP_get_part_b(&ip),
//               net_IP_get_part_c(&ip),
//               net_IP_get_part_d(&ip));
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
int_main(telnetd, STACK_DEPTH_LOW, int argc, char *argv[])
{
        (void) argc;
        (void) argv;

#warning telnetd: TODO network sockets

//        errno = 0;
//
//        thread_t *thread[NUMBER_OF_CONNECTIONS];
//        for (int i = 0; i < NUMBER_OF_CONNECTIONS; i++) {
//                thread[i] = NULL;
//        }
//
//        net_conn_t *listener = net_conn_new(NET_CONN_TYPE_TCP);
//        if (!listener) {
//                puts("Connection failed");
//                goto exit;
//        }
//
//        if (net_conn_bind(listener, (net_ip_t *)&ip_addr_any, 23) != NET_ERR_OK) {
//                puts("Bind failed");
//                goto exit;
//        }
//
//        if (net_conn_listen(listener) != NET_ERR_OK) {
//                puts("Listen error");
//                goto exit;
//        }
//
//        for (;;) {
//                net_conn_t *new_conn ;
//                if (net_conn_accept(listener, &new_conn) != NET_ERR_OK) {
//                        puts("Connection accept error");
//                        continue;
//                }
//
//                bool thread_created = false;
//                for (int i = 0; i < NUMBER_OF_CONNECTIONS; i++) {
//                        if (thread[i] == NULL) {
//                                errno = 0;
//                                thread[i] = thread_new(telnet_thread, STACK_DEPTH_LOW, new_conn);
//                                if (thread[i] == NULL) {
//                                        perror(NULL);
//                                        net_conn_delete(new_conn);
//                                } else {
//                                        net_ip_t ip;
//                                        u16_t    port;
//                                        net_conn_get_address(new_conn, &ip, &port, false);
//
//                                        printf("New connection: %d.%d.%d.%d\n",
//                                               net_IP_get_part_a(&ip),
//                                               net_IP_get_part_b(&ip),
//                                               net_IP_get_part_c(&ip),
//                                               net_IP_get_part_d(&ip));
//
//                                        thread_created = true;
//                                        break;
//                                }
//                        } else {
//                                if (thread_is_finished(thread[i])) {
//                                        thread_delete(thread[i]);
//                                        thread[i] = NULL;
//                                        i--;
//                                }
//                        }
//                }
//
//                if (!thread_created) {
//                        puts("Reached maximum number of connections");
//                        net_conn_delete(new_conn);
//                }
//        }
//
//exit:
//        if (listener) {
//                net_conn_delete(listener);
//        }


        return EXIT_FAILURE;
}

/*==============================================================================
  End of file
==============================================================================*/
