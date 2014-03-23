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
        if [[ "$1" =~ ^\s*\\begin\{.*\}$ ]]; then true; else false; fi
}

is_end_cmd()
{
        if [[ "$1" =~ ^\s*\\end\{.*\}$ ]]; then true; else false; fi
}

is_msg_cmd()
{
        if [[ "$1" =~ ^\s*\\msg\{.*\}$ ]]; then true; else false; fi
}

is_item_cmd()
{
        if [[ "$1" =~ ^\s*\\item\{.*\}$ ]]; then true; else false; fi
}

is_readsel_cmd()
{
        if [[ "$1" =~ ^\s*\\readsel\{.*\}\{.*\}$ ]]; then true; else false; fi
}

is_readint_cmd()
{
        if [[ "$1" =~ ^\s*\\readint\{.*\}\{.*\}$ ]]; then true; else false; fi
}

is_readuint_cmd()
{
        if [[ "$1" =~ ^\s*\\readuint\{.*\}\{.*\}$ ]]; then true; else false; fi
}

is_readstring_cmd()
{
        if [[ "$1" =~ ^\s*\\readstring\{.*\}\{.*\}$ ]]; then true; else false; fi
}

is_keyread_cmd()
{
        if [[ "$1" =~ ^\s*\\keyread\{.*\}\{.*\}\{.*\}\{.*\}$ ]]; then true; else false; fi
}

is_keysave_cmd()
{
        if [[ "$1" =~ ^\s*\\keysave\{.*\}\{.*\}\{.*\}\{.*\}$ ]]; then true; else false; fi
}

is_keycreate_cmd()
{
        if [[ "$1" =~ ^\s*\\keycreate\{.*\}\{.*\}\{.*\}\{.*\}$ ]]; then true; else false; fi
}

is_keydelete_cmd()
{
        if [[ "$1" =~ ^\s*\\keydelete\{.*\}\{.*\}\{.*\}$ ]]; then true; else false; fi
}

is_variable_cmd()
{
        if [[ "$1" =~ ^\s*[[:alnum:]]+=.*$ ]]; then true; else false; fi
}

is_ifeq_cmd()
{
        if [[ "$1" =~ ^\s*\\ifeq\{.*\}\{.*\}$ ]]; then true; else false; fi
}

is_ifneq_cmd()
{
        if [[ "$1" =~ ^\s*\\ifneq\{.*\}\{.*\}$ ]]; then true; else false; fi
}

#-------------------------------------------------------------------------------
# Command argument return functions
#-------------------------------------------------------------------------------
get_begin_cmd_arg()
{
        echo $1 | sed 's/^\s*\\begin{\(.*\)}$/\1/'
}

get_end_cmd_arg()
{
        echo $1 | sed 's/^\s*\\end{\(.*\)}$/\1/'
}

get_msg_cmd_arg()
{
        echo $1 | sed 's/^\s*\\msg{\(.*\)}$/\1/'
}

get_item_cmd_arg()
{
        echo $1 | sed 's/^\s*\\item{\(.*\)}{\(.*\)}$/\1 \2/'
}

get_readsel_cmd_arg()
{
        echo $1 | sed 's/^\s*\\readsel{\(.*\)}{\(.*\)}$/\1 \2/'
}

get_readint_cmd_arg()
{
        echo $1 | sed 's/^\s*\\readint{\(.*\)}{\(.*\)}$/\1 \2/'
}

get_readuint_cmd_arg()
{
        echo $1 | sed 's/^\s*\\readuint{\(.*\)}{\(.*\)}$/\1 \2/'
}

get_readstring_cmd_arg()
{
        echo $1 | sed 's/^\s*\\readstring{\(.*\)}{\(.*\)}$/\1 \2/'
}

get_keyread_cmd_arg()
{
        echo $1 | sed 's/^\s*\\keyread{\(.*\)}{\(.*\)}{\(.*\)}{\(.*\)}$/\1 \2 \3 \4/'
}

get_keysave_cmd_arg()
{
        echo $1 | sed 's/^\s*\\keysave{\(.*\)}{\(.*\)}{\(.*\)}{\(.*\)}$/\1 \2 \3 \4/'
}

get_keycreate_cmd_arg()
{
        echo $1 | sed 's/^\s*\\keycreate{\(.*\)}{\(.*\)}{\(.*\)}{\(.*\)}$/\1 \2 \3 \4/'
}

get_keydelete_cmd_arg()
{
        echo $1 | sed 's/^\s*\\keydelete{\(.*\)}{\(.*\)}{\(.*\)}$/\1 \2 \3/'
}

get_variable_cmd_arg()
{
        echo $1 | sed 's/^\s*\([[:alnum:]]*\)=\(.*\)/\1 \2/'
}

get_ifeq_cmd_arg()
{
        echo $1 | sed 's/^\s*\\ifeq{\(.*\)}{\(.*\)}$/\1 \2/'
}

get_ifneq_cmd_arg()
{
        echo $1 | sed 's/^\s*\\ifneq{\(.*\)}{\(.*\)}$/\1 \2/'
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
        local script=$1 seek=0 args=()
        local begin=false items=() itemdesc=() msgs=() save=false rewind=false
        declare -A var

        while read -r line <&9; do
                seek=$[$seek+1]

                if [[ "$line" =~ ^\s*#.* ]] || [[ "$line" == "" ]]; then
                        continue

                elif $(is_begin_cmd "$line"); then
                        if $begin; then
                                error $script $seek "\begin{} in the \begin{}..\end{} block"
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
                                error $script $seek "orphaned \end{} command"
                        fi

                elif $(is_ifeq_cmd "$line") && $begin; then
                        args=()
                        args=($(get_ifeq_cmd_arg "$line"))
                        lh=${args[0]}
                        rh=${args[1]}

                        if [ "${var["$lh"]}" != "${var["$rh"]}" ]; then
                                rewind=true
                        fi

                elif $(is_ifneq_cmd "$line") && $begin; then
                        args=()
                        args=($(get_ifneq_cmd_arg "$line"))
                        lh=${args[0]}
                        rh=${args[1]}

                        if [ "${var["$lh"]}" == "${var["$rh"]}" ]; then
                                rewind=true
                        fi

                elif $rewind; then
                        continue

                elif $(is_msg_cmd "$line") && $begin; then
                        msgs[${#msgs[@]}]=$(get_msg_cmd_arg "$line")

                elif $(is_item_cmd "$line") && $begin; then
                        args=()
                        args=($(get_item_cmd_arg "$line"))
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
                                        break
                                elif ! is_integer "$value"; then
                                        value=0
                                else
                                        var[$idx]=${items[$[$value-1]]}
                                        save=true
                                fi
                        done

                elif $(is_readint_cmd "$line") && $begin; then
                        continue

                elif $(is_readuint_cmd "$line") && $begin; then
                        continue

                elif $(is_readstring_cmd "$line") && $begin; then
                        continue

                elif $(is_keyread_cmd "$line") && $begin; then
                        continue

                elif $(is_keysave_cmd "$line") && $begin; then
                        if $save; then
                                args=()
                                args=($(get_keysave_cmd_arg "$line"))
                                type=${args[0]}
                                file=${args[1]}
                                key=${args[2]}
                                idx=${args[3]}
                                echo "Save value: ${var["$idx"]} in key $key to file: $file type: $type"
                        fi

                elif $(is_keycreate_cmd "$line") && $begin; then
                        continue

                elif $(is_keydelete_cmd "$line") && $begin; then
                        continue

                elif $(is_variable_cmd "$line"); then
                        args=()
                        args=($(get_variable_cmd_arg "$line"))
                        name=${args[0]}
                        val=${args[1]}
                        var["$name"]=$val

                else
                        if $begin; then
                                error $script $seek "unknown command: $line"
                        else
                                error $script $seek "command '$line' outsite \begin{}..\end{}"
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
