ifneq ($(KERNELRELEASE),)

# Kbuild part of makefile
obj-m := krnl_mem_test.o

else

# kernel sources

KDIR ?= $(HOME)/repos/linux-stable

default:
	$(MAKE) -C $(KDIR) M="$$PWD"

clean:
	$(MAKE) -C $(KDIR) M="$$PWD" $@

%.i %.s : %.c
	$(MAKE) -C $(KDIR) M=$$PWD $@

endif
