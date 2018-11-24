#!/usr/bin/env bash

set -e

file=""
error=false

STR_GLOBAL_IN_SECTION="Global variables shall be placed in GLOBAL_VARIABLES_SECTION only"

TYPE_STRUCT="struct|union[[:blank:]]+"
TYPE_STATIC="static[[:blank:]]+"
TYPE_DEF_EMPTY="[_[:alnum:]]+[*]*[[:blank:]]+[*]*[_[:alnum:]]+[[:blank:]]*\;"
TYPE_DEF_INIT="[_[:alnum:]]+[*]*[[:blank:]]+[*]*[_[:alnum:]]+[[:blank:]]*=[[:blank:]]*.*"

REGEXP_FUNC_BEGIN="^[[:blank:]]*(static)*[[:blank:]]*[_[:alnum:]]+[*]*[[:blank:]]+[*]*[_[:alnum:]]+[[:blank:]]*\(.*$"
REGEXP_FUNC_MAIN="^[[:blank:]]*int_main\(.*$"
REGEXP_TYPEDEF="^[[:blank:]]*typedef[[:blank:]]+[_[:alnum:]]+[[:blank:]]*[_[:alnum:]]*[[:blank:]]*[\{]*$"
REGEXP_STRUCT="^[[:blank:]]*${TYPE_STRUCT}[_[:alnum:]]+[[:blank:]]*[\{]*$"
REGEXP_GLOBAL_VAR="^[[:blank:]]*GLOBAL_VARIABLES_SECTION[[:blank:]]*[\{]*.*$"
REGEXP_COMMSINGLE="^[[:blank:]]*//.*$"
REGEXP_COMMMULTI="^[[:blank:]]*\/\*.*$"
REGEXP_INCLUDE="^[[:blank:]]*#[[:blank:]]*include.*$"
REGEXP_CONST_VAR="^[[:blank:]]*.*const.*$"

#===============================================================================
#
# Function print error message
#
# $1: error message
#
#===============================================================================
error_msg() {
    echo "$file:$line_ctr:1: error: $1"
    error=true
}

#===============================================================================
#
# Function skip C/C++ block
#
#===============================================================================
skip_block() {
    par=0

    if [[ "$line" =~ .*\{.*$ ]]; then
        par=1
    fi

    while IFS='' read -r line || [[ -n "$line" ]]; do
        line_ctr=$(($line_ctr + 1))

        if [[ $line =~ ^[[:blank:]]*.*[[:blank:]]*\{.*$ ]]; then
            par=$(($par + 1))
        fi

        if [[ $line =~ ^[[:blank:]]*\}.*$ ]]; then
            par=$(($par - 1))
        fi

        if (( $par == 0 )); then
            break
        fi
    done
}

#===============================================================================
#
# Function skip C/C++ function
#
#===============================================================================
skip_function() {

    # find function single line prototype
    if [[ "$line" =~ ^.*\)[[:blank:]]*\;$ ]]; then
        return
    fi

    # check single line function
    if [[ "$line" =~ ^.*\)[\{]*$ ]]; then
        true
    else
        # find function declaration close
        while IFS='' read -r line || [[ -n "$line" ]]; do

            line_ctr=$(($line_ctr + 1))

            if [[ "$line" =~ ^.*\)[[:blank:]]*[\{]*[[:blank:]]*$ ]]; then
                break
            elif [[ "$line" =~ ^.*\)[[:blank:]]*\;$ ]]; then
                break
            fi
        done
    fi

    # skip function block
    skip_block
}

#===============================================================================
#
# Function skip multiline comment
#
#===============================================================================
skip_comment() {
    if [[ "$line" =~ ^.*\*\/$ ]]; then
        return
    fi

    while IFS='' read -r line || [[ -n "$line" ]]; do

        line_ctr=$(($line_ctr + 1))

        if [[ "$line" =~ ^.*\*\/ ]]; then
            return
        fi
    done
}

#===============================================================================
#
# Main function
#
#===============================================================================
main() {
    if [ "$1" == "" ]; then
        echo "Usage: $0 <dir-to-check>"
        exit 1
    fi

    for file in "$@"; do

        echo "Verifing: ${file}..."

        global_section_found=0
        line_ctr=0
        main_func=false

        while IFS='' read -r line || [[ -n "$line" ]]; do

            line_ctr=$(($line_ctr + 1))

            if [[ "$line" =~ ^[[:blank:]]*GLOBAL_VARIABLES_SECTION.* ]]; then
                global_section_found=$(($global_section_found + 1))
            fi

            if [[ "$line" =~ $REGEXP_FUNC_BEGIN ]] \
            || [[ "$line" =~ $REGEXP_FUNC_MAIN ]]; then
                skip_function

            elif [[ "$line" =~ $REGEXP_TYPEDEF ]] \
              || [[ "$line" =~ $REGEXP_GLOBAL_VAR ]]; then
                skip_block

            elif [[ "$line" =~ $REGEXP_STRUCT ]] ; then
                skip_block
                if [[ "$line" =~ ^.*[_*[:alnum:]]+[[:blank:]]*(=[[:blank:]]*[\{]*)|\; ]]; then
                    error_msg "$STR_GLOBAL_IN_SECTION"
                fi

            elif [[ "$line" =~ $REGEXP_COMMSINGLE ]]; then
                true

            elif [[ "$line" =~ $REGEXP_COMMMULTI ]]; then
                skip_comment

            elif [[ "$line" =~ $REGEXP_INCLUDE ]]; then
                true

            elif [[ "$line" =~ $REGEXP_CONST_VAR ]]; then
                true

            else
                if [[ "$line" =~ ^[[:blank:]]*$TYPE_DEF_EMPTY ]] \
                || [[ "$line" =~ ^[[:blank:]]*$TYPE_DEF_INIT ]] \
                || [[ "$line" =~ ^[[:blank:]]*$TYPE_STATIC$TYPE_DEF_EMPTY ]] \
                || [[ "$line" =~ ^[[:blank:]]*$TYPE_STATIC$TYPE_DEF_INIT ]] \
                || [[ "$line" =~ ^[[:blank:]]*$TYPE_STRUCT$TYPE_DEF_EMPTY ]] \
                || [[ "$line" =~ ^[[:blank:]]*$TYPE_STRUCT$TYPE_DEF_INIT ]]
                then
                    error_msg "$STR_GLOBAL_IN_SECTION"
                fi

            fi
        done < "$file"

        line_ctr=0

        if [ $global_section_found == 0 ]; then
            #error_msg "GLOBAL_VARIABLES_SECTION is not defined."
            #exit 1
            true

        elif (( $global_section_found > 1 )); then
            error_msg "Defined too many GLOBAL_VARIABLES_SECTION (found: $global_section_found)"
        fi

        if [ $error == true ]; then
            exit 1
        fi
    done
}

main $*
