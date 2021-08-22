#include "graphic.h"
#include "../boot/WcatBootPkg/WcatBoot/wcat_boot_header.h" /* シンボリックリンクに変更をする */

extern WCAT_HEADER *wcat_information;

void gop_init(struct WCAT_HEADER *wcat_boot_information){
    wcat_information->video_information.frame_buffer_addr = wcat_boot_information->video_information.frame_buffer_addr;
    wcat_information->video_information.frame_buffer_size = wcat_boot_information->video_information.frame_buffer_size;
    wcat_information->video_information.horizen_size =  wcat_boot_information->video_information.horizen_size;
    wcat_information->video_information.vertical_size = wcat_boot_information->video_information.vertical_size;
    wcat_information->video_information.pixel_per_scanline = wcat_boot_information->video_information.pixel_per_scanline;
}

void drow_back_color(uint8_t red, uint8_t green, uint8_t blue){
    pixel_bit_mask *frame_buffer = (pixel_bit_mask *)wcat_information->video_information.frame_buffer_addr;
    for (uint32_t i = 0; i < wcat_information->video_information.frame_buffer_size; ++i) {
        frame_buffer[i].red_mask = red;
        frame_buffer[i].green_mask = green;
        frame_buffer[i].blue_mask = blue;
    }
}

/* 1ピクセル描画関数 */
void drow_pixel(uint32_t x, uint32_t y, COLOR pixel_color, VIDEO_INFO video_infomation) {
  pixel_bit_mask *p = (pixel_bit_mask *)video_infomation.frame_buffer_addr;
  uint32_t hr = video_infomation.horizen_size;
  pixel_bit_mask *draw_pixel_address = p + (hr * y) + x;
  draw_pixel_address->blue_mask = pixel_color.blue;
  draw_pixel_address->green_mask = pixel_color.green;
  draw_pixel_address->red_mask = pixel_color.red;
}

/* 水平描画関数 */
void drow_horizon_pixel(uint32_t start_x, uint32_t start_y, uint32_t end_x, COLOR pixel_color, VIDEO_INFO video_info)
{
  int i = 0;
  for (i = start_x; i < end_x; i++) {
	drow_pixel(i, start_y, pixel_color, video_info);
	  }
}

/* 垂直描画関数 */
void drow_vertical_pixel(uint32_t start_x, uint32_t start_y, uint32_t end_y, COLOR pixel_color, VIDEO_INFO video_info) {
  int i = 0;
  for (i = start_y; i < end_y; i++) {
    drow_pixel(start_x, i, pixel_color, video_info);
	  }
}
