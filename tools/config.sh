#!/usr/bin/env bash

# configuration file
script="$1"
cwd=$(dirname "$1")

#-------------------------------------------------------------------------------
# Function check that value is an integer
#-------------------------------------------------------------------------------
is_integer() {
    printf "%d" $1 > /dev/null 2>&1
    return $?
}

#-------------------------------------------------------------------------------
# Command recognition functions
#-------------------------------------------------------------------------------
is_begin_cmd()
{
        if [[ "$1" =~ ^\s*\\begin\{.*\} ]]; then true; else false; fi
}

is_end_cmd()
{
        if [[ "$1" =~ ^\s*\\end\{.*\} ]]; then true; else false; fi
}

is_msg_cmd()
{
        if [[ "$1" =~ ^\s*\\msg\{.*\} ]]; then true; else false; fi
}

is_item_cmd()
{
        if [[ "$1" =~ ^\s*\\item\{.*\} ]]; then true; else false; fi
}

is_select_cmd()
{
        if [[ "$1" =~ ^\s*\\select\{.*\} ]]; then true; else false; fi
}

is_getint_cmd()
{
        if [[ "$1" =~ ^\s*\\getint\{.*\} ]]; then true; else false; fi
}

is_getuint_cmd()
{
        if [[ "$1" =~ ^\s*\\getuint\{.*\} ]]; then true; else false; fi
}

is_getstring_cmd()
{
        if [[ "$1" =~ ^\s*\\getstring\{.*\} ]]; then true; else false; fi
}

is_keysave_cmd()
{
        if [[ "$1" =~ ^\s*\\keysave\{.*\}\{.*\}\{.*\} ]]; then true; else false; fi
}

is_keycreate_cmd()
{
        if [[ "$1" =~ ^\s*\\keycreate\{.*\}\{.*\}\{.*\}\{.*\} ]]; then true; else false; fi
}

is_keydelete_cmd()
{
        if [[ "$1" =~ ^\s*\\keydelete\{.*\}\{.*\}\{.*\} ]]; then true; else false; fi
}

#-------------------------------------------------------------------------------
# Command argument return functions
#-------------------------------------------------------------------------------
get_begin_cmd_arg()
{
        echo $1 | sed 's/^\s*\\begin{\(.*\)}/\1/'
}

get_end_cmd_arg()
{
        echo $1 | sed 's/^\s*\\end{\(.*\)}/\1/'
}

get_msg_cmd_arg()
{
        echo $1 | sed 's/^\s*\\msg{\(.*\)}/\1/'
}

get_item_cmd_arg()
{
        echo $1 | sed 's/^\s*\\item{\(.*\)}{\(.*\)}/\1 \2/'
}

get_select_cmd_arg()
{
        echo $1 | sed 's/^\s*\\select{\(.*\)}/\1/'
}

get_getint_cmd_arg()
{
        echo $1 | sed 's/^\s*\\getint{\(.*\)}/\1/'
}

get_getuint_cmd_arg()
{
        echo $1 | sed 's/^\s*\\getuint{\(.*\)}/\1/'
}

get_getstring_cmd_arg()
{
        echo $1 | sed 's/^\s*\\getstring{\(.*\)}/\1/'
}

get_keysave_cmd_arg()
{
        echo $1 | sed 's/^\s*\\keysave{\(.*\)}{\(.*\)}{\(.*\)}{\(.*\)}/\1 \2 \3 \4/'
}

get_keycreate_cmd_arg()
{
        echo $1 | sed 's/^\s*\\keycreate{\(.*\)}{\(.*\)}{\(.*\)}{\(.*\)}/\1 \2 \3 \4/'
}

get_keydelete_cmd_arg()
{
        echo $1 | sed 's/^\s*\\keydelete{\(.*\)}{\(.*\)}{\(.*\)}/\1 \2 \3/'
}

#-------------------------------------------------------------------------------
# Error message
#-------------------------------------------------------------------------------
error()
{
        echo "$1:$2: error: $3"
        exit -1
}

#-------------------------------------------------------------------------------
# Warrning message
#-------------------------------------------------------------------------------
warrning()
{
        echo "$1:$2: warrning: $3"
}

#-------------------------------------------------------------------------------
# Function read configuration commands step by step
#-------------------------------------------------------------------------------
read_script()
{
        local script=$1 seek=0
        local begin=false items=() qtype

        while read -r line <&9; do
                seek=$[$seek+1]

                if [[ "$line" =~ ^\s*#.* ]] || [[ "$line" == "" ]]; then
                        continue

                elif $(is_begin_cmd $line); then
                        if $begin; then
                                error $script $seek "\begin{} in the \begin{}..\end{} block"
                                exit -1
                        else
                                echo "$seek: begin $(get_begin_cmd_arg "$line")"
                                begin=true
                        fi

                elif $(is_end_cmd $line); then
                        if $begin; then
                                echo "$seek: end: $(get_end_cmd_arg "$line")"
                                begin=false
                        else
                                error $script $seek "orphaned \end{} command"
                        fi

                elif $(is_msg_cmd $line) && $begin; then
                        echo "$seek: msg: $(get_msg_cmd_arg "$line")"

                elif $(is_item_cmd $line) && $begin; then
                        echo "$seek: item: $(get_item_cmd_arg "$line")"

                elif $(is_select_cmd $line) && $begin; then
                        echo "$seek: select: $(get_select_cmd_arg "$line")"

                elif $(is_getint_cmd $line) && $begin; then
                        echo "$seek: getint: $(get_getint_cmd_arg "$line")"

                elif $(is_getuint_cmd $line) && $begin; then
                        echo "$seek: getuint: $(get_getuint_cmd_arg "$line")"

                elif $(is_getstring_cmd $line) && $begin; then
                        echo "$seek: getstring: $(get_getstring_cmd_arg "$line")"

                elif $(is_keysave_cmd $line) && $begin; then
                        echo "$seek: keysave: $(get_keysave_cmd_arg "$line")"

                elif $(is_keycreate_cmd $line) && $begin; then
                        echo "$seek: keycreate: $(get_keycreate_cmd_arg "$line")"

                elif $(is_keydelete_cmd $line) && $begin; then
                        echo "$seek: keydelete: $(get_keydelete_cmd_arg "$line")"

                else
                        if $begin; then
                                error $script $seek "unknown command: $line"
                        else
                                error $script $seek "command outsite \begin{}..\end{}"
                        fi

                        exit -1
                fi
        done 9< $script
}

#-------------------------------------------------------------------------------
# Main function
#-------------------------------------------------------------------------------
main()
{
        # configuration file shall be defined
        if [ "$script" == "" ]; then
                echo "Usage: $0 [script]"
                exit -1
        fi

        read_script $script

        echo "Done"
}

main
