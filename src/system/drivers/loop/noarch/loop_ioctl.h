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
 * @brief  Host request. Set this program as Host.
 * @param  None
 * @return On success 0 is returned, otherwise -1
 */
#define IOCTL_LOOP__HOST_OPEN                   _IO(_IO_GROUP_LOOP, 0x00)

/**
 * @brief  Host request. Set this program as Host.
 * @param  None
 * @return On success 0 is returned, otherwise -1
 */
#define IOCTL_LOOP__HOST_CLOSE                  _IO(_IO_GROUP_LOOP, 0x01)

/**
 * @brief  Host request. Wait for request from Client
 * @param  loop_client_rq_t             request details
 * @return On success 0 is returned, otherwise -1
 */
#define IOCTL_LOOP__HOST_WAIT_FOR_REQUEST       _IOR(_IO_GROUP_LOOP, 0x02, loop_client_rq_t*)

/**
 * @brief  Host request. Read data from buffers shared by the client. Read operation
 *         is finished and Client is resumed when buffer size is set to 0 (this
 *         means that Host read all bytes from buffer)
 * @param  loop_buffer_t                host buffer descriptor
 * @return Return 1 if client's buffer contains bytes.
 *         Return 0 if client's buffer is empty.
 *         Return -1 on error.
 */
#define IOCTL_LOOP__HOST_READ_DATA_FROM_CLIENT  _IOR(_IO_GROUP_LOOP, 0x03, loop_buffer_t*)

/**
 * @brief  Host request. Write data to the client. Write operation is finished
 *         and Client is resumed when buffer size is set to 0 (this means that
 *         Host written all data to the buffer)
 * @param  loop_buffer_t                host buffer descriptor
 * @return Return 1 if client's buffer is not filled yet.
 *         Return 0 if client's buffer is filled.
 *         Return -1 on error.
 */
#define IOCTL_LOOP__HOST_WRITE_DATA_TO_CLIENT   _IOW(_IO_GROUP_LOOP, 0x04, loop_buffer_t*)

/**
 * @brief  Host request. Response to the captured ioctl request
 * @param  loop_ioctl_response_t        ioctl response data
 * @return On success 0 is returned, otherwise -1
 */
#define IOCTL_LOOP__HOST_SET_IOCTL_STATUS       _IOW(_IO_GROUP_LOOP, 0x05, loop_ioctl_response_t*)

/**
 * @brief  Host request. Response to the captured stat request
 * @param  loop_stat_response_t         device statistics response
 * @return On success 0 is returned, otherwise -1
 */
#define IOCTL_LOOP__HOST_SET_DEVICE_STATS       _IOW(_IO_GROUP_LOOP, 0x06, loop_stat_response_t*)

/**
 * @brief  Host request. Response to the captured flush request
 * @param  None
 * @return On success 0 is returned, otherwise -1
 */
#define IOCTL_LOOP__HOST_FLUSH_DONE             _IO(_IO_GROUP_LOOP, 0x07)

/**
 * @brief  Client request. General purpose RAW request. Depends on host protocol
 * @param  n                            request number
 * @return Depends on host program protocol
 */
#define IOCTL_LOOP__CLIENT_REQUEST(n)           _IOWR(_IO_GROUP_LOOP, 0x08 + n)


/*==============================================================================
  Exported object types
==============================================================================*/
// client request codes
typedef enum {
        LOOP_CMD_IDLE,
        LOOP_CMD_TRANSMISSION_CLIENT2HOST,
        LOOP_CMD_TRANSMISSION_HOST2CLIENT,
        LOOP_CMD_IOCTL_REQUEST,
        LOOP_CMD_DEVICE_STAT,
        LOOP_CMD_FLUSH_BUFFERS
} loop_cmd_t;


// buffer descriptor to write/read access
typedef struct {
        u8_t    *data;          //!< write/read Host buffer address
        size_t   size;          //!< number of bytes to write/read
        int      errno_val;     //!< errno value if error occurred (if no error must be set to ESUCC)
} loop_buffer_t;


// response host->client to the ioctl request from client
typedef struct {
        stdret_t status;        //!< ioctl operation status
        int      errno_val;     //!< errno value if error occurred (if no error must be set to ESUCC)
} loop_ioctl_response_t;


// response host->client to the stat() request from client
typedef struct {
        u64_t size;             //!< device capacity/file size
        int   errno_val;        //!< errno value if error occurred (if no error must be set to ESUCC)
} loop_stat_response_t;


// request structure
typedef struct {
        loop_cmd_t cmd;

        union {
                struct {
                        size_t size;
                        fpos_t seek;
                } rw;

                struct {
                        int   request;
                        void *arg;
                } ioctl;
        } args;
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
