#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>
#include "wcat_boot_header.h"

#ifndef _GOP_H_
#define _GOP_H_



void boot_gop_init(struct WCAT_HEADER *wcat_boot_information, EFI_GRAPHICS_OUTPUT_PROTOCOL *gop);

#endif
