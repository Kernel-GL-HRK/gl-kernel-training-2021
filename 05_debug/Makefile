obj-m += debug_task_mod.o
debug_task_mod-objs := debug_task.o
all:
	make -C ${BUILD_KERNEL} M=$(PWD) modules
clean:
	make -C ${BUILD_KERNEL} M=$(PWD) clean
