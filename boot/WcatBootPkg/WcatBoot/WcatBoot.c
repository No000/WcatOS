/* memo */
/* ファイルの中身を読み出す場合、簡易的なテスト目的であればBufferでいいかもしれないが */
/* 本格的に読み出す場合、メモリを確保する必要がある(gBS->AllocatePool) */
/* Qemuのブート */
/* qemu-system-x86_64 -m 512 -serial mon:stdio -d cpu_reset -bios OVMF.fd -hda fat:rw:hdd */
/* テスト用疑似kernelのコンパイル */
/* clang -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -std=c17 -c kernel.c */
/* ld.lld --entry kernel_main -z norelro --image-base 0x100000 --static -o kernel.elf kernel.o */
/* statusチェックを光らせたいので、関数を作っていおきかえる */
/* 12/21 コードがツメツメで読みにくいので、余裕があるようにする */
/* 12/21 ClearscreenとPrintを高速で繰り返せば選択画面を作れそう。（キー入力があれば、内容を書き換えた画面を表示） */
/* 数値の出力にはEFI_LIGHTCYANでシアン色に変更することにする */

#include "elf_header.h"
#include "menu/menu.h"
#include "smbios.h"
#include "wcat_boot_header.h"


#include <Guid/FileInfo.h>
#include <Guid/SmBios.h> /* SMBIOS */
#include <IndustryStandard/SmBios.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h> /* gST */
#include <Library/UefiLib.h>                  /* Print */
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/DiskIo2.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/SimpleTextOut.h> /* 追加(いらんかも？) */
#include <Uefi.h>
#include <stdint.h>

#include "util.h"
#include "wcat_boot_util.h"
/* ELFヘッダーは */

EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SFSP;
EFI_SYSTEM_TABLE *ST;
/* ------------------------------------------------------------ */

/* 追加 */
/* ここはfile_infoを利用して動的に取得できるようにする */
#define MAX_FILE_NUM 10



/* このMemoryMapはヘッダーファイル化してkernelにもincludeさせる必要があるかもしれない
 */
/* mikanosのkernel/memory_map.hppを参考に作成 */
struct MemoryMap {
    unsigned long long buffer_size;
    void *buffer;
    unsigned long long map_size;
    unsigned long long map_key;
    unsigned long long descriptor_size;
    uint32_t descriptor_version;
};



EFI_STATUS
EFIAPI
UefiMain(EFI_HANDLE ImageHandle,EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS status; /* 各種EFI_STATUSの返り値を格納する変数 */
    EFI_FILE_PROTOCOL *root; /* rootを呼び出す */
    uint32_t stall_flag = 0;    /* Stall用のフラグ */
    struct WCAT_HEADER wcat_boot_information;
    
    status = watchdogtimer_disable();
    StatusCheacker(status);
    
    /* smbiosのinitプロセス */
    wcat_boot_information.smbios_address = ((uint64_t)(find_efi_smbios_table()));


    /* GOP初期化設定 */
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;
    UINTN num_gop_handles = 0;	/*  */
    EFI_HANDLE *gop_handles = NULL;

    
    status = gST->BootServices->LocateHandleBuffer(ByProtocol,
                                                   &gEfiGraphicsOutputProtocolGuid,
                                                   NULL,
                                                   &num_gop_handles,
                                                   &gop_handles);
    StatusCheacker(status);
    
    
    
    status = gST->BootServices->OpenProtocol(gop_handles[0],
                                             &gEfiGraphicsOutputProtocolGuid,
                                             (VOID **)&gop,
                                             ImageHandle,
                                             NULL,
                                             EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
    StatusCheacker(status);
    FreePool(gop_handles);		/* この処理が何なのかが気になる */

    /* カーネルに渡すグラフィックのデータ(init処理) */
    wcat_boot_information.video_information.frame_buffer_addr = (uint8_t *)gop->Mode->FrameBufferBase;
    wcat_boot_information.video_information.frame_buffer_size = (uint64_t)gop->Mode->FrameBufferSize;
    wcat_boot_information.video_information.horizen_size = (uint32_t)gop->Mode->Info->HorizontalResolution;
    wcat_boot_information.video_information.vertical_size = (uint32_t)gop->Mode->Info->VerticalResolution;
    wcat_boot_information.video_information.pixel_per_scanline = (uint32_t)gop->Mode->Info->PixelsPerScanLine;

    /* メニュー表示の処理 */
    boot_menu(&stall_flag);

    /* ルートディレクトリ展開処理 */
    clear_screen();
    StallBranch(stall_flag);
    status = OpenRootDir(ImageHandle, &root);
    StatusCheacker(status); /* statusチェック */
    Print(L"    OpenRootDir\n");
    error_hlt(status);
    StallBranch(stall_flag);


    /* ルートディレクトリのファイル・ディレクトリをダンプ */
    status = print_rootdir(root);
    StallBranch(stall_flag);
    StatusCheacker(status); /* statusチェック */
    Print(L"    Read status\n");
    error_hlt(status);
    StallBranch(stall_flag);
    
    /* Elfファイルの情報の抜き出しを行う */
    /* 流れは */
    /* １，kernelを解析するためにバイナリデータをメモリ上に展開するバッファを確保する */
    /* ２，バッファ内にkernelをいれる */
    /* ４，定義したElfheaderの構造体に紐づける */
    /* ５，構造体経由で読み込みを行う */
    /* ELFヘッダーの構造体は別のヘッダーファイルで定義する */
    
    /* kernelのファイルを開く */
    EFI_FILE_PROTOCOL *kernel_file;
    
    Print(L"kernelfile is kernel.elf\n");
    status = root->Open(root, &kernel_file, L"\\kernel.elf", EFI_FILE_MODE_READ, 0); /* kernelのファイルを読み出す*/
    
    StallBranch(stall_flag);
    
    StatusCheacker(status); /* statusチェック */
    Print(L"    kernelfile open\n");
    error_hlt(status);
    
    StallBranch(stall_flag);
    
    VOID *kernel_buffer; /* kernelのバイナリ読み出し用 */
    UINTN kernel_file_info_size = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 12;
    UINT8 kernel_file_info_buf[kernel_file_info_size];
    status = kernel_file->GetInfo(kernel_file, &gEfiFileInfoGuid,
                                  &kernel_file_info_size, kernel_file_info_buf);
    StatusCheacker(status); /* statusチェック */
    Print(L"    Getinfo\n");
    error_hlt(status);
    
    EFI_FILE_INFO *kerne_file_info = (EFI_FILE_INFO *)kernel_file_info_buf;
    UINTN kernel_file_size = kerne_file_info->FileSize;
    
    StallBranch(stall_flag);
    
    status = gBS->AllocatePool(EfiLoaderData, kernel_file_size, (void **)&kernel_buffer);
    StatusCheacker(status); /* statusチェック */
    Print(L"    AllocatePool\n");
    error_hlt(status);
    
    StallBranch(stall_flag);
    
    status = kernel_file->Read(kernel_file, &kernel_file_size, kernel_buffer);
    
    StatusCheacker(status); /* statusチェック */
    Print(L"    kernelRead\n");
    if (EFI_ERROR(status)) {
        Hlt();
    }
    
    
    StallBranch(stall_flag);
    
    uint8_t *kernele_buf_test = (uint8_t *)kernel_buffer;
    int i;
    Print(L"Magic Number:");
    for (i = 0; i < 16; i++) {
        StallBranch(stall_flag);
        Print(L"%02x ", kernele_buf_test[i]);
    }
    Print(L"\n");
    
    /* アドレスの計算を行う */
    Elf64_Ehdr *kernel_ehdr = (Elf64_Ehdr *)kernel_buffer;
    StallBranch(stall_flag);
    
    Print(L"entrypoint address: ");
    Print(L"%08x\n", kernel_ehdr->e_entry);
    
    /* 以下にELF形式の */
    /* p_offsetを記載 */
    StallBranch(stall_flag);
    
    
    Print(L"programheader offset:");
    Print(L"%08x\n", kernel_ehdr->e_phoff);
    
    /* プログラムヘッダーのエントリの数 */
    StallBranch(stall_flag);
    
    Print(L"programheader number:");
    Print(L"%08x\n", kernel_ehdr->e_phnum);
    

    UINT64 kernel_first_address, kernel_last_address;
    /* プログラムヘッダーの位置の計算を行う */
    Elf64_Phdr *phdr = (Elf64_Phdr *)((UINT64)kernel_ehdr + kernel_ehdr->e_phoff);
    kernel_first_address = MAX_UINT64;
    kernel_last_address = 0;
    
    StallBranch(stall_flag);
    
    Print(L"PT_LOAD: %d\n", PT_LOAD);
    
    for (uint16_t i = 0; i < kernel_ehdr->e_phnum; ++i) {
        if (phdr[i].p_type != PT_LOAD)
            continue; /* これはELFセグメントの型であり配列情報の解釈方法、PT_LOADを意味してる */
        kernel_first_address = MIN(kernel_first_address, phdr[i].p_vaddr); /* プログラムヘッダの情報から求める */
        kernel_last_address = MAX(kernel_last_address, phdr[i].p_vaddr + phdr[i].p_memsz); /* プログラムヘッダの情報から最終アドレスを計算している。 */
    }
    
    
    /* 必要なページの計算 */
    UINTN num_pages = (kernel_last_address - kernel_first_address + 0xfff) / 0x1000;
    
    StallBranch(stall_flag);
    
    Print(L"num pages: %d\n", num_pages);
    
    
    /* ページの割当を行う */
    status = gBS->AllocatePages(AllocateAddress, EfiLoaderData, 
                                num_pages, &kernel_first_address);
    
    
    StatusCheacker(status); /* statusチェック */
    Print(L"    allocate pages\n");
    if (EFI_ERROR(status)) {
        Hlt();
    }
    
    /* セグメントコピーの開始 */
    /* ここからはプログラムヘッダーに従ってセグメントをロードしていく */
    
    Elf64_Phdr *phdr_copy_seg = (Elf64_Phdr *)((UINT64)kernel_ehdr + kernel_ehdr->e_phoff);
    for (uint64_t i = 0; i < kernel_ehdr->e_phnum; ++i) {
        if (phdr_copy_seg[i].p_type != PT_LOAD)
            continue;
        
        /* 以下の処理がわからんので調べる */
        UINT64 segment_in_file = (UINT64)kernel_ehdr + phdr_copy_seg[i].p_offset;
        CopyMem((VOID *)phdr_copy_seg[i].p_vaddr, (VOID *)segment_in_file,
                phdr_copy_seg[i].p_filesz);
        
        UINTN remain_byte = phdr_copy_seg[i].p_memsz - phdr_copy_seg[i].p_filesz;
        SetMem((VOID *)(phdr_copy_seg[i].p_vaddr + phdr_copy_seg[i].p_filesz), remain_byte, 0);
    }
    
    StallBranch(stall_flag);
    Print(L"kernel first address:  %08x\n", kernel_first_address);
    StallBranch(stall_flag);
    Print(L"kernel last address:  %08x\n", kernel_last_address);
    
    
    
    
    
    
    /* ===================================================================================== */
    /* カーネルの情報を読み出すために使用したメモリ上の一時領域を開放する */
    status = SystemTable->BootServices->FreePool(kernel_buffer);
    StallBranch(stall_flag);
    StatusCheacker(status);
    Print(L"    free pool\n");
    if (EFI_ERROR(status)) {
        Hlt();
    }
    
    
    
    
    
    

    
    /* ====================================================================================== */
    /* ExitBootServicesするためのmemorymapを取得する */
    /* memmap_bufferの大きさの問題だが、問題が起こった場合whileで十分な値になるまで少しづつ増やす処理をする */
    
    
    /* MemoryMapを入れるためのメモリの一時領域を定義する */
    /* mikanosのブートローダ273行目あたりを参照 */
    
    CHAR8 memmap_buffer[4096 * 4];
    struct MemoryMap map = {sizeof(memmap_buffer), memmap_buffer, 0, 0x0, 0x0, 0x0};
    
    if (map.buffer == NULL) {
        status = EFI_BUFFER_TOO_SMALL;
        
        
        StallBranch(stall_flag);
        

        StatusCheacker(status);
        Print(L"    GetMemoryMap\n");
        if (EFI_ERROR(status)) {
            Hlt();
        }
    }


    /* wcat_boot_information.kernel_runtime_service = (KERNEL_EFI_RUNTIME_SERVICE*)gRT; */
    
    map.map_size = map.buffer_size;
    
    status = SystemTable->BootServices->GetMemoryMap(&map.map_size,
                                                     (EFI_MEMORY_DESCRIPTOR *)map.buffer,
                                                     &map.map_key,
                                                     &map.descriptor_size,
                                                     &map.descriptor_version);
    
    
    /* ===================================================================================== */
    /* ExitBootServicesをする。 */
    
    status = gBS->ExitBootServices(ImageHandle, map.map_key);
    
    
    if (EFI_ERROR(status)) {
        
        if (map.buffer == NULL) {
            status = EFI_BUFFER_TOO_SMALL;
            Print(L"[");
            if (EFI_ERROR(status)) {
                SystemTable->ConOut->SetAttribute(SystemTable->ConOut, EFI_LIGHTRED);
                Print(L"%r", status);
            } else {
                SystemTable->ConOut->SetAttribute(SystemTable->ConOut, EFI_LIGHTGREEN);
                Print(L"%r", status);
            }
            SystemTable->ConOut->SetAttribute(SystemTable->ConOut, EFI_WHITE);
            Print(L"]    GetMemoryMap\n");
            if (EFI_ERROR(status)) {
                Hlt();
            }
        }
        Print(L"Second ExitBootServices start");
        map.map_size = map.buffer_size;
        
        status = SystemTable->BootServices->GetMemoryMap(&map.map_size,
                                                         (EFI_MEMORY_DESCRIPTOR*)map.buffer,
                                                         &map.map_key,
                                                         &map.descriptor_size,
                                                         &map.descriptor_version);



        status = gBS->ExitBootServices(ImageHandle, map.map_key);


        UINT8 *frame_buffer = (UINT8 *)gop->Mode->FrameBufferBase;
        for (UINTN i = 0; i < gop->Mode->FrameBufferSize; ++i) {
            frame_buffer[i] = 255;
        }
    }
    /* カーネル側での手土産の設定とカーネルさんへのお願い */

    /* ELFの24に関しては以下を参照 */
    /* https://wiki.osdev.org/ELF */
#define PROGRAM_ENTRY_POSION_OFFSET 24 /* magic number対策 */

    UINT64 entry_addr = *(UINT64*)(kernel_first_address + PROGRAM_ENTRY_POSION_OFFSET);
    typedef void EntryPointType(const struct WCAT_HEADER*);
    EntryPointType* entry_point = (EntryPointType*)entry_addr;
    entry_point(&wcat_boot_information);

    while (1) {
        EFI_SUCCESS;
    }

    Hlt();
    return 0;
}
