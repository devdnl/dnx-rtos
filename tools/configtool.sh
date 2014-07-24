#!/usr/bin/env bash

cd $(dirname $0)/configtool

os=$(uname -s)
arch=$(uname -m)
script="./configtool.lua"

if [ "$os" = "Linux" ]; then
        if [ "$arch" = "x86_64" ] || [ "$arch" = "amd64" ]; then
                LD_LIBRARY_PATH="./wxLua_linux_x86_64":$LD_LIBRARY_PATH
                ./wxLua_linux_x86_64/wxLua $script
        else
                LD_LIBRARY_PATH="./wxLua_linux_i386":$LD_LIBRARY_PATH
                ./wxLua_linux_i386/wxLua $script
        fi
elif [[ "$os" =~ '_NT' ]]; then
        ./wxLua_win32/wxLua $script
else
        echo "Unfortunately binaries for $os are not supported. You can compile the wxLua library by yourself to run configtool."
fi
