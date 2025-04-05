#include <stdio.h>
#include <stdlib.h>

#include "bitfile.h"
#include "huffman.h"

void print_code(huffman_code_t code) {
	for(uint8_t i = code.count; i > 0; i--) {
		putchar('0' + (code.bits >> (i - 1) & 1));
	}
}

int main(int argc, char **argv) {
	if(argc < 2) exit(EXIT_FAILURE);

	uint32_t counts[256] = {0};
	FILE *file = fopen(argv[1], "rb");
	if(file == NULL) exit(EXIT_FAILURE);
	for(uint8_t c; ; counts[c]++)
		if(fread(&c, 1, 1, file) == 0) break;
	fclose(file);

	huffman_tree_t tree = huffman_tree_new(256);
	huffman_table_t table = huffman_table_new(256);
	huffman_tree_build(tree, counts);
	huffman_table_build(table, tree);

	for(uint16_t i = 0; i < table->symbol_count; i++) {
		if(table->codes[i].count == 0) continue;
		printf("%02x ", i);
		print_code(table->codes[i]);
		putchar('\n');
	}

	huffman_tree_del(tree);
	huffman_table_del(table);
	exit(EXIT_SUCCESS);
}
