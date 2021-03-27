# (!) using paths with spaces may not work with Kbuild

TARGET = driver
TEST_FILE = test.sh

# this is what is used by Kbuild
obj-m += $(TARGET).o
#$(TARGET)-objs := $(TARGET).o ds3231.o

V ?= 2

# directory containing Makefile for kernel build
KBUILDDIR ?= ~/BBB/kernel/linux-5.10.14
REMDIR ?= /srv/nfs/busybox/modules
PWD = $(shell pwd)

# Do not print "Entering directory" on recursive make call if not verbose
MAKEFLAGS += $(if $(value V),,--no-print-directory)

EXTRA_CFLAGS += -std=gnu11

export PATH := /opt/gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf/bin/:$(PATH)
export CROSS_COMPILE := arm-none-linux-gnueabihf-
#export ARCH := arm	

.PHONY: modules clean tidy run module_deploy test_deploy check_patch format build_and_deploy

# recur to the original kernel Makefile with some additions
modules: tidy
	$(MAKE) -C "$(KBUILDDIR)" M="$(PWD)" V=$(V) modules

tidy:
	$(MAKE) -C "$(KBUILDDIR)" M="$(PWD)" V=$(V) clean

# we link original clean to target named tidy
clean:
	-rm -rf .tmp_versions
	-rm -f modules.order .modules.order.cmd Module.symvers .Module.symvers.cmd
	-rm -f $(obj-m) $(obj-m:.o=.mod) $(obj-m:.o=.mod.o) $(obj-m:.o=.mod.c) .$(obj-m:.o=.mod.cmd)
	-rm -f $(addsuffix .cmd,$(addprefix .,$(obj-m)))
	-rm -f $(addsuffix .cmd,$(addprefix .,$(obj-m:.o=.ko)))
	-rm -f $(addsuffix .cmd,$(addprefix .,$(obj-m:.o=.mod.o)))

module_deploy:
	sudo cp $(TARGET).ko $(REMDIR)

test_deploy:
	sudo cp $(TEST_FILE) $(REMDIR)


check_patch:
	$(KBUILDDIR)/scripts/checkpatch.pl -f --no-tree $(obj-m:.o=.c)


format:
	clang-format -verbose -i $(TARGET).c -style=file

build_and_deploy: | modules module_deploy
