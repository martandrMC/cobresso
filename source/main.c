#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "methods/plain_huffman.h"
#include "methods/adaptive_huffman.h"
#include "methods/plain_arithmetic.h"
#include "methods/lempelziv_ss.h"

static void print_help(char *exe_name) {
	fprintf(stderr,
		"Usage: %s [encode|decode] <method> <input> <output>\n"
		"For input/output, specify `-` for stdin/stout, otherwise path to file.\n"
		"Methods available:\n"
		"	`phuff`: Plain Huffman coding; frequency table + encoded content.\n"
		"	`ahuff`: Adaptive Huffman coding; Implicit frequency table. SLOW!!!\n"
		"	`parith`: Plain Arithmetic coding; frequency table + encoded content.\n"
		"	`lzss`: Lempel-Ziv-Storer-Szymanski coding; Backreferences on sliding window.\n"
	, exe_name);
	exit(EXIT_FAILURE);
}

enum method { MT_ERROR, MT_PHUFF, MT_AHUFF, MT_PARITH, MT_LZSS };

static void select_encoder(
	enum method method,
	const char *input_path,
	const char *output_path
) {
	FILE *input = NULL;
	if(strcmp(input_path, "-") == 0) input = stdin;
	else input = fopen(input_path, "rb");
	if(input == NULL) perror("Input"), exit(EXIT_FAILURE);

	bitfile_t output = {0};
	output_path = (strcmp(output_path, "-") == 0) ? NULL : output_path;
	output = bitfile_open_write(output_path);
	if(output.file == NULL) perror("Output"), exit(EXIT_FAILURE);

	switch(method) {
		case MT_PHUFF: plain_huffman_encode(input, &output); break;
		case MT_AHUFF: adaptive_huffman_encode(input, &output); break;
		case MT_PARITH: plain_arithmetic_encode(input, &output); break;
		case MT_LZSS: lempelziv_ss_encode(input, &output); break;
		default:;
	}

	bitfile_close(&output);
	if(input != stdin) fclose(input);
}

static void select_decoder(
	enum method method,
	const char *input_path,
	const char *output_path
) {
	bitfile_t input = {0};
	input_path = (strcmp(input_path, "-") == 0) ? NULL : input_path;
	input = bitfile_open_read(input_path);
	if(input.file == NULL) perror("Input"), exit(EXIT_FAILURE);

	FILE *output = NULL;
	if(strcmp(output_path, "-") == 0) output = stdout;
	else output = fopen(output_path, "wb");
	if(output == NULL) perror("Ouput"), exit(EXIT_FAILURE);

	switch(method) {
		case MT_PHUFF: plain_huffman_decode(&input, output); break;
		case MT_AHUFF: adaptive_huffman_decode(&input, output); break;
		case MT_PARITH: plain_arithmetic_decode(&input, output); break;
		case MT_LZSS: lempelziv_ss_decode(&input, output); break;
		default:;
	}

	if(output != stdout) fclose(output);
	bitfile_close(&input);
}

int main(int argc, char **argv) {
	if(argc != 5) print_help(argv[0]);

	enum { OP_ERROR, OP_ENCODE, OP_DECODE } operation = OP_ERROR;
	if(strcmp(argv[1], "encode") == 0) operation = OP_ENCODE;
	else if(strcmp(argv[1], "decode") == 0) operation = OP_DECODE;
	if(operation == OP_ERROR) print_help(argv[0]);

	enum method method = MT_ERROR;
	if(strcmp(argv[2], "phuff") == 0) method = MT_PHUFF;
	else if(strcmp(argv[2], "ahuff") == 0) method = MT_AHUFF;
	else if(strcmp(argv[2], "parith") == 0) method = MT_PARITH;
	else if(strcmp(argv[2], "lzss") == 0) method = MT_LZSS;
	if(method == MT_ERROR) print_help(argv[0]);

	if(operation == OP_ENCODE) select_encoder(method, argv[3], argv[4]);
	else if(operation == OP_DECODE) select_decoder(method, argv[3], argv[4]);

	exit(EXIT_SUCCESS);
}
