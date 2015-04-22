/*=========================================================================*//**
@file    syscall.c

@author  Daniel Zorychta

@brief   System call handling - kernel space

@note    Copyright (C) 2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include "kernel/syscall.h"
#include "fs/fsctrl.h"
#include "fs/vfs.h"
#include "drivers/drvctrl.h"
#include "kernel/kwrapper.h"
#include "kernel/process.h"
#include "config.h"
#include "errno.h"
#include "lib/cast.h"
#include "dnx/misc.h"

#include "lib/printx.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define GETARG(type, var)       type var = va_arg(syscallrq->args, type)
#define GETRETURN(type, var)    type var = syscallrq->retptr
#define SETRETURN(type, var)    if (syscallrq->retptr) {*((type*)syscallrq->retptr) = (var);}
#define SETERRNO(var)           syscallres->err = var
#define GETERRNO()              syscallres->err

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        void     *retptr;
        task_t   *task;
        syscall_t syscall;
        va_list   args;
} syscallrq_t;

typedef struct {
        int       err;
} syscallres_t;

typedef void (*syscallfunc_t)(syscallrq_t*, syscallres_t*);

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void kworker_thread(void *arg);
static void syscall_mount(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_umount(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_getmntentry(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_mknod(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_mkdir(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_mkfifo(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_opendir(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_closedir(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_readdir(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_remove(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_rename(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_chmod(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_chown(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_stat(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_statfs(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_fopen(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_freopen(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_fclose(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_fwrite(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_fread(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_fseek(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_ftell(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_ioctl(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_fstat(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_fflush(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_feof(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_clearerr(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_ferror(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_sync(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_time(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_stime(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_driverinit(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_driverrelease(syscallrq_t *syscallrq, syscallres_t *syscallres);

/*==============================================================================
  Local objects
==============================================================================*/
static queue_t *call_request;
static queue_t *call_response;
static pid_t    kworker;

/* syscall table */
static const syscallfunc_t syscalltab[] = {
        [SYSCALL_MOUNT        ] = syscall_mount,
        [SYSCALL_UMOUNT       ] = syscall_umount,
        [SYSCALL_GETMNTENTRY  ] = syscall_getmntentry,
        [SYSCALL_MKNOD        ] = syscall_mknod,
        [SYSCALL_MKDIR        ] = syscall_mkdir,
        [SYSCALL_MKFIFO       ] = syscall_mkfifo,
        [SYSCALL_OPENDIR      ] = syscall_opendir,
        [SYSCALL_CLOSEDIR     ] = syscall_closedir,
        [SYSCALL_READDIR      ] = syscall_readdir,
        [SYSCALL_REMOVE       ] = syscall_remove,
        [SYSCALL_RENAME       ] = syscall_rename,
        [SYSCALL_CHMOD        ] = syscall_chmod,
        [SYSCALL_CHOWN        ] = syscall_chown,
        [SYSCALL_STAT         ] = syscall_stat,
        [SYSCALL_STATFS       ] = syscall_statfs,
        [SYSCALL_FOPEN        ] = syscall_fopen,
        [SYSCALL_FREOPEN      ] = syscall_freopen,
        [SYSCALL_FCLOSE       ] = syscall_fclose,
        [SYSCALL_FWRITE       ] = syscall_fwrite,
        [SYSCALL_FREAD        ] = syscall_fread,
        [SYSCALL_FSEEK        ] = syscall_fseek,
        [SYSCALL_FTELL        ] = syscall_ftell,
        [SYSCALL_IOCTL        ] = syscall_ioctl,
        [SYSCALL_FSTAT        ] = syscall_fstat,
        [SYSCALL_FFLUSH       ] = syscall_fflush,
        [SYSCALL_FEOF         ] = syscall_feof,
        [SYSCALL_CLEARERROR   ] = syscall_clearerr,
        [SYSCALL_FERROR       ] = syscall_ferror,
        [SYSCALL_SYNC         ] = syscall_sync,
        [SYSCALL_GETTIME      ] = syscall_gettime,
        [SYSCALL_SETTIME      ] = syscall_settime,
        [SYSCALL_DRIVERINIT   ] = syscall_driverinit,
        [SYSCALL_DRIVERRELEASE] = syscall_driverrelease,
};

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
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
void _syscall(syscall_t syscall, void *retptr, ...)
{
        syscallrq_t syscallrq;
        syscallrq.syscall  = syscall;
        syscallrq.task     = _task_get_handle();
        syscallrq.retptr   = retptr;
        va_start(syscallrq.args, retptr);

        if (_queue_send(call_request, &syscallrq, MAX_DELAY_MS)) {
                syscallres_t callres;
                if (_queue_receive(call_response, &callres, MAX_DELAY_MS)) {
                      errno = callres.err;
                }
        }

        va_end(syscallrq.args);
}

//==============================================================================
/**
 * @brief  Initialize system calls
 * @param  None
 * @return None
 */
//==============================================================================
void _syscall_init()
{
        _queue_create(1, sizeof(syscallrq_t), &call_request);
        _queue_create(1, sizeof(syscallres_t), &call_response);
        _process_create(kworker_thread, "kworker", CONFIG_RTOS_SYSCALL_STACK_DEPTH, NULL, &kworker);
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void kworker_thread(void *arg)
{
        UNUSED_ARG(arg);

        for (;;) {
                syscallrq_t  syscallrq;
                syscallres_t syscallres;

                if (_queue_receive(call_request, &syscallrq, MAX_DELAY_MS)) {

                        if (syscallrq.syscall < _SYSCALL_COUNT) {
                                syscalltab[syscallrq.syscall](&syscallrq, &syscallres);
                        } else {
                                syscallres.err = EBADRQC;
                        }

                        _queue_send(call_response, &syscallres, MAX_DELAY_MS);
                }
        }
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_mount(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(const char *, FS_name);
        GETARG(const char *, src_path);
        GETARG(const char *, mount_point);
        SETERRNO(_mount(FS_name, src_path, mount_point));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_umount(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(const char *, mount_point);
        SETERRNO(_umount(mount_point));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_getmntentry(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_mknod(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_mkdir(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_mkfifo(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_opendir(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_closedir(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_readdir(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_remove(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_rename(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_chmod(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_chown(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_stat(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_statfs(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_fopen(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(const char *, path);
        GETARG(const char *, mode);
        GETRETURN(FILE **, file);
        SETERRNO(_vfs_fopen(path, mode, file));
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_freopen(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_fclose(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_fwrite(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(const uint8_t *, buf);
        GETARG(size_t, size);
        GETARG(size_t, count);
        GETARG(FILE*, file);

        syscallres->err = 0; // FIXME
//        SETRETURN(size_t, _vfs_fwrite(buf, size, count, file));
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================;
static void syscall_fread(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================;
static void syscall_fseek(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_ftell(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_ioctl(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_fstat(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_fflush(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_feof(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_clearerr(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_ferror(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_sync(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_gettime(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}.

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_settime(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_driverinit(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(const char *, drv_name);
        GETARG(const char *, node_path);
        dev_t drvid;
        SETERRNO(_driver_init(drv_name, node_path, &drvid));
        SETRETURN(int, GETERRNO() == ESUCC ? drvid : -1);
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_driverrelease(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(const char *, drv_name);
        SETERRNO(_driver_release(drv_name));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : 1);
}

/*==============================================================================
  End of file
==============================================================================*/
