#!/usr/bin/env bash

# configuration file
working_path=$1
CPU_family=
CPU_arch=

################################################################################
# Function set selected value
################################################################################
set_key_value()
{
        local fle=$1 key=$2 val=$3

        sed -i -e "s%$key\s*=.*%$key=$val%g" $fle
}

################################################################################
# Function return value of key
################################################################################
get_key_value()
{
        local fle=$1 key=$2

        cat $fle | grep -P "^$key" | sed "s/^$key\s*=\s*//g"
}

################################################################################
# Function return line tag
################################################################################
get_line_tag()
{
        local line=$1

        echo $line | sed 's/^#\s*<\(.*\)>\s*.*/tag:\1/'
}

################################################################################
# Function remove tag from line
################################################################################
remove_line_tag()
{
        local line=$1

        echo $line | sed 's/^#\s*<.*>\s*//'
}

################################################################################
# Function check that value is an integer
################################################################################
is_integer() {
    printf "%d" $1 > /dev/null 2>&1
    return $?
}

################################################################################
# Function read configuration commands step by step
################################################################################
configure()
{
        local fle=$1
        local sel_type=
        local selection=()
        local msg=()
        local key=

        while read -r line <&9; do

                local tag=$(get_line_tag "$line")
                case $tag in
                "tag:INFO")
                        echo "$(remove_line_tag "$line")"
                        ;;

                "tag:MSG")
                        msg[${#msg[@]}]="$(remove_line_tag "$line")"
                        ;;

                "tag:KEY")
                        key=$(remove_line_tag "$line")
                        ;;

                "tag:CHOOSE")
                        selection[${#selection[@]}]="$(remove_line_tag "$line")"
                        sel_type="CHOOSE"
                        ;;

                "tag:STRING")
                        sel_type="STRING"
                        ;;

                "tag:INT")
                        sel_type="INT"
                        ;;

                "tag:UINT")
                        sel_type="UINT"
                        ;;

                "tag:ASK" | "tag:ASKIF" | "tag:ASKIFNOT")
                        local save=true

                        local tag_arg=$(remove_line_tag "$line")
                        if [ "$tag" == "tag:ASKIF" ] && [ "$CPU_family" != "$tag_arg" ]; then
                                selection=()
                                msg=()
                                continue
                        elif [ "$tag" == "tag:ASKIFNOT" ] && [ "$CPU_family" == "$tag_arg" ]; then
                                selection=()
                                msg=()
                                continue
                        fi

                        for (( i=0; i < ${#msg[@]}; i++ )); do
                                echo "${msg[$i]}"
                        done

                        echo "Now: $(get_key_value "$fle" "$key")"

                        case "$sel_type" in
                        "CHOOSE" )
                                for (( i=0; i < ${#selection[@]}; i++ )); do
                                        echo "  $[$i+1]) ${selection[$i]}"
                                done

                                choice=999
                                until [ $choice -le ${#selection[@]} ] && [ $choice -gt 0 ]; do
                                        read -p "Select option (1..${#selection[@]}): " choice

                                        if [ "$choice" == "" ]; then
                                                save=false
                                                break
                                        elif ! is_integer "$choice"; then
                                                choice=999
                                        fi
                                done
                                ;;

                        "STRING" )
                                read -p "Enter string: " choice

                                if [ "$choice" == "" ]; then
                                        save=false
                                fi

                                choice=\"$choice\"

                                ;;

                        "INT" )
                                choice=false
                                while ! is_integer "$choice"; do
                                        read -p "Enter number (bin, oct, dec, hex): " choice

                                        if [ "$choice" == "" ]; then
                                                save=false
                                                break
                                        fi
                                done
                                ;;

                        "UINT" )
                                choice=false
                                while ! is_integer "$choice"; do
                                        read -p "Enter positive number (bin, oct, dec, hex): " choice

                                        if [ "$choice" == "" ]; then
                                                save=false
                                                break
                                        fi
                                done

                                choice=${choice/-/}

                                ;;

                        * )
                                echo "Unknown variable type!"
                                exit -1
                                ;;
                        esac

                        if $save; then
                                case "$sel_type" in
                                "CHOOSE" ) set_key_value $fle "$key" "${selection[$[$choice-1]]}";;
                                "STRING" ) set_key_value $fle "$key" "$choice";;
                                "INT"    ) set_key_value $fle "$key" "$choice";;
                                "UINT"   ) set_key_value $fle "$key" "$choice";;
                                esac
                        fi

                        echo ""
                        echo "================================================================================"
                        echo ""

                        selection=()
                        msg=()
                        ;;
                esac
        done 9< $fle
}

################################################################################
# Main function
################################################################################
main()
{
        # configuration file shall be defined
        if [ "$working_path" == "" ]; then
                echo "Usage: $0 [working path]"
                exit -1
        fi

        cd "$working_path"

        configure "./project/name.config"
        configure "./project/arch.config"
        CPU_arch=$(get_key_value "./project/arch.config" "ARCHCONFIG__TARGET")
        configure "./$CPU_arch/toolchain.config"
        configure "./$CPU_arch/cpu.config"
        configure "./project/fs.config"
        configure "./$CPU_arch/mod.config"

        echo "Done"
}

main
