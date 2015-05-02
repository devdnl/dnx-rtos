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
#include "kernel/time.h"
#include "lib/cast.h"
#include "lib/unarg.h"

/*==============================================================================
  Local macros
==============================================================================*/
#define GETARG(type, var)       type var = va_arg(syscallrq->args, type)
#define GETRETURN(type, var)    type var = syscallrq->retptr
#define GETTASKHDL()            syscallrq->task
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
static void syscall_abort(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_exit(syscallrq_t *syscallrq, syscallres_t *syscallres);
static void syscall_system(syscallrq_t *syscallrq, syscallres_t *syscallres);

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
        [SYSCALL_ABORT            ] = syscall_abort,
        [SYSCALL_EXIT             ] = syscall_exit,
        [SYSCALL_SYSTEM           ] = syscall_system,
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
        _queue_create(1, sizeof(syscallrq_t), &call_request);
        _queue_create(1, sizeof(syscallres_t), &call_response);
        _process_create(&kworker, NULL, "kworker");
//        _thread_create(kworker, NULL, kworker_thread);
        _process_create(&initd, NULL, "initd");
}

//==============================================================================
/**
 * @brief  Function call selected syscall [USERLAND]
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
        syscallrq_t syscallrq;
        syscallrq.syscall  = syscall;
        syscallrq.task     = _builtinfunc(task_get_handle);
        syscallrq.retptr   = retptr;
        va_start(syscallrq.args, retptr);

        int result = _builtinfunc(queue_send, call_request, &syscallrq, MAX_DELAY_MS);
        if (result == ESUCC) {
                syscallres_t callres;
                result = _builtinfunc(queue_receive, call_response, &callres, MAX_DELAY_MS);
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
 * @brief  Main syscall process (master) [KERNELLAND]
 *
 * @param  argc         argument count
 * @param  argv         arguments
 *
 * @return Never exit (0)
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
 * @brief  This syscall mount selected file system to selected path
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
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
 * @brief  This syscall unmount selected file system
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
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
 * @brief  This syscall return information about selected file system
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_getmntentry(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_mknod(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_mkdir(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_mkfifo(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_opendir(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(const char *, path);
        DIR *dir = NULL;
        SETERRNO(_vfs_opendir(path, &dir));
        SETRETURN(DIR*, dir);
}

//==============================================================================
/**
 * @brief  This syscall close selected directory
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_closedir(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(DIR *, dir);
        SETERRNO(_vfs_closedir(dir));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall read selected directory
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_readdir(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_remove(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(const char *, path);
        SETERRNO(_vfs_remove(path));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall rename selected file
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_rename(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_chmod(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_chown(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_stat(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_fstat(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_statfs(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_fopen(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(const char *, path);
        GETARG(const char *, mode);
        FILE *file = NULL;
        SETERRNO(_vfs_fopen(path, mode, &file));
        SETRETURN(FILE*, file);
}

//==============================================================================
/**
 * @brief  This syscall close selected file
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_fclose(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(FILE *, file);
        SETERRNO(_vfs_fclose(file, false));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall write data to selected file
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_fwrite(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================;
static void syscall_fread(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================;
static void syscall_fseek(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_ftell(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_ioctl(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_fflush(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(FILE *, file);
        SETERRNO(_vfs_fflush(file));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall check that end-of-file occurred
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_feof(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_clearerr(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(FILE *, file);
        SETERRNO(_vfs_clearerr(file));
}

//==============================================================================
/**
 * @brief  This syscall return file error indicator
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_ferror(syscallrq_t *syscallrq, syscallres_t *syscallres)
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
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_sync(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        UNUSED_ARG2(syscallres, syscallrq);
        _vfs_sync();
}

//==============================================================================
/**
 * @brief  This syscall return current time value (UTC timestamp)
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_gettime(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        time_t time = -1;
        SETERRNO(_gettime(&time));
        SETRETURN(time_t, time);
}

//==============================================================================
/**
 * @brief  This syscall set current system time (UTC timestamp)
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_settime(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(time_t *, time);
        SETERRNO(_settime(time));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall initialize selected driver and create node
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
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
 * @brief  This syscall release selected driver
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_driverrelease(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(const char *, drv_name);
        SETERRNO(_driver_release(drv_name));
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall allocate memory for application
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
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
 * @brief  This syscall allocate memory for application and clear allocated block
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
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
 * @brief  This syscall free allocated memory by application
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_free(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(void *, mem);
        SETERRNO(_kfree(_MM_PROG, &mem));
}

//==============================================================================
/**
 * @brief  This syscall enable system log functionality in selected file
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
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
 * @brief  This syscall disable system log functionality
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_syslogdisable(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        SETERRNO(_printk_disable());
        SETRETURN(int, GETERRNO() == ESUCC ? 0 : -1);
}

//==============================================================================
/**
 * @brief  This syscall restart entire system
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_restart(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        UNUSED_ARG2(syscallrq, syscallres);

        _cpuctl_restart_system();
}

//==============================================================================
/**
 * @brief  This syscall check if kernel panic occurred in last session
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_kernelpanicdetect(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        UNUSED_ARG1(syscallres);
        GETARG(bool *, showmsg);
        SETRETURN(bool, _kernel_panic_detect(*showmsg));
}

//==============================================================================
/**
 * @brief  This syscall abort current process (caller) and set exit code as -1
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_abort(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

//==============================================================================
/**
 * @brief  This syscall close current process (caller)
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_exit(syscallrq_t *syscallrq, syscallres_t *syscallres)
{
        GETARG(int *, status);
        SETERRNO(_process_exit(GETTASKHDL(), *status));
}

//==============================================================================
/**
 * @brief  This syscall start shell application to run command line
 *
 * @param  syscallrq            syscall request
 * @param  syscallres           syscall response
 *
 * @return None
 */
//==============================================================================
static void syscall_system(syscallrq_t *syscallrq, syscallres_t *syscallres)
{

}

/*==============================================================================
  End of file
==============================================================================*/
