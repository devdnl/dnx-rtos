#!/usr/bin/env bash

root=$(pwd)

cd $(dirname $0)

rm -rf data
mkdir -p data

python romfsmap.py "${root}/res/romfs" data
