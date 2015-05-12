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
#include "drivers/driver.h"
#include "noarch/loop_cfg.h"
#include "noarch/loop_def.h"
#include "noarch/loop_ioctl.h"

// TODO loop: module to reimplementation because of system mechanisms change

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
                } ioctl;

                struct {
                        u64_t size;
                } stat;
        } arg;

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
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(LOOP, void **device_handle, u8_t major, u8_t minor)
{
        UNUSED_ARG1(minor);

        int result = ENODEV;

        if (major < _LOOP_NUMBER_OF_DEVICES) {
                result = _sys_zalloc(sizeof(loop_t), device_handle);
                if (result == ESUCC) {
                        loop_t *hdl = *device_handle;

                        hdl->major  = major;

                        result = _sys_mutex_create(MUTEX_TYPE_NORMAL, &hdl->lock);
                        if (result != ESUCC) {
                                goto finish;
                        }

                        result = _sys_semaphore_create(1, 0, &hdl->event_req);
                        if (result != ESUCC) {
                                goto finish;
                        }

                        result = _sys_semaphore_create(1, 0, &hdl->event_res);
                        if (result != ESUCC) {
                                goto finish;
                        }

                        finish:
                        if (result != ESUCC) {
                                if (hdl->lock)
                                        _sys_mutex_destroy(hdl->lock);

                                if (hdl->event_req)
                                        _sys_semaphore_destroy(hdl->event_req);

                                if (hdl->event_res)
                                        _sys_semaphore_destroy(hdl->event_res);

                                _sys_free(device_handle);
                        }
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Release device
 *
 * @param[in ]          *device_handle          device allocated memory
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_RELEASE(LOOP, void *device_handle)
{
        loop_t *hdl = device_handle;

        if (_sys_mutex_lock(hdl->lock, release_timeout) == ESUCC) {
                _sys_critical_section_begin();
                _sys_mutex_unlock(hdl->lock);
                _sys_mutex_destroy(hdl->lock);
                _sys_semaphore_destroy(hdl->event_req);
                _sys_semaphore_destroy(hdl->event_res);
                _sys_free(device_handle);
                _sys_critical_section_end();
                return ESUCC;
        } else {
                return EBUSY;
        }
}

//==============================================================================
/**
 * @brief Open device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           flags                  file operation flags (O_RDONLY, O_WRONLY, O_RDWR)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_OPEN(LOOP, void *device_handle, u32_t flags)
{
        UNUSED_ARG1(device_handle);
        UNUSED_ARG1(flags);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Close device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           force                  device force close (true)
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_CLOSE(LOOP, void *device_handle, bool force)
{
        UNUSED_ARG1(device_handle);
        UNUSED_ARG1(force);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Write data to device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_WRITE(LOOP,
              void             *device_handle,
              const u8_t       *src,
              size_t            count,
              fpos_t           *fpos,
              size_t           *wrcnt,
              struct vfs_fattr  fattr)
{
        UNUSED_ARG1(fattr);

        loop_t *hdl = device_handle;

        if (hdl->host == NULL)
                return ESRCH;

        int result = _sys_mutex_lock(hdl->lock, operation_timeout);
        if (result == ESUCC) {

                hdl->operation.cmd         = LOOP_CMD_TRANSMISSION_CLIENT2HOST;
                hdl->operation.arg.rw.data = const_cast(u8_t*, src);
                hdl->operation.arg.rw.size = count;
                hdl->operation.arg.rw.seek = *fpos;

                result = _sys_semaphore_signal(hdl->event_req);
                if (result == ESUCC) {

                        result = _sys_semaphore_wait(hdl->event_res, request_timeout);
                        if (result == ESUCC) {

                                if (hdl->operation.errno_val == ESUCC) {
                                        *wrcnt = count - hdl->operation.arg.rw.size;
                                } else {
                                        result = hdl->operation.errno_val;
                                }
                        }

                        hdl->operation.cmd = LOOP_CMD_IDLE;

                        _sys_mutex_unlock(hdl->lock);
                }
        }

        return result;
}

//==============================================================================
/**
 * @brief Read data from device
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ][out]     *fpos                   file position
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_READ(LOOP,
             void            *device_handle,
             u8_t            *dst,
             size_t           count,
             fpos_t          *fpos,
             size_t          *rdcnt,
             struct vfs_fattr fattr)
{
        UNUSED_ARG1(fattr);

        loop_t *hdl = device_handle;

        if (hdl->host == NULL)
                return ESRCH;

        int result = _sys_mutex_lock(hdl->lock, operation_timeout);
        if (result == ESUCC) {

                hdl->operation.cmd         = LOOP_CMD_TRANSMISSION_HOST2CLIENT;
                hdl->operation.arg.rw.data = dst;
                hdl->operation.arg.rw.seek = *fpos;

                while (count) {
                        hdl->operation.arg.rw.size = count;
                        result = _sys_semaphore_signal(hdl->event_req);
                        if (result == ESUCC) {
                                result = _sys_semaphore_wait(hdl->event_res, request_timeout);
                                if (result == ESUCC) {

                                        if (hdl->operation.errno_val == ESUCC) {

                                                if (hdl->operation.arg.rw.size == 0) {
                                                        break;

                                                } else if (hdl->operation.arg.rw.size > count) {
                                                        hdl->operation.arg.rw.size = count;
                                                }

                                                if (hdl->operation.arg.rw.size && hdl->operation.arg.rw.data) {
                                                        memcpy(dst, hdl->operation.arg.rw.data, hdl->operation.arg.rw.size);
                                                }

                                                dst    += hdl->operation.arg.rw.size;
                                                count  -= hdl->operation.arg.rw.size;
                                                *rdcnt += hdl->operation.arg.rw.size;

                                        } else {
                                                result = hdl->operation.errno_val;
                                                break;
                                        }

                                } else {
                                        break;
                                }
                        }
                }

                hdl->operation.cmd = LOOP_CMD_IDLE;

                _sys_mutex_unlock(hdl->lock);
        }

        return result;
}

//==============================================================================
/**
 * @brief IO control
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_IOCTL(LOOP, void *device_handle, int request, void *arg)
{
        loop_t *hdl = device_handle;

        int status = EINVAL;

        switch (request) {
        case IOCTL_LOOP__HOST_OPEN:
                _sys_critical_section_begin();
                        if (hdl->host == NULL) {
                                hdl->host = _sys_task_get_handle();
                                status = ESUCC;
                        } else {
                                status = EBUSY;
                        }
                _sys_critical_section_end();
                break;

        case IOCTL_LOOP__HOST_CLOSE:
                if (hdl->host == _sys_task_get_handle()) {
                        status = _sys_semaphore_wait(hdl->event_req, 0);
                        if (status == ESUCC) {
                                status = _sys_semaphore_wait(hdl->event_res, 0);
                                hdl->host = NULL;
                        }
                } else {
                        status = EPERM;
                }
                break;

        case IOCTL_LOOP__HOST_WAIT_FOR_REQUEST:
                if (arg && hdl->host == _sys_task_get_handle()) {
                        status = _sys_semaphore_wait(hdl->event_req, host_request_timeout);
                        if (status == ESUCC) {
                                loop_request_t *req = static_cast(loop_request_t*, arg);

                                req->cmd = hdl->operation.cmd;
                                switch(req->cmd) {
                                case LOOP_CMD_TRANSMISSION_CLIENT2HOST:
                                case LOOP_CMD_TRANSMISSION_HOST2CLIENT:
                                        req->arg.rw.seek = hdl->operation.arg.rw.seek;
                                        req->arg.rw.size = hdl->operation.arg.rw.size;
                                        break;

                                case LOOP_CMD_IOCTL_REQUEST:
                                        req->arg.ioctl.request = hdl->operation.arg.ioctl.rq;
                                        req->arg.ioctl.arg     = hdl->operation.arg.ioctl.arg;
                                        break;

                                default:
                                case LOOP_CMD_IDLE:
                                case LOOP_CMD_DEVICE_STAT:
                                case LOOP_CMD_FLUSH_BUFFERS:
                                        break;
                                }
                        }
                }
                break;

        case IOCTL_LOOP__HOST_READ_DATA_FROM_CLIENT:
                if (arg && hdl->host == _sys_task_get_handle()) {
                        loop_buffer_t *buf = static_cast(loop_buffer_t*, arg);

                        if (buf->errno_val == ESUCC) {
                                if (buf->size > hdl->operation.arg.rw.size) {
                                        buf->size = hdl->operation.arg.rw.size;
                                }

                                if (buf->size > 0 && buf->data) {
                                        memcpy(buf->data, hdl->operation.arg.rw.data, buf->size);

                                        hdl->operation.arg.rw.data += buf->size;
                                        hdl->operation.arg.rw.size -= buf->size;
                                }

                                if (hdl->operation.arg.rw.size == 0 || buf->size == 0 || !buf->data) {
                                        status = _sys_semaphore_signal(hdl->event_res);
                                } else {
                                        status = ESUCC;
                                }

                        } else {
                                hdl->operation.errno_val = buf->errno_val;
                                status = _sys_semaphore_signal(hdl->event_res);
                        }
                }
                break;

        case IOCTL_LOOP__HOST_WRITE_DATA_TO_CLIENT:
                if (arg && hdl->host == _sys_task_get_handle()) {
                        loop_buffer_t *buf = static_cast(loop_buffer_t*, arg);

                        if (buf->errno_val == ESUCC) {
                                if (buf->size == 0 || !buf->data || buf->size == hdl->operation.arg.rw.size) {
                                        status = ESUCC;

                                } else if (buf->size >= hdl->operation.arg.rw.size) {
                                        buf->size = hdl->operation.arg.rw.size;
                                        status = ESUCC;

                                } else {
                                        status = 1;
                                }

                                hdl->operation.arg.rw.data = buf->data;
                                hdl->operation.arg.rw.size = buf->size;
                                hdl->operation.errno_val   = buf->errno_val;

                                _sys_semaphore_signal(hdl->event_res);

                                if (status) {
                                        status = _sys_semaphore_wait(hdl->event_req, host_request_timeout);
                                }
                        } else {
                                hdl->operation.errno_val = buf->errno_val;
                                status = _sys_semaphore_signal(hdl->event_res);
                        }
                }
                break;

        case IOCTL_LOOP__HOST_SET_IOCTL_STATUS:
                if (arg && hdl->host == _sys_task_get_handle()) {
                        loop_ioctl_response_t *res  = static_cast(loop_ioctl_response_t*, arg);

                        hdl->operation.errno_val = res->errno_val;

                        status = _sys_semaphore_signal(hdl->event_res);
                }
                break;

        case IOCTL_LOOP__HOST_SET_DEVICE_STATS:
                if (arg && hdl->host == _sys_task_get_handle()) {
                        loop_stat_response_t *res = static_cast(loop_stat_response_t*, arg);

                        hdl->operation.arg.stat.size = res->size;
                        hdl->operation.errno_val     = res->errno_val;

                        status = _sys_semaphore_signal(hdl->event_res);
                }
                break;

        case IOCTL_LOOP__HOST_FLUSH_DONE:
                if (hdl->host == _sys_task_get_handle()) {
                        hdl->operation.errno_val = reinterpret_cast(int, arg);
                        status = _sys_semaphore_signal(hdl->event_res);
                }
                break;

        default: //IOCTL_LOOP__CLIENT_REQUEST(n)
                if (hdl->host != NULL) {
                        hdl->operation.cmd           = LOOP_CMD_IOCTL_REQUEST;
                        hdl->operation.arg.ioctl.arg = arg;
                        hdl->operation.arg.ioctl.rq  = request;
                        _sys_semaphore_signal(hdl->event_req);

                        status = _sys_semaphore_wait(hdl->event_res, request_timeout);
                        if (status == ESUCC) {
                                status = hdl->operation.errno_val;
                        }
                } else {
                        status = ESRCH;
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
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_FLUSH(LOOP, void *device_handle)
{
        loop_t  *hdl    = device_handle;
        int      status;

        if (hdl->host) {
                hdl->operation.cmd = LOOP_CMD_FLUSH_BUFFERS;
                _sys_semaphore_signal(hdl->event_req);

                status = _sys_semaphore_wait(hdl->event_res, request_timeout);
                if (status == ESUCC) {
                        if (hdl->operation.errno_val == ESUCC) {
                                status = ESUCC;
                        } else {
                                status = hdl->operation.errno_val;
                        }

                }

                hdl->operation.cmd = LOOP_CMD_IDLE;
        } else {
                status = ESUCC;
        }

        return status;
}

//==============================================================================
/**
 * @brief Device information
 *
 * @param[in ]          *device_handle          device allocated memory
 * @param[out]          *device_stat            device status
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_STAT(LOOP, void *device_handle, struct vfs_dev_stat *device_stat)
{
        loop_t  *hdl    = device_handle;
        int      status;

        device_stat->st_major = hdl->major;
        device_stat->st_minor = _LOOP_MINOR_NUMBER;
        device_stat->st_size  = 0;

        if (hdl->host) {
                hdl->operation.cmd = LOOP_CMD_DEVICE_STAT;
                _sys_semaphore_signal(hdl->event_req);

                status = _sys_semaphore_wait(hdl->event_res, request_timeout);
                if (status == ESUCC) {

                        if (hdl->operation.errno_val == ESUCC) {
                                device_stat->st_size = hdl->operation.arg.stat.size;
                                status = ESUCC;
                        } else {
                                status = hdl->operation.errno_val;
                        }

                }

                hdl->operation.cmd = LOOP_CMD_IDLE;
        } else {
                status = ESUCC;
        }

        return status;
}

/*==============================================================================
  End of file
==============================================================================*/
