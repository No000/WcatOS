#include "graphic.h"

/* 1ピクセル描画関数 */
void drow_pixel(uint32_t x, uint32_t y, color pixel_color, VIDEO_INFO video_infomation) {
  pixel_bit_mask *p = (pixel_bit_mask *)video_infomation.frame_buffer_addr;
  uint32_t hr = video_infomation.horizen_size;
  pixel_bit_mask *draw_pixel_address = p + (hr * y) + x;
  draw_pixel_address->blue_mask = pixel_color.blue;
  draw_pixel_address->green_mask = pixel_color.green;
  draw_pixel_address->red_mask = pixel_color.red;
}

/* 水平描画関数 */
void drow_horizon_pixel(uint32_t start_x, uint32_t start_y, uint32_t end_x, color pixel_color, VIDEO_INFO video_info)
{
  int i = 0;
  for (i = start_x; i < end_x; i++) {
	drow_pixel(i, start_y, pixel_color, video_info);
	  }
}

/* 垂直描画関数 */
void drow_vertical_pixel(uint32_t start_x, uint32_t start_y, uint32_t end_y, color pixel_color, VIDEO_INFO video_info) {
  int i = 0;
  for (i = start_y; i < end_y; i++) {
    drow_pixel(start_x, i, pixel_color, video_info);
	  }
}
