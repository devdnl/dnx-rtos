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
        if [[ "$1" =~ ^\s*\\begin\s*\{.*\} ]]; then true; else false; fi
}

is_end_cmd()
{
        if [[ "$1" =~ ^\s*\\end\s*\{.*\} ]]; then true; else false; fi
}

is_msg_cmd()
{
        if [[ "$1" =~ ^\s*\\msg\s*\{.*\} ]]; then true; else false; fi
}

is_item_cmd()
{
        if [[ "$1" =~ ^\s*\\item\s*\{.*\} ]]; then true; else false; fi
}

is_select_cmd()
{
        if [[ "$1" =~ ^\s*\\select\s*\{.*\} ]]; then true; else false; fi
}

is_getint_cmd()
{
        if [[ "$1" =~ ^\s*\\getint\s*\{.*\} ]]; then true; else false; fi
}

is_getuint_cmd()
{
        if [[ "$1" =~ ^\s*\\getuint\s*\{.*\} ]]; then true; else false; fi
}

is_getstring_cmd()
{
        if [[ "$1" =~ ^\s*\\getstring\s*\{.*\} ]]; then true; else false; fi
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

#-------------------------------------------------------------------------------
# Function read configuration commands step by step
#-------------------------------------------------------------------------------
read_script()
{
        local script=$1

        while read -r line <&9; do
                if [[ "$line" =~ ^\s*#.* ]] || [[ "$line" == "" ]]; then
                        continue
                elif $(is_begin_cmd $line); then
                        echo "begin $(get_begin_cmd_arg "$line")"

                elif $(is_end_cmd $line); then
                        echo "end: $(get_end_cmd_arg "$line")"

                elif $(is_msg_cmd $line); then
                        echo "msg: $(get_msg_cmd_arg "$line")"

                elif $(is_item_cmd $line); then
                        echo "item: $(get_item_cmd_arg "$line")"

                elif $(is_select_cmd $line); then
                        echo "select: $(get_select_cmd_arg "$line")"

                elif $(is_getint_cmd $line); then
                        echo "getint: $(get_getint_cmd_arg "$line")"

                elif $(is_getuint_cmd $line); then
                        echo "getuint: $(get_getuint_cmd_arg "$line")"

                elif $(is_getstring_cmd $line); then
                        echo "getstring: $(get_getstring_cmd_arg "$line")"

                else
                        echo "Unknown command: $line"
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
