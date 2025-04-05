#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "methods/plain_huffman.h"

void print_help(char *exe_name) {
	fprintf(stderr, "Usage: %s [encode|decode] <input> <output>\n", exe_name);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
	if(argc != 4) print_help(argv[0]);
	enum { ERROR, ENCODE, DECODE } operation = ERROR;
	if(strcmp(argv[1], "encode") == 0) operation = ENCODE;
	else if(strcmp(argv[1], "decode") == 0) operation = DECODE;

	if(operation == ENCODE) {
		FILE *input = NULL;
		if(strcmp(argv[2], "-") == 0) input = stdin;
		else input = fopen(argv[2], "rb");
		if(input == NULL) perror("Input"), exit(EXIT_FAILURE);

		bitfile_t output = {0};
		output = bitfile_open_write((strcmp(argv[3], "-") == 0) ? NULL : argv[3]);
		if(output.file == NULL) perror("Output"), exit(EXIT_FAILURE);

		plain_huffman_encode(input, &output);

		bitfile_close(&output);
		if(input != stdin) fclose(input);
	} else if(operation == DECODE) {
		bitfile_t input = {0};
		input = bitfile_open_read((strcmp(argv[2], "-") == 0) ? NULL : argv[2]);
		if(input.file == NULL) perror("Input"), exit(EXIT_FAILURE);

		FILE *output = NULL;
		if(strcmp(argv[3], "-") == 0) output = stdout;
		else output = fopen(argv[3], "wb");
		if(output == NULL) perror("Ouput"), exit(EXIT_FAILURE);

		plain_huffman_decode(&input, output);

		bitfile_close(&input);
		if(output != stdout) fclose(output);
	} else print_help(argv[0]);

	exit(EXIT_SUCCESS);
}
