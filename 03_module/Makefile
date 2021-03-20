ifneq ($(KERNELRELEASE),)
# kbuild part of makefile
obj-m   := firstmod.o
else
# normal makefile

obj-m   := firstmod.o
default:
	$(MAKE) -C $(BUILD_KERNEL) M=$(CURDIR) modules
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
endif
