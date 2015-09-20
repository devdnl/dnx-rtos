#!/usr/bin/env bash

# configuration
Makefile_name="Makefile"
program_registration_file_name="program_registration.c"

# variables
Makefile_path=
program_registration_path=
program_list=

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
function get_program_list()
{
    #echo $(ls -F "$1" | grep -P '/|@' | sed 's/\///g' | sed 's/@//g')
    echo -ne "initd\ntop\ndsh\nuname"
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

    echo 'CSRC_PROGRAMS += program_registration.c' >> "$Makefile_path"
    for prog in $program_list; do
        echo '-include $(APP_PRG_LOC)/'"$prog"'/Makefile' >> "$Makefile_path"
    done
}

#-------------------------------------------------------------------------------
# @brief  Creates empty program registration file
# @param  None
# @return None
#-------------------------------------------------------------------------------
function create_program_registration_file()
{
    echo '// file generated automatically at build process' > "$program_registration_path"
    echo '' >> "$program_registration_path"
    echo '#include <dnx/misc.h>' >> "$program_registration_path"
    echo '#include "kernel/kwrapper.h"' >> "$program_registration_path"
    echo '#include "kernel/process.h"' >> "$program_registration_path"
    echo '' >> "$program_registration_path"

    for prog in $program_list; do
        echo "_IMPORT_PROGRAM($prog);" >> "$program_registration_path"
    done

    echo '' >> "$program_registration_path"
    echo 'const struct _prog_data _prog_table[] = {' >> "$program_registration_path"
    for prog in $program_list; do
        echo "        _PROGRAM_CONFIG($prog)," >> "$program_registration_path"
    done
    echo '};' >> "$program_registration_path"

    echo '' >> "$program_registration_path"
    echo 'const int _prog_table_size = ARRAY_SIZE(_prog_table);' >> "$program_registration_path"
    echo '' >> "$program_registration_path"
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
    program_registration_path="$1/$program_registration_file_name"
    program_list=$(get_program_list "$1")

    create_makefile
    create_program_registration_file
}

main "$1"
