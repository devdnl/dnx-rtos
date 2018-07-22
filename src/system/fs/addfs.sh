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
    echo '# Makefile for GNU make - file generated at build process'
    echo ''

    for fs in $list; do
        echo 'ifeq ($(__ENABLE_'${fs^^}'__), _YES_)'
        echo 'include $(SYS_FS_LOC)/'$fs'/Makefile'
        echo 'endif'
        echo ''
    done
}

#-------------------------------------------------------------------------------
# @brief  Creates registration file
# @param  None
# @return None
#-------------------------------------------------------------------------------
function create_registration_file()
{
    echo '// File generated automatically at build process'
    echo '// by script ./src/system/fs/addfs.sh'
    echo ''
    echo '#include "fs/fsctrl.h"'
    echo '#include "fs/fs.h"'
    echo '#include <dnx/misc.h>'
    echo ''

    for fs in $list; do
        echo '#if (__ENABLE_'${fs^^}'__)'
        echo '    extern API_FS_INIT('$fs', void**, const char*, const char*);'
        echo '    extern API_FS_RELEASE('$fs', void*);'
        echo '    extern API_FS_OPEN('$fs', void*, void**, fpos_t*, const char*, u32_t);'
        echo '    extern API_FS_CLOSE('$fs', void*, void*, bool);'
        echo '    extern API_FS_WRITE('$fs', void*, void*, const u8_t*, size_t, fpos_t*, size_t*, struct vfs_fattr);'
        echo '    extern API_FS_READ('$fs', void*, void*, u8_t*, size_t, fpos_t*, size_t*, struct vfs_fattr);'
        echo '    extern API_FS_IOCTL('$fs', void*, void*, int, void*);'
        echo '    extern API_FS_FLUSH('$fs', void*, void*);'
        echo '    extern API_FS_SYNC('$fs', void*);'
        echo '    extern API_FS_MKNOD('$fs', void*, const char*, const dev_t);'
        echo '    extern API_FS_OPENDIR('$fs', void*, const char*, struct vfs_dir*);'
        echo '    extern API_FS_CLOSEDIR('$fs', void*, struct vfs_dir*);'
        echo '    extern API_FS_READDIR('$fs', void*, struct vfs_dir*);'
        echo '  #if __OS_ENABLE_FSTAT__ == _YES_'
        echo '    extern API_FS_FSTAT('$fs', void*, void*, struct stat*);'
        echo '    extern API_FS_STAT('$fs', void*, const char*, struct stat*);'
        echo '  #endif'
        echo '  #if __OS_ENABLE_MKDIR__ == _YES_'
        echo '    extern API_FS_MKDIR('$fs', void*, const char*, mode_t);'
        echo '  #endif'
        echo '  #if __OS_ENABLE_MKFIFO__ == _YES_'
        echo '    extern API_FS_MKFIFO('$fs', void*, const char*, mode_t);'
        echo '  #endif'
        echo '  #if __OS_ENABLE_REMOVE__ == _YES_'
        echo '    extern API_FS_REMOVE('$fs', void*, const char*);'
        echo '  #endif'
        echo '  #if __OS_ENABLE_RENAME__ == _YES_'
        echo '    extern API_FS_RENAME('$fs', void*, const char*, const char*);'
        echo '  #endif'
        echo '  #if __OS_ENABLE_CHMOD__ == _YES_'
        echo '    extern API_FS_CHMOD('$fs', void*, const char*, mode_t);'
        echo '  #endif'
        echo '  #if __OS_ENABLE_CHOWN__ == _YES_'
        echo '    extern API_FS_CHOWN('$fs', void*, const char*, uid_t, gid_t);'
        echo '  #endif'
        echo '  #if __OS_ENABLE_STATFS__ == _YES_'
        echo '    extern API_FS_STATFS('$fs', void*, struct statfs*);'
        echo '  #endif'
        echo '#endif'
    done

    echo ''
    echo 'const struct _FS_entry _FS_table[] = {'
    for fs in $list; do
        echo '    #if (__ENABLE_'${fs^^}'__)'
        echo '    {.FS_name = "'$fs'",'
        echo '     .FS_if   = {.fs_init    = _'$fs'_init,'
        echo '                 .fs_release = _'$fs'_release,'
        echo '                 .fs_open    = _'$fs'_open,'
        echo '                 .fs_close   = _'$fs'_close,'
        echo '                 .fs_read    = _'$fs'_read,'
        echo '                 .fs_ioctl   = _'$fs'_ioctl,'
        echo '                 .fs_flush   = _'$fs'_flush,'
        echo '                 .fs_write   = _'$fs'_write,'
        echo '                 .fs_sync    = _'$fs'_sync,'
        echo '                 .fs_mknod   = _'$fs'_mknod,'
        echo '                 .fs_opendir = _'$fs'_opendir,'
        echo '                 .fs_closedir= _'$fs'_closedir,'
        echo '                 .fs_readdir = _'$fs'_readdir,'
        echo '               #if __OS_ENABLE_FSTAT__ == _YES_'
        echo '                 .fs_fstat   = _'$fs'_fstat,'
        echo '                 .fs_stat    = _'$fs'_stat,'
        echo '               #endif'
        echo '               #if __OS_ENABLE_STATFS__ == _YES_'
        echo '                 .fs_statfs  = _'$fs'_statfs,'
        echo '               #endif'
        echo '               #if __OS_ENABLE_CHMOD__ == _YES_'
        echo '                 .fs_chmod   = _'$fs'_chmod,'
        echo '               #endif'
        echo '               #if __OS_ENABLE_CHOWN__ == _YES_'
        echo '                 .fs_chown   = _'$fs'_chown,'
        echo '               #endif'
        echo '               #if __OS_ENABLE_MKDIR__ == _YES_'
        echo '                 .fs_mkdir   = _'$fs'_mkdir,'
        echo '               #endif'
        echo '               #if __OS_ENABLE_MKFIFO__ == _YES_'
        echo '                 .fs_mkfifo  = _'$fs'_mkfifo,'
        echo '               #endif'
        echo '               #if __OS_ENABLE_REMOVE__ == _YES_'
        echo '                 .fs_remove  = _'$fs'_remove,'
        echo '               #endif'
        echo '               #if __OS_ENABLE_RENAME__ == _YES_'
        echo '                 .fs_rename  = _'$fs'_rename,'
        echo '               #endif'
        echo '                 .fs_magic   = 0xD9EFD24F}},'
        echo '    #endif'
    done
    echo '};'
    echo ''

    echo 'const uint _FS_table_size = ARRAY_SIZE(_FS_table);'
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

    create_makefile > "$Makefile_path"
    create_registration_file > "$Regfile_path"
}

main "$1"
