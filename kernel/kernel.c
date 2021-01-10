#include <stdint.h>
#include "graphic.h"
#include "font.h"

/* スケーラブルスクリーンフォントは後日へ */
/* #define SSFN_CONSOLEBITMAP_HICOLOR /\* use the special renderer for hicolor     \ */
/*                                       packed pixels *\/ */
/* #define NULL (void *)0 */
/* #include "scalable-font2/ssfn.h" */
/* extern unsigned char _binary_FreeSerifB_sfn_start; */

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

		/* 8x16サイズのbitmapfontの描画実験 */
/* 本来であれば改行コード等をswitchで分岐させる必要があるが、今回は文字の出力チェックなのでなし
 */
/* 1文字しかかけないことに注意 */
void print_char(char c, VIDEO_INFO video_info) {
  uint32_t cursor_x = 0, cursor_y = 0;
  int x = 0, y = 0;
  /* 実験なのでインデックスは0固定 */
  for (y = 0; y < FONT_HEIGHT; y++) {
    for (x = 0; x < FONT_WIDTH; x++) {
      if (font_bitmap[(uint32_t)c][y][x])
        drow_pixel(cursor_x + x, cursor_y + y, BLACK, video_info);
    }
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

  print_char('+', *video_infomation);

  /* ssfn_src = (uint8_t *)&_binary_FreeSerifB_sfn_start;      /\* the bitmap
   * font to use *\/ */

  /* ssfn_dst.ptr = video_infomation->frame_buffer_addr;                  /\*
   * address of the linear frame buffer *\/ */
  /* ssfn_dst.w = video_infomation->pixel_per_scanline; /\* width *\/ */
  /* ssfn_dst.h = video_infomation->vertical_size;                           /\*
   * height *\/ */
  /* ssfn_dst.p = video_infomation->pixel_per_scanline; /\* bytes per line *\/
   */
  /*   ssfn_dst.x = ssfn_dst.y = 0;             /\* pen position *\/ */
  /*   ssfn_dst.fg = 0xFFFFFFFF;                /\* foreground color *\/ */
  /*   ssfn_dst.bg = 0xFF000000;                /\* background color *\/ */
  /* ssfn_putc(0x41); */
 
  while (1)
    hlt();
}
