/**
 * @file   kernel.c
 * @author toto <toto@toto-vm-development>
 * @date   Tue Feb  2 00:36:02 2021
 * 
 * @brief  WcatOSのカーネル
 * 
 * 
 */



#include <stdint.h>
#include "print.h"
#include "graphic.h"
#include "font.h"
#include "io.h"
#include "../boot/WcatBootPkg/WcatBoot/wcat_boot_header.h" /* シンボリックリンクに変更をする */
#include "wcatos_info.h"
#include "color.h"
#include "kbc.h"

#define FONT_HEIGHT 10
#define FONT_WIDTH 8


void hlt(void);

void hlt() {
  __asm__("hlt");
}



#define ASCII_ESC	0x1b
#define ASCII_BS	0x08
#define ASCII_HT	0x09
#define ASCII_NOP   0x00

#define KBC_DATA_ADDR		0x0060
#define KBC_DATA_BIT_IS_BRAKE	0x80
#define KBC_STATUS_ADDR		0x0064
#define KBC_STATUS_BIT_OBF	0x01




#pragma pack(1)
typedef struct {
    uint8_t    Type;
    uint8_t          Length;
    uint16_t  Handle;
} SMBIOS_STRUCTURE;

typedef struct {
  uint8_t   AnchorString[4];
  uint8_t   EntryPointStructureChecksum;
  uint8_t   EntryPointLength;
  uint8_t   MajorVersion;
  uint8_t   MinorVersion;
  uint16_t  MaxStructureSize;
  uint8_t   EntryPointRevision;
  uint8_t   FormattedArea[5];
  uint8_t   IntermediateAnchorString[5];
  uint8_t   IntermediateChecksum;
  uint16_t  TableLength;
  uint32_t  TableAddress;
  uint16_t  NumberOfSmbiosStructures;
  uint8_t   SmbiosBcdRevision;
} SMBIOS_TABLE_ENTRY_POINT;

#pragma pack()

/* 全体に公開するための */
struct WCAT_HEADER wcat_information;
struct WCATOS_CONTOROL_INFORMATION wcat_contorol_information;




void kernel_main(struct WCAT_HEADER *wcat_boot_information) {
  gop_init(wcat_boot_information);
  k_terminal_init();
  drow_back_color(wcat_contorol_information.kernel_terminal_information.terminal_back_color);
  wait_KBC_sendReady();
  

  int i = 1234;
  k_print("dec: %d hex: %x bin: %b ", sizeof(i), i, i);


  SMBIOS_TABLE_ENTRY_POINT *smtable;
  smtable = (SMBIOS_TABLE_ENTRY_POINT*)(wcat_boot_information->smbios_address);
  k_print("%c", smtable->AnchorString[0]);
  k_print("%c", smtable->AnchorString[1]);
  k_print("%c", smtable->AnchorString[2]);
  k_print("%c", smtable->AnchorString[3]);
  k_clear();
  while (1) {
		char c = getc();

        if (('!' <= c) && (c <= '~')){
            k_print("%c", c);
        } else if (c =='\n'){
            k_print("\r");
        } else if (c == ' '){
            k_print(" ");
        }
	}

  while (1)
    hlt();
}

#undef FONT_HEIGHT
#undef FONT_WIDTH
