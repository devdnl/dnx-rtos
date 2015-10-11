#!/usr/bin/env bash

# configuration
Makefile_name="Makefile.in"

# variables
Makefile_path=
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
# @brief  Creates empty Makefile
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
# @brief  Script main function
# @param  1st argument
# @return None
#-------------------------------------------------------------------------------
function main()
{
    check_args "$1"

    Makefile_path="$1/$Makefile_name"
    list=$(get_list "$1")

    create_makefile
}

main "$1"
