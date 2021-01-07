#include <stdint.h>

/* スケーラブルスクリーンフォントは後日へ */
/* #define SSFN_CONSOLEBITMAP_HICOLOR /\* use the special renderer for hicolor     \ */
/*                                       packed pixels *\/ */
/* #define NULL (void *)0 */
/* #include "scalable-font2/ssfn.h" */
/* extern unsigned char _binary_FreeSerifB_sfn_start; */

#define INTEL_ASM_BEGIN ".intel_syntax noprefix\n\t" /* clangの場合.att_syntax prefixは必要ない */

typedef struct VIDEO_INFO {
  uint8_t *frame_buffer_addr;
  uint64_t frame_buffer_size;
  uint32_t horizen_size;
  uint32_t vertical_size;
  uint32_t pixel_per_scanline;
} VIDEO_INFO;

typedef struct pixel_bit_mask{
  uint8_t blue_mask;
  uint8_t green_mask;
  uint8_t red_mask;
  uint8_t reserved_mask;
} pixel_bit_mask;

/* カラーを指定するための構造体 */
typedef struct color{
  uint8_t blue;
  uint8_t green;
  uint8_t red;
} color;



/* ピクセル描画関数 */
void drow_pixel(uint8_t x, uint8_t y, color pixel_color, VIDEO_INFO video_infomation) {
  pixel_bit_mask *p = (pixel_bit_mask *)video_infomation.frame_buffer_addr;
  uint32_t hr = video_infomation.horizen_size;
  pixel_bit_mask *draw_pixel_address = p + (hr * y) + x;
  draw_pixel_address->blue_mask = pixel_color.blue;
  draw_pixel_address->green_mask = pixel_color.green;
  draw_pixel_address->red_mask = pixel_color.red;
}

void serialport_output(uint8_t ascii_code) {
  __asm__ volatile(INTEL_ASM_BEGIN
                   "mov dx, 0x3f8\n\t"
                   /* "mov al, 1\n" */
                   "out dx, al\n\t"
				   :
				   :"a"(ascii_code)/* EAXレジスタに変数の値を代入 */
				   :"%dx");		   /* clover_listでレジスタを破壊 */
}

void hlt() {
  __asm__("hlt");
}

void kernel_main(VIDEO_INFO *video_infomation) {
  int i;
  uint8_t output_data[14] = "kernel_success";

  color BLACK = {0x00, 0x00, 0x00};
  for (i = 0; i < 14; i++){
	serialport_output(output_data[i]);
  }

  pixel_bit_mask *frame_buffer = (pixel_bit_mask *)video_infomation->frame_buffer_addr;
  for (uint32_t i = 0; i < video_infomation->frame_buffer_size; ++i) {
    frame_buffer[i].red_mask = 0xad;
    frame_buffer[i].green_mask = 0xff;
    frame_buffer[i].blue_mask = 0x2f;
  }
  drow_pixel(1, 100, BLACK, *video_infomation);


/* ssfn_src = (uint8_t *)&_binary_FreeSerifB_sfn_start;      /\* the bitmap font to use *\/ */

/* ssfn_dst.ptr = video_infomation->frame_buffer_addr;                  /\* address of the linear frame buffer *\/ */
/* ssfn_dst.w = video_infomation->pixel_per_scanline;                          /\* width *\/ */
/* ssfn_dst.h = video_infomation->vertical_size;                           /\* height *\/ */
/* ssfn_dst.p = video_infomation->pixel_per_scanline;                          /\* bytes per line *\/ */
/*   ssfn_dst.x = ssfn_dst.y = 0;             /\* pen position *\/ */
/*   ssfn_dst.fg = 0xFFFFFFFF;                /\* foreground color *\/ */
/*   ssfn_dst.bg = 0xFF000000;                /\* background color *\/ */
/* ssfn_putc(0x41); */
 
  while (1)
    hlt();
}
