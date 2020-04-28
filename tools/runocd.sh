#!/usr/bin/env bash

openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg -c "init; halt"
openocd -f interface/stlink-v2-1.cfg -f target/stm32f1x.cfg -c "init; halt"
openocd -f interface/stlink-v2-1.cfg -f target/stm32f4x.cfg -c "init; halt"
openocd -f interface/stlink-v2-1.cfg -f target/stm32f7x.cfg -c "init; halt"
