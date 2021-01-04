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




#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Library/PrintLib.h>
#include  <Library/MemoryAllocationLib.h>
#include  <Library/BaseMemoryLib.h>
#include  <Protocol/LoadedImage.h>
#include  <Protocol/SimpleFileSystem.h>

#include  <Protocol/SimpleTextOut.h> /* 追加(いらんかも？) */

#include  <Protocol/DiskIo2.h>
#include  <Protocol/BlockIo.h>
#include  <Guid/FileInfo.h>
#include  <stdint.h>

#include "elf_header.h"
/* ELFヘッダーは */

EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SFSP;
EFI_SYSTEM_TABLE *ST;



/* 追加 */
/* ここはfile_infoを利用して動的に取得できるようにする */
#define MAX_FILE_NAME_LEN 15	/* ルートディレクトリを表示する際のファイル名の */
#define MAX_FILE_NUM 10
#define MAX_FILE_BUF 1024 /* test用のファイルバッファの大きさ */

/* ファイル名格納用構造体 */
struct FILE {
  uint16_t name[MAX_FILE_NAME_LEN];
} __attribute__((packed));



/* boot時にカーネルに渡す情報 */
/* マルチブート2に準拠した構成にするか悩み中 */
/* とりあえずで作成したminOSベースの構造体 */
struct VIDEO_INFO {
  uint8_t *frame_buffer_addr;
  uint64_t frame_buffer_size;
  uint32_t horizen_size;
  uint32_t vertical_size;
  uint32_t pixel_per_scanline;
};

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





/* statusチェックの自動化 */
void status_cheacker(EFI_SYSTEM_TABLE *SystemTable, EFI_STATUS status) {
  Print(L"[");
  if (EFI_ERROR(status)) {
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, EFI_LIGHTRED);
    Print(L"%r", status);
  } else {
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, EFI_LIGHTGREEN);
    Print(L"%r", status);
  }
  SystemTable->ConOut->SetAttribute(SystemTable->ConOut, EFI_WHITE);
  Print(L"]");
}





/* strncpyだが名前衝突のエラーがうっとおしいため */
/* 参考にした場合のミスが目立つ場合戻すこと */
void strncopy(unsigned short *dst, unsigned short *src, unsigned long long n) {
  while (n--)
    *dst++ = *src++;
}

/* while(1)で止めるのは気持ち悪いので */
/* この関数を読み出すとCPUが休止モードになる */
void hlt() {
  while (1)
    __asm__("hlt");
}


/* MikanOSのブートローダーより引用 */
EFI_STATUS OpenRootDir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL **root) {
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

EFI_STATUS
EFIAPI
UefiMain(EFI_HANDLE ImageHandle,EFI_SYSTEM_TABLE *SystemTable) {
  EFI_STATUS status; /* 各種EFI_STATUSの返り値を格納する変数 */
  EFI_FILE_PROTOCOL *root; /* rootを呼び出す */
  UINTN buf_size;
  uint8_t file_buf[MAX_FILE_BUF];
  EFI_FILE_INFO *file_info;
  struct FILE file_list[10];
  int index = 0;
  EFI_INPUT_KEY key;
  UINTN waitIndex;

  /* watchdogタイマの無効化 */
  /* 5分刻みで再起動してしまうのを防ぐ。 */
  SystemTable->BootServices->SetWatchdogTimer(0, 0, 0, NULL);

  /* 画面クリアを行う */
  SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

  /* ロゴの表示（エスケープシーケンスに注意） */
  Print(L" __       __                        __       ______    ______  \n");
  SystemTable->BootServices->Stall(100000);
  Print(L"/  |  _  /  |                      /  |     /      \\  /      \\ \n");
  SystemTable->BootServices->Stall(100000);
  Print(L"$$ | / \\ $$ |  _______   ______   _$$ |_   /$$$$$$  |/$$$$$$  |\n");
  SystemTable->BootServices->Stall(100000);
  Print(L"$$ |/$  \\$$ | /       | /      \\ / $$   |  $$ |  $$ |$$ \\__$$/ \n");
  SystemTable->BootServices->Stall(100000);
  Print(L"$$ /$$$  $$ |/$$$$$$$/  $$$$$$  |$$$$$$/   $$ |  $$ |$$      \\ \n");
  SystemTable->BootServices->Stall(100000);
  Print(L"$$ $$/$$ $$ |$$ |       /    $$ |  $$ | __ $$ |  $$ | $$$$$$  |\n");
  SystemTable->BootServices->Stall(100000);
  Print(L"$$$$/  $$$$ |$$ \\_____ /$$$$$$$ |  $$ |/  |$$ \\__$$ |/  \\__$$ |\n");
  SystemTable->BootServices->Stall(100000);
  Print(L"$$$/    $$$ |$$       |$$    $$ |  $$  $$/ $$    $$/ $$    $$/ \n");
  SystemTable->BootServices->Stall(100000);
  Print(L"$$/      $$/  $$$$$$$/  $$$$$$$/    $$$$/   $$$$$$/   $$$$$$/  \n");
  SystemTable->BootServices->Stall(100000);
  Print(L"                                                               \n");
  SystemTable->BootServices->Stall(100000);
  Print(L"                                                               \n");
  SystemTable->BootServices->Stall(100000);
  Print(L"                                                               \n");
  SystemTable->BootServices->Stall(100000);

  /* カーネルブートするかのチェックを行う */
  Print(L"Kernel boot(press RET)\n");
  SystemTable->BootServices->WaitForEvent(1, &(SystemTable->ConIn->WaitForKey),
                                          &waitIndex); //入力があるまで待機

  /* RETキーが押されると進む */
  while (1) {
    SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &key);
    if (key.UnicodeChar != '\r') {
      Print(L"KernelBoot Start\n");
      break;
    }
  }
  SystemTable->BootServices->Stall(500000);
  /* rootディレクトリの情報を読み出している */

  /* 12_04 Loacteタイプ--------------------------------------------- */
  /* status = ST->BootServices->LocateHandle(SFSP,
   * &gEfiSimpleFileSystemProtocolGuid, NULL, ImageHandle,  ) */
  /* status =
   * ST->BootServices->LocateProtocol(&gEfiSimpleFileSystemProtocolGuid, NULL,
   * (void **)&SFSP); */
  /* Print(L"%r\n", status); */
  /* status = SFSP->OpenVolume(SFSP, &root); */
  /* Print(L"%r\n", status); */
  /* -------------------------------------------------------- */

  status = OpenRootDir(ImageHandle, &root);
  status_cheacker(SystemTable, status); /* statusチェック */
  Print(L"    OpenRootDir\n");
  if (EFI_ERROR(status)) {
    hlt();
  }

  /* SystemTable->BootServices->Stall(1000000); */
  SystemTable->BootServices->Stall(500000);
  /* Rootでぃれくとりの表示 */
  Print(L"RootDirectory: ");

  /* ファイル名を繰り返すことで読み出している */
  while (1) {
    buf_size = MAX_FILE_BUF;
    status = root->Read(root, &buf_size, (void *)file_buf);
    if (!buf_size) {
      break;
    }

    file_info = (EFI_FILE_INFO *)file_buf;
    strncopy(file_list[index].name, file_info->FileName, MAX_FILE_NAME_LEN - 1);
    file_list[index].name[MAX_FILE_NAME_LEN - 1] = L'\0';
    Print(file_list[index].name);
    Print(L" ");
    index++;
  }

  Print(L"\n");
  
  SystemTable->BootServices->Stall(500000);
  status_cheacker(SystemTable, status); /* statusチェック */
  Print(L"    Read status\n");
  if (EFI_ERROR(status)) {
    hlt();
  }


  
  SystemTable->BootServices->Stall(500000);

  /* 疑似シェルのlsコマンドとする場合等はCloseが必要となるが現状はファイル名を読み出したいだけであるためなし */

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
                      
  SystemTable->BootServices->Stall(500000);
  status_cheacker(SystemTable, status); /* statusチェック */
  Print(L"    kernelfile open\n");
  if (EFI_ERROR(status)) {
    hlt();
  }

  SystemTable->BootServices->Stall(500000);
  VOID *kernel_buffer; /* kernelのバイナリ読み出し用 */
  UINTN kernel_file_info_size = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 12;
  UINT8 kernel_file_info_buf[kernel_file_info_size];
  status = kernel_file->GetInfo(kernel_file, &gEfiFileInfoGuid,
                                &kernel_file_info_size, kernel_file_info_buf);
  status_cheacker(SystemTable, status); /* statusチェック */
  Print(L"    Getinfo\n");
  if (EFI_ERROR(status)) {
    hlt();
  }

  EFI_FILE_INFO *kerne_file_info = (EFI_FILE_INFO *)kernel_file_info_buf;
  UINTN kernel_file_size = kerne_file_info->FileSize;

  SystemTable->BootServices->Stall(500000);
  status = gBS->AllocatePool(EfiLoaderData, kernel_file_size, (void **)&kernel_buffer);
  status_cheacker(SystemTable, status); /* statusチェック */
  Print(L"    AllocatePool\n");
  if (EFI_ERROR(status)) {
    hlt();
  }
  SystemTable->BootServices->Stall(500000);
  status = kernel_file->Read(kernel_file, &kernel_file_size, kernel_buffer);

  status_cheacker(SystemTable, status); /* statusチェック */
  Print(L"    kernelRead\n");
  if (EFI_ERROR(status)) {
    hlt();
  }

  SystemTable->BootServices->Stall(500000);
  uint8_t *kernele_buf_test = (uint8_t *)kernel_buffer;
  int i;
  Print(L"Magic Number:");
  for (i = 0; i < 16; i++) {
    SystemTable->BootServices->Stall(100000);
    Print(L"%02x ", kernele_buf_test[i]);
  }
  Print(L"\n");

  /* アドレスの計算を行う */
  Elf64_Ehdr *kernel_ehdr = (Elf64_Ehdr *)kernel_buffer;
  SystemTable->BootServices->Stall(500000);
  Print(L"entrypoint address:\t");
  Print(L"%08x\n", kernel_ehdr->e_entry);

  /* 以下にELF形式の */
  /* p_offsetを記載 */
  SystemTable->BootServices->Stall(500000);
  Print(L"programheader offset:");
  Print(L"%08x\n", kernel_ehdr->e_phoff);

  /* プログラムヘッダーのエントリの数 */
  SystemTable->BootServices->Stall(500000);
  Print(L"programheader number:");
  Print(L"%08x\n", kernel_ehdr->e_phnum);

  /*
   kernel_first_address:カーネルの最初のアドレス
   kernel_last_address:カーネルの最後のアドレス
  */
  UINT64 kernel_first_address, kernel_last_address;
  /* プログラムヘッダーの位置の計算を行う */
  Elf64_Phdr *phdr = (Elf64_Phdr *)((UINT64)kernel_ehdr + kernel_ehdr->e_phoff);
  kernel_first_address = MAX_UINT64;
  kernel_last_address = 0;

  SystemTable->BootServices->Stall(500000);
  Print(L"PT_LOAD: %d\n", PT_LOAD);
  
  for (uint16_t i = 0; i < kernel_ehdr->e_phnum; ++i) {
    if (phdr[i].p_type != PT_LOAD)
      continue; /* これはELFセグメントの型であり配列情報の解釈方法、PT_LOADを意味してる */
    kernel_first_address = MIN(kernel_first_address, phdr[i].p_vaddr); /* プログラムヘッダの情報から求める */
    kernel_last_address = MAX(kernel_last_address, phdr[i].p_vaddr + phdr[i].p_memsz); /* プログラムヘッダの情報から最終アドレスを計算している。 */
  }

  
  /* 必要なページの計算 */
  UINTN num_pages = (kernel_last_address - kernel_first_address + 0xfff) / 0x1000;

  SystemTable->BootServices->Stall(500000);
  Print(L"num pages: %d\n", num_pages);
 
  
  /* ページの割当を行う */
  status = gBS->AllocatePages(AllocateAddress, EfiLoaderData, 
							  num_pages, &kernel_first_address);


  status_cheacker(SystemTable, status); /* statusチェック */
  Print(L"    allocate pages\n");
  if (EFI_ERROR(status)) {
    hlt();
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

  SystemTable->BootServices->Stall(500000);
  Print(L"kernel first address:\t\t%08x\n", kernel_first_address);
  SystemTable->BootServices->Stall(500000);
  Print(L"kernel last address:\t\t%08x\n", kernel_last_address);





  
  /* ===================================================================================== */
  /* カーネルの情報を読み出すために使用したメモリ上の一時領域を開放する */
  status = SystemTable->BootServices->FreePool(kernel_buffer);
  SystemTable->BootServices->Stall(500000);
  status_cheacker(SystemTable, status);
  Print(L"    free pool\n");
  if (EFI_ERROR(status)) {
    hlt();
  }





  
  /* ====================================================================================== */
  /* GOPの設定 */
  /* プロトコルを開いていく */
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
  UINTN num_gop_handles = 0;	/*  */
  EFI_HANDLE *gop_handles = NULL;
  struct VIDEO_INFO video_infomation;
  
  status = SystemTable->BootServices->LocateHandleBuffer(ByProtocol,
														 &gEfiGraphicsOutputProtocolGuid,
														 NULL,
														 &num_gop_handles,
														 &gop_handles);
  status_cheacker(SystemTable, status);
  Print(L"GOP LocateHandleBuffer\n");
  SystemTable->BootServices->Stall(500000);



  
  
  status = SystemTable->BootServices->OpenProtocol(gop_handles[0],
												   &gEfiGraphicsOutputProtocolGuid,
												   (VOID **)&gop,
												   ImageHandle,
												   NULL,
												   EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
  status_cheacker(SystemTable, status);
  Print(L"GOP OpenProtocol\n");
  FreePool(gop_handles);		/* この処理が何なのかが気になる */


  /* ピクセルフォーマットの確認する関数 */
  Print(L"%d\n", gop->Mode->Info->PixelFormat);

  
  /* RGBで指定できるように設定をする */



  Print(L"FrameBufferSize%d\n", gop->Mode->FrameBufferSize);


  /* UINT8 *frame_buffer  =(UINT8*)gop->Mode->FrameBufferBase; */
  /* for (UINTN i = 0; i < gop->Mode->FrameBufferSize; ++i) { */
  /* 	frame_buffer[i] = 255; */
  /* } */

  /* カーネルに渡すグラフィックのデータ */
  video_infomation.frame_buffer_addr = (uint8_t *)gop->Mode->FrameBufferBase;
  video_infomation.frame_buffer_size = (uint64_t)gop->Mode->FrameBufferSize;
  video_infomation.horizen_size = (uint32_t)gop->Mode->Info->HorizontalResolution;
  video_infomation.vertical_size = (uint32_t)gop->Mode->Info->VerticalResolution;
  video_infomation.pixel_per_scanline = (uint32_t)gop->Mode->Info->PixelsPerScanLine;
  
  /* ====================================================================================== */
  /* ExitBootServicesするためのmemorymapを取得する */
  /* memmap_bufferの大きさの問題だが、問題が起こった場合whileで十分な値になるまで少しづつ増やす処理をする */
  

  /* MemoryMapを入れるためのメモリの一時領域を定義する */
  /* mikanosのブートローダ273行目あたりを参照 */

  CHAR8 memmap_buffer[4096 * 4];
  struct MemoryMap map = {sizeof(memmap_buffer), memmap_buffer, 0, 0x0, 0x0, 0x0};

  if (map.buffer == NULL) {
    status = EFI_BUFFER_TOO_SMALL;
    SystemTable->BootServices->Stall(500000);
	status_cheacker(SystemTable, status);
    Print(L"    GetMemoryMap\n");
    if (EFI_ERROR(status)) {
      hlt();
    }
  }

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
        hlt();
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

  /* UINT8 *frame_buffer  =(UINT8*)gop->Mode->FrameBufferBase; */
  unsigned int hr = gop->Mode->Info->HorizontalResolution;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *p = (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)gop->Mode->FrameBufferBase;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *q = p + (hr * 30) + 30;
  for (UINTN i = 0; i < gop->Mode->FrameBufferSize; ++i) {
	/* frame_buffer[i] = 0x24; */
	q[i].Red = 0xAD;
	q[i].Green = 0xFF;
	q[i].Blue = 0x2F;	
  }
  /* カーネル側での手土産の設定とカーネルさんへのお願い */
  UINT64 entry_addr = *(UINT64*)(kernel_first_address + 24);
  typedef void EntryPointType(const struct VIDEO_INFO*);

  EntryPointType* entry_point = (EntryPointType*)entry_addr;
  entry_point(&video_infomation);

  while (1) {
	EFI_SUCCESS;
	  }

  hlt();
  return 0;
}
