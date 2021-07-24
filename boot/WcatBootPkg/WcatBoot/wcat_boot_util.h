#ifndef _WCATBOOT_UTIL_H_
#define _WCATBOOT_UTIL_H_

#include <Uefi.h>
#include <Protocol/SimpleFileSystem.h>

/* watchdogタイマの無効化
   5分刻みで再起動してしまうのを防ぐ。 */
EFI_STATUS watchdogtimer_disable();
VOID print_rootdir(EFI_FILE_PROTOCOL *root);

#endif
