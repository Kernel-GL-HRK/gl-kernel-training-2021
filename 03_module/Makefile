PWD := $(shell pwd)

ifneq ($(KERNELRELEASE),)
	obj-m := simple_module.o
else
	KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

check:
	$(KDIR)/scripts/checkpatch.pl -f simple_module.c
send:
	sudo cp simple_module.ko /srv/nfs/busybox/

endif
