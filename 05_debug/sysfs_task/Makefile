obj-m += 05_debug_sysfs.o

BUILD_KERNEL := ~/repos/busybox/_install/lib/modules/4.19.177/build
KDIR = ~/repos/linux-stable

all:
	make -C $(BUILD_KERNEL) M=$(PWD) modules
clean:
	make -C $(BUILD_KERNEL) M=$(PWD) clean
check_error:
	$(KDIR)/scripts/checkpatch.pl --no-tree -f 05_debug_sysfs.c 
