# Makefile for GNU make
ifeq ($(ENABLE_NETWORK), YES)
   CSRC_CORE   += net/api/api_lib.c
   CSRC_CORE   += net/api/api_msg.c
   CSRC_CORE   += net/api/err.c
   CSRC_CORE   += net/api/netbuf.c
   CSRC_CORE   += net/api/netdb.c
   CSRC_CORE   += net/api/netifapi.c
   CSRC_CORE   += net/api/sockets.c
   CSRC_CORE   += net/api/tcpip.c
   CSRC_CORE   += net/core/def.c
   CSRC_CORE   += net/core/dhcp.c
   CSRC_CORE   += net/core/dns.c
   CSRC_CORE   += net/core/init.c
   CSRC_CORE   += net/core/mem.c
   CSRC_CORE   += net/core/memp.c
   CSRC_CORE   += net/core/netif.c
   CSRC_CORE   += net/core/pbuf.c
   CSRC_CORE   += net/core/raw.c
   CSRC_CORE   += net/core/stats.c
   CSRC_CORE   += net/core/sys.c
   CSRC_CORE   += net/core/tcp.c
   CSRC_CORE   += net/core/tcp_in.c
   CSRC_CORE   += net/core/tcp_out.c
   CSRC_CORE   += net/core/timers.c
   CSRC_CORE   += net/core/udp.c
   CSRC_CORE   += net/core/ipv4/autoip.c
   CSRC_CORE   += net/core/ipv4/icmp.c
   CSRC_CORE   += net/core/ipv4/igmp.c
   CSRC_CORE   += net/core/ipv4/inet.c
   CSRC_CORE   += net/core/ipv4/inet_chksum.c
   CSRC_CORE   += net/core/ipv4/ip.c
   CSRC_CORE   += net/core/ipv4/ip_addr.c
   CSRC_CORE   += net/core/ipv4/ip_frag.c
   CSRC_CORE   += net/netif/etharp.c
   CSRC_CORE   += net/port/arch/sys_arch.c

	ifeq ($(TARGET), stm32f1)
   	CSRC_ARCH   += net/port/arch/ethif_stm32f1.c
   endif
endif

HDRLOC_CORE += net/include/ipv4
HDRLOC_CORE += net/include
HDRLOC_CORE += net/port