/*=========================================================================*//**
@file    loop_ioctl.h

@author  Daniel Zorychta

@brief   Universal virtual device that allow to connect applications together

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#ifndef _LOOP_IOCTL_H_
#define _LOOP_IOCTL_H_

/*==============================================================================
  Include files
==============================================================================*/
#include "core/ioctl_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief  Host request. Wait for request from Client
 * @param  loop_client_rq_t             request details
 * @return On success 0 is returned, otherwise 1
 */
#define IOCTL_LOOP__HOST_WAIT_FOR_REQUEST       _IOR(_IO_GROUP_LOOP, 0x00, loop_client_rq_t*)

/**
 * @brief  Host request. Read data from buffers shared by the client. Read operation
 *         is finished and Client is resumed when buffer size is set to 0 (this
 *         means that Host read all bytes from buffer)
 * @param  loop_buffer_t                host buffer descriptor
 * @return On success 0 is returned, otherwise 1
 */
#define IOCTL_LOOP__HOST_READ_DATA              _IOR(_IO_GROUP_LOOP, 0x01, loop_buffer_t*)

/**
 * @brief  Host request. Write data to the client. Write operation is finished
 *         and Client is resumed when buffer size is set to 0 (this means that
 *         Host written all data to the buffer)
 * @param  loop_buffer_t                host buffer descriptor
 * @return On success 0 is returned, otherwise 1
 */
#define IOCTL_LOOP__HOST_WRITE_DATA             _IOW(_IO_GROUP_LOOP, 0x02, loop_buffer_t*)

/**
 * @brief  Host request. Response to the captured ioctl request
 * @param  loop_ioctl_response_t        ioctl response data
 * @return On success 0 is returned, otherwise 1
 */
#define IOCTL_LOOP__HOST_SET_IOCTL_STATUS       _IOW(_IO_GROUP_LOOP, 0x03, loop_ioctl_response_t*)

/**
 * @brief  Host request. Response to the captured stat request
 * @param  loop_stat_response_t         device statistics response
 * @return On success 0 is returned, otherwise 1
 */
#define IOCTL_LOOP__HOST_SET_DEVICE_STATS       _IOW(_IO_GROUP_LOOP, 0x04, loop_stat_response_t*)

/**
 * @brief  Client request. General purpose RAW request. Depends on host protocol
 * @param  n                            request number
 * @return Depends on host program protocol
 */
#define IOCTL_LOOP__CLIENT_REQUEST(n)           _IOWR(_IO_GROUP_LOOP, 0x05 + n)


/*==============================================================================
  Exported object types
==============================================================================*/
// client request codes
typedef enum {
        LOOP_CMD_WRITE,
        LOOP_CMD_READ,
        LOOP_CMD_IOCTL,
        LOOP_CMD_STAT,
        LOOP_CMD_FLUSH
} loop_cmd_t;


// buffer descriptor to write/read access
typedef struct {
        u8_t    *data;          //!< write/read Host buffer address
        ssize_t  size;          //!< 0: write/read finished; > 0: write/read operation; -1: error
        int      errno_val;     //!< errno value is valid only when size is -1
} loop_buffer_t;


// response host->client to the ioctl request from client
typedef struct {
        stdret_t status;        //!< ioctl operation status
        int      errno_val;     //!< errno value
} loop_ioctl_response_t;


// response host->client to the stat() request from client
typedef struct {
        u64_t size;
        int   errno_val;
} loop_stat_response_t;


// request structure
typedef struct {
        loop_cmd_t cmd;

        union {
                struct {
                        u8_t   *data;
                        size_t  size;
                        fpos_t  seek;
                } rdwr;

                struct {
                        int     request;
                        void   *arg;
                } ioctl;
        } args;

        int major;
} loop_request_t;


/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  Exported functions
==============================================================================*/

/*==============================================================================
  Exported inline functions
==============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _LOOP_IOCTL_H_ */
/*==============================================================================
  End of file
==============================================================================*/
