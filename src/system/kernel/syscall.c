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
#include <string.h>
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
#include "kernel/time.h"
#include "lib/cast.h"
#include "lib/unarg.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define SYSCALL_QUEUE_LENGTH    8

#define GETARG(type, var)       type var = va_arg(rq->args, type)
#define GETRETURN(type, var)    type var = rq->retptr
#define GETTASKHDL()            rq->task
#define GETPROCESS()            _process_get_container_by_task(rq->task, NULL)
#define GETTHREAD(_tid)         _process_thread_get_container(GETPROCESS(), _tid)
#define SETRETURN(type, var)    if (rq->retptr) {*((type*)rq->retptr) = (var);}
#define SETERRNO(var)           rq->err = var
#define GETERRNO()              rq->err
#define UNUSED_RQ()             UNUSED_ARG1(rq)

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        void     *retptr;
        task_t   *task;
        syscall_t syscall;
        va_list   args;
        int       err;
} syscallrq_t;

typedef void (*syscallfunc_t)(syscallrq_t*);

/*==============================================================================
  Local function prototypes
==============================================================================*/
static void syscall_do(syscallrq_t *rq);
static void syscall_kworker_thread(void *arg);
static void syscall_mount(syscallrq_t *rq);
static void syscall_umount(syscallrq_t *rq);
static void syscall_getmntentry(syscallrq_t *rq);
static void syscall_mknod(syscallrq_t *rq);
static void syscall_mkdir(syscallrq_t *rq);
static void syscall_mkfifo(syscallrq_t *rq);
static void syscall_opendir(syscallrq_t *rq);
static void syscall_closedir(syscallrq_t *rq);
static void syscall_readdir(syscallrq_t *rq);
static void syscall_remove(syscallrq_t *rq);
static void syscall_rename(syscallrq_t *rq);
static void syscall_chmod(syscallrq_t *rq);
static void syscall_chown(syscallrq_t *rq);
static void syscall_stat(syscallrq_t *rq);
static void syscall_statfs(syscallrq_t *rq);
static void syscall_fopen(syscallrq_t *rq);
static void syscall_fclose(syscallrq_t *rq);
static void syscall_fwrite(syscallrq_t *rq);
static void syscall_fread(syscallrq_t *rq);
static void syscall_fseek(syscallrq_t *rq);
static void syscall_ftell(syscallrq_t *rq);
static void syscall_ioctl(syscallrq_t *rq);
static void syscall_fstat(syscallrq_t *rq);
static void syscall_fflush(syscallrq_t *rq);
static void syscall_feof(syscallrq_t *rq);
static void syscall_clearerr(syscallrq_t *rq);
static void syscall_ferror(syscallrq_t *rq);
static void syscall_sync(syscallrq_t *rq);
static void syscall_gettime(syscallrq_t *rq);
static void syscall_settime(syscallrq_t *rq);
static void syscall_driverinit(syscallrq_t *rq);
static void syscall_driverrelease(syscallrq_t *rq);
static void syscall_malloc(syscallrq_t *rq);
static void syscall_zalloc(syscallrq_t *rq);
static void syscall_free(syscallrq_t *rq);
static void syscall_syslogenable(syscallrq_t *rq);
static void syscall_syslogdisable(syscallrq_t *rq);
static void syscall_restart(syscallrq_t *rq);
static void syscall_kernelpanicdetect(syscallrq_t *rq);
static void syscall_abort(syscallrq_t *rq);
static void syscall_exit(syscallrq_t *rq);
static void syscall_system(syscallrq_t *rq);
static void syscall_processcreate(syscallrq_t *rq);
static void syscall_processdestroy(syscallrq_t *rq);
static void syscall_processgetexitsem(syscallrq_t *rq);
static void syscall_processstatseek(syscallrq_t *rq);
static void syscall_processstatpid(syscallrq_t *rq);
static void syscall_processgetpid(syscallrq_t *rq);
static void syscall_processgetprio(syscallrq_t *rq);
static void syscall_getcwd(syscallrq_t *rq);
static void syscall_threadcreate(syscallrq_t *rq);
static void syscall_threaddestroy(syscallrq_t *rq);
static void syscall_threadexit(syscallrq_t *rq);
static void syscall_threadgetexitsem(syscallrq_t *rq);
static void syscall_semaphorecreate(syscallrq_t *rq);
static void syscall_semaphoredestroy(syscallrq_t *rq);
static void syscall_mutexcreate(syscallrq_t *rq);
static void syscall_mutexdestroy(syscallrq_t *rq);
static void syscall_queuecreate(syscallrq_t *rq);
static void syscall_queuedestroy(syscallrq_t *rq);

/*==============================================================================
  Local objects
==============================================================================*/
static queue_t *call_request;

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
        [SYSCALL_ABORT            ] = syscall_abort,
        [SYSCALL_EXIT             ] = syscall_exit,
        [SYSCALL_SYSTEM           ] = syscall_system,
        [SYSCALL_PROCESSCREATE    ] = syscall_processcreate,
        [SYSCALL_PROCESSDESTROY   ] = syscall_processdestroy,
        [SYSCALL_PROCESSGETEXITSEM] = syscall_processgetexitsem,
        [SYSCALL_PROCESSSTATSEEK  ] = syscall_processstatseek,
        [SYSCALL_PROCESSSTATPID   ] = syscall_processstatpid,
        [SYSCALL_PROCESSGETPID    ] = syscall_processgetpid,
        [SYSCALL_PROCESSGETPRIO   ] = syscall_processgetprio,
        [SYSCALL_GETCWD           ] = syscall_getcwd,
        [SYSCALL_THREADCREATE     ] = syscall_threadcreate,
        [SYSCALL_THREADDESTROY    ] = syscall_threaddestroy,
        [SYSCALL_THREADEXIT       ] = syscall_threadexit,
        [SYSCALL_THREADGETEXITSEM ] = syscall_threadgetexitsem,
        [SYSCALL_SEMAPHORECREATE  ] = syscall_semaphorecreate,
        [SYSCALL_SEMAPHOREDESTROY ] = syscall_semaphoredestroy,
        [SYSCALL_MUTEXCREATE      ] = syscall_mutexcreate,
        [SYSCALL_MUTEXDESTROY     ] = syscall_mutexdestroy,
        [SYSCALL_QUEUECREATE      ] = syscall_queuecreate,
        [SYSCALL_QUEUEDESTROY     ] = syscall_queuedestroy,
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
 * @brief  Initialize system calls
 *
 * @param  None
 *
 * @return None
 */
//==============================================================================
void _syscall_init()
{
        int result = ESUCC;

        result |= _queue_create(SYSCALL_QUEUE_LENGTH, sizeof(syscallrq_t*), &call_request);
        result |= _process_create("kworker", NULL, NULL);
        result |= _process_create("initd", NULL, NULL);

        if (result != ESUCC) {
                _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_INTERNAL);
        }
}

//==============================================================================
/**
 * @brief  Function call selected syscall [USERSPACE]
 *
 * @param  syscall      syscall number
 * @param  retptr       pointer to return value
 * @param  ...          additional arguments
 *
 * @return None
 */
//==============================================================================
void syscall(syscall_t syscall, void *retptr, ...)
{
        if (syscall < _SYSCALL_COUNT) {
                sem_t *syscall_sem = _builtinfunc(process_get_syscall_sem, _THIS_TASK);
                if (syscall_sem) {

                        syscallrq_t syscallrq;
                        syscallrq.syscall = syscall;
                        syscallrq.task    = _builtinfunc(task_get_handle);
                        syscallrq.retptr  = retptr;
                        syscallrq.err     = ESUCC;

                        va_start(syscallrq.args, retptr);
                        {
                                syscallrq_t *syscallrq_ptr = &syscallrq;

                                if (_builtinfunc(queue_send, call_request, &syscallrq_ptr, MAX_DELAY_MS) ==  ESUCC) {
                                        if (_builtinfunc(semaphore_wait, syscall_sem, MAX_DELAY_MS) == ESUCC) {
                                                _errno = syscallrq.err;
                                        }
                                }
                        }
                        va_end(syscallrq.args);
                }
        } else {
                _errno = ENOSYS;
        }
}

//==============================================================================
/**
 * @brief  Main syscall process (master) [KERNELSPACE]
 *
 * @param  argc         argument count
 * @param  argv         arguments
 *
 * @return Never exit (0)
 */
//==============================================================================
int _syscall_kworker_process(int argc, char *argv[])
{
        UNUSED_ARG2(argc, argv);

        static const thread_attr_t fs_blocking_thread_attr = {
                .stack_depth = STACK_DEPTH_CUSTOM(CONFIG_RTOS_FILE_SYSTEM_STACK_DEPTH),
                .priority    = PRIORITY_NORMAL
        };

        static const thread_attr_t net_blocking_thread_attr = {
                .stack_depth = STACK_DEPTH_CUSTOM(CONFIG_RTOS_NETWORK_STACK_DEPTH),
                .priority    = PRIORITY_NORMAL
        };

        for (;;) {
                syscallrq_t *rq;
                if (_queue_receive(call_request, &rq, MAX_DELAY_MS) == ESUCC) {
                        if (rq->syscall <= _SYSCALL_GROUP_0_OS_NON_BLOCKING) {
                                syscall_do(rq);
                        } else {
                                /* select stack size according to syscall group */
                                const thread_attr_t *thread_attr = NULL;
                                if (rq->syscall <= _SYSCALL_GROUP_1_FS_BLOCKING) {
                                        thread_attr = &fs_blocking_thread_attr;
                                } else if (rq->syscall <= _SYSCALL_GROUP_2_NET_BLOCKING) {
                                        thread_attr = &net_blocking_thread_attr;
                                } else {
                                        _kernel_panic_report(_KERNEL_PANIC_DESC_CAUSE_INTERNAL);
                                        continue;
                                }

                                /* give time for kernel to free resources after last syscall task */
                                _sleep_ms(1);

                                /* create new syscall task */
                                if (_process_thread_create(_process_get_container_by_task(_THIS_TASK, NULL),
                                                           syscall_kworker_thread,
                                                           thread_attr,
                                                           true,
                                                           rq,
                                                           NULL,
                                                           NULL) == ESUCC) {

                                        _task_yield();
                                } else {
                                        _queue_send(call_request, &rq, MAX_DELAY_MS);
                                        _sleep_ms(5);
                                }
                        }
                }
        }

        return -1;
}

//==============================================================================
/**
 * @brief  This thread is responsible for handle syscall
 *
 * @param  arg          thread argument
 *
 * @return None
 */
//==============================================================================
static void syscall_kworker_thread(void *arg)
{
        syscall_do(arg);
}

//==============================================================================
/**
 * @brief  Function inform syscall owner about finished request
 *
 * @param  rq           request information
 *
 * @return None
 */
//==============================================================================
static void syscall_do(syscallrq_t *rq)
{
        _process_t *client = GETPROCESS();
        _process_t *server = _process_get_container_by_task(_THIS_TASK, NULL);

        _process_set_CWD(server, _process_get_CWD(client));

        syscalltab[rq->syscall](rq);

        _semaphore_signal(_process_get_syscall_sem(rq->task));
}

//==============================================================================
/**
 * @brief  This syscall mount selected file system to selected path
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_mount(syscallrq_t *rq)
{
        GETARG(const char *, FS_name);
        GETARG(const char *, src_path);
        GETARG(const char *, mount_point);
        SETERRNO(_mount(FS_name, src_path, mount_point));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall unmount selected file system
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_umount(syscallrq_t *rq)
{
        GETARG(const char *, mount_point);
        SETERRNO(_umount(mount_point));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall return information about selected file system
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_getmntentry(syscallrq_t *rq)
{
        GETARG(int *, seek);
        GETARG(struct mntent *, mntent);
        SETERRNO(_vfs_getmntentry(*seek, mntent));

        int ret;
        switch (GETERRNO()) {
        case ESUCC : ret =  0; break;
        case ENOENT: ret =  1; break;
        default    : ret = -1; break;
        }

        SETRETURN(int, ret);
}

//==============================================================================
/**
 * @brief  This syscall create device node
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_mknod(syscallrq_t *rq)
{
        GETARG(const char *, pathname);
        GETARG(dev_t *, dev);
        SETERRNO(_vfs_mknod(pathname, *dev));
        SETRETURN(int, GETERRNO() ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall create directory
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_mkdir(syscallrq_t *rq)
{
        GETARG(const char *, path);
        GETARG(mode_t *, mode);
        SETERRNO(_vfs_mkdir(path, *mode));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall create FIFO pipe
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_mkfifo(syscallrq_t *rq)
{
        GETARG(const char *, path);
        GETARG(mode_t *, mode);
        SETERRNO(_vfs_mkfifo(path, *mode));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall open selected directory
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_opendir(syscallrq_t *rq)
{
        GETARG(const char *, path);

        DIR *dir = NULL;
        int  err = _vfs_opendir(path, &dir);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), static_cast(res_header_t*, dir));
                if (err != ESUCC) {
                        _vfs_closedir(dir);
                        dir = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(DIR*, dir);
}

//==============================================================================
/**
 * @brief  This syscall close selected directory
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_closedir(syscallrq_t *rq)
{
        GETARG(DIR *, dir);

        int err = _process_release_resource(GETPROCESS(), static_cast(res_header_t*, dir), RES_TYPE_DIR);
        if (err == EFAULT) {
                const char *msg = "*** Error: object is not a dir! ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));
                syscall_abort(rq);
        }

        SETERRNO(err);
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);

}

//==============================================================================
/**
 * @brief  This syscall read selected directory
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_readdir(syscallrq_t *rq)
{
        GETARG(DIR *, dir);
        dirent_t *dirent = NULL;
        SETERRNO(_vfs_readdir(dir, &dirent));
        SETRETURN(dirent_t*, dirent);
}

//==============================================================================
/**
 * @brief  This syscall remove selected file
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_remove(syscallrq_t *rq)
{
        GETARG(const char *, path);
        SETERRNO(_vfs_remove(path));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall rename selected file
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_rename(syscallrq_t *rq)
{
        GETARG(const char *, oldname);
        GETARG(const char *, newname);
        SETERRNO(_vfs_rename(oldname, newname));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall change mode of selected file
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_chmod(syscallrq_t *rq)
{
        GETARG(const char *, path);
        GETARG(mode_t *, mode);
        SETERRNO(_vfs_chmod(path, *mode));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall change owner and group of selected file
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_chown(syscallrq_t *rq)
{
        GETARG(const char *, path);
        GETARG(uid_t *, owner);
        GETARG(gid_t *, group);
        SETERRNO(_vfs_chown(path, *owner, *group));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall read statistics of selected file by path
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_stat(syscallrq_t *rq)
{
        GETARG(const char *, path);
        GETARG(struct stat *, buf);
        SETERRNO(_vfs_stat(path, buf));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall read statistics of selected file by FILE object
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_fstat(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        GETARG(struct stat *, buf);
        SETERRNO(_vfs_fstat(file, buf));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall read statistics of file system mounted in selected path
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_statfs(syscallrq_t *rq)
{
        GETARG(const char *, path);
        GETARG(struct statfs *, buf);
        SETERRNO(_vfs_statfs(path, buf));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall open selected file
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_fopen(syscallrq_t *rq)
{
        GETARG(const char *, path);
        GETARG(const char *, mode);

        FILE *file = NULL;
        int   err  = _vfs_fopen(path, mode, &file);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), static_cast(res_header_t*, file));
                if (err != ESUCC) {
                        _vfs_fclose(file, true);
                        file = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(FILE*, file);
}

//==============================================================================
/**
 * @brief  This syscall close selected file
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_fclose(syscallrq_t *rq)
{
        GETARG(FILE *, file);

        int err = _process_release_resource(GETPROCESS(), static_cast(res_header_t*, file), RES_TYPE_FILE);
        if (err == EFAULT) {
                const char *msg = "*** Error: object is not a file! ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));
                syscall_abort(rq);
        }

        SETERRNO(err);
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);

}

//==============================================================================
/**
 * @brief  This syscall write data to selected file
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_fwrite(syscallrq_t *rq)
{
        GETARG(const uint8_t *, buf);
        GETARG(size_t *, size);
        GETARG(size_t *, count);
        GETARG(FILE*, file);

        size_t wrcnt = 0;
        SETERRNO(_vfs_fwrite(buf, *count * *size, &wrcnt, file));
        SETRETURN(size_t, wrcnt);
}

//==============================================================================
/**
 * @brief  This syscall read data from selected file
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================;
static void syscall_fread(syscallrq_t *rq)
{
        GETARG(uint8_t *, buf);
        GETARG(size_t *, size);
        GETARG(size_t *, count);
        GETARG(FILE *, file);

        size_t rdcnt = 0;
        SETERRNO(_vfs_fread(buf, *count * *size, &rdcnt, file));
        SETRETURN(size_t, rdcnt);
}

//==============================================================================
/**
 * @brief  This syscall move file pointer
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================;
static void syscall_fseek(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        GETARG(i64_t *, lseek);
        GETARG(int *, orgin);
        SETERRNO(_vfs_fseek(file, *lseek, *orgin));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall return file pointer value
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_ftell(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        i64_t lseek = -1;
        SETERRNO(_vfs_ftell(file, &lseek));
        SETRETURN(i64_t, lseek);
}

//==============================================================================
/**
 * @brief  This syscall perform not standard operation on selected file/device
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_ioctl(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        GETARG(int *, request);
        GETARG(va_list *, arg);
        SETERRNO(_vfs_vfioctl(file, *request, *arg));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall flush buffers of selected file
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_fflush(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        SETERRNO(_vfs_fflush(file));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall check that end-of-file occurred
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_feof(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        int eof = EOF;
        SETERRNO(_vfs_feof(file, &eof));
        SETRETURN(int, eof);
}

//==============================================================================
/**
 * @brief  This syscall clear file errors
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_clearerr(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        SETERRNO(_vfs_clearerr(file));
}

//==============================================================================
/**
 * @brief  This syscall return file error indicator
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_ferror(syscallrq_t *rq)
{
        GETARG(FILE *, file);
        int error = EOF;
        SETERRNO(_vfs_ferror(file, &error));
        SETRETURN(int, error);
}

//==============================================================================
/**
 * @brief  This syscall synchronize all buffers of filesystems
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_sync(syscallrq_t *rq)
{
        UNUSED_RQ();
        _vfs_sync();
}

//==============================================================================
/**
 * @brief  This syscall return current time value (UTC timestamp)
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_gettime(syscallrq_t *rq)
{
        time_t time = -1;
        SETERRNO(_gettime(&time));
        SETRETURN(time_t, time);
}

//==============================================================================
/**
 * @brief  This syscall set current system time (UTC timestamp)
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_settime(syscallrq_t *rq)
{
        GETARG(time_t *, time);
        SETERRNO(_settime(time));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall initialize selected driver and create node
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_driverinit(syscallrq_t *rq)
{
        GETARG(const char *, drv_name);
        GETARG(const char *, node_path);
        dev_t drvid = -1;
        SETERRNO(_driver_init(drv_name, node_path, &drvid));
        SETRETURN(int, drvid);
}

//==============================================================================
/**
 * @brief  This syscall release selected driver
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_driverrelease(syscallrq_t *rq)
{
        GETARG(const char *, drv_name);
        SETERRNO(_driver_release(drv_name));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall allocate memory for application
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_malloc(syscallrq_t *rq)
{
        GETARG(size_t *, size);

        void *mem = NULL;
        int   err = _kmalloc(_MM_PROG, *size, &mem);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), mem);
                if (err != ESUCC) {
                        _kfree(_MM_PROG, &mem);
                }
        }

        SETERRNO(err);
        SETRETURN(void*, mem ? &static_cast(res_header_t*, mem)[1] : NULL);
}

//==============================================================================
/**
 * @brief  This syscall allocate memory for application and clear allocated block
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_zalloc(syscallrq_t *rq)
{
        GETARG(size_t *, size);

        void *mem = NULL;
        int   err = _kzalloc(_MM_PROG, *size, &mem);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), mem);
                if (err != ESUCC) {
                        _kfree(_MM_PROG, &mem);
                }
        }

        SETERRNO(err);
        SETRETURN(void*,  mem ? &static_cast(res_header_t*, mem)[1] : NULL);
}

//==============================================================================
/**
 * @brief  This syscall free allocated memory by application
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_free(syscallrq_t *rq)
{
        GETARG(void *, mem);

        int err = _process_release_resource(GETPROCESS(), static_cast(res_header_t*, mem) - 1, RES_TYPE_MEMORY);
        if (err != ESUCC) {
                const char *msg = "*** Error: double free or corruption ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));
                syscall_abort(rq);
        }

        SETERRNO(err);
}

//==============================================================================
/**
 * @brief  This syscall enable system log functionality in selected file
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_syslogenable(syscallrq_t *rq)
{
        GETARG(const char *, path);
        SETERRNO(_printk_enable(path));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall disable system log functionality
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_syslogdisable(syscallrq_t *rq)
{
        SETERRNO(_printk_disable());
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall restart entire system
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_restart(syscallrq_t *rq)
{
        UNUSED_RQ();
        _cpuctl_restart_system();
}

//==============================================================================
/**
 * @brief  This syscall check if kernel panic occurred in last session
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_kernelpanicdetect(syscallrq_t *rq)
{
        GETARG(bool *, showmsg);
        SETRETURN(bool, _kernel_panic_detect(*showmsg));
}

//==============================================================================
/**
 * @brief  This syscall abort current process (caller) and set exit code as -1
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_abort(syscallrq_t *rq)
{
        UNUSED_RQ();
        SETERRNO(_process_abort(GETPROCESS()));
}

//==============================================================================
/**
 * @brief  This syscall close current process (caller)
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_exit(syscallrq_t *rq)
{
        GETARG(int *, status);
        SETERRNO(_process_exit(GETPROCESS(), *status));
}

//==============================================================================
/**
 * @brief  This syscall start shell application to run command line
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_system(syscallrq_t *rq)
{
        UNUSED_ARG1(rq);

//        GETARG(const char *, cmd);
//        GETARG(pid_t *, pid);
//        GETARG(sem_t **, exit_sem);

        //TODO syscall system()
}

//==============================================================================
/**
 * @brief  This syscall create new process
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_processcreate(syscallrq_t *rq)
{
        GETARG(const char *, cmd);
        GETARG(process_attr_t *, attr);
        pid_t pid = 0;
        SETERRNO(_process_create(cmd, attr, &pid));
        SETRETURN(pid_t, pid);
}

//==============================================================================
/**
 * @brief  This syscall destroy existing process
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_processdestroy(syscallrq_t *rq)
{
        GETARG(pid_t *, pid);
        GETARG(int *, status);
        SETERRNO(_process_destroy(*pid, status));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall return exit semaphore
 *         then
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_processgetexitsem(syscallrq_t *rq)
{
        GETARG(pid_t *, pid);
        GETARG(sem_t **, sem);
        SETERRNO(_process_get_exit_sem(*pid, sem));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall read process statistics by seek
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_processstatseek(syscallrq_t *rq)
{
        GETARG(size_t *, seek);
        GETARG(process_stat_t*, stat);
        SETERRNO(_process_get_stat_seek(*seek, stat));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall read process statistics by pid
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_processstatpid(syscallrq_t *rq)
{
        GETARG(pid_t *, pid);
        GETARG(process_stat_t*, stat);
        SETERRNO(_process_get_stat_pid(*pid, stat));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall return PID of caller process
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_processgetpid(syscallrq_t *rq)
{
        pid_t pid = -1;
        SETERRNO(_process_get_pid(GETPROCESS(), &pid));
        SETRETURN(pid_t, pid);
}

//==============================================================================
/**
 * @brief  This syscall return PID's priority
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_processgetprio(syscallrq_t *rq)
{
        GETARG(pid_t *, pid);
        int prio = 0;
        SETERRNO(_process_get_priority(*pid, &prio));
        SETRETURN(int, prio);
}

//==============================================================================
/**
 * @brief  This syscall return CWD of current process
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_getcwd(syscallrq_t *rq)
{
        GETARG(char *, buf);
        GETARG(size_t *, size);

        const char *cwd = NULL;
        if (buf && *size) {
                cwd = _process_get_CWD(GETPROCESS());
                strncpy(buf, cwd, *size);
        }

        SETRETURN(char*, cwd ? buf : NULL);
}

//==============================================================================
/**
 * @brief  This syscall create new thread
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_threadcreate(syscallrq_t *rq)
{
        GETARG(thread_func_t, func);
        GETARG(thread_attr_t *, attr);
        GETARG(void *, arg);

        tid_t tid = 0;
        SETERRNO(_process_thread_create(GETPROCESS(), func, attr, false, arg, &tid, NULL));
        SETRETURN(tid_t, tid);
}

//==============================================================================
/**
 * @brief  This syscall destroy thread
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_threaddestroy(syscallrq_t *rq)
{
        GETARG(tid_t *, tid);
        SETERRNO(_process_release_resource(GETPROCESS(),
                                           static_cast(res_header_t*, GETTHREAD(*tid)),
                                           RES_TYPE_THREAD));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall destroy thread
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_threadexit(syscallrq_t *rq)
{
        GETARG(tid_t *, tid);
        SETERRNO(_process_thread_exit(GETTHREAD(*tid)));
}

//==============================================================================
/**
 * @brief  This syscall join thread with parent (parent wait until thread finish)
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_threadgetexitsem(syscallrq_t *rq)
{
        GETARG(tid_t *, tid);
        GETARG(sem_t **, sem);
        SETERRNO(_process_thread_get_exit_sem(GETPROCESS(), *tid, sem));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall create new semaphore
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_semaphorecreate(syscallrq_t *rq)
{
        GETARG(const size_t *, cnt_max);
        GETARG(const size_t *, cnt_init);

        sem_t *sem = NULL;
        int err    = _semaphore_create(*cnt_max, *cnt_init, &sem);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), static_cast(res_header_t*, sem));
                if (err != ESUCC) {
                        _semaphore_destroy(sem);
                        sem = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(sem_t*, sem);
}

//==============================================================================
/**
 * @brief  This syscall destroy selected semaphore
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_semaphoredestroy(syscallrq_t *rq)
{
        GETARG(sem_t *, sem);

        int err = _process_release_resource(GETPROCESS(), static_cast(res_header_t*, sem), RES_TYPE_SEMAPHORE);
        if (err != ESUCC) {
                const char *msg = "*** Error: object is not a semaphore! ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));
                syscall_abort(rq);
        }

        SETERRNO(err);
}

//==============================================================================
/**
 * @brief  This syscall create new mutex
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_mutexcreate(syscallrq_t *rq)
{
        GETARG(const enum mutex_type *, type);

        mutex_t *mtx = NULL;
        int err      = _mutex_create(*type, &mtx);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), static_cast(res_header_t*, mtx));
                if (err != ESUCC) {
                        _mutex_destroy(mtx);
                        mtx = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(mutex_t*, mtx);
}

//==============================================================================
/**
 * @brief  This syscall destroy selected mutex
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_mutexdestroy(syscallrq_t *rq)
{
        GETARG(mutex_t *, mtx);

        int err = _process_release_resource(GETPROCESS(), static_cast(res_header_t*, mtx), RES_TYPE_MUTEX);
        if (err != ESUCC) {
                const char *msg = "*** Error: object is not a mutex! ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));
                syscall_abort(rq);
        }

        SETERRNO(err);
}

//==============================================================================
/**
 * @brief  This syscall create new queue
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_queuecreate(syscallrq_t *rq)
{
        GETARG(const size_t *, length);
        GETARG(const size_t *, item_size);

        queue_t *q = NULL;
        int err    = _queue_create(*length, *item_size, &q);
        if (err == ESUCC) {
                err = _process_register_resource(GETPROCESS(), static_cast(res_header_t*, q));
                if (err != ESUCC) {
                        _queue_destroy(q);
                        q = NULL;
                }
        }

        SETERRNO(err);
        SETRETURN(queue_t*, q);
}

//==============================================================================
/**
 * @brief  This syscall destroy selected queue
 *
 * @param  rq                   syscall request
 *
 * @return None
 */
//==============================================================================
static void syscall_queuedestroy(syscallrq_t *rq)
{
        GETARG(queue_t *, q);

        int err = _process_release_resource(GETPROCESS(), static_cast(res_header_t*, q), RES_TYPE_QUEUE);
        if (err != ESUCC) {
                const char *msg = "*** Error: object is not a queue! ***\n";
                size_t wrcnt;
                _vfs_fwrite(msg, strlen(msg), &wrcnt, _process_get_stderr(GETPROCESS()));
                syscall_abort(rq);
        }

        SETERRNO(err);
}

/*==============================================================================
  End of file
==============================================================================*/
