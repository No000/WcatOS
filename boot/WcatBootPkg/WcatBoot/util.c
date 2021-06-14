#include "util.h"
#include <Library/UefiBootServicesTableLib.h> /* gST */
#include <Library/UefiLib.h>                  /* Print */
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Uefi.h>
#include <stdint.h>

#define PRIVATE static
#define PUBLIC




PUBLIC VOID StatusCheacker(EFI_STATUS status) {
    Print(L"[");
    if (EFI_ERROR(status)) {
        gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTRED);
        Print(L"%r", status);
    } else {
        gST->ConOut->SetAttribute(gST->ConOut, EFI_LIGHTGREEN);
        Print(L"%r", status);
    }
    gST->ConOut->SetAttribute(gST->ConOut, EFI_WHITE);
    Print(L"]");
}




/* MikanOSのブートローダーより引用 */
PUBLIC
EFI_STATUS
OpenRootDir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL **root) {
    EFI_STATUS status;
    EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;
    
    
    /* なぜはじめOpenVolumeでプロトコルを開けなかったか？ */
    /* EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
     * *SFSPは取得していたが、GUIDを使ってプロトコルの先頭アドレスを割り当てていなかったためプロトコルが開けなかった。
     */
    /* ST->BootServices->LocateProtocol(&sfsp_guid, NULL, (void **)&SFSP); */
    /* この処理が足りなかった */
    
    /* 自分自身へのイメージハンドルを取得している */
    /* OpenProtocolに関しては、UEFI Spec 2.8B May 2020のP188を参照 */
    /* これはHandleProtocolの拡張版 */
    /* 指定されたプロトコルをサポートしているかを判定し、サポートしているようであればプロトコルをオープンにする
     */
    
    /* 自身のハンドラを取得し */
    status = gBS->OpenProtocol(image_handle, &gEfiLoadedImageProtocolGuid,
                               (VOID **)&loaded_image, image_handle, NULL,
                               EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (EFI_ERROR(status)) {
        return status;
    }
    
    /* EFI_SIMPLE_FILESYSTEM_PROTOCOLを取得する */
    status = gBS->OpenProtocol(
                               loaded_image->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid,
                               (VOID **)&fs, image_handle, NULL, EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    if (EFI_ERROR(status)) {
        return status;
    }
    
    return fs->OpenVolume(fs, root);
}




/* strncpyだが名前衝突のエラーがうっとおしいため */
PUBLIC VOID StrnCopy(unsigned short *dst, unsigned short *src, unsigned long long n) {
    while (n--)
        *dst++ = *src++;
}




PUBLIC VOID Hlt() {
    while (1)
        __asm__("hlt");
}




VOID StallBranch(uint32_t boot_menu_index){
    if (boot_menu_index == 1){
        gST->BootServices->Stall(500000);
    }
}



