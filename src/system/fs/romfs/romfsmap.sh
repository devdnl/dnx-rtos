#!/usr/bin/env bash

root=$(pwd)

cd $(dirname $0)

rm -rf root

echo "# generated file" > Makefile.in

python romfsmap.py "${root}/res/romfs" .
