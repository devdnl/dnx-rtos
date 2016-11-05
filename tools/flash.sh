#!/usr/bin/env bash

cd $(dirname $0)

get_target_name() {
    echo $(cat ../config/project/project_flags.h | grep  -oP '^#\s*define\s+__CPU_ARCH__\s+(.*)' | sed 's/^#s*define __CPU_ARCH__ //g')
}

get_project_name() {
    echo $(cat ../config/project/project_flags.h | grep  -oP '^#\s*define\s+__PROJECT_NAME__\s+(.*)' | sed 's/^#s*define __PROJECT_NAME__ //g')
}

create_gdb_command() {
    echo "target remote localhost:3333" >> gdbcmd
    echo "monitor reset halt" >> gdbcmd
    echo "load" >> gdbcmd
    echo "monitor reset" >> gdbcmd
}

target=$(get_target_name)
project=$(get_project_name)

if [ "$target" == "" ]; then
    echo "Unknown target!"
    exit 1
fi

echo "Waiting for a connection to a target..."
create_gdb_command
arm-none-eabi-gdb --quiet --batch -x gdbcmd ../build/$target/$project.elf
rm -f gdbcmd

date "+Done [%H:%M:%S]"
