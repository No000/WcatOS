#include  <Uefi.h>
#include  <Library/UefiLib.h>                  /* Print */
#include <Library/UefiBootServicesTableLib.h>  /* gST */
#include <Library/UefiRuntimeServicesTableLib.h> /* gRT */
#include <stdint.h>

#include "menuLib.h"

#define CURSOR_INIT_X 6

#define PRIVATE static
#define PUBLIC

PUBLIC VOID shutdown_disable(MENU_INFORMATION* menu_information){
    menu_information->menu_shutdown_flag = 0;
}

PUBLIC VOID settings_menu_enable(MENU_INFORMATION *menu_information){
    menu_information->settings_menu_flag = 1;
}

PUBLIC VOID settings_menu_disable(MENU_INFORMATION *menu_information){
    menu_information->settings_menu_flag = 0;
}


PUBLIC VOID shutdown_enabel(MENU_INFORMATION* menu_information){
    menu_information->menu_shutdown_flag = 1;
}

PUBLIC VOID boot_process_disable(MENU_INFORMATION* menu_information){
    menu_information->boot_process_start_flag = 0;
}

PUBLIC VOID boot_process_enable(MENU_INFORMATION* menu_information){
    menu_information->boot_process_start_flag = 1;
}

PUBLIC VOID boot_menu_flag_init(MENU_INFORMATION* menu_information){
    menu_information->boot_process_start_flag = 0;
    menu_information->menu_shutdown_flag = 0;
    menu_information->settings_menu_flag = 0;
}


PUBLIC VOID shutdown(){
    gRT->ResetSystem(EfiResetShutdown, 0, 0, "shutdown");
    CpuDeadLoop();
    return;
}

/* menuを選択する際、カーソルの位置を調整する関数 */
PUBLIC EFI_INPUT_KEY menu_sentinel(EFI_INPUT_KEY key_data, uint32_t menu_number, uint32_t* boot_menu_index){
        key_data = efi_wait_any_key();
        if (key_data.ScanCode == 0x01 && *boot_menu_index != 0) {
            *boot_menu_index -= 1;
        } else if (key_data.ScanCode == 0x02 && *boot_menu_index != menu_number) {
            *boot_menu_index += 1;
        }
        return key_data;
}

PUBLIC VOID clear(){
    gST->ConOut->ClearScreen(gST->ConOut);
    /* gst->ConOut->SetAttribute(gST->ConOut, 0x10); */

    /* UINTN height = 0; */
    /* UINTN width = 0; */
    /* gST->ConOut->QueryMode(gST->ConOut, gST->ConOut->Mode->Mode, &width, &height); */
    /* Print(L"%d\n", height); */
    /* Print(L"%d\n", width); */
    

}

PUBLIC VOID set_cursor(uint32_t cursor_x, uint32_t cursor_y){
    gST->ConOut->SetCursorPosition(gST->ConOut, cursor_x, cursor_y);
}

PUBLIC VOID menu_init(uint32_t cursor_y){
    set_cursor(CURSOR_INIT_X + 5, cursor_y);
    Print(L"develop boot");
    set_cursor(CURSOR_INIT_X + 5, ++cursor_y);
    Print(L"normal  boot");
    set_cursor(CURSOR_INIT_X + 5, ++cursor_y);
    Print(L"information");
    set_cursor(CURSOR_INIT_X + 5, ++cursor_y);
    Print(L"settings");
    set_cursor(CURSOR_INIT_X + 5, ++cursor_y);
    Print(L"shutdown");
}

PUBLIC VOID cursor_init(uint32_t cursor_y){
    set_cursor(CURSOR_INIT_X, cursor_y);
    Print(L">    ");
}

PUBLIC VOID cursor_print(uint32_t cursor_position){/* 型大きすぎるかもしれので、あとで変更 */
    set_cursor(CURSOR_INIT_X, cursor_position - 1);
    Print(L"     ");
    set_cursor(CURSOR_INIT_X, cursor_position + 1);
    Print(L"     ");
    set_cursor(CURSOR_INIT_X, cursor_position);
    Print(L">    ");
}

PUBLIC EFI_INPUT_KEY efi_wait_any_key(){
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

PUBLIC VOID stall(uint32_t microseconds){
    gST->BootServices->Stall(microseconds);
}

PUBLIC VOID logo_print(){
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


