#!/usr/bin/env bash

openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "init"
openocd -f interface/stlink.cfg -f target/stm32f3x.cfg -c "init"
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "init"
openocd -f interface/stlink.cfg -f target/stm32f7x.cfg -c "init"
openocd -f interface/stlink.cfg -f target/stm32h7x.cfg -c "init"
