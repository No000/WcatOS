#include "io.h"
#include "kbc.h"

#define ASCII_ESC	0x1b
#define ASCII_BS	0x08
#define ASCII_HT	0x09
#define ASCII_NOP   0x00

#define KBC_DATA_ADDR		0x0060
#define KBC_DATA_BIT_IS_BRAKE	0x80
#define KBC_STATUS_ADDR		0x0064
#define KBC_STATUS_BIT_OBF	0x01

const char keymap[] = {         /* keycodeの返り値を位置情報にしている */
	ASCII_NOP,                  /* 0 */
    ASCII_ESC,                  /* 1 */
    '1',                        /* 2 */
    '2',                        /* 3 */
    '3',
    '4',
    '5',
    '6',
	'7',
    '8',
    '9',
    '0',
    '-',
    '^',
    ASCII_BS,
    ASCII_HT,
	'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
	'o',
    'p',
    '@',
    '[',
    '\n',
    0x00,
    'a',
    's',
	'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ';',
	':',
    0x00,
    0x00,
    ']',
    'z',
    'x',
    'c',
    'v',
	'b',
    'n',
    'm',
    ',',
    '.',
    '/',
    0x00,
    '*',
	0x00,
    ' ',
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
	0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    '7',
	'8',
    '9',
    '-',
    '4',
    '5',
    '6',
    '+',
    '1',
	'2',
    '3',
    '0',
    '.',
    0x00,
    0x00,
    0x00,
    0x00,
	0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
	0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
	0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
	0x00,
    0x00,
    0x00,
    '_',
    0x00,
    0x00,
    0x00,
    0x00,
	0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    '\\',
    0x00,
    0x00
};

/* static char keymap[0x80] = { */
/*     0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '^', 0,   0, */
/*     'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '@', '[', 0,   0,   'A', 'S', */
/*     'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', ':', 0,   0,   ']', 'Z', 'X', 'C', 'V', */
/*     'B', 'N', 'M', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0, */
/*     0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1', */
/*     '2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, */
/*     0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, */
/*     0,   0,   0,   0x5c, 0,  0,   0,   0,   0,   0,   0,   0,   0,   0x5c, 0,  0 */
/*     }; */

void wait_KBC_sendReady(void){
    while (1) {
        if((in8(0x64) & 0x02) == 0){
            break;
        }
    }
}

unsigned char get_kbc_data(void)
{
	/* ステータスレジスタのOBFがセットされるまで待つ */
	while (!(in8(KBC_STATUS_ADDR) & KBC_STATUS_BIT_OBF));

	return in8(KBC_DATA_ADDR);
}

unsigned char get_keycode(void)
{
	unsigned char keycode;

	/* make状態(brakeビットがセットされていない状態)まで待つ */
	while ((keycode = get_kbc_data()) & KBC_DATA_BIT_IS_BRAKE);

	return keycode;
}

char getc(void)
{
	return keymap[get_keycode()];
}
