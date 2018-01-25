#!/usr/bin/env bash

root=$(pwd)

cd $(dirname $0)

mkdir -p data

python romfsmap.py "${root}/res/romfs" data

exit 1
