#ifndef _WCATOS_INFO_H_
#define _WCATOS_INFO_H_

#include <stdint.h>
#include "graphic.h"


typedef struct BASE_TERMINAL {
    uint64_t cursor_x;
    uint64_t cursor_y;
    uint64_t tab_width;
    COLOR terminal_font_color;
    COLOR terminal_back_color;
} BASE_TERMINAL;

typedef struct WCATOS_CONTOROL_INFORMATION {
    BASE_TERMINAL kernel_terminal_information;
}WCATOS_CONTOROL_INFORMATION;

#endif

