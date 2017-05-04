objs += socket.o inet.o

ifeq ($(CONFIG_NETWORK_UDP), defined)
  objs += socket_udp.o
endif

ifeq ($(CONFIG_NETWORK_TCP), defined)
  objs += socket_tcp.o
endif

ifeq ($(CONFIG_NETWORK_SOCKET_PACKET), defined)
  objs += socket_packet.o
endif

ifeq ($(CONFIG_NETWORK_SOCKET_RAW), defined)
  objs += socket_raw.o
endif


