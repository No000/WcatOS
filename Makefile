#
# Makefile
#

# Totsugekitaiさんのをベースにカスタム
# https://github.com/Totsugekitai/minOSv2/blob/master/Makefile

.PHONY: boot kernel run tool

SHELL = /bin/bash


QEMU = qemu-system-x86_64
# メモリ指定
QEMU_OPTION += -m 512
# シリアル通信をコンソールに接続
QEMU_SERIAL = -serial mon:stdio
QEMU_OPTION += -d cpu_reset -bios $(OVMF) -hda fat:rw:test
# SMBIOSの設定
QEMU_OPTION += -smbios type=1,family=tetoto
# KVMサポート
QEMU_OPTION += -enable-kvm -machine type=pc,accel=kvm
# マルチコアの設定
QEMU_OPTION += -smp 2


ROOTDIR = $(CURDIR)
OVMF = $(ROOTDIR)/tool/OVMF/OVMF.fd
EDK_WORKSPACE = $(ROOTDIR)/tool/edk2

# default:
all:
	$(MAKE) boot
	$(MAKE) kernel
	$(MAKE) run

boot:
	$(MAKE) -C boot
	cp $(CURDIR)/tool/edk2/Build/WcatBoot/RELEASE_CLANG38/X64/WcatBootPkg/WcatBoot/WcatBoot/OUTPUT/WcatBoot.efi ./test/EFI/BOOT/BOOTX64.EFI
	

kernel:
	$(MAKE) -C kernel
	cp kernel/kernel.elf ./test	

run:
	$(QEMU) $(QEMU_OPTION) $(QEMU_SERIAL)

tool:
	cd ${EDK_WORKSPACE}; source edksetup.sh --reconfig;\
	build -p OvmfPkg/OvmfPkgX64.dsc -b RELEASE -a X64 -t CLANG38
	cp $(CURDIR)/tool/edk2/Build/OvmfX64/RELEASE_CLANG38/FV/OVMF.fd ./tool/OVMF/
	cp $(CURDIR)/tool/edk2/Build/OvmfX64/RELEASE_CLANG38/FV/OVMF_CODE.fd ./tool/OVMF/
	cp $(CURDIR)/tool/edk2/Build/OvmfX64/RELEASE_CLANG38/FV/OVMF_VARS.fd ./tool/OVMF/

setup:
	$(MAKE) tool



clean:
	$(MAKE) -C boot/ clean
	$(MAKE) -C kernel/ clean
	$(RM) test/kernel.elf
	$(RM) test/EFI/BOOT/BOOTX64.EFI
