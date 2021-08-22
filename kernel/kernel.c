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

#define FONT_HEIGHT 10
#define FONT_WIDTH 8




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

/* 定義に関してはFreeBSDを参考 */
const char keymap[] = {         /* keycodeの返り値を位置情報にしている */
	ASCII_NOP,                  /* 0 */
    ASCII_ESC,                  /* 1 */
    '1',                        /* 2 */
    '2',                        /* 3 */
    '3',
    '4',
    '5',
    '6',
	'7',
    '8',
    '9',
    '0',
    '-',
    '^',
    ASCII_BS,
    ASCII_HT,
	'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
	'o',
    'p',
    '@',
    '[',
    '\n',
    0x00,
    'a',
    's',
	'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ';',
	':',
    0x00,
    0x00,
    ']',
    'z',
    'x',
    'c',
    'v',
	'b',
    'n',
    'm',
    ',',
    '.',
    '/',
    0x00,
    '*',
	0x00,
    ' ',
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
	0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    '7',
	'8',
    '9',
    '-',
    '4',
    '5',
    '6',
    '+',
    '1',
	'2',
    '3',
    '0',
    '.',
    0x00,
    0x00,
    0x00,
    0x00,
	0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
	0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
	0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
	0x00,
    0x00,
    0x00,
    '_',
    0x00,
    0x00,
    0x00,
    0x00,
	0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    '\\',
    0x00,
    0x00
};


void wait_KBC_sendReady(void){
    while (1) {
        if((in8(0x64) & 0x02) == 0){
            break;
        }
    }
}

unsigned char get_kbc_data(void)
{
	/* ステータスレジスタのOBFがセットされるまで待つ */
	while (!(in8(KBC_STATUS_ADDR) & KBC_STATUS_BIT_OBF));

	return in8(KBC_DATA_ADDR);
}

unsigned char get_keycode(void)
{
	unsigned char keycode;

	/* make状態(brakeビットがセットされていない状態)まで待つ */
	while ((keycode = get_kbc_data()) & KBC_DATA_BIT_IS_BRAKE);

	return keycode;
}

char getc(void)
{
	return keymap[get_keycode()];
}


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
struct WCAT_HEADER *wcat_information;
struct WCATOS_CONTOROL_INFORMATION *wcat_contorol_information;




void kernel_main(struct WCAT_HEADER *wcat_boot_information) {
  gop_init(wcat_boot_information);
  /* drow_back_color(0xad, 0xff, 0x2f); */
  drow_back_color(GREENYELLOW);
  k_terminal_init();

  wait_KBC_sendReady();


  int i = 1234;
  k_print(wcat_boot_information->video_information, BLACK, "dec: %d hex: %x bin: %b ", sizeof(i), i, i);
  k_print(wcat_boot_information->video_information, BLACK, "\t");
  k_print(wcat_boot_information->video_information, BLACK, "\ra\ra\ra\raa\n\n\na");


  SMBIOS_TABLE_ENTRY_POINT *smtable;
  smtable = (SMBIOS_TABLE_ENTRY_POINT*)(wcat_boot_information->smbios_address);
  k_print(wcat_boot_information->video_information, AQUA, "%c", smtable->AnchorString[0]);
  k_print(wcat_boot_information->video_information, BLACK, "%c", smtable->AnchorString[1]);
  k_print(wcat_boot_information->video_information, BLACK, "%c", smtable->AnchorString[2]);
  k_print(wcat_boot_information->video_information, BLACK, "%c", smtable->AnchorString[3]);
  while (1) {
		char c = getc();

        if (('!' <= c) && (c <= '~')){
            k_print(wcat_boot_information->video_information, WHITE, "%c", c);
        } else if (c =='\n'){
            k_print(wcat_boot_information->video_information, AQUA, "\r");
        } else if (c == ' '){
            k_print(wcat_boot_information->video_information, AQUA, " ");
        }
	}

  while (1)
    hlt();
}

#undef FONT_HEIGHT
#undef FONT_WIDTH
