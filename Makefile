#
# Makefile
#

# Totsugekitaiさんのをベースにカスタム
# https://github.com/Totsugekitai/minOSv2/blob/master/Makefile

.PHONY: boot kernel run tool

SHELL = /bin/bash
QEMU = qemu-system-x86_64
OVMF = /tool/OVMF/OVMF.fd

ROOTDIR = $(CURDIR)
OVMF = $(ROOTDIR)/tool/OVMF/OVMF.fd
EDK_WORKSPACE = $(ROOTDIR)/tool/edk2

# default:

boot:
	$(MAKE) -C boot
	cp $(CURDIR)/tool/edk2/Build/WcatBoot/RELEASE_CLANG38/X64/WcatBootPkg/WcatBoot/WcatBoot/OUTPUT/WcatBoot.efi ./test/EFI/BOOT/BOOTX64.EFI
	

kernel:
	$(MAKE) -C kernel
	cp kernel/kernel.elf ./test	

run:
	$(QEMU) \
		-m 512 -serial mon:stdio -d cpu_reset -bios $(OVMF) -hda fat:rw:test -smbios type=1,family=tetoto

tool:
	cd ${EDK_WORKSPACE}; source edksetup.sh --reconfig;\
	build -p OvmfPkg/OvmfPkgX64.dsc -b RELEASE -a X64 -t CLANG38
	cp $(CURDIR)/tool/edk2/Build/OvmfX64/RELEASE_CLANG38/FV/OVMF.fd ./tool/OVMF/
	cp $(CURDIR)/tool/edk2/Build/OvmfX64/RELEASE_CLANG38/FV/OVMF_CODE.fd ./tool/OVMF/
	cp $(CURDIR)/tool/edk2/Build/OvmfX64/RELEASE_CLANG38/FV/OVMF_VARS.fd ./tool/OVMF/

setup:
	make tool

all:
	make boot
	make kernel
	make run

clean:
	make -C boot/ clean
	make -C kernel/ clean
	$(RM) test/kernel.elf
	$(RM) test/EFI/BOOT/BOOTX64.EFI
