#ifndef _SMBIOS_H_
#define _SMBIOS_H_

#include <Uefi.h>
#include <IndustryStandard/SmBios.h>

EFI_STATUS smbios_next_table_move(IN SMBIOS_STRUCTURE_POINTER *Smbios);
CHAR8 *get_smbios_string(IN SMBIOS_STRUCTURE_POINTER *Smbios,
                         IN UINT16 StructureMember);
void *find_efi_smbios_table(void);

#endif
