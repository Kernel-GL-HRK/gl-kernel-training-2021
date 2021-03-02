obj-m += hello_list.o

all:
	make -C ${BUILD_KERNEL} M=$(PWD) modules
clean:
	make -C ${BUILD_KERNEL} M=$(PWD) clean
