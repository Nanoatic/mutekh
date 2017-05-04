objs =

ifeq ($(CONFIG_NETWORK_IPV4), defined)
 objs += arp.o ipv4.o icmpv4.o
endif

ifeq ($(CONFIG_NETWORK_UDP), defined)
 objs += udp.o
endif

ifeq ($(CONFIG_NETWORK_TCP), defined)
 objs += tcp.o
endif


