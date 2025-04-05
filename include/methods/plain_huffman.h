#ifndef METHODS_PLAIN_HUFFMAN_H
#define METHODS_PLAIN_HUFFMAN_H

#include <stdio.h>

#include "components/bitfile.h"

void plain_huffman_encode(FILE *input, bitfile_t *output);
void plain_huffman_decode(bitfile_t *input, FILE *output);

#endif // METHODS_PLAIN_HUFFMAN_H
