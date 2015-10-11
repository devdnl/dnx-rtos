#!/usr/bin/env bash

# configuration
Makefile_name="Makefile.in"
Regfile_name="driver_registration.c"
Groupfile_name="ioctl_groups.h"

# variables
Makefile_path=
Regfile_path=
Groupfile_path=
list=

#-------------------------------------------------------------------------------
# @brief  Check incoming arguments
# @param  arguments
# @return None
#-------------------------------------------------------------------------------
function check_args()
{
    echo "1: $1"
    echo "2: $2"

    if [ "$1" == "" ] || [ "$2" == "" ]; then
        echo "Usage: $(basename $0) <path_to_scan> <path_to_ioctl_group_definitions>"
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

    for module in $list; do
        echo '-include $(SYS_DRV_LOC)/'$module'/Makefile' >> "$Makefile_path"
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
    echo '#include "drivers/drvctrl.h"' >> "$Regfile_path"
    echo '#include "drivers/driver.h"' >> "$Regfile_path"
    echo '#include "dnx/misc.h"' >> "$Regfile_path"
    echo '#include "config.h"' >> "$Regfile_path"
    echo '' >> "$Regfile_path"

    for module in $list; do
        arch=$(get_list "$1/$module")

        echo '#if (__ENABLE_'${module^^}'__)' >> "$Regfile_path"
        echo "  _IMPORT_MODULE_INTERFACE(${module^^});" >> "$Regfile_path"

        for arch in $arch; do
            echo "  #ifdef ARCH_$arch" >> "$Regfile_path"
            echo "    #include \"$arch/$module""_def.h\"" >> "$Regfile_path"
            echo "  #endif" >> "$Regfile_path"
        done

        echo '#endif' >> "$Regfile_path"
    done

    echo '' >> "$Regfile_path"
    echo 'const struct _module_entry _drvreg_module_table[] = {' >> "$Regfile_path"
    for module in $list; do
        echo '        #if (__ENABLE_'${module^^}'__)' >> "$Regfile_path"
        echo "                _MODULE_INTERFACE(${module^^})," >> "$Regfile_path"
        echo '        #endif' >> "$Regfile_path"
    done
    echo '};' >> "$Regfile_path"
    echo '' >> "$Regfile_path"

    echo 'const size_t _drvreg_number_of_modules = ARRAY_SIZE(_drvreg_module_table);' >> "$Regfile_path"
}

#-------------------------------------------------------------------------------
# @brief  Creates module enumerator file
# @param  None
# @return None
#-------------------------------------------------------------------------------
function create_module_enum()
{
    echo '// file generated automatically at build process' > "$Groupfile_path"
    echo 'enum _IO_GROUP {' >> "$Groupfile_path"
    echo '        _IO_GROUP_PIPE,       // built-in request' >> "$Groupfile_path"
    echo '        _IO_GROUP_STORAGE,    // built-in request' >> "$Groupfile_path"
    echo '        _IO_GROUP_VFS,        // built-in request' >> "$Groupfile_path"
    for module in $list; do
        echo "        _IO_GROUP_${module^^}," >> "$Groupfile_path"
    done
    echo '};' >> "$Groupfile_path"
}

#-------------------------------------------------------------------------------
# @brief  Script main function
# @param  1st argument
# @return None
#-------------------------------------------------------------------------------
function main()
{
    check_args "$1" "$2"

    Makefile_path="$1/$Makefile_name"
    Regfile_path="$1/$Regfile_name"
    Groupfile_path="$2/$Groupfile_name"
    list=$(get_list "$1")

    create_makefile
    create_registration_file $1
    create_module_enum
}

main "$1" "$2"
