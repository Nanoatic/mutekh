
objs = alloc.o barrier.o critical.o env.o iter.o      \
iter_ull.o loop.o loop_ull.o ordered.o parallel.o sections.o single.o   \
task.o team.o work.o

ifeq ($(CONFIG_OPENMP_USE_POSIX), defined)
objs += posix/affinity.o posix/bar.o posix/lock.o posix/proc.o posix/time.o
endif

DIR_CFLAGS=-I $(LOCAL_SRC_DIR)

