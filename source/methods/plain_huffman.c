#include "plain_huffman.h"

#include "components/huffman.h"

void plain_huffman_encode(FILE *input, bitfile_t *output) {
	uint32_t counts[256] = {0};
	for(uint8_t c; ; counts[c]++)
		if(fread(&c, 1, 1, input) == 0) break;
	fseek(input, 0, SEEK_SET);

	uint16_t last_symb = 0;
	for(uint16_t i = 0; i < 256; i++) if(counts[i] != 0) last_symb = i + 1;
	if(last_symb != 0) bitfile_write_num(output, last_symb - 1, 8, false);

	for(uint16_t i = 0; i < last_symb; i++)
		bitfile_write_bit(output, counts[i] != 0);

	for(uint16_t i = 0; i < last_symb; i++) {
		if(counts[i] == 0) continue;
		uint8_t bits = 32;
		for(; (counts[i] & 1 << (bits - 1)) == 0; bits--);
		bitfile_write_num(output, bits - 1, 5, false);
		if(bits == 1) continue;
		bitfile_write_num(output, counts[i], bits, false);
	}

	huffman_tree_t tree = huffman_tree_new(256);
	huffman_table_t table = huffman_table_new(256);
	huffman_tree_build(tree, counts);
	huffman_table_build(table, tree);

	for(uint8_t c; ; ) {
		if(fread(&c, 1, 1, input) == 0) break;
		huffman_code_t *code = &table->codes[c];
		bitfile_write_num(output, code->bits, code->count, true);
	}

	huffman_tree_del(tree);
	huffman_table_del(table);
}

void plain_huffman_decode(bitfile_t *input, FILE *output) {
	uint64_t size = 0;
	uint16_t last_symb = bitfile_read_num(input, 8, false) + 1;

	uint32_t counts[256] = {0};
	for(uint16_t i = 0; i < last_symb; i++)
		counts[i] = (bitfile_read_bit(input) ? 1 : 0);

	for(uint16_t i = 0; i < last_symb; i++) {
		if(counts[i] == 0) continue;
		uint8_t bits = bitfile_read_num(input, 5, false) + 1;
		if(bits > 1) counts[i] = bitfile_read_num(input, bits, false);
		size += counts[i];
	}

	huffman_tree_t tree = huffman_tree_new(256);
	huffman_table_t table = huffman_table_new(256);
	huffman_tree_build(tree, counts);
	huffman_table_build(table, tree);

	for(; size > 0; size--) {
		uint16_t my_root = tree->root_node;
		while(my_root >= tree->symbol_count) {
			bool bit = bitfile_read_bit(input);
			if(bit) my_root = tree->nodes[my_root].child1;
			else my_root = tree->nodes[my_root].child0;
		}
		fwrite(&my_root, 1, 1, output);
	}

	huffman_tree_del(tree);
	huffman_table_del(table);
}
