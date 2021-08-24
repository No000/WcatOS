#ifndef _FONT_H_
#define _FONT_H_


#include <stdint.h>
#include "graphic.h"

/* extern uint32_t cursor_x; */
/* extern uint32_t cursor_y; */

void k_print(VIDEO_INFO video_info, const char* format, ... );
void k_terminal_init();
void k_font_color_change(COLOR color);
void k_clear();
void k_back_color_change(COLOR color);

#endif
