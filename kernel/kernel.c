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

#include "graphic.h"
#include "font.h"

#define FONT_HEIGHT 10
#define FONT_WIDTH 8

color BLACK = {0x00, 0x00, 0x00};

#define INTEL_ASM_BEGIN ".intel_syntax noprefix\n\t" /* clangの場合.att_syntax prefixは必要ない */

/* out命令におきかえる */
void serialport_output(uint8_t ascii_code) {
  __asm__ volatile(INTEL_ASM_BEGIN
                   "mov dx, 0x3f8\n\t"
                   "out dx, al\n\t"
				   :
				   :"a"(ascii_code)/* EAXレジスタに変数の値を代入 */
				   :"%dx");		   /* clover_listでレジスタを破壊 */
}

void hlt() {
  __asm__("hlt");
}

/* 文字のインデックス用構造体 */
/* typedef struct CURSOR{ */
/*   uint32_t cursor_x; */
/*   uint32_t cursor_y; */
/* }CURSOR; */

		/* 8x16サイズのbitmapfontの描画実験 */
/* 本来であれば改行コード等をswitchで分岐させる必要があるが、今回は文字の出力チェックなのでなし
 */
/* 1文字しかかけないことに注意 */

uint32_t cursor_x = 0;
uint32_t cursor_y = 0;


void print_char(char c, VIDEO_INFO video_info, color pixel_color) {
  int x = 0, y = 0;
  /* 実験なのでインデックスは0固定 */
  for (y = 0; y < FONT_HEIGHT; y++) {
    for (x = 0; x < FONT_WIDTH; x++) {
      if (font_bitmap[(uint32_t)c][y][x])
        drow_pixel(cursor_x + x, cursor_y + y, pixel_color, video_info);
    }
  }
  cursor_x += FONT_WIDTH;
  if ((cursor_x + FONT_WIDTH) >= video_info.horizen_size) {
	cursor_x =0;
	cursor_y += FONT_HEIGHT;
	if ((cursor_y + FONT_HEIGHT) >= video_info.vertical_size) {
	  cursor_x = cursor_y = 0;
	}
  }
}

void print_string(char *string, VIDEO_INFO vudeo_info, color pixel_color) {
  int i = 0;
  while (string[i] != '\0') {
	print_char(string[i], vudeo_info, pixel_color);
	i++;
  }
}

//16進数からASCIIコードに変換
uint64_t hex2asc (char *str, uint64_t dec) {
    uint64_t len = 0, len_buf;
    char buf[16];
    while (1) {
        buf[len++] = dec % 16;
        if (dec < 16) break;
        dec /= 16;
    }
    len_buf = len;
    while (len) {
        len --;
        *(str++) = (buf[len]<10)?(buf[len] + 0x30):(buf[len] - 9 + 0x40);
    }
    return len_buf;
}


uint64_t dec2asc(char *str, uint64_t dec){
    uint64_t len = 0, len_buf;
    uint64_t buf[20];
    while (1){
        buf[len++] = dec % 10;
        if (dec < 10) break;
        dec /= 10;
    }
    len_buf = len;
    while(len){
        *(str++) = buf[--len] + 0x30;
    }
    return len_buf;
}

uint64_t bin2asc(char *str, uint64_t bin){
    uint64_t len = 0, len_buf;
    uint64_t buf[64];
    while (1) {
        buf[len++] = bin % 2;
        if (bin < 2) break;
        bin /= 2;
    }

    len_buf = len;
    while (len) {
        *(str++) = buf[--len] + 0x30;
    }
    return len_buf;
}


#define MAX64_DIGIT 64
void print_test(uint64_t i_i, VIDEO_INFO video_info, color pixel_color){
    char s[MAX64_DIGIT];
    uint64_t len;
    len = hex2asc(s, i_i);
    for (int i = 0; i < len; i++) {
        print_char(s[i], video_info, pixel_color);
    }
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

  
  /* print_char('-', *video_infomation); */
  /* print_char('A', *video_infomation); */

  uint64_t test_val = 10;

  /* print_string("!\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", wcat_boot_information->video_information, BLACK); */
  print_test(test_val, wcat_boot_information->video_information, BLACK);

  print_test(wcat_boot_information->video_information.horizen_size, wcat_boot_information->video_information, BLACK);
  print_char(' ', wcat_boot_information->video_information, BLACK);


  while (1)
    hlt();
}

#undef FONT_HEIGHT
#undef FONT_WIDTH
