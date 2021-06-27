#include  <Uefi.h>
#include  <Library/UefiLib.h>                  /* Print */
#include <Library/UefiBootServicesTableLib.h>  /* gST */
#include <Library/UefiRuntimeServicesTableLib.h> /* gRT */
#include <stdint.h>



#define PRIVATE static
#define PUBLIC

/* menu関連の情報（menuが何行から始まるか、menuが何個あるかなどの情報を記載） */
/* menuの数は関数が何回呼び出されたかでカウント */
typedef struct MENU_INFORMATION{
    uint32_t boot_process_start_flag;
    uint32_t menu_shutdown_flag;
}MENU_INFORMATION;

PRIVATE EFI_INPUT_KEY efi_wait_any_key();
PRIVATE VOID stall(uint32_t microseconds);
PRIVATE VOID logo_print();
PRIVATE VOID menu_init(uint32_t cursor_x, uint32_t cursor_y);
PRIVATE VOID set_cursor(uint32_t cursor_x, uint32_t cursor_y);
PRIVATE VOID clear();
PRIVATE EFI_INPUT_KEY menu_sentinel(EFI_INPUT_KEY key_data,
                                    uint32_t menu_number,
                                    uint32_t* boot_menu_index);
PRIVATE VOID shutdown_menu();
PRIVATE VOID boot_menu_flag_init(MENU_INFORMATION* menu_information);
PRIVATE VOID boot_process_enable(MENU_INFORMATION* menu_information);
PRIVATE VOID boot_process_disable(MENU_INFORMATION* menu_information);
PRIVATE VOID menu_shutdown_enabel(MENU_INFORMATION* menu_information);
PRIVATE VOID menu_shutdown_disable(MENU_INFORMATION* menu_information);


PUBLIC VOID boot_menu(uint32_t* stall_flag){
    clear();
    logo_print();
    menu_init(0, 18);
    EFI_INPUT_KEY result_key_data = {0, 0};
    PRIVATE MENU_INFORMATION menu_information;
    boot_menu_flag_init(&menu_information);
    uint32_t boot_menu_index = 0;
    for(;;) {
        /* result_key_dataとboot_menu_indexは、構造体でまとめても良かったのですが */
        /* グローバル変数が増えるのが嫌だったので、ポインタ渡しで実装 */
        result_key_data = menu_sentinel(result_key_data, 3, &boot_menu_index);

        /* case内のカーソルの位置とメニューの数を渡せば勝手に描画を行ってくれる関数を記載する */
        /* case 内の関数に文字列を渡すことにより、それがmenuの表示になる */
        switch (boot_menu_index) {
        case 0:
            set_cursor(0, 18);
            Print(L">    ");
            set_cursor(0, 19);
            Print(L"     ");
            set_cursor(0, 20);
            Print(L"     ");
            set_cursor(0, 21);
            Print(L"     ");
            boot_process_enable(&menu_information);
            menu_shutdown_disable(&menu_information);
            *stall_flag = 0;
            break;
        case 1:
            set_cursor(0, 18);
            Print(L"     ");
            set_cursor(0, 19);
            Print(L">    ");
            set_cursor(0, 20);
            Print(L"     ");
            set_cursor(0, 21);
            Print(L"     ");
            boot_process_enable(&menu_information);
            menu_shutdown_disable(&menu_information);
            *stall_flag = 1;
            break;
        case 2:
            set_cursor(0, 18);
            Print(L"     ");
            set_cursor(0, 19);
            Print(L"     ");
            set_cursor(0, 20);
            Print(L">    ");
            set_cursor(0, 21);
            Print(L"     ");
            boot_process_disable(&menu_information);
            menu_shutdown_disable(&menu_information);
            break;
        case 3:
            set_cursor(0, 18);
            Print(L"     ");
            set_cursor(0, 19);
            Print(L"     ");
            set_cursor(0, 20);
            Print(L"     ");
            set_cursor(0, 21);
            Print(L">    ");
            boot_process_disable(&menu_information);
            menu_shutdown_enabel(&menu_information);
            break;
        }
        if (result_key_data.UnicodeChar == '\r' && menu_information.boot_process_start_flag == 1){
            return;
        } else if (result_key_data.UnicodeChar == '\r' && menu_information.menu_shutdown_flag == 1){
            shutdown_menu();
            return;
        }
    }
}

PRIVATE VOID menu_shutdown_disable(MENU_INFORMATION* menu_information){
    menu_information->menu_shutdown_flag = 0;
}

PRIVATE VOID menu_shutdown_enabel(MENU_INFORMATION* menu_information){
    menu_information->menu_shutdown_flag = 1;
}

PRIVATE VOID boot_process_disable(MENU_INFORMATION* menu_information){
    menu_information->boot_process_start_flag = 0;
}

PRIVATE VOID boot_process_enable(MENU_INFORMATION* menu_information){
    menu_information->boot_process_start_flag = 1;
}

PRIVATE VOID boot_menu_flag_init(MENU_INFORMATION* menu_information){
    menu_information->boot_process_start_flag = 0;
    menu_information->menu_shutdown_flag = 0;
}


PRIVATE VOID shutdown_menu(){
    gRT->ResetSystem(EfiResetShutdown, 0, 0, "shutdown");
    CpuDeadLoop();
    return;
}

PRIVATE EFI_INPUT_KEY menu_sentinel(EFI_INPUT_KEY key_data, uint32_t menu_number, uint32_t* boot_menu_index){
        key_data = efi_wait_any_key();
        if (key_data.ScanCode == 0x01 && *boot_menu_index != 0) {
            *boot_menu_index -= 1;
        } else if (key_data.ScanCode == 0x02 && *boot_menu_index != menu_number) {
            *boot_menu_index += 1;
        }
        return key_data;
}

PRIVATE VOID clear(){
    gST->ConOut->ClearScreen(gST->ConOut);
}

PRIVATE VOID set_cursor(uint32_t cursor_x, uint32_t cursor_y){
    gST->ConOut->SetCursorPosition(gST->ConOut, cursor_x, cursor_y);
}

PRIVATE VOID menu_init(uint32_t cursor_x, uint32_t cursor_y){
    set_cursor(cursor_x, cursor_y);
    Print(L">    develop boot");
    set_cursor(cursor_x + 5, ++cursor_y);
    Print(L"normal  boot");
    set_cursor(cursor_x + 5, ++cursor_y);
    Print(L"information");
    set_cursor(cursor_x + 5, ++cursor_y);
    Print(L"shutdown");
}

PRIVATE EFI_INPUT_KEY efi_wait_any_key(){
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

PRIVATE VOID stall(uint32_t microseconds){
    gST->BootServices->Stall(microseconds);
}

PRIVATE VOID logo_print(){
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
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, 11); /* QueryMode()でカーソルの位置をq指定するAPI */
    Print(L"                                                               \n");
    stall(100000);
}

#undef PRIVATE
#undef PUBLIC
