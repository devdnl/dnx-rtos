#!/usr/bin/env bash

# configuration
Makefile_name="Makefile"

# variables
Makefile_path=
library_list=

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
function get_library_list()
{
    echo $(ls -F "$1" | grep / | sed 's/\///g')
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

    for lib in $library_list; do
        echo '-include $(LIB_LOC)/'"$lib"'/Makefile' >> "$Makefile_path"
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
    library_list=$(get_library_list "$1")

    create_makefile
}

main "$1"
