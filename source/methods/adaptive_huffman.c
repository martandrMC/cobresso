#include "adaptive_huffman.h"

#include "components/huffman.h"

#define SYM_EOF 256
#define SYM_ESC 257

void adaptive_huffman_encode(FILE *input, bitfile_t *output) {
	uint32_t counts[258] = {0};
	counts[SYM_EOF] = counts[SYM_ESC] = 1;

	huffman_tree_t tree = huffman_tree_new(258);
	huffman_table_t table = huffman_table_new(258);
	huffman_tree_build(tree, counts);
	huffman_table_build(table, tree);

	huffman_code_t *eof = &table->codes[SYM_EOF];
	huffman_code_t *esc = &table->codes[SYM_ESC];

	for(uint8_t c; fread(&c, 1, 1, input) != 0; ) {
		if(counts[c] == 0) {
			bitfile_write_num(output, esc->bits, esc->count, true);
			bitfile_write_num(output, c, 8, false);
		} else {
			huffman_code_t *code = &table->codes[c];
			bitfile_write_num(output, code->bits, code->count, true);
		}
		counts[c]++;
		huffman_tree_build(tree, counts);
		huffman_table_build(table, tree);
	}
	bitfile_write_num(output, eof->bits, eof->count, true);

	huffman_tree_del(tree);
	huffman_table_del(table);
}

void adaptive_huffman_decode(bitfile_t *input, FILE *output) {
	uint32_t counts[258] = {0};
	counts[SYM_EOF] = counts[SYM_ESC] = 1;

	huffman_tree_t tree = huffman_tree_new(258);
	huffman_table_t table = huffman_table_new(258);
	huffman_tree_build(tree, counts);
	huffman_table_build(table, tree);

	while(true) {
		uint16_t my_root = tree->root_node;
		while(my_root >= tree->symbol_count) {
			bool bit = bitfile_read_bit(input);
			if(bit) my_root = tree->nodes[my_root].child1;
			else my_root = tree->nodes[my_root].child0;
		}

		if(my_root == SYM_EOF) break;
		uint8_t c;
		if(my_root != SYM_ESC) c = my_root & 0xFF;
		else c = bitfile_read_num(input, 8, false);
		fwrite(&c, 1, 1, output);

		counts[c]++;
		huffman_tree_build(tree, counts);
		huffman_table_build(table, tree);
	}

	huffman_tree_del(tree);
	huffman_table_del(table);
}
