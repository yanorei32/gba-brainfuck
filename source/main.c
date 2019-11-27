#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>

#define CONSOLE_WIDTH	30
#define CONSOLE_HEIGHT	20

typedef enum BRAINFUCK_OPS {
	B_BLANK		= 0,
	B_INC_VAL	= 1,
	B_DEC_VAL	= 2,
	B_INC_PTR	= 3,
	B_DEC_PTR	= 4,
	B_PUT_VAL	= 5,
	B_GET_VAL	= 6,
	B_WHILE		= 7,
	B_DO_WHILE	= 8,
} BRAINFUCK_OPS;

char brainfuck_op_chars[] = {
	' ',
	'+', '-',
	'>', '<',
	'.', ',',
	'[', ']',
};

typedef enum STATUS_FLAGS {
	S_RUNNED		= (1<<0),
	S_NEED_REDRAW	= (1<<1),
	S_PRAM_RESET	= (1<<2),
} STATUS_FLAGS;

typedef enum MODE {
	M_EDITOR	= 0,
	M_RUNNER	= 1,
} MODE;

unsigned char pram[CONSOLE_WIDTH*CONSOLE_HEIGHT];
unsigned char dram[128];

void run() {
	int dram_p, pram_p, block_cnt;
	u16 key_state;

	dram_p ^= dram_p;
	for(;dram_p < sizeof(dram)/sizeof(char);++dram_p)
		dram[dram_p] ^= dram[dram_p];

	iprintf("\033[2J\x1b[0;0H");

	dram_p ^= dram_p;
	block_cnt ^= block_cnt;
	pram_p ^= pram_p;

	for (;pram_p < sizeof(pram)/sizeof(char);++pram_p) {
		switch(pram[pram_p]) {
			case B_BLANK:
				break;

			case B_INC_PTR:
				++dram_p;
				break;

			case B_DEC_PTR:
				--dram_p;
				break;

			case B_INC_VAL:
				if (dram_p < 0) {
					iprintf("\nERROR: PTR < 0");
					return;
				}

				if (sizeof(dram) / sizeof(char) <= dram_p) {
					iprintf("\nERROR: PTR >= RAM SIZE");
					return;
				}

				++dram[dram_p];

				break;

			case B_DEC_VAL:
				if (dram_p < 0) {
					iprintf("\nERROR: PTR < 0");
					return;
				}

				if (sizeof(dram) / sizeof(char) <= dram_p) {
					iprintf("\nERROR: PTR >= RAM SIZE");
					return;
				}

				--dram[dram_p];

				break;

			case B_PUT_VAL:
				if (dram_p < 0) {
					iprintf("\nERROR: PTR < 0");
					return;
				}

				if (sizeof(dram) / sizeof(char) <= dram_p) {
					iprintf("\nERROR: PTR >= RAM SIZE");
					return;
				}

				if (!dram[dram_p]) {
					iprintf("\nERROR: '\\0' OUTPUT.");
					return;
				}

				iprintf("%c", dram[dram_p]);
				break;

			case B_GET_VAL:
				iprintf("\n");

				while (1) {
					iprintf("\033[9DINPUT: %02X", dram[dram_p]);

					scanKeys();
					key_state = keysDown();

					if (key_state & KEY_START)
						break;

					if (key_state & (KEY_A | KEY_UP))
						++dram[dram_p];

					if (key_state & (KEY_B | KEY_DOWN))
						--dram[dram_p];

					if (key_state & KEY_LEFT)
						dram[dram_p] -= 0x10;

					if (key_state & KEY_RIGHT)
						dram[dram_p] += 0x10;

					if (key_state & KEY_SELECT)
						goto stop;

					VBlankIntrWait();
				}
				iprintf("\n");
				
				break;

			case B_WHILE:
				if (!dram[dram_p]) {
					for (;;++pram_p) {
						if (sizeof(pram) / sizeof(char) <= pram_p) {
							iprintf("\nERROR: FAILED TO FIND ']'");
							return;
						}

						if (pram[pram_p] == B_WHILE) {
							++block_cnt;
							continue;
						}

						if (pram[pram_p] == B_DO_WHILE && !--block_cnt)
							break;
					}
				}
				break;

			case B_DO_WHILE:
				if (dram[dram_p]) {
					for (;;--pram_p) {
						if (pram_p < 0) {
							iprintf("\nERROR: FAILED TO FIND '['");
							return;
						}

						if (pram[pram_p] == B_DO_WHILE) {
							++block_cnt;
							continue;
						}

						if (pram[pram_p] == B_WHILE && !--block_cnt)
							break;
					}
				}
				break;
		}

		scanKeys();
		if (keysDown() & KEY_SELECT) {
stop:
			iprintf("\nKeyboard Interrupt");
			break;
		}
	}
}

void draw_pram() {
	char draw_pram_str[sizeof(pram)/sizeof(char)+1];
	int pram_p;

	pram_p ^= pram_p;

	for (;pram_p < sizeof(pram)/sizeof(char);++pram_p)
		draw_pram_str[pram_p] = brainfuck_op_chars[pram[pram_p]];

	draw_pram_str[sizeof(draw_pram_str)/sizeof(char)-1] = '\0';

	iprintf("\x1b[0;0H%s", draw_pram_str);
}

void reset_pram() {
	int pram_p;

	pram_p ^= pram_p;

	for (;pram_p < sizeof(pram)/sizeof(char);++pram_p)
		pram[pram_p] = B_BLANK;
}

int main() {
	int cur;
	MODE m;
	STATUS_FLAGS s;
	u16 key_state;

	irqInit();
	irqEnable(IRQ_VBLANK);

	consoleDemoInit();
	reset_pram();

	cur ^= cur;
	m ^= m;
	s = S_NEED_REDRAW;

	while (1) {
		scanKeys();
		key_state = keysDown();

		switch (m) {
			case M_EDITOR:
				if (s & S_RUNNED) {
					s &= ~S_RUNNED;
					s |= S_NEED_REDRAW;
				}

				if (key_state & (KEY_UP | KEY_A)) {
					if (++pram[cur] == sizeof(brainfuck_op_chars)/sizeof(char))
						pram[cur] ^= pram[cur];

					s |= S_NEED_REDRAW;
					s &= ~S_PRAM_RESET;
				}

				if (key_state & KEY_DOWN) {
					if (!pram[cur]--)
						pram[cur] = sizeof(brainfuck_op_chars)/sizeof(char)-1;

					s |= S_NEED_REDRAW;
					s &= ~S_PRAM_RESET;
				}

				if (key_state & (KEY_RIGHT | KEY_B)) {
					if (++cur >= sizeof(pram)/sizeof(char))
						cur ^= cur;

					s &= ~S_PRAM_RESET;
				}

				if (key_state & KEY_LEFT) {
					if (!cur--)
						cur = sizeof(pram)/sizeof(char)-1;

					s &= ~S_PRAM_RESET;
				}

				if (key_state & KEY_START) {
					m = M_RUNNER;
					s &= ~S_PRAM_RESET;
				}

				if (key_state & KEY_SELECT) {
					if (!(s & S_PRAM_RESET)) {
						s |= S_PRAM_RESET;
					} else {
						reset_pram();
						cur ^= cur;
						s &= ~S_PRAM_RESET;
						s |= S_NEED_REDRAW;
					}
				}

				if (s & S_NEED_REDRAW)
					draw_pram();

				break;

			case M_RUNNER:
				if (!(s & S_RUNNED)) {
					run();
					s |= S_RUNNED;
				}

				if (key_state & KEY_SELECT)
					m = M_EDITOR;

				break;
		}

		VBlankIntrWait();
	}
}

