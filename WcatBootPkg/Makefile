#
# EDK2でブートローダをビルドする

# TotsugekitaiさんのMakefileから引用し、改変。
# https://github.com/Totsugekitai/minOSv2/blob/master/boot/Makefile

# target.txtを動的にリンクするようにする

.PHONY: default clean

ROOTDIR = $(CURDIR)/..
EDK_WORKSPACE = $(WORKSPACE)

default:
	echo $(EDK_WORKSPACE)
ifeq ($(EDK_WORKSPACE),)
	echo "$(error EDK PATH not set)"
else
	ln -sf $(ROOTDIR)/boot/MinLoaderPkg $(EDK_WORKSPACE)
	cd $(EDK_WORKSPACE); build
endif
