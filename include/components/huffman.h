#ifndef COMPONENTS_HUFFMAN_H
#define COMPONENTS_HUFFMAN_H

#include <stdint.h>

typedef struct huffman_node {
	uint32_t count;
	uint16_t child0;
	uint16_t child1;
} huffman_node_t;

typedef struct huffman_tree_object {
	uint16_t root_node;
	uint16_t node_count;
	uint16_t symbol_count;
	huffman_node_t nodes[];
} huffman_tree_object_t;

typedef struct huffman_tree_object *huffman_tree_t;

huffman_tree_t huffman_tree_new(uint16_t symbol_count);
void huffman_tree_build(huffman_tree_t tree, const uint32_t *symbol_freqs);
void huffman_tree_del(huffman_tree_t tree);

typedef struct huffman_code {
	uint16_t bits;
	uint16_t count;
} huffman_code_t;

typedef struct huffman_table_object {
	uint16_t symbol_count;
	huffman_code_t codes[];
} huffman_table_object_t;

typedef struct huffman_table_object *huffman_table_t;

huffman_table_t huffman_table_new(uint16_t symbol_count);
void huffman_table_build(huffman_table_t table, huffman_tree_t tree);
void huffman_table_del(huffman_table_t table);

#endif // COMPONENTS_HUFFMAN_H
