#ifndef _WCAT_BOOT_HEADER_H_
#define _WCAT_BOOT_HEADER_H_
#include <stdint.h>

#pragma pack(1)

typedef  struct VIDEO_INFO {
  uint8_t *frame_buffer_addr;
  uint64_t frame_buffer_size;
  uint32_t horizen_size;
  uint32_t vertical_size;
  uint32_t pixel_per_scanline;
}VIDEO_INFO;

typedef struct WCAT_HEADER {
    VIDEO_INFO video_information;
}WCAT_HEADER;

#pragma pack()

#endif
