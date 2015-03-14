#!/usr/bin/env bash

openocd -f interface/stlink-v2.cfg -f target/stm32f1x_stlink.cfg -c "init; halt"
