#!/usr/bin/env bash

cd $(dirname $0)
cd ../doc/doxygen

doxygen Doxyfile
