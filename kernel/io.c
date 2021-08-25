#include <stdint.h>
#include "io.h"

#define PRIVATE static
#define PUBLIC                  /* extern ok */

#define INTEL_ASM_BEGIN ".intel_syntax noprefix\n"
/* \tでエラーがおきたので */
/* clangの場合.att_syntax prefixは必要ない */

/* out命令におきかえる */
PUBLIC void serialport_output(uint8_t ascii_code) {
    __asm__ volatile(INTEL_ASM_BEGIN
                     "mov dx, 0x3f8\n\t"
                     "out dx, al\n\t"
                     :
                     :"a"(ascii_code)/* EAXレジスタに変数の値を代入 */
                     :"%dx");		   /* clover_listでレジスタを破壊 */
}

PUBLIC uint8_t in8(uint16_t addr)
{
	unsigned char value;
	__asm__ volatile (INTEL_ASM_BEGIN
                      "in al, dx\n\t"
                      :"=a"(value)
                      :"d"(addr)
                      );
	return value;
}


PUBLIC void out8(uint16_t addr, uint16_t data)
{
	__asm__ volatile (INTEL_ASM_BEGIN
                      "out dx, al\n\t"
                      :"=a"(data),"=d"(addr)
                      );
}

#undef PRIVATE
#undef PUBLIC
