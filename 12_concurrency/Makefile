PWD := $(shell pwd)

ifneq ($(KERNELRELEASE),)
	obj-m := concurency.o
else
	KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

check:
	$(KDIR)/scripts/checkpatch.pl -f concurency.c
send:
	sudo cp concurency.ko test.sh /srv/nfs/busybox/tmp

endif
