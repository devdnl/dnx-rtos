#!/usr/bin/env bash

FILE=$1
NM="arm-none-eabi-nm"

search_global_variables() {

    err_file=$(mktemp)
    rm -f $err_file

    $NM -l --format=bsd $1 | while read line; do
        if [[ "$line" =~ ^[0-9a-fA-F]*[[:space:]][bBdD][[:space:]][_0-9a-zA-Z]*[:space:]*.*$ ]]; then
            echo $line | sed 's/^[0-9a-fA-F]* [bBdD] \([_[:alnum:]]*\)[[:space:]]*\(.*\)$/\2: error: "\1" variable shall be placed in GLOBAL_VARIABLES_SECTION or dynamic container/'
            touch $err_file
        fi
    done

    if [ -f $err_file ]; then
        rm -f $err_file
        exit 1
    fi
}

main() {

    if [ "$1" == "" ]; then
        echo "Usage: $0 <file-object-to-check>"
        exit 1
    fi

    search_global_variables $1
}

main $FILE
