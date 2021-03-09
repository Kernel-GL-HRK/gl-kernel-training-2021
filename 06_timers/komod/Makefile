obj-target = timers_task
obj-m += $(obj-target)-mod.o
$(obj-target)-mod-objs := $(obj-target).o
all:
	@echo "Kernel: building target $(OBJ).ko..."
	make -C ${BUILD_KERNEL} M=$(PWD) modules
clean:
	@echo "Kernel: cleaning..."
	make -C ${BUILD_KERNEL} M=$(PWD) clean
