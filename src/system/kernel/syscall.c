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
#include "config.h"
#include "fs/fsctrl.h"
#include "fs/vfs.h"
#include "drivers/drvctrl.h"
#include "kernel/syscall.h"
#include "kernel/kwrapper.h"
#include "kernel/process.h"
#include "kernel/printk.h"
#include "kernel/kpanic.h"
#include "kernel/errno.h"
#include "lib/cast.h"
#include "dnx/misc.h"

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
static void syscall_gettime(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_settime(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_driverinit(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_driverrelease(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_malloc(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_zalloc(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_free(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_syslogenable(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_syslogdisable(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_restart(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_kernelpanicdetect(syscallrq_t *syscallrq, syscallres_t *syscallres);

/*==============================================================================
  Local objects
==============================================================================*/
static queue_t *call_request;
static queue_t *call_response;
static pid_t    kworker;
static pid_t    initd;

/* syscall table */
static const syscallfunc_t syscalltab[] = {
        [SYSCALL_MOUNT            ] = syscall_mount,
        [SYSCALL_UMOUNT           ] = syscall_umount,
        [SYSCALL_GETMNTENTRY      ] = syscall_getmntentry,
        [SYSCALL_MKNOD            ] = syscall_mknod,
        [SYSCALL_MKDIR            ] = syscall_mkdir,
        [SYSCALL_MKFIFO           ] = syscall_mkfifo,
        [SYSCALL_OPENDIR          ] = syscall_opendir,
        [SYSCALL_CLOSEDIR         ] = syscall_closedir,
        [SYSCALL_READDIR          ] = syscall_readdir,
        [SYSCALL_REMOVE           ] = syscall_remove,
        [SYSCALL_RENAME           ] = syscall_rename,
        [SYSCALL_CHMOD            ] = syscall_chmod,
        [SYSCALL_CHOWN            ] = syscall_chown,
        [SYSCALL_STAT             ] = syscall_stat,
        [SYSCALL_STATFS           ] = syscall_statfs,
        [SYSCALL_FOPEN            ] = syscall_fopen,
        [SYSCALL_FCLOSE           ] = syscall_fclose,
        [SYSCALL_FWRITE           ] = syscall_fwrite,
        [SYSCALL_FREAD            ] = syscall_fread,
        [SYSCALL_FSEEK            ] = syscall_fseek,
        [SYSCALL_FTELL            ] = syscall_ftell,
        [SYSCALL_IOCTL            ] = syscall_ioctl,
        [SYSCALL_FSTAT            ] = syscall_fstat,
        [SYSCALL_FFLUSH           ] = syscall_fflush,
        [SYSCALL_FEOF             ] = syscall_feof,
        [SYSCALL_CLEARERROR       ] = syscall_clearerr,
        [SYSCALL_FERROR           ] = syscall_ferror,
        [SYSCALL_SYNC             ] = syscall_sync,
        [SYSCALL_GETTIME          ] = syscall_gettime,
        [SYSCALL_SETTIME          ] = syscall_settime,
        [SYSCALL_DRIVERINIT       ] = syscall_driverinit,
        [SYSCALL_DRIVERRELEASE    ] = syscall_driverrelease,
        [SYSCALL_MALLOC           ] = syscall_malloc,
        [SYSCALL_ZALLOC           ] = syscall_zalloc,
        [SYSCALL_FREE             ] = syscall_free,
        [SYSCALL_SYSLOGENABLE     ] = syscall_syslogenable,
        [SYSCALL_SYSLOGDISABLE    ] = syscall_syslogdisable,
        [SYSCALL_RESTART          ] = syscall_restart,
        [SYSCALL_KERNELPANICDETECT] = syscall_kernelpanicdetect,
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
void syscall(syscall_t syscall, void *retptr, ...)
{
        syscallrq_t syscallrq;
        syscallrq.syscall  = syscall;
        syscallrq.task     = _task_get_handle();
        syscallrq.retptr   = retptr;
        va_start(syscallrq.args, retptr);

        int result = _queue_send(call_request, &syscallrq, MAX_DELAY_MS);
        if (result == ESUCC) {
                syscallres_t callres;
                result = _queue_receive(call_response, &callres, MAX_DELAY_MS);
                if (result == ESUCC) {
                        _errno = callres.err;
                } else {
                        _errno = result;
                }
        } else {
                _errno = result;
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
        _process_create(&kworker, NULL, "kworker", "");
//        _thread_create(kworker, NULL, kworker_thread);
        _process_create(&initd, NULL, "initd", "");
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
int _syscall_kworker_master(int argc, char *argv[])
{
        UNUSED_ARG2(argc, argv);

        for (;;) {
                syscallrq_t  syscallrq;
                syscallres_t syscallres;

                if (_queue_receive(call_request, &syscallrq, MAX_DELAY_MS) == ESUCC) {

                        if (syscallrq.syscall < _SYSCALL_COUNT) {
                                syscallres.err = ESUCC;
                                syscalltab[syscallrq.syscall](&syscallrq, &syscallres);
                        } else {
                                syscallres.err = EBADRQC;
                        }

                        _queue_send(call_response, &syscallres, MAX_DELAY_MS);
                }
        }

        return 0;
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
        GETARG(const char *, path);
        GETARG(mode_t,       mode); // FIXME should be a pointer
        SETERRNO(_vfs_mkdir(path, mode));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
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
        GETARG(size_t, size); // FIXME should be a pointer
        GETARG(size_t, count); // FIXME should be a pointer
        GETARG(FILE*,  file); // FIXME should be a pointer

        size_t wrcnt = 0;
        SETERRNO(_vfs_fwrite(buf, count * size, &wrcnt, file));
        SETRETURN(size_t, wrcnt);
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
        GETARG(uint8_t *, buf);
        GETARG(size_t, size); // FIXME should be a pointer
        GETARG(size_t, count); // FIXME should be a pointer
        GETARG(FILE*,  file); // FIXME should be a pointer

        size_t rdcnt = 0;
        SETERRNO(_vfs_fread(buf, count * size, &rdcnt, file));
        SETRETURN(size_t, rdcnt);
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

}

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
        dev_t drvid = -1;
        SETERRNO(_driver_init(drv_name, node_path, &drvid));
        SETRETURN(int, drvid);
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

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_malloc(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(size_t *, size);
        void * mem = NULL;
        SETERRNO(_kmalloc(_MM_PROG, *size, &mem));
        SETRETURN(void*, mem);
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_zalloc(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(size_t *, size);
        void * mem = NULL;
        SETERRNO(_kzalloc(_MM_PROG, *size, &mem));
        SETRETURN(void*, mem);
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_free(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(void *, mem);
        SETERRNO(_kfree(_MM_PROG, &mem));
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_syslogenable(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(const char *, path);
        SETERRNO(_printk_enable(path));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_syslogdisable(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        SETERRNO(_printk_disable());
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_restart(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        UNUSED_ARG2(syscallrq, syscallres);

        _cpuctl_restart_system();
}

//==============================================================================
/**
 * @brief  ?
 * @param  ?
 * @return ?
 */
//==============================================================================
static void syscall_kernelpanicdetect(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(bool *, showmsg);
        SETRETURN(bool, _kernel_panic_detect(*showmsg));
        SETERRNO(ESUCC);
}

/*==============================================================================
  End of file
==============================================================================*/
