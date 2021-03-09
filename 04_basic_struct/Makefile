PWD := $(shell pwd)

ifneq ($(KERNELRELEASE),)
	obj-m := basic_struct.o
else
	KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

check:
	$(KDIR)/scripts/checkpatch.pl -f basic_struct.c
send:
	sudo cp basic_struct.ko test.sh /srv/nfs/busybox/tmp

endif
