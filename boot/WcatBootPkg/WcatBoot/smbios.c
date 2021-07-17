#include  <Uefi.h>
#include  <Guid/SmBios.h>                       /* SMBIOS */
#include  <IndustryStandard/SmBios.h>
#include  <Library/UefiBootServicesTableLib.h> /* gST */
#include "smbios.h"

#define PRIVATE static
#define PUBLIC


PUBLIC EFI_STATUS smbios_next_table_move(IN SMBIOS_STRUCTURE_POINTER *Smbios){
    CHAR8 *String;

    String = (CHAR8 *) (Smbios->Raw + Smbios->Hdr->Length);
    /* 構造体をスキップする */
    /* SMBIOS仕様書2.7のP23、727行目を参照 */
    /* 各構造体はdouble null(0000h)で終了しなければならない */
    /* よって、ChAR8の０２つを検出すれば良い */
    /*  */
    /* １つめの０の検出 */
    /*  */
    for(; *String != 0; String++);
    String++;
    /*  */
    /* ２つめの０の検出 */
    /*  */
    if (*String == 0){    /* NULLが連続で２つ来たときので構造体の終了*/
        Smbios->Raw = (UINT8 *)++String;
        return EFI_SUCCESS;
    }
    return  EFI_NOT_FOUND;      /* このエラー処理はまずいかもしれない */
}




PUBLIC CHAR8* get_smbios_string(IN SMBIOS_STRUCTURE_POINTER *Smbios,
                         IN UINT16 StructureNumber)
{
    UINT16 Index;
    CHAR8 *String;

    String = (CHAR8 *) (Smbios->Raw + Smbios->Hdr->Length);

    /* unformated sectionから目的の文字列を見つける */
    /* 文字列を参照する場合は文字列番号を指定する（参照する情報がない場合はNULLが返される） */
    for (Index = 1; Index <=StructureNumber; Index++) { /* 目的の文字列に移動 */
        if (StructureNumber == Index) {
            return String;
        }
        for(; *String != 0; String++); /* 参照してる文字列が終了するときNULL(0)が参照される */
        String++;                      /* 次の文字列に進める */
    }
    return NULL;
}




/* GUIDからSMBIOSのエントリテーブルを探す */
PUBLIC void *find_efi_smbios_table(void) {
    EFI_GUID efi_smbios_table = SMBIOS_TABLE_GUID;
  /* const EFI_GUID efi_smbios = SMBIOS_TABLE_GUID; */
  unsigned long long i;
  for (i = 0; i < gST->NumberOfTableEntries; i++) {
    EFI_GUID *guid = &gST->ConfigurationTable[i].VendorGuid;
    if ((guid->Data1 == efi_smbios_table.Data1) &&
        (guid->Data2 == efi_smbios_table.Data2) &&
        (guid->Data3 == efi_smbios_table.Data3)) {
      unsigned char is_equal = TRUE;
      unsigned char j;
      for (j = 0; j < 8; j++) {
        if (guid->Data4[j] != efi_smbios_table.Data4[j])
          is_equal = FALSE;
      }
      if (is_equal == TRUE)
        return gST->ConfigurationTable[i].VendorTable;
    }
  }
  return NULL;
}



