
doc_headers = crypto/crypto.h crypto/arc4.h crypto/crc32.h

ifeq ($(CONFIG_LIBCRYPTO_ARC4), defined)
objs += arc4.o
endif

ifeq ($(CONFIG_LIBCRYPTO_CRC32), defined)
objs += crc32.o
endif

ifeq ($(CONFIG_LIBCRYPTO_MD5), defined)
objs += md5.o
endif

