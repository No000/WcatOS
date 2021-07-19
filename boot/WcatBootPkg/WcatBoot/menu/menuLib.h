#ifndef _MENULIB_H_
#define _MENULIB_H_


#include  <Uefi.h>
#include <stdint.h>


/* menu内公開マクロ */

typedef struct MENU_INFORMATION{
    uint32_t boot_process_start_flag;
    uint32_t menu_shutdown_flag;
    uint64_t count;             /* menuの個数を記録 */
    uint64_t now_cursor_place;    /* 現在どこにカーソルがあるか */
    uint32_t startcurser_place; /* menuが何行から始まるかを記録 */
    uint32_t settings_menu_flag;
}MENU_INFORMATION;

EFI_INPUT_KEY efi_wait_any_key();
VOID stall(uint32_t microseconds);
VOID logo_print();
VOID menu_init(uint32_t cursor_y);
VOID cursor_init(uint32_t cursor_y);
VOID cursor_print(uint32_t cursor_position); /* cursor_positionは次に描画するカーソルの位置を指定 */
VOID set_cursor(uint32_t cursor_x, uint32_t cursor_y);
VOID clear();
EFI_INPUT_KEY menu_sentinel(EFI_INPUT_KEY key_data,
                                    uint32_t menu_number,
                                    uint32_t* boot_menu_index);
VOID shutdown();
VOID boot_menu_flag_init(MENU_INFORMATION* menu_information);
VOID boot_process_enable(MENU_INFORMATION* menu_information);
VOID boot_process_disable(MENU_INFORMATION* menu_information);
VOID shutdown_enabel(MENU_INFORMATION* menu_information);
VOID shutdown_disable(MENU_INFORMATION* menu_information);
VOID settings_menu_enable(MENU_INFORMATION* menu_information);
VOID settings_menu_disable(MENU_INFORMATION *menu_information);

#endif
