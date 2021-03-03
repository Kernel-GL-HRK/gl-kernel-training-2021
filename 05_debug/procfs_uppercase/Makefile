BBB=1
LOCAL=0
TARGET = BBB

ifneq ($(KERNELRELEASE),)

# kbuild part of makefile
obj-m := procfs_uppercase_module.o

else

ifeq ($(TARGET),BBB) 

REMDIR ?= /srv/nfs/busybox/
KDIR ?= ~/repos/linux-stable/

else

REMDIR = ~
KDIR ?= /lib/modules/`uname -r`/build

endif

default:
	$(MAKE) -C $(KDIR) M=$$PWD

check:
	~/repos/linux-stable/scripts/checkpatch.pl --no-tree -f *.c | tee checkpatch.log

deploy:
	sudo cp *.ko $(REMDIR)

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean

endif
