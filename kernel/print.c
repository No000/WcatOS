#include <stdint.h>
#include <stdarg.h>
#include "graphic.h"
#include "font.h"
#include "print.h"

#define PRIVATE static
#define PUBLIC                  /* extern ok */


#define FONT_HEIGHT 10
#define FONT_WIDTH 8

PUBLIC uint32_t cursor_x = 0;
PUBLIC uint32_t cursor_y = 0;

PRIVATE uint64_t bin2asc(char *str, uint64_t bin);
/* PRIVATE void print_char(char c, VIDEO_INFO video_info, color pixel_color); */
PRIVATE uint64_t hex2asc(char *str, uint64_t dec);
PRIVATE uint64_t dec2asc(char *str, uint64_t dec);
PRIVATE uint64_t bin2asc(char *str, uint64_t bin);


PUBLIC void print_char(char c, VIDEO_INFO video_info, color pixel_color);
PUBLIC void print_string(char* string, VIDEO_INFO vudeo_info, color pixel_color);
PUBLIC void print_test(uint64_t i_i, VIDEO_INFO video_info, color pixel_color);






PUBLIC void k_print(VIDEO_INFO video_info, color pixel_color,const char* format, ... ){
      va_list ap;
      va_start(ap, format);

      char* string_buff;
      int i = 0;
      
      for (string_buff = format; *string_buff != '\0'; string_buff++, i++) {
          if (*string_buff == '%') {
              string_buff++;
              i++;
              switch (*string_buff) {
              case 's':
                  print_string(va_arg(ap, char *),video_info, pixel_color);
                  break;
              case '%':
                  print_char('%', video_info, pixel_color);
                  break;
              case 'c':
                  print_char(va_arg(ap, int), video_info, pixel_color);
                  break;
              }
          } else {
              print_char(string_buff[i], video_info, pixel_color); /* うごいてない？ */
          }
      }

      va_end(ap);
}

PUBLIC void print_char(char c, VIDEO_INFO video_info, color pixel_color) {
  int x = 0, y = 0;
  /* 実験なのでインデックスは0固定 */
  for (y = 0; y < FONT_HEIGHT; y++) {
    for (x = 0; x < FONT_WIDTH; x++) {
      if (font_bitmap[(uint32_t)c][y][x])
        drow_pixel(cursor_x + x, cursor_y + y, pixel_color, video_info);
    }
  }
  cursor_x += FONT_WIDTH;
  if ((cursor_x + FONT_WIDTH) >= video_info.horizen_size) {
	cursor_x =0;
	cursor_y += FONT_HEIGHT;
	if ((cursor_y + FONT_HEIGHT) >= video_info.vertical_size) {
	  cursor_x = cursor_y = 0;
	}
  }
}

PUBLIC void print_string(char* string, VIDEO_INFO video_info, color pixel_color) {
  int i = 0;
  while (string[i] != '\0') {
	print_char(string[i], video_info, pixel_color);
	i++;
  }
}

//16進数からASCIIコードに変換
PRIVATE uint64_t hex2asc(char *str, uint64_t dec) {
    uint64_t len = 0, len_buf;
    char buf[16];
    while (1) {
        buf[len++] = dec % 16;
        if (dec < 16) break;
        dec /= 16;
    }
    len_buf = len;
    while (len) {
        len --;
        *(str++) = (buf[len]<10)?(buf[len] + 0x30):(buf[len] - 9 + 0x40);
    }
    return len_buf;
}


PRIVATE uint64_t dec2asc(char *str, uint64_t dec){
    uint64_t len = 0, len_buf;
    uint64_t buf[20];
    while (1){
        buf[len++] = dec % 10;
        if (dec < 10) break;
        dec /= 10;
    }
    len_buf = len;
    while(len){
        *(str++) = buf[--len] + 0x30;
    }
    return len_buf;
}

PRIVATE uint64_t bin2asc(char *str, uint64_t bin){
    uint64_t len = 0, len_buf;
    uint64_t buf[64];
    while (1) {
        buf[len++] = bin % 2;
        if (bin < 2) break;
        bin /= 2;
    }

    len_buf = len;
    while (len) {
        *(str++) = buf[--len] + 0x30;
    }
    return len_buf;
}


#define MAX64_DIGIT 64
PUBLIC void print_test(uint64_t i_i, VIDEO_INFO video_info, color pixel_color){
    char s[MAX64_DIGIT];
    uint64_t len;
    len = hex2asc(s, i_i);
    for (int i = 0; i < len; i++) {
        print_char(s[i], video_info, pixel_color);
    }
}
