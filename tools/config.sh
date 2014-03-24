#!/usr/bin/env bash

# configuration file
script=$(basename "$1")
cd $(dirname "$1")

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
        if [[ "$1" =~ ^\s*begin$ ]]; then true; else false; fi
}

is_end_cmd()
{
        if [[ "$1" =~ ^\s*end$ ]]; then true; else false; fi
}

is_msg_cmd()
{
        if [[ "$1" =~ ^\s*msg(.*)$ ]]; then true; else false; fi
}

is_setitem_cmd()
{
        if [[ "$1" =~ ^\s*setitem(.*\s*,\s*.*)$ ]]; then true; else false; fi
}

is_readsel_cmd()
{
        if [[ "$1" =~ ^\s*readsel(.*\s*,\s*.*)$ ]]; then true; else false; fi
}

is_readint_cmd()
{
        if [[ "$1" =~ ^\s*readint(.*\s*,\s*.*)$ ]]; then true; else false; fi
}

is_readuint_cmd()
{
        if [[ "$1" =~ ^\s*readuint(.*\s*,\s*.*)$ ]]; then true; else false; fi
}

is_readstring_cmd()
{
        if [[ "$1" =~ ^\s*readstring(.*\s*,\s*.*)$ ]]; then true; else false; fi
}

is_keyread_cmd()
{
        if [[ "$1" =~ ^\s*keyread(.*\s*,\s*.*,\s*.*,\s*.*)$ ]]; then true; else false; fi
}

is_keysave_cmd()
{
        if [[ "$1" =~ ^\s*keysave(.*\s*,\s*.*,\s*.*,\s*.*)$ ]]; then true; else false; fi
}

is_keycreate_cmd()
{
        if [[ "$1" =~ ^\s*keycreate(.*\s*,\s*.*,\s*.*,\s*.*)$ ]]; then true; else false; fi
}

is_keydelete_cmd()
{
        if [[ "$1" =~ ^\s*keydelete(.*\s*,\s*.*,\s*.*)$ ]]; then true; else false; fi
}

is_variable_cmd()
{
        if [[ "$1" =~ ^\s*[a-zA-Z0-9_]+=.*$ ]]; then true; else false; fi
}

is_ifeq_cmd()
{
        if [[ "$1" =~ ^\s*ifeq(.*\s*,\s*.*)$ ]]; then true; else false; fi
}

is_ifneq_cmd()
{
        if [[ "$1" =~ ^\s*ifneq(.*\s*,\s*.*)$ ]]; then true; else false; fi
}

is_exit_cmd()
{
        if [[ "$1" =~ ^\s*exit()$ ]]; then true; else false; fi
}

#-------------------------------------------------------------------------------
# Command argument return functions
#-------------------------------------------------------------------------------
get_msg_cmd_arg()
{
        echo $1 | sed 's/^\s*msg(\(.*\))$/\1/'
}

get_setitem_cmd_arg()
{
        echo $1 | sed 's/^\s*setitem(\(.*\)\s*,\s*\(.*\))$/\1 \2/'
}

get_readsel_cmd_arg()
{
        echo $1 | sed 's/^\s*readsel(\(.*\)\s*,\s*\(.*\))$/\1 \2/'
}

get_readint_cmd_arg()
{
        echo $1 | sed 's/^\s*readint(\(.*\)\s*,\s*\(.*\))$/\1 \2/'
}

get_readuint_cmd_arg()
{
        echo $1 | sed 's/^\s*readuint(\(.*\)\s*,\s*\(.*\))$/\1 \2/'
}

get_readstring_cmd_arg()
{
        echo $1 | sed 's/^\s*readstring(\(.*\)\s*,\s*\(.*\))$/\1 \2/'
}

get_keyread_cmd_arg()
{
        echo $1 | sed 's/^\s*keyread(\(.*\)\s*,\s*\(.*\),\s*\(.*\),\s*\(.*\))$/\1 \2 \3 \4/'
}

get_keysave_cmd_arg()
{
        echo $1 | sed 's/^\s*keysave(\(.*\)\s*,\s*\(.*\),\s*\(.*\),\s*\(.*\))$/\1 \2 \3 \4/'
}

get_keycreate_cmd_arg()
{
        echo $1 | sed 's/^\s*keycreate(\(.*\)\s*,\s*\(.*\),\s*\(.*\),\s*\(.*\))$/\1 \2 \3 \4/'
}

get_keydelete_cmd_arg()
{
        echo $1 | sed 's/^\s*\\keydelete(\(.*\)\s*,\s*\(.*\),\s*\(.*\))$/\1 \2 \3/'
}

get_variable_cmd_arg()
{
        echo $1 | sed 's/^\s*\([a-zA-Z0-9_]*\)=\(.*\)/\1 \2/'
}

get_ifeq_cmd_arg()
{
        echo $1 | sed 's/^\s*ifeq(\(.*\)\s*,\s*\(.*\))$/\1 \2/'
}

get_ifneq_cmd_arg()
{
        echo $1 | sed 's/^\s*ifneq(\(.*\)\s*,\s*\(.*\))$/\1 \2/'
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
# Remove key and value in specified file
#-------------------------------------------------------------------------------
key_remove()
{
        local file=$1 type=$2 key=$3

        if [ "$type" = "makefile" ]; then
                sed -i "/^\s*$key\s*=.*$/d" $file
        elif [ "$type" = "header" ]; then
                sed -i "/^\s*#\s*define\s*$key\s*.*$/d" $file
        fi
}

#-------------------------------------------------------------------------------
# Add key and value in specified file
#-------------------------------------------------------------------------------
key_create()
{
        local file=$1 type=$2 key=$3 val=$4

        if [ "$type" = "makefile" ]; then
                sed -i "$ a\\$key=$val" $file
        elif [ "$type" = "header" ]; then
                sed -i "/^\s*#\s*ifdef\s.*/a #define $key $val" $file
        fi
}

#-------------------------------------------------------------------------------
# Change key value
#-------------------------------------------------------------------------------
key_save()
{
        local file=$1 type=$2 key=$3 val=$4

        if [ "$type" = "makefile" ]; then
                sed -i -e "s%^\s*$key\s*=.*%$key=$val%g" $file
        elif [ "$type" = "header" ]; then
                sed -i -e "s%^\s*#\s*define\s*$key\s*.*%#define $key $val%g" $file
        fi
}

#-------------------------------------------------------------------------------
# Read key value
#-------------------------------------------------------------------------------
key_read()
{
        local file=$1 type=$2 key=$3

        if [ "$type" = "makefile" ]; then
                cat $file | grep -P "^\s*$key" | sed "s/^\s*$key\s*=\s*//g"
        elif [ "$type" = "header" ]; then
                cat $file | grep -P "^\s*#\s*define\s*$key\s*.*" | sed "s/^\s*#\s*define\s*$key\s*//g"
        fi
}

#-------------------------------------------------------------------------------
# Function read configuration commands step by step
#-------------------------------------------------------------------------------
read_script()
{
        local script=$1 seek=0 args=()
        local begin=false items=() itemdesc=() msgs=() rewind=false
        declare -A variable

        while read -r line <&9; do
                seek=$[$seek+1]

                if [[ "$line" =~ ^\s*#.* ]] || [[ "$line" == "" ]]; then
                        continue

                elif $(is_begin_cmd "$line"); then
                        if $begin; then
                                error $script $seek "nested 'begin'"
                        else
                                begin=true
                        fi

                elif $(is_end_cmd "$line"); then
                        if $begin; then
                                begin=false
                                msgs=()
                                items=()
                                itemdesc=()
                                save=false
                                rewind=false
                                echo ""
                        else
                                error $script $seek "orphaned 'end'"
                        fi

                elif $(is_ifeq_cmd "$line") && $begin; then
                        args=()
                        args=($(get_ifeq_cmd_arg "$line"))
                        lh=${args[0]}
                        rh=${args[1]}

                        if [ "${variable["$lh"]}" != "${variable["$rh"]}" ]; then
                                rewind=true
                        fi

                elif $(is_ifneq_cmd "$line") && $begin; then
                        args=()
                        args=($(get_ifneq_cmd_arg "$line"))
                        lh=${args[0]}
                        rh=${args[1]}

                        if [ "${variable["$lh"]}" == "${variable["$rh"]}" ]; then
                                rewind=true
                        fi

                elif $rewind; then
                        continue

                elif $(is_msg_cmd "$line") && $begin; then
                        msgs[${#msgs[@]}]=$(get_msg_cmd_arg "$line")

                elif $(is_setitem_cmd "$line") && $begin; then
                        args=()
                        args=($(get_setitem_cmd_arg "$line"))
                        items[${#items[@]}]=${args[0]}
                        itemdesc[${#itemdesc[@]}]=${args[*]/${args[0]}/}

                elif $(is_readsel_cmd "$line") && $begin; then
                        args=()
                        args=($(get_readsel_cmd_arg "$line"))
                        idx=${args[0]}
                        msg=${args[*]/${args[0]}/}

                        if [ "$idx" == "" ]; then
                                error $script $seek "undefined variable"
                        fi

                        if [ "$msg" == "" ]; then
                                msg="Select item (1..${#items[@]})"
                        fi

                        for ((i=0; i<${#msgs[*]}; i++)); do
                                echo "${msgs[$i]}"
                        done

                        for ((i=0; i<${#items[*]}; i++)); do
                                echo "  $[$i+1]) ${items[$i]} - ${itemdesc[$i]}"
                        done

                        value=0
                        until [ $value -le ${#items[@]} ] && [ $value -gt 0 ]; do
                                read -p "$msg: " value

                                if [ "$value" == "" ]; then
                                        rewind=true
                                        break
                                elif ! is_integer "$value"; then
                                        value=0
                                else
                                        variable[$idx]=${items[$[$value-1]]}
                                fi
                        done

                elif $(is_readint_cmd "$line") && $begin; then
                        continue

                elif $(is_readuint_cmd "$line") && $begin; then
                        continue

                elif $(is_readstring_cmd "$line") && $begin; then
                        continue

                elif $(is_keyread_cmd "$line") && $begin; then
                        args=()
                        args=($(get_keyread_cmd_arg "$line"))
                        type=${args[0]}
                        file=${args[1]}
                        key=${args[2]}
                        var=${args[3]}
                        variable[$var]=$(key_read "$file" "$type" "$key")

                elif $(is_keysave_cmd "$line") && $begin; then
                        args=()
                        args=($(get_keysave_cmd_arg "$line"))
                        type=${args[0]}
                        file=${args[1]}
                        key=${args[2]}
                        var=${args[3]}
                        key_save "$file" "$type" "$key" "${variable[$var]}"

                elif $(is_keycreate_cmd "$line") && $begin; then
                        args=()
                        args=($(get_keycreate_cmd_arg "$line"))
                        type=${args[0]}
                        file=${args[1]}
                        key=${args[2]}
                        var=${args[3]}
                        key_create "$file" "$type" "$key" "${variable[$var]}"

                elif $(is_keydelete_cmd "$line") && $begin; then
                        args=()
                        args=($(get_keydelete_cmd_arg "$line"))
                        type=${args[0]}
                        file=${args[1]}
                        key=${args[2]}
                        key_remove "$file" "$type" "$key"

                elif $(is_exit_cmd "$line") && $begin; then
                        exit 0

                elif $(is_variable_cmd "$line"); then
                        args=()
                        args=($(get_variable_cmd_arg "$line"))
                        name=${args[0]}
                        val=${args[1]}
                        variable["$name"]=$val

                else
                        if $begin; then
                                error $script $seek "unknown command: $line"
                        else
                                error $script $seek "command '$line' outsite begin..end"
                        fi
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
