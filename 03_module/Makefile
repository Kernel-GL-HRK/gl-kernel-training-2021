ifneq ($(KERNELRELEASE),)
# kbuild part of makefile
obj-m   := firstmod.o
else
# normal makefile

obj-m   := firstmod.o
default:
	$(MAKE) -C $(KDIR) M=$$PWD modules
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
endif
