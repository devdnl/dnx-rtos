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
    echo '// file generated automatically at build process' > "$Regfile_path"
    echo '' >> "$Regfile_path"
    echo '#include "fs/fsctrl.h"' >> "$Regfile_path"
    echo '#include "fs/fs.h"' >> "$Regfile_path"
    echo '#include <dnx/misc.h>' >> "$Regfile_path"
    echo '' >> "$Regfile_path"

    for fs in $list; do
        echo '#if (__ENABLE_'${fs^^}'__)' >> "$Regfile_path"
        echo "_IMPORT_FILE_SYSTEM($fs);" >> "$Regfile_path"
        echo '#endif' >> "$Regfile_path"
    done

    echo '' >> "$Regfile_path"
    echo 'const struct _FS_entry _FS_table[] = {' >> "$Regfile_path"
    for fs in $list; do
        echo '    #if (__ENABLE_'${fs^^}'__)' >> "$Regfile_path"
        echo "    _FILE_SYSTEM_INTERFACE($fs)," >> "$Regfile_path"
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
