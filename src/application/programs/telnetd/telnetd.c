/*=========================================================================*//**
@file    telnetd.c

@author  Daniel Zorychta

@brief   Telnet example program

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
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <dnx/net.h>
#include <dnx/thread.h>
#include <dnx/os.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define NUMBER_OF_CONNECTIONS           3
#define BUF_SIZE                        100
#define PIPE_NAME_LEN                   24
#define TELNET_CFG_BYTE                 0xFF
#define PROGRAM_NAME                    "dsh"
#define RECEIVE_TIMOUT                  100
#define SEND_TIMEOUT                    3000
#define TELNET_PORT                     23

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
        const char *msg;
        mutex_t    *mtx;
        uint8_t     conn_cnt;
};


static const NET_INET_sockaddr_t IP_ADDR_ANY = {
        .addr = NET_INET_IPv4_ANY,
        .port = TELNET_PORT
};

static const thread_attr_t thread_attr = {
        .priority    = PRIORITY_NORMAL,
        .stack_depth = STACK_DEPTH_LOW,
        .detached    = true
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
static char *create_and_open_pipe(SOCKET *socket, char c, FILE **f)
{
        char *name = calloc(1, PIPE_NAME_LEN);
        if (name) {
                snprintf(name, PIPE_NAME_LEN, "/run/tn%x%c", cast(int, socket), c);

                if (mkfifo(name, 0666) == 0) {
                        *f = fopen(name, "r+");
                        if (*f) {
                                return name;
                        }

                        remove(name);
                }

                free(name);
                name = NULL;
        }

        return name;
}

//==============================================================================
/**
 * @brief Function replaces CRLF line ending by LF.
 * @param buf   buffer to modify
 * @param len   buffer length
 */
//==============================================================================
static void replace_CRLF_by_LF(char *buf, size_t len)
{
        if (buf[len - 1] == '\n' && buf[len - 2] == '\r') {
                buf[len - 2] = '\n';
                buf[len - 1] = '\0';
        }
}

//==============================================================================
/**
 * @brief  Telnet thread
 * @param  arg   thread argument - connection
 * @return None
 */
//==============================================================================
static void telnet_thread(void *arg)
{
        SOCKET *sock          = arg;
        FILE   *fin           = NULL;
        FILE   *fout          = NULL;
        char   *buf           = NULL;
        pid_t   proc          = 0;
        char   *pipe_in_name  = NULL;
        char   *pipe_out_name = NULL;

        // increase number of threads
        if (mutex_lock(global->mtx, MAX_DELAY_MS)) {
                global->conn_cnt++;
                mutex_unlock(global->mtx);
        }

        // print client IP
        NET_INET_sockaddr_t addr;
        socket_get_address(sock, &addr);

        printf("New connection from: %d.%d.%d.%d\n",
               NET_INET_IPv4_a(addr.addr),
               NET_INET_IPv4_b(addr.addr),
               NET_INET_IPv4_c(addr.addr),
               NET_INET_IPv4_d(addr.addr));

        // create buffer for program output data
        buf = malloc(BUF_SIZE);
        if (!buf)
                goto exit;

        // create program input pipe
        pipe_in_name = create_and_open_pipe(sock, 'i', &fin);
        if (!pipe_in_name)
                goto exit;

        // create program output pipe
        pipe_out_name = create_and_open_pipe(sock, 'o', &fout);
        if (!pipe_out_name)
                goto exit;

        // start program
        process_attr_t process_attr = {
                .cwd = "/",
                .f_stderr = fout,
                .f_stdout = fout,
                .f_stdin  = fin,
                .priority = PRIORITY_NORMAL,
                .detached = false
        };

        proc = process_create(PROGRAM_NAME, &process_attr);
        if (proc == 0)
                goto exit;

        socket_set_recv_timeout(sock, RECEIVE_TIMOUT);
        socket_set_send_timeout(sock, SEND_TIMEOUT);

        // handle telnet connection
        while (true) {
                // receive input packet from telnet client
                errno = 0;
                int len = socket_read(sock, buf, BUF_SIZE);

                if ((len == -1) && (errno != ETIME)) {
                        break;
                }

                // write incoming data to running program
                if (len > 0 && buf[0] != TELNET_CFG_BYTE) {
                        replace_CRLF_by_LF(buf, len);
                        len = strnlen(buf, len);
                        fwrite(buf, 1, len, fin);

                }

                // send data from running program
                do {
                        ioctl(fileno(fout), IOCTL_VFS__NON_BLOCKING_RD_MODE);
                        len = fread(buf, 1, BUF_SIZE, fout);
                        if (len > 0) {
                                len = socket_write(sock, buf, len);
                        }
                } while (len > 0);

                // check if program is finished
                if (process_wait(proc, NULL, 0) == 0) {
                        break;
                }
        }

exit:
        if (buf)
                free(buf);

        if (fin)
                fclose(fin);

        if (fout)
                fclose(fout);

        if (proc) {
                process_kill(proc);
        }

        if (pipe_in_name) {
                remove(pipe_in_name);
                free(pipe_in_name);
        }

        if (pipe_out_name) {
                remove(pipe_out_name);
                free(pipe_out_name);
        }

        NET_INET_sockaddr_t sockaddr;
        socket_get_address(sock, &sockaddr);
        printf("Connection closed: %d.%d.%d.%d\n",
               NET_INET_IPv4_a(sockaddr.addr),
               NET_INET_IPv4_b(sockaddr.addr),
               NET_INET_IPv4_c(sockaddr.addr),
               NET_INET_IPv4_d(sockaddr.addr));

        socket_close(sock);

        if (mutex_lock(global->mtx, MAX_DELAY_MS)) {
                global->conn_cnt--;
                mutex_unlock(global->mtx);
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
int_main(telnetd, STACK_DEPTH_VERY_LOW, int argc, char *argv[])
{
        (void) argc;
        (void) argv;

        errno = 0;

        mkdir("/run", 0777);


        NET_INET_status_t netstat = {0};

        do {
                ifstatus(NET_FAMILY__INET, &netstat);
                msleep(500);
        } while (! ( (netstat.state == NET_INET_STATE__DHCP_CONFIGURED)
                   ||(netstat.state == NET_INET_STATE__STATIC_IP) ) );



        SOCKET *listener = socket_open(NET_FAMILY__INET, NET_PROTOCOL__TCP);
        if (!listener) {
                global->msg = "Connection failed";
                goto exit;
        }

        global->mtx = mutex_new(MUTEX_TYPE_NORMAL);
        if (!global->mtx) {
                global->msg = "Mutex not created";
                goto exit;
        }

        if (socket_bind(listener, &IP_ADDR_ANY) != 0) {
                global->msg = "Bind failed";
                goto exit;
        }

        if (socket_listen(listener) != 0) {
                global->msg = "Listen error";
                goto exit;
        }

        for (;;) {
                puts("Waiting for connection...");

                // wait for connection
                SOCKET *client = NULL;
                if (socket_accept(listener, &client) != 0) {
                        puts("Connection accept error");
                        continue;
                }

                // check number of connections
                if (global->conn_cnt >= NUMBER_OF_CONNECTIONS) {
                        puts("Reached maximum number of connections.");
                        socket_close(client);
                        continue;
                }

                // create new thread to handle connection
                if (thread_create(telnet_thread, &thread_attr, client) == 0) {
                        puts("Thread not started");
                }
        }

exit:
        if (errno) {
                perror(global->msg);
        }

        if (listener) {
                socket_close(listener);
        }

        if (global->mtx) {
                mutex_delete(global->mtx);
        }

        return EXIT_FAILURE;
}

/*==============================================================================
  End of file
==============================================================================*/
