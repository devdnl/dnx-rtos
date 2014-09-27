/*=========================================================================*//**
@file    loop.c

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

/*==============================================================================
  Include files
==============================================================================*/
#include "core/module.h"
#include <dnx/misc.h>
#include <dnx/thread.h>
#include "noarch/loop_cfg.h"
#include "noarch/loop_def.h"
#include "noarch/loop_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        loop_cmd_t cmd;

        union {
                struct {
                        u8_t   *data;
                        size_t  size;
                        fpos_t  seek;
                } rw;

                struct {
                        void *arg;
                        int   rq;
                        int   status;
                } ioctl;

                struct {
                        u64_t size;
                } stat;
        };

        int errno_val;
} req_t;


typedef struct {
        mutex_t *lock;
        sem_t   *event_req;
        sem_t   *event_res;
        task_t  *host;
        u8_t     major;
        req_t    operation;
} loop_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME(LOOP);

static uint release_timeout      = 1000;
static uint operation_timeout    = 60000;
static uint request_timeout      = 20000;
static uint host_request_timeout = MAX_DELAY_MS;

/*==============================================================================
  Exported objects
==============================================================================*/

/*==============================================================================
  External objects
==============================================================================*/

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize device
 *
 * @param[out]          **device_handle        device allocated memory
 * @param[in ]            major                major device number
 * @param[in ]            minor                minor device number
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_INIT(LOOP, void **device_handle, u8_t major, u8_t minor)
{
        UNUSED_ARG(minor);

        if (major < _LOOP_NUMBER_OF_DEVICES) {
                loop_t *hdl = calloc(1, sizeof(loop_t));
                if (hdl) {
                        hdl->lock      = mutex_new(MUTEX_NORMAL);
                        hdl->event_req = semaphore_new(1, 0);
                        hdl->event_res = semaphore_new(1, 0);
                        hdl->major   = major;

                        if (hdl->lock && hdl->event_req && hdl->event_res) {
                                *device_handle = hdl;
                                return STD_RET_OK;

                        } else {
                                if (hdl->lock) {
                                        mutex_delete(hdl->lock);
                                }

                                if (hdl->event_req) {
                                        semaphore_delete(hdl->event_req);
                                }

                                if (hdl->event_res) {
                                        semaphore_delete(hdl->event_res);
                                }
                        }
                }
        }

        return STD_RET_ERROR;
}

//==============================================================================
/**
 * @brief Release device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_RELEASE(LOOP, void *device_handle)
{
        loop_t *hdl = device_handle;

        if (mutex_lock(hdl->lock, release_timeout)) {
                critical_section_begin();
                mutex_unlock(hdl->lock);
                mutex_delete(hdl->lock);
                semaphore_delete(hdl->event_req);
                semaphore_delete(hdl->event_res);
                free(hdl);
                critical_section_end();
                return STD_RET_OK;
        } else {
                errno = EBUSY;
                return STD_RET_ERROR;
        }
}

//==============================================================================
/**
 * @brief Open device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_OPEN(LOOP, void *device_handle, vfs_open_flags_t flags)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(flags);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_CLOSE(LOOP, void *device_handle, bool force)
{
        UNUSED_ARG(device_handle);
        UNUSED_ARG(force);

        return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Write data to device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[in ]           fattr                  file attributes
 *
 * @return number of written bytes, -1 if error
 */
//==============================================================================
API_MOD_WRITE(LOOP, void *device_handle, const u8_t *src, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(fattr);

        loop_t *hdl = device_handle;

        ssize_t n = 0;

        if (hdl->host == NULL)
                return n;

        if (mutex_lock(hdl->lock, operation_timeout)) {

                hdl->operation.cmd     = LOOP_CMD_TRANSMISSION_CLIENT2HOST;
                hdl->operation.rw.data = const_cast(u8_t*, src);
                hdl->operation.rw.size = count;
                hdl->operation.rw.seek = *fpos;

                semaphore_signal(hdl->event_req);

                if (semaphore_wait(hdl->event_res, request_timeout)) {

                        if (hdl->operation.errno_val == ESUCC) {
                                n = count - hdl->operation.rw.size;
                        } else {
                                errno = hdl->operation.errno_val;
                                n = -1;
                        }

                } else {
                        errno = ETIME;
                        n = -1;
                }

                hdl->operation.cmd = LOOP_CMD_IDLE;

                mutex_unlock(hdl->lock);
        }

        return n;
}

//==============================================================================
/**
 * @brief Read data from device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[in ]           fattr                  file attributes
 *
 * @return number of read bytes, -1 if error
 */
//==============================================================================
API_MOD_READ(LOOP, void *device_handle, u8_t *dst, size_t count, fpos_t *fpos, struct vfs_fattr fattr)
{
        UNUSED_ARG(fattr);

        loop_t *hdl = device_handle;

        ssize_t n = 0;

        if (hdl->host == NULL)
                return n;

        if (mutex_lock(hdl->lock, operation_timeout)) {

                hdl->operation.cmd     = LOOP_CMD_TRANSMISSION_CLIENT2HOST;
                hdl->operation.rw.data = dst;
                hdl->operation.rw.size = count;
                hdl->operation.rw.seek = *fpos;

                while (count) {
                        semaphore_signal(hdl->event_req);

                        if (semaphore_wait(hdl->event_res, request_timeout)) {

                                if (hdl->operation.errno_val == ESUCC) {

                                        if (hdl->operation.rw.size > count) {
                                                hdl->operation.rw.size = count;
                                        }

                                        memcpy(dst, hdl->operation.rw.data, hdl->operation.rw.size);

                                        dst   += hdl->operation.rw.size;
                                        count -= hdl->operation.rw.size;

                                } else {
                                        errno = hdl->operation.errno_val;
                                        n = -1;
                                        break;
                                }

                        } else {
                                errno = ETIME;
                                n = -1;
                                break;
                        }
                }

                hdl->operation.cmd = LOOP_CMD_IDLE;

                mutex_unlock(hdl->lock);
        }

        return n;
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_IOCTL(LOOP, void *device_handle, int request, void *arg)
{
        loop_t *hdl = device_handle;

        stdret_t status = -1;

        switch (request) {
        case IOCTL_LOOP__HOST_OPEN:
                critical_section_begin();
                if (hdl->host == NULL) {
                        hdl->host = task_get_handle();
                        status = STD_RET_OK;
                } else {
                        errno = EBUSY;
                }
                critical_section_end();
                break;

        case IOCTL_LOOP__HOST_CLOSE:
                if (hdl->host == task_get_handle()) {
                        hdl->host = NULL;
                        status = STD_RET_OK;
                } else {
                        errno = EPERM;
                }
                break;

        case IOCTL_LOOP__HOST_WAIT_FOR_REQUEST:
                if (arg && hdl->host == task_get_handle()) {
                        if (semaphore_wait(hdl->event_req, host_request_timeout)) {
                                loop_request_t *req = static_cast(loop_request_t*, arg);

                                req->cmd = hdl->operation.cmd;
                                req->args.rw.seek = hdl->operation.rw.seek;
                                req->args.rw.size = hdl->operation.rw.size;

                                status = STD_RET_OK;
                        } else {
                                errno = ETIME;
                        }
                } else {
                        errno = EINVAL;
                }
                break;

        case IOCTL_LOOP__HOST_READ_DATA_FROM_CLIENT:
                if (arg && hdl->host == task_get_handle()) {
                        loop_buffer_t *buf = static_cast(loop_buffer_t*, arg);

                        if (buf->errno_val == ESUCC) {
                                if (buf->size > hdl->operation.rw.size) {
                                        buf->size = hdl->operation.rw.size;
                                }

                                memcpy(buf->data, hdl->operation.rw.data, buf->size);

                                hdl->operation.rw.data += buf->size;
                                hdl->operation.rw.size -= buf->size;

                                if (hdl->operation.rw.size == 0) {
                                        semaphore_signal(hdl->event_res);
                                        status = 0;
                                } else {
                                        status = 1;
                                }

                        } else {
                                hdl->operation.errno_val = buf->errno_val;
                                semaphore_signal(hdl->event_res);

                                status = 0;
                        }

                } else {
                        errno = EINVAL;
                }
                break;

        case IOCTL_LOOP__HOST_WRITE_DATA_TO_CLIENT:
                if (arg && hdl->host == task_get_handle()) {
                        loop_buffer_t *buf = static_cast(loop_buffer_t*, arg);

                        if (buf->errno_val == ESUCC) {
                                if (buf->size >= hdl->operation.rw.size) {
                                        buf->size = hdl->operation.rw.size;
                                        status = 0;
                                } else {
                                        status = 1;
                                }

                                hdl->operation.rw.data = buf->data;
                                hdl->operation.rw.size = buf->size;

                                semaphore_signal(hdl->event_res);

                                if (status) {
                                        if (semaphore_wait(hdl->event_req, host_request_timeout) == false) {
                                                errno  = ETIME;
                                                status = -1;
                                        }
                                }

                        } else {
                                hdl->operation.errno_val = buf->errno_val;
                                semaphore_signal(hdl->event_res);

                                status = 0;
                        }

                } else {
                        errno = EINVAL;
                }
                break;

        case IOCTL_LOOP__HOST_SET_IOCTL_STATUS:
                if (arg && hdl->host == task_get_handle()) {
                        loop_ioctl_response_t *res  = static_cast(loop_ioctl_response_t*, arg);

                        hdl->operation.ioctl.status = res->status;
                        hdl->operation.errno_val    = res->errno_val;

                        semaphore_signal(hdl->event_res);

                        status = 0;
                } else {
                        errno = EINVAL;
                }
                break;

        case IOCTL_LOOP__HOST_SET_DEVICE_STATS:
                if (arg && hdl->host == task_get_handle()) {
                        loop_stat_response_t *res = static_cast(loop_stat_response_t*, arg);

                        hdl->operation.stat.size  = res->size;
                        hdl->operation.errno_val  = res->errno_val;

                        semaphore_signal(hdl->event_res);

                        status = 0;
                } else {
                        errno = EINVAL;
                }
                break;

        case IOCTL_LOOP__HOST_FLUSH_DONE:
                if (hdl->host == task_get_handle()) {
                        semaphore_signal(hdl->event_req);
                        status = STD_RET_OK;
                } else {
                        errno = EPERM;
                }
                break;

        default: //IOCTL_LOOP__CLIENT_REQUEST(n)
                if (hdl->host != NULL) {
                        hdl->operation.cmd       = LOOP_CMD_IOCTL_REQUEST;
                        hdl->operation.ioctl.arg = arg;
                        hdl->operation.ioctl.rq  = request;

                        semaphore_signal(hdl->event_req);

                        if (semaphore_wait(hdl->event_res, request_timeout)) {
                                status = hdl->operation.ioctl.status;
                        } else {
                                errno  = ETIME;
                        }
                } else {
                        errno = ESRCH;
                }
                break;
        }

        return status;
}

//==============================================================================
/**
 * @brief Flush device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_FLUSH(LOOP, void *device_handle)
{
        loop_t *hdl = device_handle;

        if (hdl->host == NULL)
                return STD_RET_OK;

        hdl->operation.cmd = LOOP_CMD_FLUSH_BUFFERS;

        semaphore_signal(hdl->event_req);

        if (semaphore_wait(hdl->event_res, request_timeout)) {

                if (hdl->operation.errno_val == ESUCC) {
                        return STD_RET_OK;
                } else {
                        errno = hdl->operation.errno_val;
                        return STD_RET_ERROR;
                }

        } else {
                errno = ETIME;
                return STD_RET_ERROR;
        }
}

//==============================================================================
/**
 * @brief Device information
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
API_MOD_STAT(LOOP, void *device_handle, struct vfs_dev_stat *device_stat)
{
        loop_t *hdl = device_handle;

        device_stat->st_major = hdl->major;
        device_stat->st_minor = _LOOP_MINOR_NUMBER;
        device_stat->st_size  = 0;

        if (hdl->host == NULL) {
                return STD_RET_OK;
        }

        hdl->operation.cmd = LOOP_CMD_DEVICE_STAT;

        semaphore_signal(hdl->event_req);

        if (semaphore_wait(hdl->event_res, request_timeout)) {

                if (hdl->operation.errno_val == ESUCC) {
                        device_stat->st_size = hdl->operation.stat.size;
                        return STD_RET_OK;
                } else {
                        errno = hdl->operation.errno_val;
                }

        } else {
                errno = ETIME;
        }

        return STD_RET_ERROR;
}

/*==============================================================================
  End of file
==============================================================================*/
