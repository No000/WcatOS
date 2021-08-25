#ifndef GRAPHIC_H_
#define GRAPHIC_H_

#include <stdint.h>
#include "../boot/WcatBootPkg/WcatBoot/wcat_boot_header.h"


/* #pragma pack(1) */
/* カラーを指定するための構造体 */
typedef struct COLOR{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}__attribute__((packed)) COLOR;


typedef struct pixel_bit_mask{
    uint8_t blue_mask;
    uint8_t green_mask;
    uint8_t red_mask;
    uint8_t reserved_mask;
}__attribute__((packed)) pixel_bit_mask;

/* #pragma pack() */

void gop_init(struct WCAT_HEADER *wcat_boot_information);


void drow_back_color(COLOR color);

void drow_pixel(uint32_t x, uint32_t y, COLOR pixel_color);

void drow_horizon_pixel(uint32_t start_x, uint32_t start_y, uint32_t end_x,
                        COLOR pixel_color, VIDEO_INFO video_info);

void drow_vertical_pixel(uint32_t start_x, uint32_t start_y, uint32_t end_y,
                         COLOR pixel_color,
                         VIDEO_INFO video_info);

#endif

