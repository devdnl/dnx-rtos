#!/usr/bin/env bash

cd $(dirname $0)

os=$(uname -s)
arch=$(uname -m)
script="./configtool.lua"

if [ "$os" = "Linux" ]; then
        ./bin/wxLua $script
elif [[ "$os" =~ '_NT' ]]; then
        ./bin/wxLua.exe $script
else
        echo "Unfortunately binaries for $os are not supported. You can compile the wxLua library by yourself to run configtool."
fi
