ifneq ($(KERNELRELEASE),)
# kbuild part of makefile
	obj-m := debug_sysfs.o
else
# normal makefile
KDIR ?= /lib/modules/`uname -r`/build
default:
	$(MAKE) -C $(KDIR) M=$$PWD
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
endif

