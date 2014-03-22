# Makefile for GNU make

####################################################################################################
# INSERT HERE SOURCES NOT ARCHITECTURE DEPENDED
####################################################################################################
CSRC_noarch   += drivers/tty/tty.c
CSRC_noarch   += drivers/tty/tty_bfr.c
CSRC_noarch   += drivers/tty/tty_cmd.c
CSRC_noarch   += drivers/tty/tty_editln.c

CXXSRC_noarch += 

HDRLOC_noarch += drivers/tty

####################################################################################################
# INSERT HERE SOURCES stm32f1 ARCHITECTURE DEPENDED
####################################################################################################
CSRC_stm32f1   += drivers/gpio/stm32f1/gpio.c
CSRC_stm32f1   += drivers/uart/stm32f1/uart.c
CSRC_stm32f1   += drivers/pll/stm32f1/pll.c
CSRC_stm32f1   += drivers/sdspi/stm32f1/sdspi.c
CSRC_stm32f1   += drivers/ethmac/stm32f1/ethmac.c
CSRC_stm32f1   += drivers/ethmac/stm32f1/stm32_eth_driver/stm32_eth.c
CSRC_stm32f1   += drivers/spi/stm32f1/spi.c
CSRC_stm32f1   += drivers/crc/stm32f1/crc.c
CSRC_stm32f1   += drivers/wdg/stm32f1/wdg.c
CSRC_stm32f1   += portable/stm32f1/cpuctl.c
CSRC_stm32f1   += portable/stm32f1/cpuhooks.c
CSRC_stm32f1   += portable/stm32f1/stm32f10x_vectors.c
CSRC_stm32f1   += portable/stm32f1/lib/misc.c
CSRC_stm32f1   += portable/stm32f1/lib/stm32f10x_rcc.c
CSRC_stm32f1   += kernel/FreeRTOS/Source/portable/GCC/ARM_CM3/port.c
CSRC_stm32f1   += net/port/arch/ethif_stm32f1.c

CXXSRC_stm32f1 += 

ASRC_stm32f1   += portable/stm32f1/cmx_startup.s

HDRLOC_stm32f1 += drivers/gpio
HDRLOC_stm32f1 += drivers/gpio/stm32f1
HDRLOC_stm32f1 += drivers/pll
HDRLOC_stm32f1 += drivers/uart
HDRLOC_stm32f1 += drivers/sdspi
HDRLOC_stm32f1 += drivers/ethmac
HDRLOC_stm32f1 += drivers/spi
HDRLOC_stm32f1 += drivers/crc
HDRLOC_stm32f1 += drivers/wdg
HDRLOC_stm32f1 += portable/lib/CMSIS
HDRLOC_stm32f1 += portable/stm32f1

####################################################################################################
# CORE SOURCES
####################################################################################################
include $(CORE_LOC)/Makefile
include $(FS_LOC)/Makefile
include $(KERNEL_LOC)/Makefile

CSRC_CORE += user/initd.c
CSRC_CORE += drivers/driver_registration.c

CSRC_CORE += ../programs/program_registration.c
CSRC_CORE += net/api/api_lib.c
CSRC_CORE += net/api/api_msg.c
CSRC_CORE += net/api/err.c
CSRC_CORE += net/api/netbuf.c
CSRC_CORE += net/api/netdb.c
CSRC_CORE += net/api/netifapi.c
CSRC_CORE += net/api/sockets.c
CSRC_CORE += net/api/tcpip.c
CSRC_CORE += net/core/def.c
CSRC_CORE += net/core/dhcp.c
CSRC_CORE += net/core/dns.c
CSRC_CORE += net/core/init.c
CSRC_CORE += net/core/mem.c
CSRC_CORE += net/core/memp.c
CSRC_CORE += net/core/netif.c
CSRC_CORE += net/core/pbuf.c
CSRC_CORE += net/core/raw.c
CSRC_CORE += net/core/stats.c
CSRC_CORE += net/core/sys.c
CSRC_CORE += net/core/tcp.c
CSRC_CORE += net/core/tcp_in.c
CSRC_CORE += net/core/tcp_out.c
CSRC_CORE += net/core/timers.c
CSRC_CORE += net/core/udp.c
CSRC_CORE += net/core/ipv4/autoip.c
CSRC_CORE += net/core/ipv4/icmp.c
CSRC_CORE += net/core/ipv4/igmp.c
CSRC_CORE += net/core/ipv4/inet.c
CSRC_CORE += net/core/ipv4/inet_chksum.c
CSRC_CORE += net/core/ipv4/ip.c
CSRC_CORE += net/core/ipv4/ip_addr.c
CSRC_CORE += net/core/ipv4/ip_frag.c
CSRC_CORE += net/netif/etharp.c
CSRC_CORE += net/port/arch/sys_arch.c

HDRLOC_CORE += kernel/FreeRTOS/Source/include
HDRLOC_CORE += kernel
HDRLOC_CORE += config
HDRLOC_CORE += portable
HDRLOC_CORE += include
HDRLOC_CORE += include/stdc
HDRLOC_CORE += include/stdc/sys
HDRLOC_CORE += net/include
HDRLOC_CORE += net/include/ipv4
HDRLOC_CORE += net/port
