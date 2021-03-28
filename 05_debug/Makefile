ifneq ($(KERNELRELEASE),)
# kbuild part of makefile
obj-m   := task05_sys.o task05_proc.o
else
# normal makefile

obj-m   := task05_sys.o task05_proc.o
default:
	$(MAKE) -C $(KDIR) M=$$PWD modules
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
endif
