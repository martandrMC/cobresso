#include "plain_arithmetic.h"

void plain_arithmetic_encode(FILE *input, bitfile_t *output) {
	(void) output;
	uint32_t counts[256] = {0};
	for(uint8_t c; ; counts[c]++)
		if(fread(&c, 1, 1, input) == 0) break;
	fseek(input, 0, SEEK_SET);
}

void plain_arithmetic_decode(bitfile_t *input, FILE *output) {
	(void) input;
	(void) output;
}
