#include  <Uefi.h>
#include  <Library/UefiLib.h>                  /* Print */
#include <Library/UefiBootServicesTableLib.h>  /* gST */
#include <stdint.h>


EFI_INPUT_KEY efi_wait_any_key();
VOID stall(uint32_t microseconds);
VOID logo_print();
VOID menu_init(uint32_t cursor_x, uint32_t cursor_y);
VOID set_cursor(uint32_t cursor_x, uint32_t cursor_y);
EFI_INPUT_KEY menu_sentinel(EFI_INPUT_KEY key_data, uint32_t menu_number, uint32_t* boot_menu_index);


VOID boot_menu(uint32_t* stall_flag){
    logo_print();
    menu_init(0, 18);
    EFI_INPUT_KEY result_key_data = {0, 0};
    uint32_t boot_menu_index = 0;
    uint32_t boot_process_start_flag = 0;
    for(;;) {
        /* result_key_dataとboot_menu_indexは */
        /* 構造体でまとめても良かったのですが、グローバル変数が増えるのが嫌だったので、ポインタ渡しで実装 */
        result_key_data = menu_sentinel(result_key_data, 2, &boot_menu_index);

        /* case内のカーソルの位置とメニューの数を渡せば勝手に描画を行ってくれる関数を記載する */
        switch (boot_menu_index) {
        case 0:
            set_cursor(0, 18);
            Print(L">    ");
            set_cursor(0, 19);
            Print(L"     ");
            set_cursor(0, 20);
            Print(L"     ");
            boot_process_start_flag = 1;
            *stall_flag = 0;
            break;
        case 1:
            set_cursor(0, 18);
            Print(L"     ");
            set_cursor(0, 19);
            Print(L">    ");
            set_cursor(0, 20);
            Print(L"     ");
            boot_process_start_flag = 1;
            *stall_flag = 1;
            break;
        case 2:
            set_cursor(0, 18);
            Print(L"     ");
            set_cursor(0, 19);
            Print(L"     ");
            set_cursor(0, 20);
            Print(L">    ");
            boot_process_start_flag = 0;
            break;
        }
        if (result_key_data.UnicodeChar == '\r' && boot_process_start_flag == 1){
            return;
        }
    }
}

EFI_INPUT_KEY menu_sentinel(EFI_INPUT_KEY key_data, uint32_t menu_number, uint32_t* boot_menu_index){
        key_data = efi_wait_any_key();
        if (key_data.ScanCode == 0x01 && *boot_menu_index != 0) {
            *boot_menu_index -= 1;
        } else if (key_data.ScanCode == 0x02 && *boot_menu_index != menu_number) {
            *boot_menu_index += 1;
        }
        return key_data;
}

VOID set_cursor(uint32_t cursor_x, uint32_t cursor_y){
    gST->ConOut->SetCursorPosition(gST->ConOut, cursor_x, cursor_y);
}

VOID menu_init(uint32_t cursor_x, uint32_t cursor_y){
    set_cursor(cursor_x, cursor_y);
    Print(L">    develop boot");
    set_cursor(cursor_x + 5, ++cursor_y);
    Print(L"normal  boot");
    set_cursor(cursor_x + 5, ++cursor_y);
    Print(L"information");
}

EFI_INPUT_KEY efi_wait_any_key(){
    EFI_INPUT_KEY ret_keydata = { 0, 0};
    EFI_STATUS status;
    UINTN index = 0;

    status = gBS->WaitForEvent(1, &(gST->ConIn->WaitForKey), &index);
    while (1) {
        if (!EFI_ERROR(status)){
            break;
        }
    }
    if(!EFI_ERROR(status)) {
        if(index == 0) {
            EFI_INPUT_KEY key;
            status = gST->ConIn->ReadKeyStroke(gST->ConIn, &key);
            if (!EFI_ERROR(status)) {
                ret_keydata = key;
            }
        }
    } else {
        Print(L"waitforevent error\n");
    }
    return  ret_keydata;
}

void stall(uint32_t microseconds){
    gST->BootServices->Stall(microseconds);
}

void logo_print(){
     /* ロゴの表示（エスケープシーケンスに注意） */
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0); /* QueryMode()でカーソルの位置を指定するAPI */
    Print(L" __       __                        __       ______    ______  \n");
    stall(100000);
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, 1); /* QueryMode()でカーソルの位置を指定するAPI */
    Print(L"/  |  _  /  |                      /  |     /      \\  /      \\ \n");
    stall(100000);
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, 2); /* QueryMode()でカーソルの位置を指定するAPI */
    Print(L"$$ | / \\ $$ |  _______   ______   _$$ |_   /$$$$$$  |/$$$$$$  |\n");
    stall(100000);
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, 3); /* QueryMode()でカーソルの位置を指定するAPI */
    Print(L"$$ |/$  \\$$ | /       | /      \\ / $$   |  $$ |  $$ |$$ \\__$$/ \n");
    stall(100000);
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, 4); /* QueryMode()でカーソルの位置を指定するAPI */
    Print(L"$$ /$$$  $$ |/$$$$$$$/  $$$$$$  |$$$$$$/   $$ |  $$ |$$      \\ \n");
    stall(100000);
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, 5); /* QueryMode()でカーソルの位置を指定するAPI */
    Print(L"$$ $$/$$ $$ |$$ |       /    $$ |  $$ | __ $$ |  $$ | $$$$$$  |\n");
    stall(100000);
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, 6); /* QueryMode()でカーソルの位置を指定するAPI */
    Print(L"$$$$/  $$$$ |$$ \\_____ /$$$$$$$ |  $$ |/  |$$ \\__$$ |/  \\__$$ |\n");
    stall(100000);
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, 7); /* QueryMode()でカーソルの位置を指定するAPI */
    Print(L"$$$/    $$$ |$$       |$$    $$ |  $$  $$/ $$    $$/ $$    $$/ \n");
    stall(100000);
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, 8); /* QueryMode()でカーソルの位置を指定するAPI */
    Print(L"$$/      $$/  $$$$$$$/  $$$$$$$/    $$$$/   $$$$$$/   $$$$$$/  \n");
    stall(100000);
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, 9); /* QueryMode()でカーソルの位置を指定するAPI */
    Print(L"                                                               \n");
    stall(100000);
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, 10); /* QueryMode()でカーソルの位置を指定するAPI */
    Print(L"                                                               \n");
    stall(100000);
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, 11); /* QueryMode()でカーソルの位置を指定するAPI */
    Print(L"                                                               \n");
    stall(100000);
}
