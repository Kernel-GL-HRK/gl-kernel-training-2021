# (!) using paths with spaces may not work with Kbuild

# this is what is used by Kbuild
obj-m += khmod.o

V ?= 2

# directory containing Makefile for kernel build
KBUILDDIR ?= ../../repos/linux-stable
REMDIR ?= /srv/nfs/busybox/home

# Do not print "Entering directory" on recursive make call if not verbose
MAKEFLAGS += $(if $(value V),,--no-print-directory)

.PHONY: modules clean tidy module_deploy, tests_deploy set_env

# recur to the original kernel Makefile with some additions
modules: set_env
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
	sudo cp *.ko $(REMDIR)

tests_deploy:
	sudo cp *.sh $(REMDIR)

check_patch:
	"$(KBUILDDIR)"/scripts/checkpatch.pl -f $(obj-m:.o=.c)

set_env:
	#export PATH=/opt/gcc-arm-8.3-2019.03-x86_64-arm-eabi/bin:$$PATH
	#export CROSS_COMPILE='ccache arm-eabi-'
	#export ARCH=arm
