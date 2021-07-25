#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h> /* gST */
#include <Library/UefiLib.h>                  /* Print */
#include <Guid/FileInfo.h>
#include <Protocol/SimpleFileSystem.h>



#include "wcat_boot_header.h"
#include "util.h"

#define PRIVATE static
#define PUBLIC



PUBLIC EFI_STATUS watchdogtimer_disable(){
    EFI_STATUS return_status;
    return_status = gST->BootServices->SetWatchdogTimer(0, 0, 0, NULL);
    return return_status;
}


#define MAX_FILE_BUF 1024 /* test用のファイルバッファの大きさ */
#define MAX_FILE_NAME_LEN 15	/* ルートディレクトリを表示する際のファイル名の */

/* ファイル名格納用構造体 */
struct FILE {
    uint16_t name[MAX_FILE_NAME_LEN];
} __attribute__((packed));

PUBLIC EFI_STATUS print_rootdir(EFI_FILE_PROTOCOL *root){
    UINTN buf_size;
    EFI_STATUS status;
    uint8_t file_buf[MAX_FILE_BUF];
    EFI_FILE_INFO *file_info;

    struct FILE file_list[10];
    
    
    int index = 0;

    Print(L"RootDirectory: ");
    
    /* ファイル名を繰り返すことで読み出している */
    while (1) {
        buf_size = MAX_FILE_BUF;
        status = root->Read(root, &buf_size, (void *)file_buf);    /* 疑似シェルのlsコマンドとする場合等はCloseが必要となるが現状はファイル名を読み出したいだけであるためなし */
        if (!buf_size) {
            break;
        }
        
        file_info = (EFI_FILE_INFO *)file_buf;
        StrnCopy(file_list[index].name, file_info->FileName, MAX_FILE_NAME_LEN - 1);
        file_list[index].name[MAX_FILE_NAME_LEN - 1] = L'\0';
        Print(file_list[index].name);
        Print(L" ");
        index++;
    }
    Print(L"\n");
    return status;
}
