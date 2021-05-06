#
# EDK2でブートローダをビルドする

# TotsugekitaiさんのMakefileから引用し、改変。
# https://github.com/Totsugekitai/minOSv2/blob/master/boot/Makefile

# target.txtを動的にリンクするようにする

SHELL = /bin/bash

.PHONY: default clean

ROOTDIR = $(CURDIR)/..
EDK_WORKSPACE = $(ROOTDIR)/tool/edk2

default:
	echo $(EDK_WORKSPACE)
	ln -sf $(ROOTDIR)/boot/WcatBootPkg $(EDK_WORKSPACE)
	cd ${EDK_WORKSPACE}; source edksetup.sh --reconfig;\
	build -p WcatBootPkg/WcatBoot.dsc -b RELEASE -a X64 -t CLANG38

