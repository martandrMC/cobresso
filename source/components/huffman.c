#include "huffman.h"

#include <assert.h>
#include <stdlib.h>

huffman_tree_t huffman_tree_new(uint16_t symbol_count) {
	if(symbol_count > 32767) return NULL;
	uint16_t node_count = symbol_count * 2 + 1;
	size_t alloc_size = sizeof(huffman_tree_object_t);
	alloc_size += sizeof(huffman_node_t) * node_count;
	huffman_tree_t tree = calloc(1, alloc_size);
	if(tree == NULL) return NULL;
	tree->symbol_count = symbol_count;
	tree->node_count = node_count;
	tree->root_node = UINT16_MAX;
	return tree;
}

void huffman_tree_build(huffman_tree_t tree, const uint32_t *symbol_freqs) {
	if(tree == NULL || symbol_freqs == NULL) return;

	tree->nodes[tree->node_count - 1].count = UINT32_MAX;
	for(uint16_t i = 0; i < tree->symbol_count; i++) {
		tree->nodes[i].child0 = tree->nodes[i].child1 = UINT16_MAX;
		tree->nodes[i].count = symbol_freqs[i];
	}

	for(uint16_t next_free = tree->symbol_count; ; next_free++) {
		uint16_t min1 = tree->node_count - 1;
		uint16_t min2 = tree->node_count - 1;
		for(uint16_t i = 0; i < next_free; i++) {
			if(tree->nodes[i].count == 0) continue;
			if(tree->nodes[i].count < tree->nodes[min1].count) min2 = min1, min1 = i;
			else if(tree->nodes[i].count < tree->nodes[min2].count) min2 = i;
		}
		if(min2 == tree->node_count - 1) {
			if(next_free == tree->symbol_count) break;
			tree->root_node = next_free - 1;
			break;
		}
		tree->nodes[next_free].count = tree->nodes[min1].count + tree->nodes[min2].count;
		tree->nodes[next_free].child0 = min1, tree->nodes[next_free].child1 = min2;
		tree->nodes[min1].count = 0, tree->nodes[min2].count = 0;
	}
}

void huffman_tree_del(huffman_tree_t tree) {
	if(tree != NULL) free(tree);
}

huffman_table_t huffman_table_new(uint16_t symbol_count) {
	if(symbol_count > 32767) return NULL;
	size_t alloc_size = sizeof(huffman_table_object_t);
	alloc_size += sizeof(huffman_node_t) * symbol_count;
	huffman_table_t table = calloc(1, alloc_size);
	if(table != NULL) table->symbol_count = symbol_count;
	return table;
}

static void tree_walker(
	huffman_tree_t tree, uint16_t my_root,
	huffman_table_t table, huffman_code_t code_so_far
) {
	if(my_root < tree->symbol_count) {
		table->codes[my_root] = code_so_far;
		return;
	}

	code_so_far.count++, code_so_far.bits <<= 1;
	assert(code_so_far.count <= 16);
	tree_walker(tree, tree->nodes[my_root].child0, table, code_so_far);
	code_so_far.bits |= 1;
	tree_walker(tree, tree->nodes[my_root].child1, table, code_so_far);
}

void huffman_table_build(huffman_table_t table, huffman_tree_t tree) {
	if(table == NULL || tree == NULL) return;
	if(table->symbol_count != tree->symbol_count) return;
	if(tree->root_node == UINT16_MAX) return;
	tree_walker(tree, tree->root_node, table, (huffman_code_t){0});
}

void huffman_table_del(huffman_table_t table) {
	if(table != NULL) free(table);
}
