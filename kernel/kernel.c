#include <stdint.h>

#define INTEL_ASM_BEGIN ".intel_syntax noprefix\n\t" /* clangの場合.att_syntax prefixは必要ない */

typedef struct VIDEO_INFO {
  uint8_t *frame_buffer_addr;
  uint64_t frame_buffer_size;
  uint32_t horizen_size;
  uint32_t vertical_size;
  uint32_t pixel_per_scanline;
} VIDEO_INFO;

typedef struct pixel_bit_mask{
  uint8_t red_mask;
  uint8_t green_mask;
  uint8_t blue_mask;
  uint8_t reserved_mask;
} pixel_bit_mask;


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
  uint8_t output_data[14] = {75, 69, 82, 78, 69, 76, 95, 83, 85, 67, 67, 69, 83, 83};

  for (i = 0; i < 14; i++){
	serialport_output(output_data[i]);
  }

  pixel_bit_mask *frame_buffer = (pixel_bit_mask *)video_infomation->frame_buffer_addr;
    for (uint32_t i = 0; i < video_infomation->frame_buffer_size; ++i) {
	  frame_buffer[i].red_mask = 0xad;
	  frame_buffer[i].green_mask = 0xff;
	  frame_buffer[i].blue_mask  = 0x2f;
	}

  while (1) 	hlt();
}
