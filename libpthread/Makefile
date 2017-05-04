
objs += thread.o

ifeq ($(CONFIG_PTHREAD_MUTEX), defined)
objs += mutex.o
endif

ifeq ($(CONFIG_PTHREAD_KEYS), defined)
objs += keys.o
endif

ifeq ($(CONFIG_PTHREAD_COND), defined)
objs += cond.o
endif

ifeq ($(CONFIG_PTHREAD_CANCEL), defined)
objs += cancel.o
endif

ifeq ($(CONFIG_PTHREAD_BARRIER), defined)
objs += barrier.o
endif

ifeq ($(CONFIG_PTHREAD_MAIN), defined)
objs += main_wrap.o
endif

doc_headers = \
	pthread.h
