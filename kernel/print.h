#ifndef _FONT_H_
#define _FONT_H_
#include <stdint.h>
#include "graphic.h"

extern uint32_t cursor_x;
extern uint32_t cursor_y;

void print_char(char c, VIDEO_INFO video_info, color pixel_color);
void print_string(char *string, VIDEO_INFO vudeo_info, color pixel_color);
void print_test(uint64_t i_i, VIDEO_INFO video_info, color pixel_color);

#endif
