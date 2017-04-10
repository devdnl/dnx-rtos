#!/usr/bin/env bash

set -e

cd $(dirname $0)

FSNAME=$(echo "$1" | sed 's/[-+/*,."!\@#$%^&*()+-]/_/g')
FSNAME=${FSNAME,,}
BRIEF=$2
AUTHOR=$3
EMAIL=$4

if [[ "$*" < "2" ]]; then
    echo "Usage: $(basename $0) <filesystem-name> [brief] [author] [email]"
    exit 1
else
    cd ../src/system/fs
    mkdir $FSNAME
    cd $FSNAME


#-------------------------------------------------------------------------------
cat << EOF > $FSNAME.c
/*==============================================================================
File    $FSNAME.c

Author  $AUTHOR

Brief   $BRIEF

        Copyright (C) $(date "+%Y") $AUTHOR <$EMAIL>

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

==============================================================================*/

/*==============================================================================
  Include files
==============================================================================*/
#include "fs/fs.h"

/*==============================================================================
  Local macros
==============================================================================*/

/*==============================================================================
  Local object types
==============================================================================*/
typedef struct {
        // ...
} ${FSNAME}_t;

/*==============================================================================
  Local function prototypes
==============================================================================*/

/*==============================================================================
  Local objects
==============================================================================*/

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
 * @brief Initialize file system.
 *
 * @param[out]          **fs_handle             file system allocated memory
 * @param[in ]           *src_path              file source path
 * @param[in ]           *opts                  file system options (can be NULL)
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_INIT($FSNAME, void **fs_handle, const char *src_path, const char *opts)
{
        int err = sys_zalloc(sizeof(${FSNAME}_t), fs_handle);
        if (!err) {
                // ...
        }

        return err;
}

//==============================================================================
/**
 * @brief Release file system.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_RELEASE($FSNAME, void *fs_handle)
{
        ${FSNAME}_t *hdl = fs_handle;

        // ...

        sys_free(fs_handle);

        return ESUCC;
}

//==============================================================================
/**
 * @brief Open file.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *fhdl                   file extra data
 * @param[out]          *fpos                   file position
 * @param[in]           *path                   file path
 * @param[in]            flags                  file open flags
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_OPEN($FSNAME, void *fs_handle, void **fhdl, fpos_t *fpos, const char *path, u32_t flags)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Close file.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[in ]           force                  force close
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_CLOSE($FSNAME, void *fs_handle, void *fhdl, bool force)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Write data to the file.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[in ]          *src                    data source
 * @param[in ]           count                  number of bytes to write
 * @param[in ]          *fpos                   position in file
 * @param[out]          *wrcnt                  number of written bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_WRITE($FSNAME,
             void            *fs_handle,
             void            *fhdl,
             fd_t             fd,
             const u8_t      *src,
             size_t           count,
             fpos_t          *fpos,
             size_t          *wrcnt,
             struct vfs_fattr fattr)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Read data from file.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[out]          *dst                    data destination
 * @param[in ]           count                  number of bytes to read
 * @param[in ]          *fpos                   position in file
 * @param[out]          *rdcnt                  number of read bytes
 * @param[in ]           fattr                  file attributes
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_READ($FSNAME,
            void            *fs_handle,
            void            *fhdl,
            u8_t            *dst,
            size_t           count,
            fpos_t          *fpos,
            size_t          *rdcnt,
            struct vfs_fattr fattr)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief IO operations on files.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[in ]           request                request
 * @param[in ][out]     *arg                    request's argument
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_IOCTL($FSNAME, void *fs_handle, void *fhdl, int request, void *arg)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Flush file data.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_FLUSH($FSNAME, void *fs_handle, void *fhdl)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Return file status.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *fhdl                   file handle
 * @param[out]          *stat                   file status
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_FSTAT($FSNAME, void *fs_handle, void *fhdl, struct stat *stat)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Create directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created directory
 * @param[in ]           mode                   dir mode
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_MKDIR($FSNAME, void *fs_handle, const char *path, mode_t mode)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Create pipe.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created pipe
 * @param[in ]           mode                   pipe mode
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_MKFIFO($FSNAME, void *fs_handle, const char *path, mode_t mode)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Create node for driver file.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of created node
 * @param[in ]           dev                    driver number
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_MKNOD($FSNAME, void *fs_handle, const char *path, const dev_t dev)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Open directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of opened directory
 * @param[in ]          *dir                    directory object
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_OPENDIR($FSNAME, void *fs_handle, const char *path, DIR *dir)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Close directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *dir                    directory object
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_CLOSEDIR($FSNAME, void *fs_handle, DIR *dir)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Read directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in,out]       *dir                    directory object
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_READDIR($FSNAME, void *fs_handle, DIR *dir)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Remove file/directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   name of removed file/directory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_REMOVE($FSNAME, void *fs_handle, const char *path)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Rename file/directory.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *old_name               old object name
 * @param[in ]          *new_name               new object name
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_RENAME($FSNAME, void *fs_handle, const char *old_name, const char *new_name)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Change file's mode.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           mode                   new file mode
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_CHMOD($FSNAME, void *fs_handle, const char *path, mode_t mode)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Change file's owner and group.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[in ]           owner                  new file owner
 * @param[in ]           group                  new file group
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_CHOWN($FSNAME, void *fs_handle, const char *path, uid_t owner, gid_t group)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Return file/dir status.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[in ]          *path                   file path
 * @param[out]          *stat                   file status
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_STAT($FSNAME, void *fs_handle, const char *path, struct stat *stat)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

//==============================================================================
/**
 * @brief Return file system status.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 * @param[out]          *statfs                 file system status
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_STATFS($FSNAME, void *fs_handle, struct statfs *statfs)
{
        ${FSNAME}_t *hdl = fs_handle;

        statfs->f_bsize  = 0;
        statfs->f_blocks = 0;
        statfs->f_bfree  = 0;
        statfs->f_ffree  = 0;
        statfs->f_files  = 0;
        statfs->f_type   = SYS_FS_TYPE__SOLID;
        statfs->f_fsname = "$FSNAME";

        return ESUCC;
}

//==============================================================================
/**
 * @brief Synchronize all buffers to a medium.
 *
 * @param[in ]          *fs_handle              file system allocated memory
 *
 * @return One of errno value (errno.h).
 */
//==============================================================================
API_FS_SYNC($FSNAME, void *fs_handle)
{
        ${FSNAME}_t *hdl = fs_handle;

        int err = ESUCC;

        return err;
}

/*==============================================================================
  End of file
==============================================================================*/
EOF

#-------------------------------------------------------------------------------
cat << EOF > Makefile
# Makefile for GNU make
CSRC_CORE += fs/$FSNAME/$FSNAME.c
EOF

#-------------------------------------------------------------------------------
cd ../../../../config/filesystems

cat << EOF > ${FSNAME}_flags.h
/*==============================================================================
@file    ${FSNAME}_flags.h

@author  $AUTHOR

@brief   $BRIEF

@note    Copyright (C) $(date "+%Y") $AUTHOR <$EMAIL>

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


==============================================================================*/

/*
 * NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as prefix
 *       and suffix) are exported to the single configuration file
 *       (by using Configtool) when entire project configuration is exported.
 *       All other flag definitions and statements are ignored.
 */

#ifndef _${FSNAME^^}_FLAGS_H_
#define _${FSNAME^^}_FLAGS_H_

/*--
this:SetLayout("TitledGridBack", 2, "Home > File Systems > $FSNAME",
               function() this:LoadFile("filesystems/filesystems_flags.h") end)
++*/

/*--
this:AddWidget("Checkbox", "Example configuration")
--*/
#define __${FSNAME^^}_CFG_EXAMPLE__ _NO_

#endif /* _${FSNAME^^}_FLAGS_H_ */
/*==============================================================================
  End of file
==============================================================================*/
EOF

#-------------------------------------------------------------------------------
put_fs_enable() {
cat << EOF >> filesystems_flags.h
#/*--
# this:AddWidget("Checkbox", "Enable $FSNAME")
# this:SetToolTip("$BRIEF")
# this:AddExtraWidget("Hyperlink", "${FSNAME^^}_CONFIGURE", "Configure")
# this:SetEvent("clicked", "${FSNAME^^}_CONFIGURE", function() this:LoadFile("filesystems/${FSNAME}_flags.h") end)
#--*/
#include "../filesystems/${FSNAME}_flags.h"
#define __ENABLE_${FSNAME^^}__ _YES_
#/*
__ENABLE_${FSNAME^^}__=_YES_
#*/

EOF
}

    readarray file < filesystems_flags.h
    echo -n "" > filesystems_flags.h

    for line in "${file[@]}"; do
        if [[ "$line" =~ '#endif /* _FILE_SYSTEMS_FLAGS_H_ */' ]]; then
            put_fs_enable
        fi

        echo -n "$line" >> filesystems_flags.h
    done

    echo "Done"
fi
