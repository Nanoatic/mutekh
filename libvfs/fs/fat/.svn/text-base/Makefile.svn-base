objs += fat-dirent.o fat-file.o fat-sector-cache.o fat.o

ifeq ($(CONFIG_DRIVER_FS_FAT16), defined)
objs += fat16.o
endif

ifeq ($(CONFIG_DRIVER_FS_FAT32), defined)
objs += fat32.o
endif

ifeq ($(CONFIG_DRIVER_FS_FAT_LFN), defined)
objs += fat-lfn.o
else
objs += fat-83.o
endif
