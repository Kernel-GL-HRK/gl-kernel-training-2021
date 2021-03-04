PWD := $(shell pwd)

ifneq ($(KERNELRELEASE),)
	obj-m := time_oprev.o
else
	KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

check:
	$(KDIR)/scripts/checkpatch.pl -f time_oprev.c
send:
	sudo cp time_oprev.ko test.sh /srv/nfs/busybox/tmp

endif
