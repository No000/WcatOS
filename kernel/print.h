#ifndef _FONT_H_
#define _FONT_H_


#include <stdint.h>
#include "graphic.h"

/* extern uint32_t cursor_x; */
/* extern uint32_t cursor_y; */

void k_print(VIDEO_INFO video_info, color pixel_color,const char* format, ... );
void terminal_init();

#endif
