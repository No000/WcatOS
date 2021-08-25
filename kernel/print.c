#include <stdint.h>
#include <stdarg.h>
#include "graphic.h"
#include "font.h"
#include "print.h"
#include "wcatos_info.h"
#include "color.h"

#define PRIVATE static
#define PUBLIC                  /* extern ok */


#define FONT_HEIGHT 10
#define FONT_WIDTH 8


extern WCAT_HEADER wcat_information;
extern WCATOS_CONTOROL_INFORMATION wcat_contorol_information;
#define CURSOR_X wcat_contorol_information.kernel_terminal_information.cursor_x
#define CURSOR_Y wcat_contorol_information.kernel_terminal_information.cursor_y
#define TERMINAL_FONT_COLOR wcat_contorol_information.kernel_terminal_information.terminal_font_color
#define TERMINAL_BACK_COLOR wcat_contorol_information.kernel_terminal_information.terminal_back_color

#define TAB_WIDTH 4             /* 構造体で管理するようにする */

PRIVATE uint64_t bin2asc(char *str, uint64_t bin);
PRIVATE uint64_t hex2asc(char *str, uint64_t dec);
PRIVATE uint64_t dec2asc(char *str, uint64_t dec);
PRIVATE void draw_char(char c);
PRIVATE void print_string(char* string);

PUBLIC void k_terminal_init(void){
    CURSOR_X = 0;
    CURSOR_Y = 0;
    TERMINAL_FONT_COLOR = BLACK;
    TERMINAL_BACK_COLOR = WHITE;
}

PUBLIC void k_font_color_change(COLOR color){
    TERMINAL_FONT_COLOR = color;
}

PUBLIC void k_clear(void){
    CURSOR_X = 0;
    CURSOR_Y = 0;
    drow_back_color(wcat_contorol_information.kernel_terminal_information.terminal_back_color);
    
}

PUBLIC void k_back_color_change(COLOR color){
    wcat_contorol_information.kernel_terminal_information.terminal_back_color = AQUA;
}

/* %0x系はsizeofを使えばいい */
#define BIN_MAX64_DIGIT 64
#define HEX_MAX64_DIGIT 16
#define DEC_MAX64_DIGIT 20
PUBLIC void k_print(const char* format, ... ){
      va_list ap;
      va_start(ap, format);

      const char* string_buff;

      for (string_buff = format; *string_buff != '\0'; string_buff++) {
          if (*string_buff == '%') {
              string_buff++;
              /* i++; */
              switch (*string_buff) {
              case 's':
                  print_string(va_arg(ap, char *));
                  break;
              case '%':
                  draw_char('%');
                  break;
              case 'c':
                  draw_char(va_arg(ap, int));
                  break;
              case 'd':
                  char dec_s[DEC_MAX64_DIGIT];
                  uint64_t dec_len;
                  dec_len = dec2asc(dec_s, va_arg(ap, uint64_t));
                  for (int i = 0; i < dec_len; i++) {
                      draw_char(dec_s[i]);
                  }
                  break;
              case 'x':
                  char hex_s[HEX_MAX64_DIGIT];
                  uint64_t hex_len;
                  hex_len = hex2asc(hex_s, va_arg(ap, uint64_t));
                  for (int i = 0; i < hex_len; i++) {
                      draw_char(hex_s[i]);
                  }
                  break;
              case 'b':
                  char bin_s[BIN_MAX64_DIGIT];
                  uint64_t bin_len;
                  bin_len = bin2asc(bin_s, va_arg(ap, uint64_t));
                  for (int i = 0; i < bin_len; i++) {
                      draw_char(bin_s[i]);
                  }
                  break;
              }
          } else {
              draw_char(string_buff[0]); /* ポインタを直接動かしているので先頭のみを表示 */
          }
      }
      va_end(ap);
}


/* ASCIIの処理もここで行ってる */
PRIVATE void draw_char(char c) {
    uint32_t x = 0, y = 0;
    switch (c) {
    case '\r':
        CURSOR_Y += FONT_HEIGHT; /* ここでveriticalsizeのif分岐を追加（bug） */
        break;
    case '\t':
        CURSOR_X += (TAB_WIDTH * FONT_WIDTH);/* ここでhorizen_sizeのif分岐を追加（bug） */
        break;
    case '\n':
        CURSOR_Y += FONT_HEIGHT;
        break;
    default:
        for (y = 0; y < FONT_HEIGHT; y++) {
            for (x = 0; x < FONT_WIDTH; x++) {
                if (font_bitmap[(uint32_t)c][y][x])
                    drow_pixel(CURSOR_X + x, CURSOR_Y + y, TERMINAL_FONT_COLOR);
            }
        }
        CURSOR_X += FONT_WIDTH;
    }
    if ((CURSOR_X + FONT_WIDTH) >= wcat_information.video_information.horizen_size) {
        CURSOR_X =0;
        CURSOR_Y += FONT_HEIGHT;
        if ((CURSOR_Y + FONT_HEIGHT) >= wcat_information.video_information.vertical_size) {
            CURSOR_X = CURSOR_Y = 0;
        }
    }
}

PRIVATE void print_string(char* string) {
    int i = 0;
    while (string[i] != '\0') {
        draw_char(string[i]);
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

#undef CURSOR_X
#undef CURSOR_Y
#undef TERMINAL_FONT_COLOR
#undef BIN_MAX64_DIGIT
#undef HEX_MAX64_DIGIT
#undef DEC_MAX64_DIGIT

