ifneq ($(KERNELRELEASE),)
# kbuild part of makefile
obj-m   := chardev.o
else
# normal makefile

obj-m   := chardev.o
default:
	$(MAKE) -C $(KDIR) M=$$PWD modules
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
endif
