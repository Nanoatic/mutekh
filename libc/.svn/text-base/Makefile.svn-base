objs =						\
string.o					\
alloc.o						\
intconv.o					\
printf.o					\
rand.o						\
error.o						\
qsort.o						\
stdlib.o					\
unistd.o					\
formatter.o

ifeq ($(CONFIG_LIBC_ASSERT), defined)
objs += assert.o
endif

ifeq ($(CONFIG_LIBC_STREAM), defined)
objs += stdio.o
endif

ifeq ($(CONFIG_LIBC_FORMATTER_FLOAT), defined)
objs += formatter_float.o
endif

doc_headers = \
	sys/stat.h ctype.h unistd.h	\
	semaphore.h fcntl.h errno.h stdio.h time.h stdarg.h	\
	stddef.h stdint.h stdlib.h setjmp.h string.h assert.h limits.h \
        libc/formatter.h
