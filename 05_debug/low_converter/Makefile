PWD := $(shell pwd)

ifneq ($(KERNELRELEASE),)
	obj-m := low_converter.o
else
	KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

check:
	$(KDIR)/scripts/checkpatch.pl -f low_converter.c
send:
	sudo cp low_converter.ko /srv/nfs/busybox/tmp

endif
