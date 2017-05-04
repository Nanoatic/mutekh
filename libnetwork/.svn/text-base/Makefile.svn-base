
objs += protocols.o packet.o if.o route.o dispatch.o

ifeq ($(CONFIG_NETWORK_FILTER), defined)
 objs += filter.o
endif

subdirs = protocols

ifeq ($(CONFIG_NETWORK_UDP), defined)
 subdirs += libudp
endif

ifeq ($(CONFIG_NETWORK_TCP), defined)
 subdirs += libtcp
endif

ifeq ($(CONFIG_NETWORK_SOCKET), defined)
 subdirs += libsocket
endif

ifeq ($(CONFIG_NETWORK_NFS), defined)
 subdirs += lwnfs
endif

ifeq ($(CONFIG_NETWORK_RARP), defined)
 subdirs += rarp
endif

ifeq ($(CONFIG_NETWORK_PING), defined)
 subdirs += ping
endif

ifeq ($(CONFIG_NETWORK_DHCLIENT), defined)
 subdirs += dhclient
endif


