#!/usr/bin/env bash

root=$(pwd)

cd $(dirname $0)

rm -rf root

python romfsmap.py "${root}/res/romfs" .

exit 1
