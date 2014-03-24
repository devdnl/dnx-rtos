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
is_config_cmd()
{
        if [[ "$1" =~ ^\s*config$ ]]; then true; else false; fi
}

is_endconfig_cmd()
{
        if [[ "$1" =~ ^\s*endconfig$ ]]; then true; else false; fi
}

is_msg_cmd()
{
        if [[ "$1" =~ ^\s*msg(.*)$ ]]; then true; else false; fi
}

# setitem(item, [description])
is_additem_cmd()
{
        if [[ "$1" =~ ^\s*additem(.*\s*,\s*.*)$ ]]; then true; else false; fi
}

# readsel(var, [description])
is_readsel_cmd()
{
        if [[ "$1" =~ ^\s*readsel(.*\s*,\s*.*)$ ]]; then true; else false; fi
}

# readint(var, [description])
is_readint_cmd()
{
        if [[ "$1" =~ ^\s*readint(.*\s*,\s*.*)$ ]]; then true; else false; fi
}

# readuint(var, [description])
is_readuint_cmd()
{
        if [[ "$1" =~ ^\s*readuint(.*\s*,\s*.*)$ ]]; then true; else false; fi
}

# readstring(var, [description])
is_readstring_cmd()
{
        if [[ "$1" =~ ^\s*readstring(.*\s*,\s*.*)$ ]]; then true; else false; fi
}

# keyread(makefile|header, file, key, var)
is_keyread_cmd()
{
        if [[ "$1" =~ ^\s*keyread(.*\s*,\s*.*,\s*.*,\s*.*)$ ]]; then true; else false; fi
}

# keysave(makefile|header, file, key, var)
is_keysave_cmd()
{
        if [[ "$1" =~ ^\s*keysave(.*\s*,\s*.*,\s*.*,\s*.*)$ ]]; then true; else false; fi
}

# keycreate(makefile|header, file, key, var)
is_keycreate_cmd()
{
        if [[ "$1" =~ ^\s*keycreate(.*\s*,\s*.*,\s*.*,\s*.*)$ ]]; then true; else false; fi
}

# keydelete(makefile|header, file, key)
is_keydelete_cmd()
{
        if [[ "$1" =~ ^\s*keydelete(.*\s*,\s*.*,\s*.*)$ ]]; then true; else false; fi
}

is_variable_cmd()
{
        if [[ "$1" =~ ^\s*[a-zA-Z0-9_]+=.*$ ]]; then true; else false; fi
}

# if(var1 =|==|=~|~=|!=|<|>|>=|<= var2)
is_if_cmd()
{
        if [[ "$1" =~ ^\s*if(.*\s*[!=<>]+\s*.*)$ ]]; then true; else false; fi
}

is_endif_cmd()
{
        if [[ "$1" =~ ^\s*endif$ ]]; then true; else false; fi
}

# exit()
is_exit_cmd()
{
        if [[ "$1" =~ ^\s*exit(.*)$ ]]; then true; else false; fi
}

# print(string [@var] ...)
is_print_cmd()
{
        if [[ "$1" =~ ^\s*print(.*\s*)$ ]]; then true; else false; fi
}

#-------------------------------------------------------------------------------
# Command argument return functions
#-------------------------------------------------------------------------------
get_cmd_1arg()
{
        echo $1 | sed 's/^\s*[a-z]*(\(.*\))$/\1/'
}

get_cmd_2args()
{
        echo $1 | sed 's/^\s*[a-z]*(\(.*\)\s*,\s*\(.*\))$/\1 \2/'
}

get_cmd_3args()
{
        echo $1 | sed 's/^\s*[a-z]*(\(.*\)\s*,\s*\(.*\),\s*\(.*\))$/\1 \2 \3/'
}

get_cmd_4args()
{
        echo $1 | sed 's/^\s*[a-z]*(\(.*\)\s*,\s*\(.*\),\s*\(.*\),\s*\(.*\))$/\1 \2 \3 \4/'
}

get_if_args()
{
        echo $1 | sed 's/^\s*if(\(.*\)\s*\([!=<>]*\)\s*\(.*\))/\1 \2 \3/'
}

get_variable_args()
{
        echo $1 | sed 's/^\s*\([a-zA-Z0-9_]*\)=\(.*\)/\1 \2/'
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
                sed -i "/^\s*#\s*define\s.*/a #define $key $val" $file
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
        local begin=false items=() itemdesc=() msgs=() nestedif=0 nestedtarget=0 rewind=false
        declare -A variable

        while read -r line <&9; do
                seek=$[$seek+1]

                if [[ "$line" =~ ^\s*#.* ]] || [[ "$line" == "" ]]; then
                        continue

                elif $(is_config_cmd "$line"); then
                        if $begin; then
                                error $script $seek "nested 'config'"
                        else
                                begin=true
                        fi

                elif $(is_endconfig_cmd "$line"); then
                        if $begin; then
                                begin=false
                                msgs=()
                                items=()
                                itemdesc=()
                                save=false
                                rewind=false

                                if [ $nestedif -gt 0 ]; then
                                        error $script $seek "not closed 'if-endif', too less 'endif'"
                                elif [ $nestedif -lt 0 ]; then
                                        error $script $seek "not closed 'if-endif', too more 'endif'"
                                fi
                        else
                                error $script $seek "orphaned 'endconfig'"
                        fi

                elif $(is_if_cmd "$line") && $begin; then
                        args=()
                        args=($(get_if_args "$line"))
                        lh=${args[0]}
                        op=${args[1]}
                        rh=${args[2]}

                        nestedif=$[$nestedif+1]

                        if ! $rewind; then
                                local condition=false

                                case "$op" in
                                "="  | "==" | "=~" ) if [ "${variable["$lh"]}" ==  "${variable["$rh"]}" ]; then condition=true; fi ;;
                                "!=" | "~="        ) if [ "${variable["$lh"]}" !=  "${variable["$rh"]}" ]; then condition=true; fi ;;
                                ">"                ) if [ "${variable["$lh"]}" -gt "${variable["$rh"]}" ] >/dev/null 2>&1; then condition=true; fi ;;
                                "<"                ) if [ "${variable["$lh"]}" -lt "${variable["$rh"]}" ] >/dev/null 2>&1; then condition=true; fi ;;
                                ">="               ) if [ "${variable["$lh"]}" -ge "${variable["$rh"]}" ] >/dev/null 2>&1; then condition=true; fi ;;
                                "<="               ) if [ "${variable["$lh"]}" -le "${variable["$rh"]}" ] >/dev/null 2>&1; then condition=true; fi ;;
                                *                  ) error $script $seek "unknown operator: '$op'"
                                esac

                                if ! $condition; then
                                        nestedtarget=$nestedif
                                        rewind=true
                                fi
                        fi

                elif $(is_endif_cmd "$line") && $begin; then
                        if [ $nestedif -eq $nestedtarget ]; then
                                rewind=false
                        fi

                        nestedif=$[$nestedif-1]

                elif $rewind; then
                        continue

                elif $(is_msg_cmd "$line") && $begin; then
                        msgs[${#msgs[@]}]=$(get_cmd_1arg "$line")

                elif $(is_additem_cmd "$line") && $begin; then
                        args=()
                        args=($(get_cmd_2args "$line"))
                        items[${#items[@]}]=${args[0]}
                        itemdesc[${#itemdesc[@]}]=${args[*]/${args[0]}/}

                elif $(is_readsel_cmd "$line") && $begin; then
                        args=()
                        args=($(get_cmd_2args "$line"))
                        var=${args[0]}
                        msg=${args[*]/${args[0]}/}

                        if [ "$var" == "" ]; then
                                error $script $seek "undefined variable"
                        fi

                        if [ "$msg" == "" ]; then
                                msg="Select item (1..${#items[@]})"
                        fi

                        for ((i=0; i<${#msgs[*]}; i++)); do
                                echo "${msgs[$i]}"
                        done

                        for ((i=0; i<${#items[*]}; i++)); do
                                echo "  $[$i+1]) ${items[$i]} ${itemdesc[$i]}"
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
                                        variable[$var]=${items[$[$value-1]]}
                                fi
                        done

                        items=()
                        msgs=()

                elif $(is_readint_cmd "$line") && $begin; then
                        args=()
                        args=($(get_cmd_2args "$line"))
                        var=${args[0]}
                        msg=${args[*]/${args[0]}/}

                        if [ "$var" == "" ]; then
                                error $script $seek "undefined variable"
                        fi

                        if [ "$msg" == "" ]; then
                                msg="Enter number (oct, dec, hex)"
                        fi

                        value=false
                        while ! is_integer "$value"; do
                                read -p "$msg: " value

                                if [ "$value" == "" ]; then
                                        rewind=true
                                        break
                                elif ! is_integer "$value"; then
                                        value=false
                                else
                                        variable[$var]=$value
                                fi
                        done

                elif $(is_readuint_cmd "$line") && $begin; then
                        args=()
                        args=($(get_cmd_2args "$line"))
                        var=${args[0]}
                        msg=${args[*]/${args[0]}/}

                        if [ "$var" == "" ]; then
                                error $script $seek "undefined variable"
                        fi

                        if [ "$msg" == "" ]; then
                                msg="Enter positive number (oct, dec, hex)"
                        fi

                        value=false
                        while ! is_integer "$value"; do
                                read -p "$msg: " value

                                if [ "$value" == "" ]; then
                                        rewind=true
                                        break
                                elif ! is_integer "$value"; then
                                        echo "not integer"
                                        value=false
                                else

                                        variable[$var]=${value/-/}
                                fi
                        done

                elif $(is_readstring_cmd "$line") && $begin; then
                        args=()
                        args=($(get_cmd_2args "$line"))
                        var=${args[0]}
                        msg=${args[*]/${args[0]}/}

                        if [ "$var" == "" ]; then
                                error $script $seek "undefined variable"
                        fi

                        if [ "$msg" == "" ]; then
                                msg="Enter string"
                        fi

                        read -p "$msg: " value

                        if [ "$value" == "" ]; then
                                rewind=true
                        else
                                variable[$var]=\"$value\"
                        fi

                elif $(is_print_cmd "$line") && $begin; then
                        args=()
                        args=($(get_cmd_1arg "$line"))

                        msg=
                        for word in ${args[*]}; do
                                if [[ $word =~ @[a-zA-Z0-9_] ]]; then
                                        msg="$msg ${variable[${word/@/}]}"
                                else
                                        msg="$msg $word"
                                fi
                        done

                        echo $msg

                elif $(is_keyread_cmd "$line") && $begin; then
                        args=()
                        args=($(get_cmd_4args "$line"))
                        type=${args[0]}
                        file=${args[1]}
                        key=${args[2]}
                        var=${args[3]}
                        variable[$var]=$(key_read "$file" "$type" "$key")

                elif $(is_keysave_cmd "$line") && $begin; then
                        args=()
                        args=($(get_cmd_4args "$line"))
                        type=${args[0]}
                        file=${args[1]}
                        key=${args[2]}
                        var=${args[3]}
                        key_save "$file" "$type" "$key" "${variable[$var]}"

                elif $(is_keycreate_cmd "$line") && $begin; then
                        args=()
                        args=($(get_cmd_4args "$line"))
                        type=${args[0]}
                        file=${args[1]}
                        key=${args[2]}
                        var=${args[3]}
                        key_create "$file" "$type" "$key" "${variable[$var]}"

                elif $(is_keydelete_cmd "$line") && $begin; then
                        args=()
                        args=($(get_cmd_3args "$line"))
                        type=${args[0]}
                        file=${args[1]}
                        key=${args[2]}
                        key_remove "$file" "$type" "$key"

                elif $(is_exit_cmd "$line") && $begin; then
                        exit 0

                elif $(is_variable_cmd "$line"); then
                        args=()
                        args=($(get_variable_args "$line"))
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
}

main
