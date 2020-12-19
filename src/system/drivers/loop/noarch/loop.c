/*=========================================================================*//**
@file    loop.c

@author  Daniel Zorychta

@brief   Universal virtual device that allow to connect applications together

@note    Copyright (C) 2014 Daniel Zorychta <daniel.zorychta@gmail.com>

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

#define FLAG_REQUEST            (1<<0)
#define FLAG_RESPONSE           (1<<1)

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

        int err;
} req_t;


typedef struct {
        mutex_t    *mtx;
        flag_t     *flag;
        dev_lock_t  host_lock;
        req_t       action;
} loop_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/
static int submit_request(loop_t *hdl);
static int submit_response(loop_t *hdl);
static int wait_for_response(loop_t *hdl, u32_t timeout);
static int wait_for_request(loop_t *hdl, u32_t timeout);

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
 * @param[in ]            config               optional module configuration
 *
 * @return One of errno value (errno.h)
 */
//==============================================================================
API_MOD_INIT(LOOP, void **device_handle, u8_t major, u8_t minor, const void *config)
{
        UNUSED_ARG2(major, config);

        if (minor != 0) {
                return ENODEV;
        }

        int err = sys_zalloc(sizeof(loop_t), device_handle);
        if (!err) {
                loop_t *hdl = *device_handle;

                sys_device_unlock(&hdl->host_lock, true);

                if (!err) {
                        err = sys_mutex_create(MUTEX_TYPE_NORMAL, &hdl->mtx);
                }

                if (!err) {
                        err = sys_flag_create(&hdl->flag);
                }

                if (err) {
                        if (hdl->mtx) {
                                sys_mutex_destroy(hdl->mtx);
                        }

                        if (hdl->flag) {
                                sys_flag_destroy(hdl->flag);
                        }

                        sys_free(device_handle);
                }
        }

        return err;
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

        int err = sys_mutex_lock(hdl->mtx, RELEASE_TIMEOUT);
        if (!err) {
                mutex_t *mtx = hdl->mtx;
                sys_mutex_unlock(mtx);
                sys_mutex_destroy(mtx);
                sys_flag_destroy(hdl->flag);
                sys_free(&device_handle);
        }

        return err;
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

        if (sys_device_is_unlocked(&hdl->host_lock)) {
                return ESRCH;
        }

        int err = sys_mutex_lock(hdl->mtx, OPERATION_TIMEOUT);
        if (!err) {

                hdl->action.cmd         = LOOP_CMD__TRANSMISSION_CLIENT2HOST;
                hdl->action.arg.rw.data = const_cast(u8_t*, src);
                hdl->action.arg.rw.size = count;
                hdl->action.arg.rw.seek = *fpos;

                err = submit_request(hdl);
                if (!err) {

                        err = wait_for_response(hdl, REQUEST_TIMEOUT);
                        if (!err) {

                                err = hdl->action.err;
                                if (!err) {
                                        *wrcnt = count - hdl->action.arg.rw.size;
                                }
                        }

                        hdl->action.cmd = LOOP_CMD__IDLE;
                }

                sys_mutex_unlock(hdl->mtx);
        }

        return err;
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

        if (sys_device_is_unlocked(&hdl->host_lock)) {
                return ESRCH;
        }

        int err = sys_mutex_lock(hdl->mtx, OPERATION_TIMEOUT);
        if (!err) {

                hdl->action.cmd         = LOOP_CMD__TRANSMISSION_HOST2CLIENT;
                hdl->action.arg.rw.data = dst;
                hdl->action.arg.rw.seek = *fpos;

                while (count) {
                        hdl->action.arg.rw.size = count;

                        err = submit_request(hdl);
                        if (err) {
                                break;
                        }

                        err = wait_for_response(hdl, REQUEST_TIMEOUT);
                        if (!err) {

                                err = hdl->action.err;

                                if (!err) {

                                        if (  hdl->action.arg.rw.size == 0
                                           || hdl->action.arg.rw.data == NULL) {
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
                                        break;
                                }

                        } else {
                                break;
                        }
                }

                hdl->action.cmd = LOOP_CMD__IDLE;

                sys_mutex_unlock(hdl->mtx);
        }

        return err;
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

        int err = EINVAL;

        switch (request) {
        case IOCTL_LOOP__HOST_OPEN:
                err = sys_device_lock(&hdl->host_lock);
                break;

        case IOCTL_LOOP__HOST_CLOSE:
                err = sys_device_get_access(&hdl->host_lock);
                if (!err) {
                        sys_flag_clear(hdl->flag, FLAG_REQUEST | FLAG_RESPONSE);
                        sys_device_unlock(&hdl->host_lock, false);
                }
                break;

        case IOCTL_LOOP__HOST_WAIT_FOR_REQUEST:
                err = sys_device_get_access(&hdl->host_lock);
                if (arg && !err) {
                        err = wait_for_request(hdl, HOST_REQUEST_TIMEOUT);
                        if (!err) {
                                LOOP_request_t *req = cast(LOOP_request_t*, arg);

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
                err = sys_device_get_access(&hdl->host_lock);
                if (arg && !err) {
                        LOOP_buffer_t *buf = cast(LOOP_buffer_t*, arg);

                        hdl->action.err = buf->err;

                        if (!buf->err) {
                                buf->size = min(buf->size, hdl->action.arg.rw.size);

                                if (buf->size > 0 && buf->data) {
                                        memcpy(buf->data, hdl->action.arg.rw.data, buf->size);

                                        hdl->action.arg.rw.data += buf->size;
                                        hdl->action.arg.rw.size -= buf->size;
                                }

                                if (hdl->action.arg.rw.size == 0 || buf->size == 0 || !buf->data) {
                                        err = submit_response(hdl);
                                }

                        } else {
                                err = submit_response(hdl);
                        }
                }
                break;

        case IOCTL_LOOP__HOST_WRITE_DATA_TO_CLIENT:
                err = sys_device_get_access(&hdl->host_lock);
                if (arg && !err) {
                        LOOP_buffer_t *buf = cast(LOOP_buffer_t*, arg);

                        hdl->action.err = buf->err;

                        if (!buf->err) {
                                hdl->action.arg.rw.data = buf->data;
                                hdl->action.arg.rw.size = min(buf->size, hdl->action.arg.rw.size);
                                hdl->action.err         = buf->err;
                        }

                        err = submit_response(hdl);
                }
                break;

        case IOCTL_LOOP__HOST_SET_IOCTL_STATUS:
                err = sys_device_get_access(&hdl->host_lock);
                if (arg && err == ESUCC) {
                        LOOP_ioctl_response_t *res  = cast(LOOP_ioctl_response_t*, arg);

                        hdl->action.err = res->err;

                        err = submit_response(hdl);
                }
                break;

        case IOCTL_LOOP__HOST_SET_DEVICE_STATS:
                err = sys_device_get_access(&hdl->host_lock);
                if (arg && err == ESUCC) {
                        LOOP_stat_response_t *res = cast(LOOP_stat_response_t*, arg);

                        hdl->action.arg.stat.size = res->size;
                        hdl->action.err           = res->err;

                        err = submit_response(hdl);
                }
                break;

        case IOCTL_LOOP__HOST_FLUSH_DONE:
                err = sys_device_get_access(&hdl->host_lock);
                if (arg && err == ESUCC) {
                        hdl->action.err = *cast(int*, arg);
                        err = submit_response(hdl);
                }
                break;

        default: //IOCTL_LOOP__CLIENT_REQUEST(n)
                if (sys_device_is_locked(&hdl->host_lock)) {
                        hdl->action.cmd           = LOOP_CMD__IOCTL_REQUEST;
                        hdl->action.arg.ioctl.arg = arg;
                        hdl->action.arg.ioctl.rq  = request;

                        submit_request(hdl);

                        err = wait_for_response(hdl, REQUEST_TIMEOUT);
                        if (!err) {
                                err = hdl->action.err;
                        }
                } else {
                        err = ESRCH;
                }
                break;
        }

        return err;
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
        loop_t *hdl = device_handle;
        int     err = ESUCC;

        if (sys_device_is_locked(&hdl->host_lock)) {

                hdl->action.cmd = LOOP_CMD__FLUSH_BUFFERS;
                submit_request(hdl);

                err = wait_for_response(hdl, REQUEST_TIMEOUT);
                if (!err) {
                        err = hdl->action.err;
                }

                hdl->action.cmd = LOOP_CMD__IDLE;
        }

        return err;
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
        int      err = ESUCC;

        device_stat->st_size = 0;

        if (sys_device_is_locked(&hdl->host_lock)) {

                hdl->action.cmd = LOOP_CMD__DEVICE_STAT;
                submit_request(hdl);

                err = wait_for_response(hdl, REQUEST_TIMEOUT);
                if (!err) {
                        err = hdl->action.err;

                        if (!err) {
                                device_stat->st_size = hdl->action.arg.stat.size;
                        }
                }

                hdl->action.cmd = LOOP_CMD__IDLE;
        }

        return err;
}

//==============================================================================
/**
 * @brief  Function submit request.
 *
 * @param  hdl          driver handle
 *
 * @return One of errno value.
 */
//==============================================================================
static int submit_request(loop_t *hdl)
{
        return sys_flag_set(hdl->flag, FLAG_REQUEST);
}

//==============================================================================
/**
 * @brief  Function submit response.
 *
 * @param  hdl          driver handle
 *
 * @return One of errno value.
 */
//==============================================================================
static int submit_response(loop_t *hdl)
{
        return sys_flag_set(hdl->flag, FLAG_RESPONSE);
}

//==============================================================================
/**
 * @brief  Function wait for response.
 *
 * @param  hdl          driver handle
 * @param  timeout      timeout in ms
 *
 * @return One of errno value.
 */
//==============================================================================
static int wait_for_response(loop_t *hdl, u32_t timeout)
{
        return sys_flag_wait(hdl->flag, FLAG_RESPONSE, timeout);
}

//==============================================================================
/**
 * @brief  Function wait for request.
 *
 * @param  hdl          driver handle
 * @param  timeout      timeout in ms
 *
 * @return One of errno value.
 */
//==============================================================================
static int wait_for_request(loop_t *hdl, u32_t timeout)
{
        return sys_flag_wait(hdl->flag, FLAG_REQUEST, timeout);
}

/*==============================================================================
  End of file
==============================================================================*/
