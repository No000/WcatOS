#ifndef KBC_H_
#define KBC_H_

void wait_KBC_sendReady(void);

unsigned char get_kbc_data(void);

unsigned char get_keycode(void);

char getc(void);

#endif
