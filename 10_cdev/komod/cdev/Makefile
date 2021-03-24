obj-target = cdev_task
obj-m += $(obj-target)_mod.o
$(obj-target)_mod-objs := $(obj-target).o gl_procfs.o gl_sysfs.o
all:
	@echo "Kernel: building target $(OBJ).ko..."
	make -C ${BUILD_KERNEL} M=$(PWD) modules
clean:
	@echo "Kernel: cleaning..."
	make -C ${BUILD_KERNEL} M=$(PWD) clean
