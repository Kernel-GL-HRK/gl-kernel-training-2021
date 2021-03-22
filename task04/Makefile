ifneq ($(KERNELRELEASE),)
# kbuild part of makefile
obj-m   := task04.o
else
# normal makefile

obj-m   := task04.o
default:
	$(MAKE) -C $(KDIR) M=$$PWD modules
check:
	$(KDIR)/scripts/checkpatch.pl -f task04.c
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
endif
