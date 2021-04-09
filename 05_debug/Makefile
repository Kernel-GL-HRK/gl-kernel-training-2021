TARGET = debug_module
obj-m+=$(TARGET).o
default:
	$(MAKE) -C $(BUILD_KERNEL) M=$(CURDIR) modules

clean:
	$(MAKE) -C $(BUILD_KERNEL) M=$(CURDIR) clean
