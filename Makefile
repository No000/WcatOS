#
# Makefile
#


.PHONY: default setup boot kernel run clean
QEMU = qemu-system-x86_64

default:

setup:
	mkdir test/EFI/BOOT/

boot:
	make -C boot/
	cp $(WORKSPACE)/Build/WcatBoot/RELEASE_CLANG38/X64/WcatBootPkg/WcatBoot/WcatBoot/OUTPUT/WcatBoot.efi ./test/EFI/BOOT/BOOTX64.EFI

kernel:
	make -C kernel/
	cp kernel/kernel.elf ./test

run:
	$(QEMU) \
		-m 512 -serial mon:stdio -d cpu_reset -bios tool/OVMF.fd -hda fat:rw:hdd
