#include <Uefi.h>
#include  <Library/UefiLib.h>                  /* Print */
#include <Library/UefiBootServicesTableLib.h>  /* gST */
#include <Library/UefiRuntimeServicesTableLib.h> /* gRT */
#include "settings_menu.h"




VOID settings_menu(){
    gST->ConOut->ClearScreen(gST->ConOut);
    CHAR16 topline[] = {BOXDRAW_DOWN_RIGHT, BOXDRAW_HORIZONTAL, BOXDRAW_DOWN_HORIZONTAL, BOXDRAW_HORIZONTAL};

    UINTN height = 0;
    UINTN width = 0;
    INTN i;
    INTN j;
    EFI_STATUS a1;
    a1 = gST->ConOut->QueryMode(gST->ConOut, gST->ConOut->Mode->Mode, &width, &height);
    /* setcursorpositionは意外と処理が遅いので、Printを複数回するほうがいいっぽい */
    gST->ConOut->SetAttribute(gST->ConOut, EFI_BACKGROUND_GREEN);
    for (i = 0; i < height; i++) {
        gST->ConOut->SetCursorPosition(gST->ConOut, j, i);
        for (j = 0; j < width; j++) {
            Print(L" ");
        }
    }
    Print(topline);
    while (1);
    return;
}


