ifneq ($(KERNELRELEASE),)
# kbuild part of makefile
obj-m   := task04_1.o
else
# normal makefile

obj-m   := task04_1.o
default:
	$(MAKE) -C $(BUILD_KERNEL) M=$(CURDIR) modules
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
endif
