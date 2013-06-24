#!/usr/bin/env bash

cd $(dirname $0)

create_gdb_command() {
    echo "target remote localhost:3333" >> gdbcmd
    echo "monitor reset halt" >> gdbcmd
    echo "load" >> gdbcmd
    echo "monitor reset halt" >> gdbcmd
}

if [ "$1" == "" ]; then
    echo "Usage: $0 <target>"
    exit 1
fi

if ! test ../target/$1; then
    echo "Target doesn't exist!"
    exit 1
fi

echo "Waiting for a connection to a target..."
create_gdb_command
arm-none-eabi-gdb --quiet --batch -x gdbcmd ../build/$1/dnx.elf
rm -f gdbcmd

echo "Done."
