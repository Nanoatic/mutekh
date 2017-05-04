
objs = main.o

ifeq ($(CONFIG_SRL_SOCLIB),defined)
objs += srl_log.o hw_init.o
endif

ifeq ($(CONFIG_PTHREAD),defined)
objs += posix_wait_cycles.o
else
objs += barrier.o sched_wait.o
endif

doc_headers = \
	srl/srl_barrier.h srl/srl_lock.h srl/srl_memspace.h						\
	srl/srl_public_types.h srl/srl_system_functions.h srl/srl_hw_helpers.h	\
	srl/srl_log.h srl/srl_mwmr.h srl/srl_sched_wait.h srl_private_types.h
