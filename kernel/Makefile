#
# kernel.elf生成用のMakefile
#

# マクロメモ
# $@:ターゲット名
# $M:依存ファイルの先頭のファイル名

CC = clang
CFLAGS = -O2 \
		-Wall \
		-g \
		--target=x86_64-elf \
		-ffreestanding \
		-mno-red-zone \
		-fno-exceptions \
		-fno-rtti \
		-std=c17


LD = ld.lld
LDFLAGS = 	--entry kernel_main \
			-z norelro \
			--image-base 0x100000 \
			--static

.PHONY: clean

kernel.o: kernel.c font.h
	$(CC) $(CFLAGS) -c $<

graphic.o: graphic.c graphic.h
	$(CC) $(CFLAGS) -c $<

kernel.elf: kernel.o graphic.o
	$(LD) $(LDFLAGS) -o $@ $^

clean:
	$(RM) -f *.o
	$(RM) -f *.elf
