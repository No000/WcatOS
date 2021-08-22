#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>
#include "wcat_boot_header.h"

#include "gop.h"

void boot_gop_init(struct WCAT_HEADER *wcat_boot_information, EFI_GRAPHICS_OUTPUT_PROTOCOL *gop){
    wcat_boot_information->video_information.frame_buffer_addr = (uint8_t *)gop->Mode->FrameBufferBase;
    wcat_boot_information->video_information.frame_buffer_size = (uint64_t)gop->Mode->FrameBufferSize;
    wcat_boot_information->video_information.horizen_size = (uint32_t)gop->Mode->Info->HorizontalResolution;
    wcat_boot_information->video_information.vertical_size = (uint32_t)gop->Mode->Info->VerticalResolution;
    wcat_boot_information->video_information.pixel_per_scanline = (uint32_t)gop->Mode->Info->PixelsPerScanLine;
} 
