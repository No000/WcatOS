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

#define FONT_HEIGHT 10
#define FONT_WIDTH 8

color BLACK = {0x00, 0x00, 0x00};


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

/* static char keymap[0x80] = { */
/*     0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0, */
/*     'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S', */
/*     'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V', */
/*     'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0, */
/*     0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1', */
/*     '2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, */
/*     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, */
/*     0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0 */
/*     }; */

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

void kernel_main(struct WCAT_HEADER *wcat_boot_information) {
  int i;
  uint8_t output_data[14] = "kernel_success";

  for (i = 0; i < 14; i++){
	serialport_output(output_data[i]);
  }


  pixel_bit_mask *frame_buffer =
      (pixel_bit_mask *)wcat_boot_information->video_information.frame_buffer_addr;
  for (uint32_t i = 0; i < wcat_boot_information->video_information.frame_buffer_size; ++i) {
    frame_buffer[i].red_mask = 0xad;
    frame_buffer[i].green_mask = 0xff;
    frame_buffer[i].blue_mask = 0x2f;
  }
  drow_pixel(1, 100, BLACK, wcat_boot_information->video_information);
  drow_horizon_pixel(100, 100, 500, BLACK, wcat_boot_information->video_information);
  drow_vertical_pixel(100, 100, 300, BLACK, wcat_boot_information->video_information);

  


  uint64_t test_val = 10;

  print_test(test_val, wcat_boot_information->video_information, BLACK);

  print_test(wcat_boot_information->video_information.horizen_size, wcat_boot_information->video_information, BLACK);
  print_string(" ", wcat_boot_information->video_information, BLACK);




  wait_KBC_sendReady();
  out8(0x60, 0xad);


  

  while (1) {
		char c = getc();
        if (c == '\n')
            print_char('\r', wcat_boot_information->video_information, BLACK);

        print_char(c, wcat_boot_information->video_information, BLACK);
	}
  /* out8(0x0064, 0xf4); */
  /* unsigned char test_data = 0; */
  /* while (1) { */
  /*       test_data = in8(KBC_DATA_ADDR); */
  /*       print_test(test_data, wcat_boot_information->video_information, BLACK); */
  /*         } */
  /* test_data = in8(0x0060); */
  /* print_test(test_data, wcat_boot_information->video_information, BLACK); */
  /* while (1) { */
  /*     unsigned char c = get_keycode(); */
  /*     print_test(c, wcat_boot_information->video_information, BLACK); */
  /*         } */

  while (1)
    hlt();
}

#undef FONT_HEIGHT
#undef FONT_WIDTH
