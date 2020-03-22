# Makefile for GNU make
ifeq ($(__ENABLE_NETWORK__), _YES_)
   CSRC_CORE += net/netm.c
   
   ifeq ($(__ENABLE_TCPIP_STACK__), _YES_)
      ifeq ($(__NETWORK_TCPIP_STACK__), __STACK_LWIP__)
         CSRC_CORE   += net/inet/lwip/core/init.c
         CSRC_CORE   += net/inet/lwip/core/def.c
         CSRC_CORE   += net/inet/lwip/core/dns.c
         CSRC_CORE   += net/inet/lwip/core/inet_chksum.c
         CSRC_CORE   += net/inet/lwip/core/ip.c
         CSRC_CORE   += net/inet/lwip/core/mem.c
         CSRC_CORE   += net/inet/lwip/core/memp.c
         CSRC_CORE   += net/inet/lwip/core/netif.c
         CSRC_CORE   += net/inet/lwip/core/pbuf.c
         CSRC_CORE   += net/inet/lwip/core/raw.c
         CSRC_CORE   += net/inet/lwip/core/stats.c
         CSRC_CORE   += net/inet/lwip/core/sys.c
         CSRC_CORE   += net/inet/lwip/core/altcp.c
         CSRC_CORE   += net/inet/lwip/core/altcp_alloc.c
         CSRC_CORE   += net/inet/lwip/core/altcp_tcp.c
         CSRC_CORE   += net/inet/lwip/core/tcp.c
         CSRC_CORE   += net/inet/lwip/core/tcp_in.c
         CSRC_CORE   += net/inet/lwip/core/tcp_out.c
         CSRC_CORE   += net/inet/lwip/core/timeouts.c
         CSRC_CORE   += net/inet/lwip/core/udp.c
         CSRC_CORE   += net/inet/lwip/core/ipv4/autoip.c
         CSRC_CORE   += net/inet/lwip/core/ipv4/dhcp.c
         CSRC_CORE   += net/inet/lwip/core/ipv4/etharp.c
         CSRC_CORE   += net/inet/lwip/core/ipv4/icmp.c
         CSRC_CORE   += net/inet/lwip/core/ipv4/igmp.c
         CSRC_CORE   += net/inet/lwip/core/ipv4/ip4_frag.c
         CSRC_CORE   += net/inet/lwip/core/ipv4/ip4.c
         CSRC_CORE   += net/inet/lwip/core/ipv4/ip4_addr.c
         CSRC_CORE   += net/inet/lwip/api/api_lib.c
         CSRC_CORE   += net/inet/lwip/api/api_msg.c
         CSRC_CORE   += net/inet/lwip/api/err.c
         CSRC_CORE   += net/inet/lwip/api/if_api.c
         CSRC_CORE   += net/inet/lwip/api/netbuf.c
         CSRC_CORE   += net/inet/lwip/api/netdb.c
         CSRC_CORE   += net/inet/lwip/api/netifapi.c
         CSRC_CORE   += net/inet/lwip/api/sockets.c
         CSRC_CORE   += net/inet/lwip/api/tcpip.c
         CSRC_CORE   += net/inet/lwip/netif/ethernet.c
         CSRC_CORE   += net/inet/lwip/arch/inet_drv.c
         CSRC_CORE   += net/inet/lwip/arch/inet.c
         CSRC_CORE   += net/inet/lwip/arch/sys_arch.c
      
         HDRLOC_CORE += net/inet/lwip/include/ipv4
         HDRLOC_CORE += net/inet/lwip/include
         HDRLOC_CORE += net/inet/lwip/arch
         HDRLOC_CORE += net/inet/lwip/
      endif
   endif
   
   ifeq ($(__ENABLE_SIPC_STACK__), _YES_)
   	CSRC_CORE   += net/sipc/sipc.c
   	CSRC_CORE   += net/sipc/sipcbuf.c
   	HDRLOC_CORE += net/sipc
   endif
endif


