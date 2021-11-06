#!/usr/bin/env bash

set -e

switch_to_bsp()
{
    echo "====================================================================="
    echo "= SWITCHING TO '$1' CONFIGURATION"
    echo "====================================================================="
    make clean
    ./tools/configtool/configtool -- import=../test_bsp/$1.dnxc:../../config
}

switch_to_bsp "stm32f1"
make all

switch_to_bsp "stm32f3"
make all

switch_to_bsp "stm32f4"
make all

switch_to_bsp "stm32f7"
make all

switch_to_bsp "stm32h7"
make all

switch_to_bsp "../../BSP/default"
