#!/usr/bin/env bash

# configuration
Makefile_name="Makefile.in"
driver_registration_file_name="driver_registration.c"
ioctl_groups_file_name="ioctl_groups.h"
ioctl_requests_file_name="ioctl_requests.h"

# variables
Makefile_path=
driver_registration_file_path=
ioctl_groups_file_path=
ioctl_requests_file_path=
module_list=

#-------------------------------------------------------------------------------
# @brief  Check incoming arguments
# @param  arguments
# @return None
#-------------------------------------------------------------------------------
function check_args()
{
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
    echo $(ls -F "$1" --ignore=class --ignore=~* | grep -P '/|@' | sed 's/\///g' | sed 's/@//g')
}

#-------------------------------------------------------------------------------
# @brief  Creates Makefile
# @param  None
# @return None
#-------------------------------------------------------------------------------
function create_makefile()
{
    local content='# Makefile for GNU make - file generated at build process\n\n'
    for module in $module_list; do
        content="$content-include "'$(SYS_DRV_LOC)'"/$module/Makefile\n"
    done

    echo -e $content
}

#-------------------------------------------------------------------------------
# @brief  Creates registration file
# @param  None
# @return None
#-------------------------------------------------------------------------------
function create_driver_registration_file()
{
    local content=""
    local ifimport=""
    local modtable="const struct _module_entry _drvreg_module_table[] = {\n"
    for module in $module_list; do
        archlist=$(get_list "$1/$module")

        enable="#if (__ENABLE_${module^^}__) && ("
        or=""
        for arch in $archlist; do enable=$enable"$or""defined(ARCH_$arch)"; or=" || "; done
        enable=$enable")\n"

        ifimport="$ifimport$enable"
        ifimport="$ifimport\t_IMPORT_MODULE_INTERFACE(${module^^});\n"
        ifimport="$ifimport#endif\n"

        modtable="$modtable\t$enable"
        modtable="$modtable\t\t_MODULE_INTERFACE(${module^^}),\n"
        modtable="$modtable\t#endif\n"
    done
    modtable=$modtable"};\n"

    content=$content'// file generated automatically at build process\n\n'
    content=$content'#include "drivers/drvctrl.h"\n'
    content=$content'#include "drivers/driver.h"\n'
    content=$content'#include "dnx/misc.h"\n'
    content=$content'#include "config.h"\n\n'
    content=$content$ifimport
    content=$content$modtable
    content=$content'const size_t _drvreg_number_of_modules = ARRAY_SIZE(_drvreg_module_table);'

    echo -e $content
}

#-------------------------------------------------------------------------------
# @brief  Creates module enumerator file
# @param  None
# @return None
#-------------------------------------------------------------------------------
function create_ioctl_groups_file()
{
    enum="// file generated automatically at build process\n"
    enum=$enum"enum _IO_GROUP {\n"
    enum=$enum"\t_IO_GROUP_PIPE,\n"
    enum=$enum"\t_IO_GROUP_STORAGE,\n"
    enum=$enum"\t_IO_GROUP_VFS,\n"
    for module in $module_list; do enum=$enum"\t_IO_GROUP_${module^^},\n"; done
    enum=$enum"};"

    echo -e $enum
    
    # prevents compilation of all related objects at every build
    /bin/touch --reference=./Makefile "$ioctl_groups_file_path"
}

#-------------------------------------------------------------------------------
# @brief  Creates module enumerator file
# @param  None
# @return None
#-------------------------------------------------------------------------------
function create_ioctl_requests_file()
{
    content="// file generated automatically at build process\n"
    for module in $module_list; do
        content="$content#include \""$module"_ioctl.h\"\n"
    done

    echo -ne $content
    
    # prevents compilation of all related objects at every build
    /bin/touch --reference=./Makefile "$ioctl_requests_file_path"
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
    driver_registration_file_path="$1/$driver_registration_file_name"
    ioctl_groups_file_path="$2/$ioctl_groups_file_name"
    ioctl_requests_file_path="$2/$ioctl_requests_file_name"
    module_list=$(get_list "$1")

    create_makefile > "$Makefile_path"
    create_driver_registration_file $1 > "$driver_registration_file_path"
    create_ioctl_groups_file > "$ioctl_groups_file_path"
    create_ioctl_requests_file $1 > "$ioctl_requests_file_path"
}

main "$1" "$2"
