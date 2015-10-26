# Makefile for GNU make

# NOTE: All flags defined as: __FLAG_NAME__ (with doubled underscore as suffix
#       and prefix) are exported to the single configuration file
#       (by using Configtool) when entire project configuration is exported.
#       All other flag definitions and statements are ignored.

__CPUCONFIG_CPUNAME__=STM32F107VCxx
CPUCONFIG_AFLAGS=-mcpu=cortex-m3 -mthumb -mthumb-interwork -DGCC_ARMCM3
CPUCONFIG_CFLAGS=-mcpu=cortex-m3 -mthumb -mthumb-interwork -DGCC_ARMCM3
CPUCONFIG_CXXFLAGS=-mcpu=cortex-m3 -mthumb -mthumb-interwork -DGCC_ARMCM3
CPUCONFIG_LDFLAGS=-mcpu=cortex-m3 -mthumb -mthumb-interwork
CPUCONFIG_LD=./src/system/portable/stm32f1/ld/$(__CPUCONFIG_CPUNAME__).ld
