NAME := chdev
DEPLOY_DIR := 10_chdev
NFS_PATH ?= /srv/nfs/busybox
#KDIR ?= /lib/modules/`uname -r`/build
KDIR ?= ~/repos/busybox/_install/lib/modules/4.19.176/build

export PATH := /opt/gcc-arm-8.3-2019.03-x86_64-arm-eabi/bin:$(PATH)
export CROSS_COMPILE := ccache arm-eabi-
export ARCH := arm

ifneq ($(KERNELRELEASE),)
# kbuild part of makefile
	obj-m := $(NAME).o
else
# normal makefile
all:
	$(MAKE) -C $(KDIR) M=$$PWD 
clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
%.i %.s : %.c
	$(MAKE) -C $(KDIR) M=$$PWD $@
deploy: 
	cp ./$(NAME).ko $(NFS_PATH)/workdir/$(DEPLOY_DIR)/
check: 
	$(KDIR)/scripts/checkpatch.pl --no-tree -f ./$(NAME).c
clean_white: 
	$(KDIR)/scripts/cleanfile ./$(NAME).c
endif
