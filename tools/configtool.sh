#!/usr/bin/env bash

cd $(dirname $0)

./configtool/configtool ../../config/config.h PLUGINS-DIR=../configtool-plugins/
