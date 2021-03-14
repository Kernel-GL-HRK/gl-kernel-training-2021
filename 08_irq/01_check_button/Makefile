PWD := $(shell pwd)

ifneq ($(KERNELRELEASE),)
	obj-m := check_button.o
else
	KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

check:
	$(KDIR)/scripts/checkpatch.pl -f check_button.c
send:
	sudo cp check_button.ko /srv/nfs/busybox/tmp

endif
