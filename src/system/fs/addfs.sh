#!/usr/bin/env bash

# configuration
Makefile_name="Makefile.in"
Regfile_name="fs_registration.c"

# variables
Makefile_path=
Regfile_path=
list=

#-------------------------------------------------------------------------------
# @brief  Check incoming arguments
# @param  arguments
# @return None
#-------------------------------------------------------------------------------
function check_args()
{
    local args="$1"

    if [ "$args" == "" ]; then
        echo "Usage: $args <path_to_scan>"
        exit 1
    fi
}

#-------------------------------------------------------------------------------
# @brief  Gets program list in the current directory. Folder are interpreted as
#         programs. Files are ignored.
# @param  path to scan
# @return program list
#-------------------------------------------------------------------------------
function get_list()
{
    echo $(ls -F "$1" | grep -P '/|@' | sed 's/\///g' | sed 's/@//g')
}

#-------------------------------------------------------------------------------
# @brief  Creates Makefile
# @param  None
# @return None
#-------------------------------------------------------------------------------
function create_makefile()
{
    echo '# Makefile for GNU make - file generated at build process' > "$Makefile_path"
    echo '' >> "$Makefile_path"

    for fs in $list; do
        echo 'ifeq ($(__ENABLE_'${fs^^}'__), _YES_)' >> "$Makefile_path"
        echo 'include $(SYS_FS_LOC)/'$fs'/Makefile' >> "$Makefile_path"
        echo 'endif' >> "$Makefile_path"
        echo '' >> "$Makefile_path"
    done
}

#-------------------------------------------------------------------------------
# @brief  Creates registration file
# @param  None
# @return None
#-------------------------------------------------------------------------------
function create_registration_file()
{
    echo '// File generated automatically at build process' > "$Regfile_path"
    echo '// by script ./src/system/fs/addfs.sh' > "$Regfile_path"
    echo '' >> "$Regfile_path"
    echo '#include "fs/fsctrl.h"' >> "$Regfile_path"
    echo '#include "fs/fs.h"' >> "$Regfile_path"
    echo '#include <dnx/misc.h>' >> "$Regfile_path"
    echo '' >> "$Regfile_path"

    for fs in $list; do
        echo '#if (__ENABLE_'${fs^^}'__)' >> "$Regfile_path"
        echo '    extern API_FS_INIT('$fs', void**, const char*);' >> "$Regfile_path"
        echo '    extern API_FS_RELEASE('$fs', void*);' >> "$Regfile_path"
        echo '    extern API_FS_OPEN('$fs', void*, void**, fd_t*, fpos_t*, const char*, u32_t);' >> "$Regfile_path"
        echo '    extern API_FS_CLOSE('$fs', void*, void*, fd_t, bool);' >> "$Regfile_path"
        echo '    extern API_FS_WRITE('$fs', void*, void*, fd_t, const u8_t*, size_t, fpos_t*, size_t*, struct vfs_fattr);' >> "$Regfile_path"
        echo '    extern API_FS_READ('$fs', void*, void*, fd_t, u8_t*, size_t, fpos_t*, size_t*, struct vfs_fattr);' >> "$Regfile_path"
        echo '    extern API_FS_IOCTL('$fs', void*, void*, fd_t, int, void*);' >> "$Regfile_path"
        echo '    extern API_FS_FSTAT('$fs', void*, void*, fd_t, struct stat*);' >> "$Regfile_path"
        echo '    extern API_FS_FLUSH('$fs', void*, void*, fd_t);' >> "$Regfile_path"
        echo '    extern API_FS_SYNC('$fs', void*);' >> "$Regfile_path"
        echo '    extern API_FS_OPENDIR('$fs', void*, const char*, struct vfs_dir*);' >> "$Regfile_path"
        echo '    extern API_FS_MKNOD('$fs', void*, const char*, const dev_t);' >> "$Regfile_path"
        echo '  #if __OS_ENABLE_MKDIR__ == _YES_' >> "$Regfile_path"
        echo '    extern API_FS_STAT('$fs', void*, const char*, struct stat*);' >> "$Regfile_path"
        echo '  #endif' >> "$Regfile_path"
        echo '  #if __OS_ENABLE_MKDIR__ == _YES_' >> "$Regfile_path"
        echo '    extern API_FS_MKDIR('$fs', void*, const char*, mode_t);' >> "$Regfile_path"
        echo '  #endif' >> "$Regfile_path"
        echo '  #if __OS_ENABLE_MKFIFO__ == _YES_' >> "$Regfile_path"
        echo '    extern API_FS_MKFIFO('$fs', void*, const char*, mode_t);' >> "$Regfile_path"
        echo '  #endif' >> "$Regfile_path"
        echo '  #if __OS_ENABLE_REMOVE__ == _YES_' >> "$Regfile_path"
        echo '    extern API_FS_REMOVE('$fs', void*, const char*);' >> "$Regfile_path"
        echo '  #endif' >> "$Regfile_path"
        echo '  #if __OS_ENABLE_RENAME__ == _YES_' >> "$Regfile_path"
        echo '    extern API_FS_RENAME('$fs', void*, const char*, const char*);' >> "$Regfile_path"
        echo '  #endif' >> "$Regfile_path"
        echo '  #if __OS_ENABLE_CHMOD__ == _YES_' >> "$Regfile_path"
        echo '    extern API_FS_CHMOD('$fs', void*, const char*, mode_t);' >> "$Regfile_path"
        echo '  #endif' >> "$Regfile_path"
        echo '  #if __OS_ENABLE_CHOWN__ == _YES_' >> "$Regfile_path"
        echo '    extern API_FS_CHOWN('$fs', void*, const char*, uid_t, gid_t);' >> "$Regfile_path"
        echo '  #endif' >> "$Regfile_path"
        echo '  #if __OS_ENABLE_STATFS__ == _YES_' >> "$Regfile_path"
        echo '    extern API_FS_STATFS('$fs', void*, struct statfs*);' >> "$Regfile_path"
        echo '  #endif' >> "$Regfile_path"
        echo '#endif' >> "$Regfile_path"
    done

    echo '' >> "$Regfile_path"
    echo 'const struct _FS_entry _FS_table[] = {' >> "$Regfile_path"
    for fs in $list; do
        echo '    #if (__ENABLE_'${fs^^}'__)' >> "$Regfile_path"
        echo '    {.FS_name = "'$fs'",' >> "$Regfile_path"
        echo '     .FS_if   = {.fs_init    = _'$fs'_init,' >> "$Regfile_path"
        echo '                 .fs_release = _'$fs'_release,' >> "$Regfile_path"
        echo '                 .fs_open    = _'$fs'_open,' >> "$Regfile_path"
        echo '                 .fs_close   = _'$fs'_close,' >> "$Regfile_path"
        echo '                 .fs_opendir = _'$fs'_opendir,' >> "$Regfile_path"
        echo '                 .fs_read    = _'$fs'_read,' >> "$Regfile_path"
        echo '                 .fs_ioctl   = _'$fs'_ioctl,' >> "$Regfile_path"
        echo '                 .fs_fstat   = _'$fs'_fstat,' >> "$Regfile_path"
        echo '                 .fs_flush   = _'$fs'_flush,' >> "$Regfile_path"
        echo '                 .fs_write   = _'$fs'_write,' >> "$Regfile_path"
        echo '                 .fs_sync    = _'$fs'_sync,' >> "$Regfile_path"
        echo '                 .fs_mknod   = _'$fs'_mknod,' >> "$Regfile_path"
        echo '               #if __OS_ENABLE_FSTAT__ == _YES_' >> "$Regfile_path"
        echo '                 .fs_stat    = _'$fs'_stat,' >> "$Regfile_path"
        echo '               #endif' >> "$Regfile_path"
        echo '               #if __OS_ENABLE_STATFS__ == _YES_' >> "$Regfile_path"
        echo '                 .fs_statfs  = _'$fs'_statfs,' >> "$Regfile_path"
        echo '               #endif' >> "$Regfile_path"
        echo '               #if __OS_ENABLE_CHMOD__ == _YES_' >> "$Regfile_path"
        echo '                 .fs_chmod   = _'$fs'_chmod,' >> "$Regfile_path"
        echo '               #endif' >> "$Regfile_path"
        echo '               #if __OS_ENABLE_CHOWN__ == _YES_' >> "$Regfile_path"
        echo '                 .fs_chown   = _'$fs'_chown,' >> "$Regfile_path"
        echo '               #endif' >> "$Regfile_path"
        echo '               #if __OS_ENABLE_MKDIR__ == _YES_' >> "$Regfile_path"
        echo '                 .fs_mkdir   = _'$fs'_mkdir,' >> "$Regfile_path"
        echo '               #endif' >> "$Regfile_path"
        echo '               #if __OS_ENABLE_MKFIFO__ == _YES_' >> "$Regfile_path"
        echo '                 .fs_mkfifo  = _'$fs'_mkfifo,' >> "$Regfile_path"
        echo '               #endif' >> "$Regfile_path"
        echo '               #if __OS_ENABLE_REMOVE__ == _YES_' >> "$Regfile_path"
        echo '                 .fs_remove  = _'$fs'_remove,' >> "$Regfile_path"
        echo '               #endif' >> "$Regfile_path"
        echo '               #if __OS_ENABLE_RENAME__ == _YES_' >> "$Regfile_path"
        echo '                 .fs_rename  = _'$fs'_rename,' >> "$Regfile_path"
        echo '               #endif' >> "$Regfile_path"
        echo '                 .fs_magic   = 0xD9EFD24F}},' >> "$Regfile_path"
        echo '    #endif' >> "$Regfile_path"
    done
    echo '};' >> "$Regfile_path"
    echo '' >> "$Regfile_path"

    echo 'const uint _FS_table_size = ARRAY_SIZE(_FS_table);' >> "$Regfile_path"
}

#-------------------------------------------------------------------------------
# @brief  Script main function
# @param  1st argument
# @return None
#-------------------------------------------------------------------------------
function main()
{
    check_args "$1"

    Makefile_path="$1/$Makefile_name"
    Regfile_path="$1/$Regfile_name"
    list=$(get_list "$1")

    create_makefile
    create_registration_file
}

main "$1"
