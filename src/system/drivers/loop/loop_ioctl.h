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
#include "drivers/ioctl_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
  Exported macros
==============================================================================*/
/**
 * @brief  Host request. Set this program as Host. After this operation this
 *         program has only possibility to gets requests from Client program.
 * @param  None
 * @return On success 0 is returned, otherwise -1
 */
#define IOCTL_LOOP__HOST_OPEN                   _IO(LOOP, 0x00)

/**
 * @brief  Host request. Disconnect this program as host function. After this
 *         operation any program can be a host if request OPEN function.
 * @param  None
 * @return On success 0 is returned, otherwise -1
 */
#define IOCTL_LOOP__HOST_CLOSE                  _IO(LOOP, 0x01)

/**
 * @brief  Host request. Wait for request from Client. Client request and
 *         arguments are set in the loop_client_rq_t-type variable. Host application
 *         should read this variable and perform requested actions.
 * @param  loop_client_rq_t             request details
 * @return On success 0 is returned, otherwise -1
 */
#define IOCTL_LOOP__HOST_WAIT_FOR_REQUEST       _IOR(LOOP, 0x02, loop_client_rq_t*)

/**
 * @brief  Host request. Read data from buffers shared by the Client. When
 *         host send all bytes or send 0-length buffer then read operation is
 *         finished. If operation is not finished, then timeout was generated.
 *         If host wants to finish operation earlier the ZLB (Zero-Length Buffer)
 *         should be send (buffer size = 0).
 * @param  loop_buffer_t                host buffer descriptor
 * @return On success 0 is returned, otherwise -1
 */
#define IOCTL_LOOP__HOST_READ_DATA_FROM_CLIENT  _IOR(LOOP, 0x03, loop_buffer_t*)

/**
 * @brief  Host request. Write data to the client. Write operation is finished
 *         and Client is resumed when buffer size is set to 0 (this means that
 *         Host written all data to the buffer) or all bytes was written. Write
 *         operation can be done by sending small buffers (is not required to send
 *         entire requested buffer in one part).
 * @param  loop_buffer_t                host buffer descriptor
 * @return On success 0 is returned, otherwise -1
 */
#define IOCTL_LOOP__HOST_WRITE_DATA_TO_CLIENT   _IOW(LOOP, 0x04, loop_buffer_t*)

/**
 * @brief  Host request. Response to the captured ioctl request. This request
 *         is send as response when Client requested IOCTL action in the command
 *         capture request.
 * @param  loop_ioctl_response_t        ioctl response data
 * @return On success 0 is returned, otherwise -1
 */
#define IOCTL_LOOP__HOST_SET_IOCTL_STATUS       _IOW(LOOP, 0x05, loop_ioctl_response_t*)

/**
 * @brief  Host request. Response to the captured stat request.
 * @param  loop_stat_response_t         device statistics response
 * @return On success 0 is returned, otherwise -1
 */
#define IOCTL_LOOP__HOST_SET_DEVICE_STATS       _IOW(LOOP, 0x06, loop_stat_response_t*)

/**
 * @brief  Host request. Response to the captured flush request
 * @param  int                          errno value
 * @return On success 0 is returned, otherwise -1
 */
#define IOCTL_LOOP__HOST_FLUSH_DONE             _IOW(LOOP, 0x07, int*)

/**
 * @brief  Client request. General purpose RAW request. Depends on host protocol.
 *         By this request Client can send request from another device type.
 *         In this case is not required to use CLIENT_REQUEST() macro.
 * @param  n                            request number (macro's argument)
 * @param  void*                        request argument
 * @return Depends on host program protocol
 */
#define IOCTL_LOOP__CLIENT_REQUEST(n)           _IOWR(LOOP, 0x08 + n, void*)


/*==============================================================================
  Exported object types
==============================================================================*/
// client request codes
typedef enum {
        LOOP_CMD__IDLE,                         //!< idle command, no action
        LOOP_CMD__TRANSMISSION_CLIENT2HOST,     //!< transmission request client to host (rw arguments are valid)
        LOOP_CMD__TRANSMISSION_HOST2CLIENT,     //!< transmission request host to client (rw arguments are valid)
        LOOP_CMD__IOCTL_REQUEST,                //!< ioctl operation request (ioctl arguments are valid)
        LOOP_CMD__DEVICE_STAT,                  //!< request to response by device's statistics
        LOOP_CMD__FLUSH_BUFFERS                 //!< request to flush device buffers
} LOOP_cmd_t;


// buffer descriptor to write/read access
typedef struct {
        u8_t    *data;                          //!< write/read Host buffer address
        size_t   size;                          //!< number of bytes to write/read
        int      err_no;                        //!< errno value if error occurred (if no error must be set to ESUCC)
} LOOP_buffer_t;


// response host->client to the ioctl request from client
typedef struct {
        int err_no;                             //!< errno value if error occurred (if no error must be set to ESUCC)
} LOOP_ioctl_response_t;


// response host->client to the stat() request from client
typedef struct {
        u64_t size;                             //!< device capacity/file size
        int   err_no;                           //!< errno value if error occurred (if no error must be set to ESUCC)
} LOOP_stat_response_t;


// request structure
typedef struct {
        LOOP_cmd_t cmd;                         //!< requested action (command from Client)

        union {
                struct {
                        size_t size;            //!< requested size of read/write operation
                        fpos_t seek;            //!< position in the device's file
                } rw;                           //!< read/write transmission arguments group

                struct {
                        int   request;          //!< ioctl's request number
                        void *arg;              //!< ioctl's request argument
                } ioctl;                        //!< ioctl argument group
        } arg;                                  //!< command's arguments
} LOOP_request_t;


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
