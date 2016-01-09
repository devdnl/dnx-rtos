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
#include "../loop_ioctl.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define RELEASE_TIMEOUT         1000
#define OPERATION_TIMEOUT       60000
#define REQUEST_TIMEOUT         20000
#define HOST_REQUEST_TIMEOUT    MAX_DELAY_MS

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        LOOP_cmd_t cmd;

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

        int err_no;
} req_t;


typedef struct {
        mutex_t    *mtx;
        sem_t      *event_req;
        sem_t      *event_res;
        dev_lock_t  host_lock;
        u8_t        major;
        req_t       action;
} loop_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME(LOOP);

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
        if (minor != 0) {
                return ENODEV;
        }

        int result = sys_zalloc(sizeof(loop_t), device_handle);
        if (result == ESUCC) {
                loop_t *hdl = *device_handle;

                hdl->major = major;

                sys_device_unlock(&hdl->host_lock, true);

                result = sys_mutex_create(MUTEX_TYPE_NORMAL, &hdl->mtx);
                if (result != ESUCC) {
                        goto finish;
                }

                result = sys_semaphore_create(1, 0, &hdl->event_req);
                if (result != ESUCC) {
                        goto finish;
                }

                result = sys_semaphore_create(1, 0, &hdl->event_res);
                if (result != ESUCC) {
                        goto finish;
                }

                finish:
                if (result != ESUCC) {
                        if (hdl->mtx) {
                                sys_mutex_destroy(hdl->mtx);
                        }

                        if (hdl->event_req) {
                                sys_semaphore_destroy(hdl->event_req);
                        }

                        if (hdl->event_res) {
                                sys_semaphore_destroy(hdl->event_res);
                        }

                        sys_free(device_handle);
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

        if (sys_mutex_lock(hdl->mtx, RELEASE_TIMEOUT) == ESUCC) {
                sys_critical_section_begin();
                {
                        sys_mutex_unlock(hdl->mtx);
                        sys_mutex_destroy(hdl->mtx);
                        sys_semaphore_destroy(hdl->event_req);
                        sys_semaphore_destroy(hdl->event_res);
                        sys_free(device_handle);
                }
                sys_critical_section_end();
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
        UNUSED_ARG2(device_handle, flags);
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
        UNUSED_ARG2(device_handle, force);
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

        if (hdl->host_lock == 0) {
                return ESRCH;
        }

        int result = sys_mutex_lock(hdl->mtx, OPERATION_TIMEOUT);
        if (result == ESUCC) {

                hdl->action.cmd         = LOOP_CMD__TRANSMISSION_CLIENT2HOST;
                hdl->action.arg.rw.data = const_cast(u8_t*, src);
                hdl->action.arg.rw.size = count;
                hdl->action.arg.rw.seek = *fpos;

                result = sys_semaphore_signal(hdl->event_req);
                if (result == ESUCC) {

                        result = sys_semaphore_wait(hdl->event_res, REQUEST_TIMEOUT);
                        if (result == ESUCC) {

                                if (hdl->action.err_no == ESUCC) {
                                        *wrcnt = count - hdl->action.arg.rw.size;
                                } else {
                                        result = hdl->action.err_no;
                                }
                        }

                        hdl->action.cmd = LOOP_CMD__IDLE;
                }

                sys_mutex_unlock(hdl->mtx);
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

        if (hdl->host_lock == NULL)
                return ESRCH;

        int result = sys_mutex_lock(hdl->mtx, OPERATION_TIMEOUT);
        if (result == ESUCC) {

                hdl->action.cmd         = LOOP_CMD__TRANSMISSION_HOST2CLIENT;
                hdl->action.arg.rw.data = dst;
                hdl->action.arg.rw.seek = *fpos;

                while (count) {
                        hdl->action.arg.rw.size = count;

                        result = sys_semaphore_signal(hdl->event_req);
                        if (result != ESUCC) {
                                break;
                        }

                        result = sys_semaphore_wait(hdl->event_res, REQUEST_TIMEOUT);
                        if (result == ESUCC) {

                                if (hdl->action.err_no == ESUCC) {

                                        if (hdl->action.arg.rw.size == 0) {
                                                break;

                                        } else if (hdl->action.arg.rw.size > count) {
                                                hdl->action.arg.rw.size = count;
                                        }

                                        if (  hdl->action.arg.rw.size
                                           && hdl->action.arg.rw.data) {

                                                memcpy(dst,
                                                       hdl->action.arg.rw.data,
                                                       hdl->action.arg.rw.size);
                                        }

                                        dst    += hdl->action.arg.rw.size;
                                        count  -= hdl->action.arg.rw.size;
                                        *rdcnt += hdl->action.arg.rw.size;

                                } else {
                                        result = hdl->action.err_no;
                                        break;
                                }

                        } else {
                                break;
                        }
                }

                hdl->action.cmd = LOOP_CMD__IDLE;

                sys_mutex_unlock(hdl->mtx);
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
                status = sys_device_lock(&hdl->host_lock);
                break;

        case IOCTL_LOOP__HOST_CLOSE:
                status = sys_device_get_access(&hdl->host_lock);
                if (status == ESUCC) {
                        sys_semaphore_wait(hdl->event_req, 0);
                        sys_semaphore_wait(hdl->event_res, 0);
                        sys_device_unlock(&hdl->host_lock, false);
                }
                break;

        case IOCTL_LOOP__HOST_WAIT_FOR_REQUEST:
                status = sys_device_get_access(&hdl->host_lock);
                if (arg && status == ESUCC) {
                        status = sys_semaphore_wait(hdl->event_req, HOST_REQUEST_TIMEOUT);
                        if (status == ESUCC) {
                                LOOP_request_t *req = static_cast(LOOP_request_t*, arg);

                                req->cmd = hdl->action.cmd;
                                switch(req->cmd) {
                                case LOOP_CMD__TRANSMISSION_CLIENT2HOST:
                                case LOOP_CMD__TRANSMISSION_HOST2CLIENT:
                                        req->arg.rw.seek = hdl->action.arg.rw.seek;
                                        req->arg.rw.size = hdl->action.arg.rw.size;
                                        break;

                                case LOOP_CMD__IOCTL_REQUEST:
                                        req->arg.ioctl.request = hdl->action.arg.ioctl.rq;
                                        req->arg.ioctl.arg     = hdl->action.arg.ioctl.arg;
                                        break;

                                default:
                                case LOOP_CMD__IDLE:
                                case LOOP_CMD__DEVICE_STAT:
                                case LOOP_CMD__FLUSH_BUFFERS:
                                        break;
                                }
                        }
                }
                break;

        case IOCTL_LOOP__HOST_READ_DATA_FROM_CLIENT:
                status = sys_device_get_access(&hdl->host_lock);
                if (arg && status == ESUCC) {
                        LOOP_buffer_t *buf = static_cast(LOOP_buffer_t*, arg);

                        if (buf->err_no == ESUCC) {
                                if (buf->size > hdl->action.arg.rw.size) {
                                        buf->size = hdl->action.arg.rw.size;
                                }

                                if (buf->size > 0 && buf->data) {
                                        memcpy(buf->data, hdl->action.arg.rw.data, buf->size);

                                        hdl->action.arg.rw.data += buf->size;
                                        hdl->action.arg.rw.size -= buf->size;
                                }

                                if (hdl->action.arg.rw.size == 0 || buf->size == 0 || !buf->data) {
                                        status = sys_semaphore_signal(hdl->event_res);
                                } else {
                                        status = ESUCC;
                                }

                        } else {
                                hdl->action.err_no = buf->err_no;
                                status = sys_semaphore_signal(hdl->event_res);
                        }
                }
                break;

        case IOCTL_LOOP__HOST_WRITE_DATA_TO_CLIENT:
                status = sys_device_get_access(&hdl->host_lock);
                if (arg && status == ESUCC) {
                        LOOP_buffer_t *buf = static_cast(LOOP_buffer_t*, arg);

                        if (buf->err_no == ESUCC) {
                                if (  buf->size == 0
                                   || !buf->data
                                   || buf->size == hdl->action.arg.rw.size) {

                                        status = ESUCC;

                                } else if (buf->size >= hdl->action.arg.rw.size) {
                                        buf->size = hdl->action.arg.rw.size;
                                        status = ESUCC;

                                } else {
                                        status = 1;
                                }

                                hdl->action.arg.rw.data = buf->data;
                                hdl->action.arg.rw.size = buf->size;
                                hdl->action.err_no      = buf->err_no;

                                sys_semaphore_signal(hdl->event_res);

                                if (status) {
                                        status = sys_semaphore_wait(hdl->event_req, HOST_REQUEST_TIMEOUT);
                                }
                        } else {
                                hdl->action.err_no = buf->err_no;
                                status = sys_semaphore_signal(hdl->event_res);
                        }
                }
                break;

        case IOCTL_LOOP__HOST_SET_IOCTL_STATUS:
                status = sys_device_get_access(&hdl->host_lock);
                if (arg && status == ESUCC) {
                        LOOP_ioctl_response_t *res  = static_cast(LOOP_ioctl_response_t*, arg);

                        hdl->action.err_no = res->err_no;

                        status = sys_semaphore_signal(hdl->event_res);
                }
                break;

        case IOCTL_LOOP__HOST_SET_DEVICE_STATS:
                status = sys_device_get_access(&hdl->host_lock);
                if (arg && status == ESUCC) {
                        LOOP_stat_response_t *res = static_cast(LOOP_stat_response_t*, arg);

                        hdl->action.arg.stat.size = res->size;
                        hdl->action.err_no        = res->err_no;

                        status = sys_semaphore_signal(hdl->event_res);
                }
                break;

        case IOCTL_LOOP__HOST_FLUSH_DONE:
                status = sys_device_get_access(&hdl->host_lock);
                if (arg && status == ESUCC) {
                        hdl->action.err_no = *static_cast(int*, arg);
                        status = sys_semaphore_signal(hdl->event_res);
                }
                break;

        default: //IOCTL_LOOP__CLIENT_REQUEST(n)
                if (sys_device_is_locked(&hdl->host_lock)) {
                        hdl->action.cmd           = LOOP_CMD__IOCTL_REQUEST;
                        hdl->action.arg.ioctl.arg = arg;
                        hdl->action.arg.ioctl.rq  = request;
                        sys_semaphore_signal(hdl->event_req);

                        status = sys_semaphore_wait(hdl->event_res, REQUEST_TIMEOUT);
                        if (status == ESUCC) {
                                status = hdl->action.err_no;
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

        if (sys_device_is_locked(&hdl->host_lock)) {

                hdl->action.cmd = LOOP_CMD__FLUSH_BUFFERS;
                sys_semaphore_signal(hdl->event_req);

                status = sys_semaphore_wait(hdl->event_res, REQUEST_TIMEOUT);
                if (status == ESUCC) {
                        if (hdl->action.err_no == ESUCC) {
                                status = ESUCC;
                        } else {
                                status = hdl->action.err_no;
                        }

                }

                hdl->action.cmd = LOOP_CMD__IDLE;
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
        loop_t  *hdl = device_handle;
        int      status;

        device_stat->st_major = hdl->major;
        device_stat->st_minor = 0;
        device_stat->st_size  = 0;

        if (hdl->host_lock) {
                hdl->action.cmd = LOOP_CMD__DEVICE_STAT;
                sys_semaphore_signal(hdl->event_req);

                status = sys_semaphore_wait(hdl->event_res, REQUEST_TIMEOUT);
                if (status == ESUCC) {

                        if (hdl->action.err_no == ESUCC) {
                                device_stat->st_size = hdl->action.arg.stat.size;
                                status = ESUCC;
                        } else {
                                status = hdl->action.err_no;
                        }

                }

                hdl->action.cmd = LOOP_CMD__IDLE;
        } else {
                status = ESUCC;
        }

        return status;
}

/*==============================================================================
  End of file
==============================================================================*/
