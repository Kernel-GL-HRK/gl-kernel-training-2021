PWD := $(shell pwd)

ifneq ($(KERNELRELEASE),)
	obj-m := up_converter.o
else
	KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

check:
	$(KDIR)/scripts/checkpatch.pl -f up_converter.c
send:
	sudo cp up_converter.ko /srv/nfs/busybox/tmp

endif
