PWD := $(shell pwd)

ifneq ($(KERNELRELEASE),)
	obj-m := irq_button.o
else
	KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

check:
	$(KDIR)/scripts/checkpatch.pl -f irq_button.c
send:
	sudo cp irq_button.ko /srv/nfs/busybox/tmp

endif
