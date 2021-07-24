/* 各種ブートローダーの便利な関数をまとめたクラス */

#ifndef _UTIL_H_
#define _UTIL_H_
#include  <Uefi.h>
#include <Protocol/SimpleFileSystem.h>
#include <stdint.h>

VOID StrnCopy(unsigned short *dst, unsigned short *src, unsigned long long n);
VOID Hlt();
VOID clear_screen();
VOID StatusCheacker(EFI_STATUS status);
EFI_STATUS OpenRootDir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL **root);
VOID StallBranch(uint32_t boot_menu_index);
EFI_INPUT_KEY efi_wait_any_key();
VOID error_hlt(EFI_STATUS status);

#endif
