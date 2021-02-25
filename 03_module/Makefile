BBB=1
LOCAL=0
TARGET ?= LOCAL

ifneq ($(KERNELRELEASE),)

# kbuild part of makefile
obj-m := first_module.o

else

ifeq ($(TARGET),BBB) 

REMDIR = /srv/nfs/busybox/home
KDIR = ~/Projects/repos/linux/kernel/linux-stable/

else

REMDIR = ~
KDIR = /lib/modules/`uname -r`/build

endif

default:
	$(MAKE) -C $(KDIR) M=$$PWD

deploy:
	sudo cp *.ko $(REMDIR)

chpatch:
	$(KDIR)/scripts/checkpatch.pl --no-tree -f *.c > checkpatch.log

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

endif

