#!/usr/bin/env bash
#-------------------------------------------------------------------------------
#
# NOTE: script add programs and libraries to the system.
#
#-------------------------------------------------------------------------------

# configuration
Makefile_name="Makefile"
program_registration_file_name="program_registration.c"

# variables
program_list=

#-------------------------------------------------------------------------------
# @brief  Check incoming arguments
# @param  arguments
# @return None
#-------------------------------------------------------------------------------
function check_args()
{
    local args="$1"

    if [ "$1" == "" ] || [ "$2" == "" ]; then
        echo "Usage: $args <path_to_scan> <output_path>"
        exit 1
    fi
}

#-------------------------------------------------------------------------------
# @brief  Gets program list in the current directory. Folders are interpreted as
#         programs. Files are ignored.
# @param  path to scan
# @return program list
#-------------------------------------------------------------------------------
function get_program_list()
{
    echo $(ls -F "$1/programs" | grep -P '/|@' | grep -Pv '#' | sed 's/\///g' | sed 's/@//g')
}

#-------------------------------------------------------------------------------
# @brief  Gets program list in the current directory. Folders are interpreted as
#         libraries. Files are ignored.
# @param  path to scan
# @return program list
#-------------------------------------------------------------------------------
function get_library_list()
{
    echo $(ls -F "$1/libs" | grep -P '/|@' | grep -Pv '#' | sed 's/\///g' | sed 's/@//g')
}

#-------------------------------------------------------------------------------
# @brief  Creates empty Makefile
# @param  None
# @return None
#-------------------------------------------------------------------------------
function create_makefile_programs()
{
    echo '# Makefile for GNU make - file generated at build process'
    echo ''

    echo 'CSRC_PROGRAMS += ../../../$(GEN_PROG_DIR)/program_registration.c'
    echo 'HDRLOC_PROGRAMS += ../programs'
    for prog in $program_list; do
        echo '-include $(APP_PRG_LOC)/'"$prog"'/Makefile'
    done
}

#-------------------------------------------------------------------------------
# @brief  Creates empty Makefile
# @param  None
# @return None
#-------------------------------------------------------------------------------
function create_makefile_libs()
{
    echo ""
    echo '# libraries'
    echo 'HDRLOC_LIB += ../libs'

    for lib in $library_list; do
        echo '-include $(APP_LIB_LOC)/'"$lib"'/Makefile'
    done
}

#-------------------------------------------------------------------------------
# @brief  Creates empty program registration file
# @param  None
# @return None
#-------------------------------------------------------------------------------
function create_program_registration_file()
{
    echo '// file generated automatically at build process'
    echo ''
    echo '#include <stdint.h>'
    echo '#include <stddef.h>'
    echo ''

    echo '#define _PROGMAN_EXTERN_C extern'
    echo ''
    
    echo '#define _IMPORT_PROGRAM(_name_)\'
    echo '    _PROGMAN_EXTERN_C const size_t __builtin_app_##_name_##_gs__;\'
    echo '    _PROGMAN_EXTERN_C const size_t __builtin_app_##_name_##_ss__;\'
    echo '    _PROGMAN_EXTERN_C int __builtin_app_##_name_##_main(int, char**);'
    echo ''
    
    echo '#define _PROGRAM_ENTRY(_name_)\'
    echo '    {.name          = #_name_,\'
    echo '     .main          = __builtin_app_##_name_##_main,\'
    echo '     .globals_size  = &__builtin_app_##_name_##_gs__,\'
    echo '     .stack_depth   = &__builtin_app_##_name_##_ss__}'
    echo ''
    
    echo 'typedef struct {'
    echo '    const char     *name;'
    echo '    const size_t   *globals_size;'
    echo '    const size_t   *stack_depth;'
    echo '    int (*main)(int, char**);'
    echo '} _program_entry_t;'
    echo ''
    echo 'typedef struct {'
    echo '        uint32_t magic;'
    echo '        uint32_t number_of_programs;'
    echo '        const _program_entry_t *const program_entry;'
    echo '} _program_table_desc_t;'
    echo ''

    for prog in $program_list; do
        echo "_IMPORT_PROGRAM($prog);"
    done

    echo ''
    echo 'const _program_entry_t _program_table[] = {'
    for prog in $program_list; do
        echo "        _PROGRAM_ENTRY($prog),"
    done
    echo '};'
    echo ''

    echo '/*__attribute__((section (".program_table_descriptor")))*/ const _program_table_desc_t _program_table_desc = {'
    echo "        .magic = 0,"
    echo "        .number_of_programs = (sizeof(_program_table) / sizeof(_program_table[0])),"
    echo "        .program_entry = _program_table"
    echo "};"
}

#-------------------------------------------------------------------------------
# @brief  Script main function
# @param  1st argument
# @return None
#-------------------------------------------------------------------------------
function main()
{
    check_args "$1" "$2"

    program_list=$(get_program_list "$1")

    create_makefile_programs > "$2/$Makefile_name"
    create_program_registration_file > "$2/$program_registration_file_name"

    library_list=$(get_library_list "$1")
    create_makefile_libs >> "$2/$Makefile_name"
}

main "$1" "$2"
