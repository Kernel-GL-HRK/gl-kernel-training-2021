ifneq ($(KERNELRELEASE),)

# Kbuild part of makefile
obj-m := onboard_io.o 

else

# kernel sources

KDIR ?= $(HOME)/repos/linux-stable
REMDIR ?= /srv/nfs/busybox/

default:
	$(MAKE) -C $(KDIR) M="$$PWD"

clean:
	$(MAKE) -C $(KDIR) M="$$PWD" $@

%.i %.s : %.c
	$(MAKE) -C $(KDIR) M=$$PWD $@

deploy:
	sudo cp *.ko $(REMDIR)

endif
