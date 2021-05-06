#
# Makefile
#

# Totsugekitaiさんのをベースにカスタム
# https://github.com/Totsugekitai/minOSv2/blob/master/Makefile

.PHONY: boot kernel run
QEMU = qemu-system-x86_64

default:

boot:
	$(MAKE) -C boot
	cp $(CURDIR)/tool/edk2/Build/WcatBoot/RELEASE_CLANG38/X64/WcatBootPkg/WcatBoot/WcatBoot/OUTPUT/WcatBoot.efi ./test/EFI/BOOT/BOOTX64.EFI
	

kernel:
	$(MAKE) -C kernel
	cp kernel/kernel.elf ./test	

run:
	$(QEMU) \
		-m 512 -serial mon:stdio -d cpu_reset -bios tool/OVMF.fd -hda fat:rw:test

all:
	make boot
	make kernel
	make run

clean:
	make -C boot/ clean
	make -C kernel/ clean
	$(RM) test/kernel.elf
	$(RM) test/EFI/BOOT/BOOTX64.EFI
