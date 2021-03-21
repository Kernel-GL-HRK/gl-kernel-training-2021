PWD := $(shell pwd)

ifneq ($(KERNELRELEASE),)
	obj-m := mem_alloc.o
else
	KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

check:
	$(KDIR)/scripts/checkpatch.pl -f mem_alloc.c
send:
	sudo cp mem_alloc.ko test.sh /srv/nfs/busybox/tmp

endif
