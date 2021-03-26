PWD := $(shell pwd)

ifneq ($(KERNELRELEASE),)
	obj-m := rtc_ds3231x.o
else
	KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

check:
	$(KDIR)/scripts/checkpatch.pl -f rtc_ds3231x.c
send:
	sudo cp rtc_ds3231x.ko test.sh /srv/nfs/busybox/tmp

endif
