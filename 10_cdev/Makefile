PWD := $(shell pwd)

obj-m += chdev_mod.o
chdev_mod-objs := chrdev.o procfs_entry.o sysfs_entry.o

default:
	$(MAKE) -C $(KDIR) M=$(PWD)
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

check:
	$(KDIR)/scripts/checkpatch.pl -f chrdev.c procfs_entry.c sysfs_entry.c
send:
	sudo cp chdev_mod.ko test.sh /srv/nfs/busybox/tmp

