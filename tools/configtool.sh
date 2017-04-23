#!/usr/bin/env bash

cd $(dirname $0)

./configtool/configtool $1 ../../config/config.h PLUGINS-DIR=../configtool-plugins/
