#ifndef METHODS_ADAPTIVE_HUFFMAN_H
#define METHODS_ADAPTIVE_HUFFMAN_H

#include <stdio.h>

#include "components/bitfile.h"

void adaptive_huffman_encode(FILE *input, bitfile_t *output);
void adaptive_huffman_decode(bitfile_t *input, FILE *output);

#endif // METHODS_ADAPTIVE_HUFFMAN_H
