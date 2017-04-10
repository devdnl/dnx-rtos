/*=========================================================================*//**
@file    loopd.cpp

@author  Daniel Zorychta

@brief   Realize the virtual loop device (transparent device operations example).

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
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <dnx/misc.h>
#include <stdint.h>
#include <dnx/thread.h>
#include <sys/types.h>

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define BUFSIZE         128

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
        FILE           *loop_dev;
        FILE           *src_dev;
        uint8_t         buffer[BUFSIZE];
        bool            log;
        LOOP_request_t  client_req;
};


/*==============================================================================
  Exported object definitions
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief  Realize client to host transaction
 * @param  i    client request iterator
 * @return None
 */
//==============================================================================
static void client2host_transaction(int i)
{
        if (global->log) {
                printf("CRQ %d: client2host [size = %d, seek = %d] => ", i,
                       static_cast<int>(global->client_req.arg.rw.size),
                       static_cast<int>(global->client_req.arg.rw.seek) );
        }

        fseek(global->src_dev, global->client_req.arg.rw.seek, SEEK_SET);

        size_t n  = 0;
        size_t rd = global->client_req.arg.rw.size;
        while (rd) {
                size_t part = min(rd, sizeof(global->buffer));

                LOOP_buffer_t buf;
                buf.data = reinterpret_cast<uint8_t*>(global->buffer);
                buf.err  = ESUCC;
                buf.size = part;

                int s = ioctl(global->loop_dev, IOCTL_LOOP__HOST_READ_DATA_FROM_CLIENT, &buf);
                if (s == 0) {
                        n  += fwrite(global->buffer, 1, part, global->src_dev);
                        rd -= part;
                } else {
                        break;
                }
        }

        if (global->log) {
                printf("%u\n", n);
        }
}

//==============================================================================
/**
 * @brief  Realize host to client transaction
 * @param  i    client request iterator
 * @return None
 */
//==============================================================================
static void host2client_transaction(int i)
{
        if (global->log) {
                printf("CRQ %d: host2client [size = %d, seek = %d] => ", i,
                       static_cast<int>(global->client_req.arg.rw.size),
                       static_cast<int>(global->client_req.arg.rw.seek) );
        }

        fseek(global->src_dev, global->client_req.arg.rw.seek, SEEK_SET);

        size_t m  = 0;
        size_t wr = global->client_req.arg.rw.size;
        while (wr) {
                errno = 0;
                int n = fread(global->buffer, 1, min(wr, sizeof(global->buffer)), global->src_dev);
                if (n == 0) {
                        break;
                }

                LOOP_buffer_t buf;
                buf.data = reinterpret_cast<u8_t*>(global->buffer);
                buf.err  = ESUCC;
                buf.size = n;

                if (ioctl(global->loop_dev, IOCTL_LOOP__HOST_WRITE_DATA_TO_CLIENT, &buf) != 0) {
                        break;
                } else {
                        wr -= n;
                        m  += n;
                }
        }

        if (wr) {
                LOOP_buffer_t buf;
                buf.data = NULL;
                buf.size = 0;
                buf.err  = errno;
                ioctl(global->loop_dev, IOCTL_LOOP__HOST_WRITE_DATA_TO_CLIENT, &buf);
        }

        if (global->log) {
                printf("%u\n", m);
        }
}

//==============================================================================
/**
 * @brief  Realize ioctl request handling
 * @param  i    client request iterator
 * @return None
 */
//==============================================================================
static void ioctl_request(int i)
{
        if (global->log) {
                printf("CRQ %i: ioctl [rq = %d, arg = 0x%x] => ", i,
                       global->client_req.arg.ioctl.request,
                       reinterpret_cast<int>(global->client_req.arg.ioctl.arg));
        }

        errno = 0;
        LOOP_ioctl_response_t ioctl_resp;

        // send to source file the same IO request as was got from loop device
        if (ioctl(global->src_dev,
                  global->client_req.arg.ioctl.request,
                  global->client_req.arg.ioctl.arg) != 0) {

                ioctl_resp.err = errno;

        } else {
                ioctl_resp.err = ESUCC;
        }

        ioctl(global->loop_dev, IOCTL_LOOP__HOST_SET_IOCTL_STATUS, &ioctl_resp);

        if (global->log) {
                printf("%s\n", strerror(errno));
        }
}

//==============================================================================
/**
 * @brief  Realize file flush request handling
 * @param  i    client request iterator
 * @return None
 */
//==============================================================================
static void flush_request(int i)
{
        if (global->log) {
                printf("CRQ %d: flush => ", i);
        }

        errno = 0;
        fflush(global->src_dev);
        ioctl(global->loop_dev, IOCTL_LOOP__HOST_FLUSH_DONE, &errno);

        if (global->log) {
                printf("%s\n", strerror(errno));
        }
}

//==============================================================================
/**
 * @brief  Realize file statistics request handling
 * @param  i    client request iterator
 * @return None
 */
//==============================================================================
static void stat_request(int i)
{
        if (global->log) {
                printf("CRQ %d: stat => ", i);
        }

        errno = 0;
        struct stat stat;
        LOOP_stat_response_t stat_resp;

        if (fstat(global->src_dev, &stat) == 0) {
                stat_resp.err  = ESUCC;
                stat_resp.size = stat.st_size;
        } else {
                stat_resp.err  = errno;
                stat_resp.size = 0;
        }

        ioctl(global->loop_dev, IOCTL_LOOP__HOST_SET_DEVICE_STATS, &stat_resp);

        if (global->log) {
                printf("size = %d: %s\n", static_cast<int>(stat.st_size), strerror(errno));
        }
}

//==============================================================================
/**
 * @brief  Main loop daemon
 * @param  i    client request iterator
 * @return None
 */
//==============================================================================
static void loopd(const char *src, const char *loop)
{
        errno = 0;

        global->loop_dev = fopen(loop, "r+");
        if (!global->loop_dev) {
                perror(loop);
                exit(EXIT_FAILURE);
        }

        if (ioctl(global->loop_dev, IOCTL_LOOP__HOST_OPEN) != 0) {
                perror(loop);
                fclose(global->loop_dev);
                exit(EXIT_FAILURE);
        }

        global->src_dev = fopen(src, "r+");
        if (!global->src_dev) {
                perror(src);
                ioctl(global->loop_dev, IOCTL_LOOP__HOST_CLOSE);
                fclose(global->loop_dev);
                exit(EXIT_FAILURE);
        }

        ioctl(stdin, IOCTL_VFS__NON_BLOCKING_RD_MODE);

        puts("Enter 'q' to quit");

        int i = 1;
        while (getchar() != 'q') {

                errno = 0;
                if (ioctl(global->loop_dev, IOCTL_LOOP__HOST_WAIT_FOR_REQUEST, &global->client_req) != 0) {
                        perror(loop);
                        continue;
                }

                switch (global->client_req.cmd) {
                case LOOP_CMD__IDLE:
                        if (global->log) {
                                printf("CRQ %d: idle\n", i);
                        }
                        break;

                case LOOP_CMD__TRANSMISSION_CLIENT2HOST:
                        client2host_transaction(i);
                        break;

                case LOOP_CMD__TRANSMISSION_HOST2CLIENT:
                        host2client_transaction(i);
                        break;

                case LOOP_CMD__IOCTL_REQUEST:
                        ioctl_request(i);
                        break;

                case LOOP_CMD__FLUSH_BUFFERS:
                        flush_request(i);
                        break;

                case LOOP_CMD__DEVICE_STAT:
                        stat_request(i);
                        break;

                default:
                        if (global->log) {
                                printf("CRQ %i: unknown\n", i);
                        }
                        break;
                }

                i++;
        }

        ioctl(stdin, IOCTL_VFS__DEFAULT_RD_MODE);

        ioctl(global->loop_dev, IOCTL_LOOP__HOST_CLOSE);
        fclose(global->loop_dev);
        fclose(global->src_dev);
}


//==============================================================================
/**
 * @brief loopd main function
 */
//==============================================================================
int_main(loopd, STACK_DEPTH_LOW, int argc, char *argv[])
{
        if (argc < 3) {
                printf("Usage: %s [-l] [source-file] [loop-file]\n", argv[0]);
                puts("  -l   show log\n");
                return EXIT_FAILURE;
        } else {
                for (int i = 1; i < argc && !global->log; i++) {
                        if (strcmp(argv[i], "-l") == 0)
                                global->log = true;
                }

                if (global->log && argc == 4) {
                        loopd(argv[2], argv[3]);
                } else if (argc == 3) {
                        loopd(argv[1], argv[2]);
                }

                return EXIT_SUCCESS;
        }
}

/*==============================================================================
  End of file
==============================================================================*/
