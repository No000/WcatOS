#include  <Uefi.h>
#include  <Library/UefiLib.h>                  /* Print */
#include <Library/UefiBootServicesTableLib.h>  /* gST */
#include <Library/UefiRuntimeServicesTableLib.h> /* gRT */
#include <stdint.h>
#include "menu.h"
#include "menuLib.h"

#include "settings_menu/settings_menu.h"
#include "information/information.h"

/* menuの追加方法 */
/* (1)PUBLIC VOID menu_init(uint32_t cursor_y)関数にメニュー名を記載 */
/* (2)switch文の中に追加 */
/* (3)最後のif文にフラグで分岐する、実行したい関数を記載 */


VOID boot_menu(uint32_t* stall_flag){
    clear();
    stall_logo_print();
    /* init処理をまとめる */
    menu_init(18);
    BOOLEAN ret_menuprint_flag = FALSE;
    cursor_init(18);
    EFI_INPUT_KEY result_key_data = {0, 0};
    MENU_INFORMATION menu_information;
    boot_menu_flag_init(&menu_information);
    uint32_t boot_menu_index = 0;
    for(;;) {
        if (ret_menuprint_flag){
            clear();
            logo_print();
            menu_init(18);
            ret_menuprint_flag = FALSE;
            cursor_init(18 + boot_menu_index); /* 18はメニューのcursor_yの初期値（相対値） */
        }
        /* result_key_dataとboot_menu_indexは、構造体でまとめても良かったのですが */
        /* グローバル変数が増えるのが嫌だったので、ポインタ渡しで実装 */
        /* ここにsetting_menu.cから戻ってきたときのみ走るinit処理を記載。分岐はenumで定義したflagによる分岐 */
        result_key_data = menu_sentinel(result_key_data, 4, &boot_menu_index);

        /* ここですべてのフラグをdisableする処理を記載============================================================== */
        
        switch (boot_menu_index) {
        case 0:
            cursor_print(18);
            boot_process_enable(&menu_information);
            shutdown_disable(&menu_information);
            settings_menu_disable(&menu_information);
            information_disable(&menu_information);
            *stall_flag = 0;    /* stall_flagに関してはsetting_menuで変更したいので、別の構造体を利用する */
            break;
        case 1:
            cursor_print(19);
            boot_process_enable(&menu_information);
            shutdown_disable(&menu_information);
            settings_menu_disable(&menu_information);
            information_disable(&menu_information);
            *stall_flag = 1;
            break;
        case 2:
            cursor_print(20);
            boot_process_disable(&menu_information);
            shutdown_disable(&menu_information);
            settings_menu_disable(&menu_information);
            information_enable(&menu_information);
            break;
        case 3:
            cursor_print(21);
            boot_process_disable(&menu_information);
            shutdown_disable(&menu_information);
            settings_menu_enable(&menu_information);
            information_disable(&menu_information);
            break;
        case 4:
            cursor_print(22);
            boot_process_disable(&menu_information);
            shutdown_enabel(&menu_information);
            settings_menu_disable(&menu_information);
            information_disable(&menu_information);
            break;
        }
        if (result_key_data.UnicodeChar == '\r' && menu_information.boot_process_start_flag == 1){ /* ここ、enumと関数使えばまとめられないかな */
            return;
        } else if (result_key_data.UnicodeChar == '\r' && menu_information.menu_shutdown_flag == 1){
            shutdown();
            return;             /* ここにはこない */
        } else if (result_key_data.UnicodeChar == '\r' && menu_information.settings_menu_flag == 1){
            /* settings_menu=================================================== */
            settings_menu();
        } else if (result_key_data.UnicodeChar == '\r' && menu_information.information_flag == 1){
            information();
            ret_menuprint_flag = TRUE;
        }
    }
}

