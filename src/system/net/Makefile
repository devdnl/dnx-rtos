# Makefile for GNU make
ifeq ($(__ENABLE_NETWORK__), _YES_)
   CSRC_CORE   += net/socket.c
   CSRC_CORE   += net/lwip/api/api_lib.c
   CSRC_CORE   += net/lwip/api/api_msg.c
   CSRC_CORE   += net/lwip/api/err.c
   CSRC_CORE   += net/lwip/api/netbuf.c
   CSRC_CORE   += net/lwip/api/netdb.c
   CSRC_CORE   += net/lwip/api/netifapi.c
   CSRC_CORE   += net/lwip/api/sockets.c
   CSRC_CORE   += net/lwip/api/tcpip.c
   CSRC_CORE   += net/lwip/core/def.c
   CSRC_CORE   += net/lwip/core/dhcp.c
   CSRC_CORE   += net/lwip/core/dns.c
   CSRC_CORE   += net/lwip/core/init.c
   CSRC_CORE   += net/lwip/core/mem.c
   CSRC_CORE   += net/lwip/core/memp.c
   CSRC_CORE   += net/lwip/core/netif.c
   CSRC_CORE   += net/lwip/core/pbuf.c
   CSRC_CORE   += net/lwip/core/raw.c
   CSRC_CORE   += net/lwip/core/stats.c
   CSRC_CORE   += net/lwip/core/sys.c
   CSRC_CORE   += net/lwip/core/tcp.c
   CSRC_CORE   += net/lwip/core/tcp_in.c
   CSRC_CORE   += net/lwip/core/tcp_out.c
   CSRC_CORE   += net/lwip/core/timers.c
   CSRC_CORE   += net/lwip/core/udp.c
   CSRC_CORE   += net/lwip/core/ipv4/autoip.c
   CSRC_CORE   += net/lwip/core/ipv4/icmp.c
   CSRC_CORE   += net/lwip/core/ipv4/igmp.c
   CSRC_CORE   += net/lwip/core/ipv4/inet.c
   CSRC_CORE   += net/lwip/core/ipv4/inet_chksum.c
   CSRC_CORE   += net/lwip/core/ipv4/ip.c
   CSRC_CORE   += net/lwip/core/ipv4/ip_addr.c
   CSRC_CORE   += net/lwip/core/ipv4/ip_frag.c
   CSRC_CORE   += net/lwip/netif/etharp.c
   CSRC_CORE   += net/lwip/port/arch/sys_arch.c
   CSRC_CORE   += net/lwip/port/arch/netman.c

	ifeq ($(TARGET), stm32f1)
   	CSRC_ARCH   += net/lwip/port/arch/netman_stm32f1.c
   endif
endif

HDRLOC_CORE += net/lwip/include/ipv4
HDRLOC_CORE += net/lwip/include
HDRLOC_CORE += net/lwip/port
