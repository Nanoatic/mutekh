
objs = main.o bitmap.o block.o joystick.o i2c.o

ifeq ($(CONFIG_DRIVER_LCD),defined)
objs += lcd.o
endif

ifeq ($(CONFIG_MUTEK_TIMERMS),defined)
objs += timer.o
endif

