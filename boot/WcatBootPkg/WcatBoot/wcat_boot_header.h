#ifndef WCAT_BOOT_HEADER_H_
#define WCAT_BOOT_HEADER_H_
#include <stdint.h>


#pragma pack(1)


typedef  struct VIDEO_INFO {
  uint8_t *frame_buffer_addr;
  uint64_t frame_buffer_size;
  uint32_t horizen_size;
  uint32_t vertical_size;
  uint32_t pixel_per_scanline;
}VIDEO_INFO;

typedef struct MEMORY_MAP {
    unsigned long long buffer_size;
    void *buffer;
    unsigned long long map_size;
    unsigned long long map_key;
    unsigned long long descriptor_size;
    uint32_t descriptor_version;
}MEMORY_MAP;


/* 渡す構造体の本体 */
typedef struct WCAT_HEADER {
    VIDEO_INFO video_information;
    MEMORY_MAP memory_information;
    uint64_t smbios_address;
}WCAT_HEADER;

#pragma pack()



#endif
