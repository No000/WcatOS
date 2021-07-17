#include  <Uefi.h>
#include  <Library/UefiLib.h>                  /* Print */
#include <Library/UefiBootServicesTableLib.h>  /* gST */
#include <Library/UefiRuntimeServicesTableLib.h> /* gRT */
#include <stdint.h>
#include "menu.h"
#include "menuLib.h"

#include "settings_menu/settings_menu.h"




VOID boot_menu(uint32_t* stall_flag){
    clear();
    logo_print();
    /* init処理をまとめる */
    menu_init(18);
    cursor_init(18);
    EFI_INPUT_KEY result_key_data = {0, 0};
    MENU_INFORMATION menu_information;
    boot_menu_flag_init(&menu_information);
    uint32_t boot_menu_index = 0;
    for(;;) {
        /* result_key_dataとboot_menu_indexは、構造体でまとめても良かったのですが */
        /* グローバル変数が増えるのが嫌だったので、ポインタ渡しで実装 */
        /* ここにsetting_menu.cから戻ってきたときのみ走るinit処理を記載。分岐はenumで定義したflagによる分岐 */
        result_key_data = menu_sentinel(result_key_data, 3, &boot_menu_index);
        

        /* ここですべてのフラグをdisableする処理を記載============================================================== */
        
        switch (boot_menu_index) {
        case 0:
            cursor_print(18);
            boot_process_enable(&menu_information);
            shutdown_disable(&menu_information);
            *stall_flag = 0;    /* stall_flagに関してはsetting_menuで変更したいので、別の構造体を利用する */
            break;
        case 1:
            cursor_print(19);
            boot_process_enable(&menu_information);
            shutdown_disable(&menu_information);
            *stall_flag = 1;
            break;
        case 2:
            cursor_print(20);
            boot_process_disable(&menu_information);
            shutdown_disable(&menu_information);
            break;
        case 3:
            cursor_print(21);
            boot_process_disable(&menu_information);
            shutdown_disable(&menu_information);
            settings_menu_enable(&menu_information);
            break;
        case 4:
            cursor_print(22);
            boot_process_disable(&menu_information);
            shutdown_enabel(&menu_information);
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
        }
    }
}

