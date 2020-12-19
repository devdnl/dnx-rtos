#!/usr/bin/env bash

# configuration
Makefile_name="Makefile.in"
driver_registration_file_c="driver_registration.c"
driver_registration_file_h="driver_registration.h"
ioctl_groups_file_name="ioctl_groups.h"
ioctl_requests_file_name="ioctl_requests.h"

# variables
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
        echo "Usage: $(basename $0) <path_to_scan> <output_path>"
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
# @brief  Creates driver registration source file
# @param  None
# @return None
#-------------------------------------------------------------------------------
function create_driver_registration_file_c()
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

    echo -e $content
}

#-------------------------------------------------------------------------------
# @brief  Creates driver registration header file
# @param  None
# @return None
#-------------------------------------------------------------------------------
function create_driver_registration_file_h()
{
    local content=""
    local modenum="enum _MODID {\n"

    for module in $module_list; do
        archlist=$(get_list "$1/$module")

        enable="#if (__ENABLE_${module^^}__) && ("
        or=""
        for arch in $archlist; do enable=$enable"$or""defined(ARCH_$arch)"; or=" || "; done
        enable=$enable")\n"

        modenum="$modenum\t$enable"
        modenum="$modenum\t_MODID_${module^^},\n"
        modenum="$modenum\t#endif\n"
    done
    modenum=$modenum"\t_drvreg_number_of_modules\n};\n"

    content=$content'// file generated automatically at build process\n\n'
    content=$content'#include "config.h"\n\n'
    content=$content$modenum

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
    enum=$enum"\t_IO_GROUP_DEVICE,\n"
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
# @param  path to scan drivers
# @param  output path
#-------------------------------------------------------------------------------
function main()
{
    check_args "$1" "$2"

    ioctl_groups_file_path="$2/$ioctl_groups_file_name"
    ioctl_requests_file_path="$2/$ioctl_requests_file_name"
    module_list=$(get_list "$1")

    create_makefile > "$2/$Makefile_name"
    create_driver_registration_file_c $1 > "$2/$driver_registration_file_c"
    create_driver_registration_file_h $1 > "$2/$driver_registration_file_h"
    create_ioctl_groups_file > "$ioctl_groups_file_path"
    create_ioctl_requests_file $1 > "$ioctl_requests_file_path"
}

main "$1" "$2"
