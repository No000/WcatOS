#include <Uefi.h>
#include <stdint.h>
#include <Library/UefiBootServicesTableLib.h> /* gST */
#include <Library/UefiLib.h>                  /* Print */
#include <Guid/SmBios.h> /* SMBIOS */
#include <IndustryStandard/SmBios.h>
#include "../../smbios.h"

#include "../../util.h"
#include "information.h"

VOID information(){
    gST->ConOut->ClearScreen(gST->ConOut);
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);
    Print(L"UEFI information \n");
    Print(L"UEFI Vendor information: %s\n", gST->FirmwareVendor);
    Print(L"UEFI Firmware version: 0x%x\n", gST->FirmwareRevision);
    Print(L"Support UEFI Specification: UEFI");
    switch (gST->Hdr.Revision) {
    case EFI_1_02_SYSTEM_TABLE_REVISION:
        Print(L" 1.02 ");
        break;
    case EFI_1_10_SYSTEM_TABLE_REVISION:
        Print(L" 1.10 ");
        break;
    case EFI_2_00_SYSTEM_TABLE_REVISION:
        Print(L" 2.00 ");
        break;
    case EFI_2_10_SYSTEM_TABLE_REVISION:
        Print(L" 2.10 ");
        break;
    case EFI_2_20_SYSTEM_TABLE_REVISION:
        Print(L" 2.20 ");
        break;
    case EFI_2_30_SYSTEM_TABLE_REVISION:
        Print(L" 2.30 ");
        break;
    case EFI_2_31_SYSTEM_TABLE_REVISION:
        Print(L" 2.31 ");
        break;
    case EFI_2_40_SYSTEM_TABLE_REVISION:
        Print(L" 2.40 ");
        break;
    case EFI_2_50_SYSTEM_TABLE_REVISION:
        Print(L" 2.50 ");
        break;
    case EFI_2_60_SYSTEM_TABLE_REVISION:
        Print(L" 2.60 ");
        break;
    case EFI_2_70_SYSTEM_TABLE_REVISION:
        Print(L" 2.70 ");
        break;
    case EFI_2_80_SYSTEM_TABLE_REVISION:
        Print(L" 2.80 ");
        break;
    default:
        Print(L"%x", gST->Hdr.Revision);
    }
    Print(L"supported\n");


    /* gST->ConOut->SetCursorPosition(gST->ConOut, 0, 12); */
    SMBIOS_TABLE_ENTRY_POINT *smtable;
    smtable = find_efi_smbios_table();
    Print(L"%c", smtable->AnchorString[2]);
    SMBIOS_STRUCTURE_POINTER Smbios_struct;
    
    Smbios_struct.Hdr = (SMBIOS_STRUCTURE *)((UINTN)(smtable->TableAddress));
    
    Print(L"%x", smtable->TableLength);
    
    IN EFI_GUID *SystemGuidTest = NULL;
    Print(L"=");
    Print(L"%d.%d", smtable->MajorVersion, smtable->MinorVersion);
    Print(L"=");
    Print(L"%d", Smbios_struct.Type1->WakeUpType);  
    Print(L"=");
    /* 以下の関数を呼ぶたびにテーブルが一つ進む */
    int smbios_count;
    for (smbios_count = 0; smbios_count < smtable->NumberOfSmbiosStructures; smbios_count++){
        if (Smbios_struct.Hdr->Type == 1) {
            AsciiPrint(get_smbios_string(&Smbios_struct, Smbios_struct.Type1->Family));
            break;
        } else {
            smbios_next_table_move(&Smbios_struct);
        }
    }
    
    Print(L"=");
    
    Print(L"=");

    
    Print(L"%c", smtable->IntermediateAnchorString[0]);
    Print(L"%c", smtable->IntermediateAnchorString[1]);
    Print(L"%x", smtable->MajorVersion);
    Print(L"%x", smtable->MinorVersion);
    Print(L"  ");
    Print(L"%x", smtable->NumberOfSmbiosStructures);
    Print(L"%x", SystemGuidTest);

    Print(L"\n\n\n");           /* 臨時の改行 */
    Print(L"quit:press q");
    EFI_INPUT_KEY wait_key_data = {0, 0};
    wait_key_data = efi_wait_any_key();
    if (wait_key_data.UnicodeChar == 'q'){
        return;
    }
}




