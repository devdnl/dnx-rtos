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
        mutex_t         *lock;
        sem_t           *event;
        queue_t         *request;
        u8_t             major;
        ssize_t          n;
} loop_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/
MODULE_NAME(LOOP);

static uint release_timeout   = 1000;
static uint operation_timeout = 60000;
static uint request_timeout   = 20000;

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
                        hdl->lock    = mutex_new(MUTEX_NORMAL);
                        hdl->event   = semaphore_new(1, 0);
                        hdl->request = queue_new(1, sizeof(loop_request_t));
                        hdl->major   = major;

                        if (hdl->lock && hdl->event && hdl->request) {
                                *device_handle = hdl;
                                return STD_RET_OK;

                        } else {
                                if (hdl->lock) {
                                        mutex_delete(hdl->lock);
                                }

                                if (hdl->event) {
                                        semaphore_delete(hdl->event);
                                }

                                if (hdl->request) {
                                        queue_delete(hdl->request);
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
                semaphore_delete(hdl->event);
                queue_delete(hdl->request);
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

        if (mutex_lock(hdl->lock, operation_timeout)) {

                loop_request_t rq;
                rq.cmd = LOOP_CMD_WRITE;
                rq.args.rdwr.data = const_cast(u8_t*, src);
                rq.args.rdwr.size = count;
                rq.args.rdwr.seek = *fpos;
                rq.major          = hdl->major;

                // TODO tutaj musi dzialac to w petli zeby dalo sie czesciowe dane czytac malymi porcjami

                if (queue_send(hdl->request, &rq, request_timeout)) {
                        if (semaphore_wait(hdl->event, request_timeout) == false) {
                                errno = ETIME;
                                n = -1;
                        } else {
                                n = hdl->n;
                        }

                } else {
                        errno = ETIME;
                        n = -1;
                }

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
        return 0;
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
        switch (request) {
        case IOCTL_LOOP__HOST_WAIT_FOR_REQUEST:
        case IOCTL_LOOP__HOST_READ_DATA:
        case IOCTL_LOOP__HOST_WRITE_DATA:
        case IOCTL_LOOP__HOST_SET_IOCTL_STATUS:
        case IOCTL_LOOP__HOST_SET_DEVICE_STATS:
        default: //IOCTL_LOOP__CLIENT_REQUEST(n)
                break;
        }

        return STD_RET_OK;
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
        return STD_RET_OK;
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
        device_stat->st_size  = 0;
        device_stat->st_major = 0;
        device_stat->st_minor = _LOOP_MINOR_NUMBER;

        return STD_RET_OK;
}

/*==============================================================================
  End of file
==============================================================================*/
