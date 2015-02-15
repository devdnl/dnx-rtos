/*=========================================================================*//**
@file    vfs.c

@author  Daniel Zorychta

@brief   This file support virtual file system

@note    Copyright (C) 2012-2015 Daniel Zorychta <daniel.zorychta@gmail.com>

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
#include <dnx/thread.h>
#include <dnx/misc.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "core/vfs.h"
#include "core/llist.h"
#include "core/sysmoni.h"

/*==============================================================================
  Local symbolic constants/macros
==============================================================================*/
#define PATH_MAX_LEN             16384

/*==============================================================================
  Local types, enums definitions
==============================================================================*/
/** file flags */
typedef struct file_flags {
        bool                    eof:1;
        bool                    error:1;
        struct vfs_fattr        fattr;
} file_flags_t;

/** file type */
struct vfs_file
{
        void            *FS_hdl;
        stdret_t       (*f_close)(void *FS_hdl, void *extra_data, fd_t fd, bool force);
        ssize_t        (*f_write)(void *FS_hdl, void *extra_data, fd_t fd, const u8_t *src, size_t count, fpos_t *fpos, struct vfs_fattr fattr);
        ssize_t        (*f_read )(void *FS_hdl, void *extra_data, fd_t fd, u8_t *dst, size_t count, fpos_t *fpos, struct vfs_fattr fattr);
        int            (*f_ioctl)(void *FS_hdl, void *extra_data, fd_t fd, int iorq, void *args);
        stdret_t       (*f_stat )(void *FS_hdl, void *extra_data, fd_t fd, struct stat *stat);
        stdret_t       (*f_flush)(void *FS_hdl, void *extra_data, fd_t fd);
        void            *f_extra_data;
        struct vfs_file *this;
        fd_t             fd;
        fpos_t           f_lseek;
        file_flags_t     f_flag;
};

typedef struct FS_entry {
        const char              *mount_point;
        struct FS_entry         *parent_FS_ref;
        void                    *handle;
        struct vfs_FS_interface *interface;
        u8_t                     children_cnt;
} FS_entry_t;

enum path_correction {
        ADD_SLASH,
        SUB_SLASH,
        NO_SLASH_ACTION,
};

/*==============================================================================
  Local function prototypes
==============================================================================*/
static bool             can_be_root_FS          (const char *mount_point);
static FS_entry_t      *new_FS_entry            (FS_entry_t *parent_FS, const char *fs_mount_point, const char *fs_src_file, struct vfs_FS_interface *fs_interface);
static bool             delete_FS_entry         (FS_entry_t *this);
static int              fclose                  (FILE *file, bool force);
static bool             is_file_valid           (FILE *file);
static bool             is_dir_valid            (DIR *dir);
static int              increase_task_priority  (void);
static inline void      restore_priority        (int priority);
static vfs_open_flags_t file_mode_str_to_flags  (const char *str);
static FS_entry_t      *get_path_FS             (const char *path, size_t len, int *position);
static FS_entry_t      *get_path_base_FS        (const char *path, const char **extPath);
static char            *new_CWD_path            (const char *path, enum path_correction corr);

/*==============================================================================
  Local object definitions
==============================================================================*/
static _llist_t *vfs_mnt_list;
static mutex_t  *vfs_resource_mtx;

/*==============================================================================
  Function definitions
==============================================================================*/

//==============================================================================
/**
 * @brief Initialize VFS module
 *
 * @retval STD_RET_OK
 * @retval STD_RET_ERROR
 */
//==============================================================================
stdret_t _vfs_init(void)
{
        vfs_mnt_list     = _llist_new(sysm_sysmalloc, sysm_sysfree, NULL, NULL);
        vfs_resource_mtx = mutex_new(MUTEX_RECURSIVE);

        if (!vfs_mnt_list || !vfs_resource_mtx)
                return STD_RET_ERROR;
        else
                return STD_RET_OK;
}

//==============================================================================
/**
 * @brief Function mount file system in VFS
 *
 * @param[in] *src_path         path to source file when file system load data
 * @param[in] *mount_point      path when dir shall be mounted
 * @param[in] *fs_interface     pointer to description of mount interface
 *
 * @retval STD_RET_OK           mount success
 * @retval STD_RET_ERROR        mount error
 */
//==============================================================================
stdret_t vfs_mount(const char *src_path, const char *mount_point, struct vfs_FS_interface *fs_interface)
{
        if (  !mount_point
           || !src_path
           || !fs_interface
           || !fs_interface->fs_init
           || !fs_interface->fs_release
           || !fs_interface->fs_opendir) {

                errno = EINVAL;
                return STD_RET_ERROR;
        }

        stdret_t status = STD_RET_ERROR;

        // create new paths that are corrected by CWD
        char *cwd_mount_point = new_CWD_path(mount_point, ADD_SLASH);
        char *cwd_src_path    = new_CWD_path(src_path, SUB_SLASH);

        if (cwd_mount_point && cwd_src_path) {

                if (mutex_lock(vfs_resource_mtx, MAX_DELAY_MS)) {

                        FS_entry_t *mounted_fs = get_path_FS(cwd_mount_point, PATH_MAX_LEN, NULL);
                        const char *ext_path   = NULL;
                        FS_entry_t *base_fs    = get_path_base_FS(cwd_mount_point, &ext_path);

                        /*
                         * create new FS in existing DIR and FS, otherwise create new FS if
                         * first mount
                         */
                        FS_entry_t *new_FS = NULL;
                        if (can_be_root_FS(cwd_mount_point)) {
                                new_FS = new_FS_entry(base_fs, cwd_mount_point, cwd_src_path, fs_interface);

                        } else if (base_fs && !mounted_fs && ext_path) {
                                DIR dir;
                                if (base_fs->interface->fs_opendir(base_fs->handle, ext_path, &dir) == STD_RET_OK) {
                                        base_fs->children_cnt++;
                                        dir.f_closedir(dir.f_handle, &dir);

                                        new_FS = new_FS_entry(base_fs, cwd_mount_point, cwd_src_path, fs_interface);
                                }
                        } else {
                                errno = ENOENT;
                        }

                        sysm_sysfree(cwd_src_path);
                        cwd_src_path = NULL;

                        /*
                         * mount FS if created
                         */
                        if (new_FS) {
                                if (_llist_push_back(vfs_mnt_list, new_FS)) {
                                        status = STD_RET_OK;

                                } else {
                                        delete_FS_entry(new_FS);
                                        errno = ENOMEM;
                                }
                        }

                        mutex_unlock(vfs_resource_mtx);
                }
        }

        if (status == STD_RET_ERROR && cwd_mount_point)
                sysm_sysfree(cwd_mount_point);

        if (cwd_src_path)
                sysm_sysfree(cwd_src_path);

        return status;
}

//==============================================================================
/**
 * @brief Function unmount dir from file system
 *
 * @param[in] *path             dir path
 *
 * @retval STD_RET_OK           mount success
 * @retval STD_RET_ERROR        mount error
 */
//==============================================================================
stdret_t vfs_umount(const char *path)
{
        if (!path) {
                errno = EINVAL;
                return STD_RET_ERROR;
        }

        stdret_t status = STD_RET_ERROR;

        char *cwd_path = new_CWD_path(path, ADD_SLASH);
        if (cwd_path) {
                if (mutex_lock(vfs_resource_mtx, MAX_DELAY_MS)) {

                        int position;
                        FS_entry_t *mount_fs = get_path_FS(cwd_path, PATH_MAX_LEN, &position);

                        sysm_sysfree(cwd_path);

                        if (mount_fs) {
                                if (mount_fs->children_cnt == 0) {
                                        if (delete_FS_entry(mount_fs)) {
                                                _llist_take(vfs_mnt_list, position);
                                                status = STD_RET_OK;
                                        } else {
                                                errno = EBUSY;
                                        }
                                } else {
                                        errno = EBUSY;
                                }
                        } else {
                                errno = ENOENT;
                        }

                        mutex_unlock(vfs_resource_mtx);
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief Function return file system describe object
 * After operation object must be freed using free() function.
 *
 * @param item          n-item to read
 * @param mntent        pointer to mntent object
 *
 * @return 0 if success, 1 if all items read, -1 on error
 */
//==============================================================================
int vfs_getmntentry(int item, struct mntent *mntent)
{
        if (!mntent) {
                errno = EINVAL;
                return -1;
        }

        FS_entry_t *fs = NULL;
        if (mutex_lock(vfs_resource_mtx, MAX_DELAY_MS)) {
                fs = _llist_at(vfs_mnt_list, item);
                mutex_unlock(vfs_resource_mtx);
        }

        if (fs) {
                struct statfs stat_fs = {.f_fsname = NULL};

                if (fs->interface->fs_statfs) {
                        int priority = increase_task_priority();
                        fs->interface->fs_statfs(fs->handle, &stat_fs);
                        restore_priority(priority);
                } else {
                        return -1;
                }

                if (stat_fs.f_fsname) {
                        mntent->mnt_fsname = stat_fs.f_fsname;
                        mntent->mnt_dir    = fs->mount_point;
                        mntent->free       = (u64_t)stat_fs.f_bfree  * stat_fs.f_bsize;
                        mntent->total      = (u64_t)stat_fs.f_blocks * stat_fs.f_bsize;

                        return 0;
                } else {
                        return -1;
                }
        } else {
                return 1;
        }
}

//==============================================================================
/**
 * @brief Function create node for driver file
 *
 * @param[in] path              path when driver-file shall be created
 * @param[in] dev               device number
 *
 * @return zero on success. On error, -1 is returned
 */
//==============================================================================
int vfs_mknod(const char *path, dev_t dev)
{
        if (!path || dev < 0) {
                errno = EINVAL;
                return -1;
        }

        int status = -1;

        char *cwd_path = new_CWD_path(path, NO_SLASH_ACTION);
        if (cwd_path) {
                const char *external_path = NULL;
                FS_entry_t *fs = get_path_base_FS(cwd_path, &external_path);

                if (fs && fs->interface->fs_mknod) {
                        status = fs->interface->fs_mknod(fs->handle, external_path, dev) == STD_RET_OK ? 0 : -1;
                }

                sysm_sysfree(cwd_path);
        }

        return status;
}

//==============================================================================
/**
 * @brief Create directory
 *
 * @param[in] path              path to new directory
 * @param[in] mode              directory mode
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_mkdir(const char *path, mode_t mode)
{
        if (!path) {
                errno = EINVAL;
                return -1;
        }

        int status = -1;

        char *cwd_path = new_CWD_path(path, SUB_SLASH);
        if (cwd_path) {
                const char *external_path = NULL;
                FS_entry_t *fs = get_path_base_FS(cwd_path, &external_path);

                if (fs && fs->interface->fs_mkdir) {
                        int priority = increase_task_priority();
                        status = fs->interface->fs_mkdir(fs->handle, external_path, mode) == STD_RET_OK ? 0 : -1;
                        restore_priority(priority);
                }

                sysm_sysfree(cwd_path);
        }

        return status;
}

//==============================================================================
/**
 * @brief Create pipe
 *
 * @param[in] path              path to pipe
 * @param[in] mode              directory mode
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_mkfifo(const char *path, mode_t mode)
{
        if (!path) {
                errno = EINVAL;
                return -1;
        }

        int status = -1;

        char *cwd_path = new_CWD_path(path, NO_SLASH_ACTION);
        if (cwd_path) {
                const char *external_path = NULL;
                FS_entry_t *fs = get_path_base_FS(cwd_path, &external_path);

                if (fs && fs->interface->fs_mkfifo) {
                        int priority = increase_task_priority();
                        status = fs->interface->fs_mkfifo(fs->handle, external_path, mode) == STD_RET_OK ? 0 : -1;
                        restore_priority(priority);
                }

                sysm_sysfree(cwd_path);
        }

        return status;
}

//==============================================================================
/**
 * @brief Function open directory
 *
 * @param[in] *path                 directory path
 *
 * @return directory object
 */
//==============================================================================
DIR *vfs_opendir(const char *path)
{
        if (!path) {
                errno = EINVAL;
                return NULL;
        }

        DIR *dir = sysm_sysmalloc(sizeof(DIR));
        if (dir) {
                stdret_t status = STD_RET_ERROR;

                char *cwd_path = new_CWD_path(path, ADD_SLASH);
                if (cwd_path) {
                        const char *external_path = NULL;
                        FS_entry_t *fs = get_path_base_FS(cwd_path, &external_path);

                        if (fs && fs->interface->fs_opendir) {
                                dir->f_handle = fs->handle;

                                int priority = increase_task_priority();
                                status = fs->interface->fs_opendir(fs->handle, external_path, dir);
                                restore_priority(priority);
                        }

                        sysm_sysfree(cwd_path);
                }

                if (status == STD_RET_ERROR) {
                        sysm_sysfree(dir);
                        dir = NULL;
                } else {
                        dir->self = dir;
                }
        }

        return dir;
}

//==============================================================================
/**
 * @brief Function close opened directory
 *
 * @param[in] *dir                  directory object
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_closedir(DIR *dir)
{
        if (is_dir_valid(dir) && dir->f_closedir) {
                if (dir->f_closedir(dir->f_handle, dir) == STD_RET_OK) {
                        dir->self = NULL;
                        sysm_sysfree(dir);
                        return 0;
                }
        }

        errno = EINVAL;
        return -1;
}

//==============================================================================
/**
 * @brief Function read next item of opened directory
 *
 * @param[in] *dir                  directory object
 *
 * @return element attributes
 */
//==============================================================================
dirent_t vfs_readdir(DIR *dir)
{
        dirent_t direntry;
        direntry.name = NULL;
        direntry.size = 0;

        if (is_dir_valid(dir) && dir->f_readdir) {
                int priority = increase_task_priority();
                direntry = dir->f_readdir(dir->f_handle, dir);
                restore_priority(priority);
        } else {
                errno = EINVAL;
        }

        return direntry;
}

//==============================================================================
/**
 * @brief Remove file
 * Removes file or directory. Removes directory if is not a mount point.
 *
 * @param[in] *patch                localization of file/directory
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_remove(const char *path)
{
        if (!path) {
                errno = EINVAL;
                return -1;
        }

        int status = -1;

        char *cwd_path = new_CWD_path(path, ADD_SLASH);
        if (cwd_path) {
                const char *external_path = NULL;
                FS_entry_t *mount_fs      = NULL;
                FS_entry_t *base_fs       = NULL;
                if (mutex_lock(vfs_resource_mtx, MAX_DELAY_MS)) {
                        mount_fs = get_path_FS(cwd_path, PATH_MAX_LEN, NULL);
                        LAST_CHARACTER(cwd_path) = '\0';        // remove slash at the end
                        base_fs  = get_path_base_FS(cwd_path, &external_path);
                        mutex_unlock(vfs_resource_mtx);
                }

                if (base_fs && !mount_fs) {
                        if (base_fs->interface->fs_remove) {
                                status = base_fs->interface->fs_remove(base_fs->handle,
                                                                       external_path) == STD_RET_OK ? 0 : -1;
                        }
                } else if (mount_fs) {
                        errno = EBUSY;
                }

                sysm_sysfree(cwd_path);
        }

        return status;
}

//==============================================================================
/**
 * @brief Rename file name
 * The implementation of rename can move files only if external FS provide
 * functionality. Local VFS cannot do this. Cross FS move is also not possible.
 *
 * @param[in] *old_name                  old file name
 * @param[in] *new_name                  new file name
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_rename(const char *old_name, const char *new_name)
{
        if (!old_name || !new_name) {
                errno = EINVAL;
                return -1;
        }

        int   status       = -1;
        char *cwd_old_name = new_CWD_path(old_name, NO_SLASH_ACTION);
        char *cwd_new_name = new_CWD_path(new_name, NO_SLASH_ACTION);

        if (cwd_old_name && cwd_new_name) {
                FS_entry_t *old_fs          = NULL;
                FS_entry_t *new_fs          = NULL;
                const char *old_extern_path = NULL;
                const char *new_extern_path = NULL;

                if (mutex_lock(vfs_resource_mtx, MAX_DELAY_MS)) {
                        old_fs = get_path_base_FS(cwd_old_name, &old_extern_path);
                        new_fs = get_path_base_FS(cwd_new_name, &new_extern_path);
                        mutex_unlock(vfs_resource_mtx);
                }

                if (old_fs && new_fs && old_fs == new_fs && old_fs->interface->fs_rename) {
                        int priority = increase_task_priority();
                        status = old_fs->interface->fs_rename(old_fs->handle,
                                                              old_extern_path,
                                                              new_extern_path) == STD_RET_OK ? 0 : -1;
                        restore_priority(priority);
                }
        }

        if (cwd_old_name) {
                sysm_sysfree(cwd_old_name);
        }

        if (cwd_new_name) {
                sysm_sysfree(cwd_new_name);
        }

        return status;
}

//==============================================================================
/**
 * @brief Function change file mode
 *
 * @param[in] *path         file path
 * @param[in]  mode         file mode
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_chmod(const char *path, mode_t mode)
{
        if (!path) {
                errno = EINVAL;
                return -1;
        }

        int status = -1;

        char *cwd_path = new_CWD_path(path, NO_SLASH_ACTION);
        if (cwd_path) {
                const char *external_path = NULL;
                FS_entry_t *fs = get_path_base_FS(cwd_path, &external_path);

                if (fs && fs->interface->fs_chmod) {
                        int priority = increase_task_priority();
                        status = fs->interface->fs_chmod(fs->handle, external_path, mode) == STD_RET_OK ? 0 : -1;
                        restore_priority(priority);
                }

                sysm_sysfree(cwd_path);
        }

        return status;
}

//==============================================================================
/**
 * @brief Function change file owner and group
 *
 * @param[in] *path         file path
 * @param[in]  owner        file owner
 * @param[in]  group        file group
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_chown(const char *path, uid_t owner, gid_t group)
{
        if (!path) {
                errno = EINVAL;
                return -1;
        }

        int status = -1;

        char *cwd_path = new_CWD_path(path, NO_SLASH_ACTION);
        if (cwd_path) {
                const char *external_path = NULL;
                FS_entry_t *fs = get_path_base_FS(cwd_path, &external_path);

                if (fs && fs->interface->fs_chown) {
                        int priority = increase_task_priority();
                        status = fs->interface->fs_chown(fs->handle, external_path, owner, group) == STD_RET_OK ? 0 : -1;
                        restore_priority(priority);
                }

                sysm_sysfree(cwd_path);
        }

        return status;
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param[in]  *path            file/dir path
 * @param[out] *stat            pointer to structure
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_stat(const char *path, struct stat *stat)
{
        if (!path || !stat) {
                errno = EINVAL;
                return -1;
        }

        int status = -1;

        char *cwd_path = new_CWD_path(path, NO_SLASH_ACTION);
        if (cwd_path) {
                const char *external_path = NULL;
                FS_entry_t *fs = get_path_base_FS(cwd_path, &external_path);

                if (fs && fs->interface->fs_stat) {
                        int priority = increase_task_priority();
                        status = fs->interface->fs_stat(fs->handle, external_path, stat) == STD_RET_OK ? 0 : -1;
                        restore_priority(priority);
                }

                sysm_sysfree(cwd_path);
        }

        return status;
}

//==============================================================================
/**
 * @brief Function returns file system status
 *
 * @param[in]  *path            fs path
 * @param[out] *statfs          pointer to FS status structure
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_statfs(const char *path, struct statfs *statfs)
{
        if (!path || !statfs) {
                errno = EINVAL;
                return -1;
        }

        int status = -1;

        char *cwd_path = new_CWD_path(path, ADD_SLASH);
        if (cwd_path) {
                const char *external_path = NULL;
                FS_entry_t *fs = get_path_base_FS(cwd_path, &external_path);

                if (fs && fs->interface->fs_statfs) {
                        int priority = increase_task_priority();
                        status = fs->interface->fs_statfs(fs->handle, statfs) == STD_RET_OK ? 0 : -1;
                        restore_priority(priority);
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief Function open selected file
 *
 * @param[in] *name             file path
 * @param[in] *mode             file mode
 *
 * @retval NULL if file can't be created
 */
//==============================================================================
FILE *vfs_fopen(const char *path, const char *mode)
{
        if (!path || !mode) {
                errno = EINVAL;
                return NULL;
        }

        if (LAST_CHARACTER(path) == '/') { /* path is a directory */
                errno = EISDIR;
                return NULL;
        }

        vfs_open_flags_t flags = file_mode_str_to_flags(mode);
        if (flags == 0) {
                return NULL;
        }

        char *cwd_path = new_CWD_path(path, NO_SLASH_ACTION);
        FILE *file     = sysm_syscalloc(1, sizeof(FILE));

        if (cwd_path && file) {
                const char *external_path = NULL;
                FS_entry_t *fs = get_path_base_FS(cwd_path, &external_path);

                if (fs && fs->interface->fs_open) {
                        int priority = increase_task_priority();

                        if (fs->interface->fs_open(fs->handle,
                                                  &file->f_extra_data,
                                                  &file->fd,
                                                  &file->f_lseek,
                                                  external_path,
                                                  flags) == STD_RET_OK) {

                                file->FS_hdl  = fs->handle;
                                file->f_close = fs->interface->fs_close;
                                file->f_ioctl = fs->interface->fs_ioctl;
                                file->f_stat  = fs->interface->fs_fstat;
                                file->f_flush = fs->interface->fs_flush;

                                if (strncmp("r", mode, 2) != 0) {
                                        file->f_write = fs->interface->fs_write;
                                }

                                if (  strncmp("w", mode, 2) != 0
                                   && strncmp("a", mode, 2) != 0) {
                                        file->f_read = fs->interface->fs_read;
                                }

                                file->this = file;
                        }

                        restore_priority(priority);
                }
        }

        if (!cwd_path || !file || !file->this) {
                if (cwd_path)
                        sysm_sysfree(cwd_path);

                if (file)
                        sysm_sysfree(file);

                file = NULL;
        }

        return file;
}

//==============================================================================
/**
 * @brief Function close old stream and open new
 *
 * @param[in] *name             file path
 * @param[in] *mode             file mode
 * @param[in] *file             old stream
 *
 * @retval NULL if file can't be created
 */
//==============================================================================
FILE *vfs_freopen(const char *name, const char *mode, FILE *file)
{
        if (name || mode || file) {
                if (vfs_fclose(file) == STD_RET_OK) {
                        return vfs_fopen(name, mode);
                }
        } else {
                errno = EINVAL;
        }

        return NULL;
}

//==============================================================================
/**
 * @brief Function close opened file
 *
 * @param[in] *file             pinter to file
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_fclose(FILE *file)
{
        return fclose(file, false);
}

//==============================================================================
/**
 * @brief Function force close opened file (used by system to close all files)
 *
 * @param[in] *file             pinter to file
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_fclose_force(FILE *file)
{
        return fclose(file, true);
}

//==============================================================================
/**
 * @brief Function write data to file
 *
 * @param[in] *ptr              address to data (src)
 * @param[in]  size             item size
 * @param[in]  count            number of items
 * @param[in] *file             pointer to file object
 *
 * @return the number of items successfully written. If an error occurs, or the
 *         end-of-file is reached, the return value is a short item count (or 0).
 */
//==============================================================================
size_t vfs_fwrite(const void *ptr, size_t size, size_t count, FILE *file)
{
        ssize_t n = 0;

        if (ptr && size && count && is_file_valid(file)) {
                if (file->f_write) {
                        n = file->f_write(file->FS_hdl, file->f_extra_data, file->fd,
                                          ptr, size * count, &file->f_lseek, file->f_flag.fattr);

                        if (n < 0) {
                                file->f_flag.error = true;
                                n = 0;
                        } else if (n < (ssize_t)(size * count) && !file->f_flag.fattr.non_blocking_wr) {
                                file->f_flag.eof = true;
                        }

                        if (n >= 0) {
                                file->f_lseek += (u64_t)n;
                                n /= size;
                        }
                } else {
                        errno = ENOENT;
                }
        } else {
                errno = EINVAL;
        }

        return n;
}

//==============================================================================
/**
 * @brief Function read data from file
 *
 * @param[out] *ptr             address to data (dst)
 * @param[in]   size            item size
 * @param[in]   count           number of items
 * @param[in]  *file            pointer to file object
 *
 * @return the number of items successfully read. If an error occurs, or the
 *         end-of-file is reached, the return value is a short item count (or 0).
 */
//==============================================================================
size_t vfs_fread(void *ptr, size_t size, size_t count, FILE *file)
{
        ssize_t n = 0;

        if (ptr && size && count && is_file_valid(file)) {
                if (file->f_read) {
                        n = file->f_read(file->FS_hdl, file->f_extra_data, file->fd,
                                         ptr, size * count, &file->f_lseek, file->f_flag.fattr);

                        if (n < 0) {
                                file->f_flag.error = true;
                                n = 0;
                        } else if (n < (ssize_t)(size * count) && !file->f_flag.fattr.non_blocking_rd) {
                                file->f_flag.eof = true;
                        }

                        if (n >= 0) {
                                file->f_lseek += (u64_t)n;
                                n /= size;
                        }
                } else {
                        errno = ENOENT;
                }
        } else {
                errno = EINVAL;
        }

        return n;
}

//==============================================================================
/**
 * @brief Function set seek value
 *
 * @param[in] *file             file object
 * @param[in]  offset           seek value
 * @param[in]  mode             seek mode
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_fseek(FILE *file, i64_t offset, int mode)
{
        struct stat stat;

        if (!is_file_valid(file) || mode > VFS_SEEK_END) {
                return -1;
        }

        if (mode == VFS_SEEK_END) {
                stat.st_size = 0;
                if (vfs_fstat(file, &stat) != 0) {
                        return -1;
                }
        }

        switch (mode) {
        case VFS_SEEK_SET: file->f_lseek  = offset; break;
        case VFS_SEEK_CUR: file->f_lseek += offset; break;
        case VFS_SEEK_END: file->f_lseek  = stat.st_size + offset; break;
        default: return -1;
        }

        file->f_flag.eof   = false;
        file->f_flag.error = false;

        return 0;
}

//==============================================================================
/**
 * @brief Function returns seek value
 *
 * @param[in] *file             file object
 *
 * @return -1 if error, otherwise correct value
 */
//==============================================================================
i64_t vfs_ftell(FILE *file)
{
        if (is_file_valid(file)) {
                return file->f_lseek;
        } else {
                return -1;
        }
}

//==============================================================================
/**
 * @brief Function support not standard operations on devices
 *
 * @param[in]     *file         file
 * @param[in]      rq           request
 * @param[in,out] ...           additional function arguments
 *
 * @return 0 on success. On error, different from 0 is returned
 */
//==============================================================================
int vfs_ioctl(FILE *file, int rq, ...)
{
        va_list arg;
        va_start(arg, rq);
        int status = vfs_vioctl(file, rq, arg);
        va_end(arg);
        return status;
}

//==============================================================================
/**
 * @brief Function support not standard operations on devices
 *
 * @param[in]     *file         file
 * @param[in]      rq           request
 * @param[in,out]  arg          arguments
 *
 * @return 0 on success. On error, different from 0 is returned
 */
//==============================================================================
int vfs_vioctl(FILE *file, int rq, va_list arg)
{
        if (is_file_valid(file)) {
                switch (rq) {
                case IOCTL_VFS__NON_BLOCKING_RD_MODE: file->f_flag.fattr.non_blocking_rd = true;  return 0;
                case IOCTL_VFS__NON_BLOCKING_WR_MODE: file->f_flag.fattr.non_blocking_wr = true;  return 0;
                case IOCTL_VFS__DEFAULT_RD_MODE     : file->f_flag.fattr.non_blocking_rd = false; return 0;
                case IOCTL_VFS__DEFAULT_WR_MODE     : file->f_flag.fattr.non_blocking_wr = false; return 0;
                }

                return file->f_ioctl(file->FS_hdl, file->f_extra_data, file->fd, rq, va_arg(arg, void*));
        } else {
                return -1;
        }
}

//==============================================================================
/**
 * @brief Function returns file/dir status
 *
 * @param[in]  *path            file/dir path
 * @param[out] *stat            pointer to stat structure
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_fstat(FILE *file, struct stat *stat)
{
        if (!is_file_valid(file) || !stat || !file->f_stat) {
                return -1;
        }

        int priority = increase_task_priority();
        int status   = file->f_stat(file->FS_hdl, file->f_extra_data, file->fd, stat) == STD_RET_OK ? 0 : -1;
        restore_priority(priority);

        return status;
}

//==============================================================================
/**
 * @brief Function flush file data
 *
 * @param[in] *file     file to flush
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_fflush(FILE *file)
{
        if (!is_file_valid(file) && !file->f_flush) {
                return -1;
        }

        int priority = increase_task_priority();
        int status   = file->f_flush(file->FS_hdl, file->f_extra_data, file->fd) == STD_RET_OK ? 0 : -1;
        restore_priority(priority);

        return status;
}

//==============================================================================
/**
 * @brief Function check end of file
 *
 * @param[in] *file     file
 *
 * @return 0 if there is not a file end, otherwise greater than 0
 */
//==============================================================================
int vfs_feof(FILE *file)
{
        if (is_file_valid(file)) {
                return file->f_flag.eof ? 1 : 0;
        } else {
                return 1;
        }
}

//==============================================================================
/**
 * @brief Function clear file's error
 *
 * @param[in] *file     file
 */
//==============================================================================
void vfs_clearerr(FILE *file)
{
        if (is_file_valid(file)) {
                file->f_flag.eof   = false;
                file->f_flag.error = false;
        }
}

//==============================================================================
/**
 * @brief Function check that file has no errors
 *
 * @param[in] *file     file
 *
 * @return nonzero value if the file stream has errors occurred, 0 otherwise
 */
//==============================================================================
int vfs_ferror(FILE *file)
{
        if (is_file_valid(file)) {
                return file->f_flag.error ? 1 : 0;
        } else {
                return 1;
        }
}

//==============================================================================
/**
 * @brief Function rewind file
 *
 * @param[in] *file     file
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
int vfs_rewind(FILE *file)
{
        return vfs_fseek(file, 0, VFS_SEEK_SET);
}

//==============================================================================
/**
 * @brief Synchronize internal buffers of mounted file systems
 *
 * @param None
 *
 * @errors None
 *
 * @return None
 */
//==============================================================================
void vfs_sync(void)
{
        if (mutex_lock(vfs_resource_mtx, MAX_DELAY_MS)) {

                _llist_foreach(FS_entry_t*, fs, vfs_mnt_list) {
                        fs->interface->fs_sync(fs->handle);
                }

                mutex_unlock(vfs_resource_mtx);
        }
}

//==============================================================================
/**
 * @brief  Function check if selected mount point and current mount status
 *         allow to mount this file system as root.
 *
 * @param  mount_point          mount point path
 *
 * @return If file system can be mount as root file system then true is returned,
 *         otherwise false.
 */
//==============================================================================
static bool can_be_root_FS(const char *mount_point)
{
        return _llist_size(vfs_mnt_list) == 0 && strcmp(mount_point, "/") == 0;
}

//==============================================================================
/**
 * @brief  Create a new file system entry. Function initialize selected file system.
 *
 * @param  parent_FS            parent file system (can be NULL if none)
 * @param  fs_mount_point       file system mount point. This object must be permanent
 * @param  fs_src_file          file system source file
 * @param  fs_interface         file system interface (is copied to the object)
 *
 * @return On success pointer to file system entry is returned, otherwise NULL.
 */
//==============================================================================
static FS_entry_t *new_FS_entry(FS_entry_t              *parent_FS,
                                const char              *fs_mount_point,
                                const char              *fs_src_file,
                                struct vfs_FS_interface *fs_interface)
{
        FS_entry_t *new_FS = sysm_sysmalloc(sizeof(FS_entry_t));
        if (new_FS) {
                new_FS->interface = fs_interface;

                if (fs_interface->fs_init(&new_FS->handle, fs_src_file) == STD_RET_OK) {
                        new_FS->mount_point   = fs_mount_point;
                        new_FS->parent_FS_ref = parent_FS;
                        new_FS->children_cnt  = 0;
                } else {
                        sysm_sysfree(new_FS);
                        new_FS = NULL;
                }
        }

        return new_FS;
}

//==============================================================================
/**
 * @brief  Delete created file system entry. Function try release mounted file system.
 *
 * @param  this         file system entry object
 *
 * @return On success true is returned, otherwise false.
 */
//==============================================================================
static bool delete_FS_entry(FS_entry_t *this)
{
        bool status = false;

        if (this) {
                if (this->interface->fs_release(this->handle) == STD_RET_OK) {
                        if (this->parent_FS_ref && this->parent_FS_ref->children_cnt) {
                                this->parent_FS_ref->children_cnt--;
                        }

                        if (this->mount_point) {
                                sysm_sysfree(const_cast(char*, this->mount_point));
                        }

                        sysm_sysfree(this);
                        status = true;
                }
        }

        return status;
}

//==============================================================================
/**
 * @brief Generic file close
 *
 * @param[in] file              pinter to file
 * @param[in] force             force close
 *
 * @return 0 on success. On error, -1 is returned
 */
//==============================================================================
static int fclose(FILE *file, bool force)
{
        if (is_file_valid(file) && file->f_close) {
                if (file->f_close(file->FS_hdl, file->f_extra_data, file->fd, force) == STD_RET_OK) {
                        file->this = NULL;
                        sysm_sysfree(file);
                        return 0;
                }
        }

        return -1;
}

//==============================================================================
/**
 * @brief Check if file object is valid
 *
 * @errno EINVAL
 *
 * @param file  file object to examine
 *
 * @return true if object is valid, otherwise false
 */
//==============================================================================
static bool is_file_valid(FILE *file)
{
        if (file && file->this == file) {
                return true;
        } else {
                errno = EINVAL;
                return false;
        }
}

//==============================================================================
/**
 * @brief Check if dir object is valid
 *
 * @errno EINVAL
 *
 * @param file  file object to examine
 *
 * @return true if object is valid, otherwise false
 */
//==============================================================================
static bool is_dir_valid(DIR *dir)
{
        if (dir && dir->self == dir) {
                return true;
        } else {
                errno = EINVAL;
                return false;
        }
}

//==============================================================================
/**
 * @brief Function increase task priority and return original priority value
 *
 * @return original task priority
 */
//==============================================================================
static int increase_task_priority(void)
{
        int priority = task_get_priority();

        if (priority < HIGHEST_PRIORITY) {
                task_set_priority(priority + 1);
        }

        return priority;
}

//==============================================================================
/**
 * @brief Function restore original priority
 *
 * @param priority
 */
//==============================================================================
static inline void restore_priority(int priority)
{
        task_set_priority(priority);
}

//==============================================================================
/**
 * @brief Function convert file open mode string to flags
 *        Function set errno: EINVAL
 *
 * @param[in] *str      file open mode string
 *
 * @return file open flags, 0 if error
 */
//==============================================================================
static vfs_open_flags_t file_mode_str_to_flags(const char *str)
{
        if (strcmp("r", str) == 0 || strcmp("rb", str) == 0) {
                return (O_RDONLY);
        }

        if (strcmp("r+", str) == 0 || strcmp("rb+", str) == 0) {
                return (O_RDWR);
        }

        if (strcmp("w", str) == 0 || strcmp("wb", str) == 0) {
                return (O_WRONLY | O_CREATE);
        }

        if (strcmp("w+", str) == 0 || strcmp("wb+", str) == 0) {
                return (O_RDWR | O_CREATE);
        }

        if (strcmp("a", str) == 0 || strcmp("ab", str) == 0) {
                return (O_WRONLY | O_CREATE | O_APPEND);
        }

        if (strcmp("a+", str) == 0 || strcmp("ab+", str) == 0) {
                return (O_RDWR | O_CREATE | O_APPEND);
        }

        errno = EINVAL;

        return 0;
}

//==============================================================================
/**
 * @brief Function return file system entry of selected path
 *        Function set errno: ENXIO (if no entry)
 *
 * @param[in]  path             path to FS
 * @param[in]  len              path length
 * @param[out] position         object position in list
 *
 * @return On success pointer to file system entry is returned, otherwise NULL.
 */
//==============================================================================
static FS_entry_t *get_path_FS(const char *path, size_t len, int *position)
{
        int pos = 0;

        _llist_foreach(FS_entry_t*, entry, vfs_mnt_list) {
                if (strncmp(path, entry->mount_point, len) == 0) {
                        if (position) {
                                *position = pos;
                        }

                        return entry;
                }

                pos++;
        }

        return NULL;
}

//==============================================================================
/**
 * @brief Function returned the base file system of selected path. The external
 *        path is passed by pointer ext_path.
 *        Function is thread safe.
 *        Function set errno: ENOENT
 *
 * @param[in]  path           path to FS
 * @param[out] ext_path       pointer to external part of path
 *
 * @return On success base file system entry is returned, otherwise NULL.
 */
//==============================================================================
static FS_entry_t *get_path_base_FS(const char *path, const char **ext_path)
{
        const char *path_tail = path + strlen(path);

        if (*(path_tail - 1) == '/') {
                path_tail--;
        }

        FS_entry_t *FS_entry = NULL;
        if (mutex_lock(vfs_resource_mtx, MAX_DELAY_MS)) {

                while (path_tail >= path) {
                        FS_entry_t *entry = get_path_FS(path, path_tail - path + 1, NULL);
                        if (entry) {
                                FS_entry = entry;
                                break;
                        } else {
                                while (*(--path_tail) != '/' && path_tail >= path);
                        }
                }

                mutex_unlock(vfs_resource_mtx);
        }

        if (FS_entry && ext_path) {
                *ext_path = path_tail;
        }

        if (!FS_entry) {
                errno = ENOENT;
        }

        return FS_entry;
}

//==============================================================================
/**
 * @brief Function create new path with slash and cwd correction
 *        Function set errno: ENOMEM
 *
 * @param[in] *path             path to correct
 * @param[in]  corr             path correction kind
 *
 * @return pointer to new path
 */
//==============================================================================
static char *new_CWD_path(const char *path, enum path_correction corr)
{
        char       *new_path;
        uint        new_path_len = strlen(path);
        const char *cwd;
        uint        cwd_len = 0;

        /* correct ending slash */
        if (corr == SUB_SLASH && LAST_CHARACTER(path) == '/') {
                new_path_len--;
        } else if (corr == ADD_SLASH && LAST_CHARACTER(path) != '/') {
                new_path_len++;
        }

        /* correct cwd */
        if (FIRST_CHARACTER(path) != '/') {
                cwd = _task_get_data()->f_cwd;
                if (cwd) {
                        cwd_len       = strlen(cwd);
                        new_path_len += cwd_len;

                        if (LAST_CHARACTER(cwd) != '/' && cwd_len) {
                                new_path_len++;
                                cwd_len++;
                        }
                }
        }

        new_path = sysm_syscalloc(new_path_len + 1, sizeof(char));
        if (new_path) {
                if (cwd_len && cwd) {
                        strcpy(new_path, cwd);

                        if (LAST_CHARACTER(cwd) != '/') {
                                strcat(new_path, "/");
                        }
                }

                if (corr == SUB_SLASH) {
                        strncat(new_path, path, new_path_len - cwd_len);
                } else if (corr == ADD_SLASH) {
                        strcat(new_path, path);

                        if (LAST_CHARACTER(new_path) != '/') {
                                strcat(new_path, "/");
                        }
                } else {
                        strcat(new_path, path);
                }
        } else {
                errno = ENOMEM;
        }

        return new_path;
}

/*==============================================================================
  End of file
==============================================================================*/
