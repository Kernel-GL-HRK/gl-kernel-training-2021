obj-m += 06_timer_module.o

BUILD_KERNEL := ~/repos/busybox/_install/lib/modules/4.19.177/build
KDIR = ~/repos/linux-stable

all:
	make -C $(BUILD_KERNEL) M=$(PWD) modules
clean:
	make -C $(BUILD_KERNEL) M=$(PWD) clean
test:
	sudo dmesg -C
	sudo insmod 06_timer_module.ko
	sudo rmmod 06_timer_module.ko
	dmesg
check_error:
	$(KDIR)/scripts/checkpatch.pl --no-tree -f 06_timer_module.c 
