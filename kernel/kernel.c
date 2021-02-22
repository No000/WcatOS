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



void kernel_main(VIDEO_INFO *video_infomation) {
  int i;
  uint8_t output_data[14] = "kernel_success";

  

  for (i = 0; i < 14; i++){
	serialport_output(output_data[i]);
  }

  pixel_bit_mask *frame_buffer =
      (pixel_bit_mask *)video_infomation->frame_buffer_addr;
  for (uint32_t i = 0; i < video_infomation->frame_buffer_size; ++i) {
    frame_buffer[i].red_mask = 0xad;
    frame_buffer[i].green_mask = 0xff;
    frame_buffer[i].blue_mask = 0x2f;
  }
  drow_pixel(1, 100, BLACK, *video_infomation);
  drow_horizon_pixel(100, 100, 500, BLACK, *video_infomation);
  drow_vertical_pixel(100, 100, 300, BLACK, *video_infomation);

  
  /* print_char('-', *video_infomation); */
  /* print_char('A', *video_infomation); */


  print_string("!\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~", *video_infomation, BLACK);
 
  while (1)
    hlt();
}
