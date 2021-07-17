#include <Uefi.h>
#include  <Library/UefiLib.h>                  /* Print */
#include <Library/UefiBootServicesTableLib.h>  /* gST */
#include <Library/UefiRuntimeServicesTableLib.h> /* gRT */
#include "settings_menu.h"


VOID settings_menu(){
    UINTN height = 0;
    UINTN width = 0;
    gST->ConOut->QueryMode(gST->ConOut, gST->ConOut->Mode->Mode, &width, &height);
    while (1);
    return;
}


