ifneq ($(KERNELRELEASE),)
# kbuild part of makefile
obj-m   := task04.o
else
# normal makefile

obj-m   := task04.o
default:
	$(MAKE) -C $(KDIR) M=$$PWD modules
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
endif
