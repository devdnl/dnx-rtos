#!/usr/bin/env bash

cd $(dirname $0)

os=$(uname -s)
arch=$(uname -m)
script="./configtool.lua"

if [ "$os" = "Linux" ]; then
        ./bin/wxLua $script
elif [[ "$os" =~ '_NT' ]]; then
        ./bin/wxLua.exe $script
elif [ "$os" == "Darwin" ]; then
        echo "Unfortunately binaries for Mac OS X are not supported yet."
else
        echo "Not supported operating system."
fi
