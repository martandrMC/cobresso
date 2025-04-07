#include "lempelziv_ss.h"

#include <stdlib.h>

#define SB_SIZE 16384
#define LA_SIZE 128
#define WIN_SIZE (SB_SIZE + LA_SIZE)

#define SYM_EOF 256

// TODO: Rewrite this... Moving the window manually + doing linear search is slow.

static void shift_in(
	uint16_t *window, uint16_t win_size,
	FILE *input, uint16_t amount
) {
	for(uint16_t i = 0; i < win_size - amount; i++)
		window[i] = window[i + amount];

	for(uint16_t i = 0; i < amount; i++) {
		uint8_t c; uint16_t sym;
		if(fread(&c, 1, 1, input) != 0) sym = c;
		else sym = SYM_EOF;
		window[win_size - amount + i] = sym;
	}
}

void lempelziv_ss_encode(FILE *input, bitfile_t *output) {
	uint8_t bits_sb = 1; while((1 << bits_sb) < SB_SIZE) bits_sb++;
	uint8_t bits_la = 1; while((1 << bits_la) < LA_SIZE) bits_la++;
	bitfile_write_num(output, bits_sb - 1, 5, false);
	bitfile_write_num(output, bits_la - 1, 3, false);

	uint16_t window[WIN_SIZE], *la_buf = &window[SB_SIZE];
	for(uint16_t i = 0; i < WIN_SIZE; i++) window[i] = SYM_EOF;

	for(uint16_t i = 0; i < LA_SIZE; i++) {
		uint8_t c; if(fread(&c, 1, 1, input) == 0) break;
		la_buf[i] = c;
	}

	do {
		uint16_t pair_pos, pair_len = 0;
		for(uint16_t i = SB_SIZE - 1; window[i] != SYM_EOF; ) {
			uint16_t match_size = 0;
			for(; match_size < LA_SIZE; match_size++)
				if(window[i + match_size] != la_buf[match_size]) break;
			if(match_size > pair_len) {
				pair_len = match_size;
				pair_pos = SB_SIZE - i;
			}
			if(i-- == 0) break;
		}
		if(pair_len > 2) {
			bitfile_write_bit(output, true);
			bitfile_write_num(output, pair_pos - 1, bits_sb, false);
			bitfile_write_num(output, pair_len - 1, bits_la, false);
			shift_in(window, WIN_SIZE, input, pair_len);
		} else {
			bitfile_write_bit(output, false);
			bitfile_write_num(output, la_buf[0], 9, false);
			shift_in(window, WIN_SIZE, input, 1);
		}
	} while(window[SB_SIZE - 1] != SYM_EOF);
}

static void shift_out(
	uint16_t *window, uint16_t *la_buf, uint16_t win_size,
	FILE *output, uint16_t amount
) {
	for(uint16_t i = 0; i < amount; i ++) {
		if(la_buf[i] == SYM_EOF) break;
		uint8_t c = la_buf[i] & 0xFF;
		fwrite(&c, 1, 1, output);
	}

	for(uint16_t i = 0; i < win_size - amount; i++)
		window[i] = window[i + amount];
}

void lempelziv_ss_decode(bitfile_t *input, FILE *output) {
	(void) output;
	uint8_t bits_sb = bitfile_read_num(input, 5, false) + 1;
	uint8_t bits_la = bitfile_read_num(input, 3, false) + 1;

	const uint16_t sb_size = 1 << bits_sb, la_size = 1 << bits_la;
	const uint16_t win_size = sb_size + la_size;
	uint16_t *window = malloc(win_size * sizeof(uint16_t)), *la_buf = &window[sb_size];
	for(uint16_t i = 0; i < win_size; i++) window[i] = SYM_EOF;

	while(true) {
		if(bitfile_read_bit(input)) {
			uint16_t pair_pos = bitfile_read_num(input, bits_sb, false) + 1;
			uint16_t pair_len = bitfile_read_num(input, bits_la, false) + 1;
			for(uint16_t i = 0; i < pair_len; i++)
				la_buf[i] = window[sb_size - pair_pos + i];
			shift_out(window, la_buf, win_size, output, pair_len);
		} else {
			la_buf[0] = bitfile_read_num(input, 9, false);
			if(la_buf[0] == SYM_EOF) break;
			shift_out(window, la_buf, win_size, output, 1);
		}
	}

	free(window);
}
