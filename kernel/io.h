#ifndef IO_H_
#define IO_H_
#include <stdint.h>

/* uint8_t inb(uint16_t addr); */
uint8_t in8(uint16_t addr);
void serialport_output(uint8_t ascii_code);
void out8(uint16_t addr, uint16_t data);

#endif

