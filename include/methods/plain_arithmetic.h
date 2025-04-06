#ifndef METHODS_PLAIN_ARITHMETIC_H
#define METHODS_PLAIN_ARITHMETIC_H

#include <stdio.h>

#include "components/bitfile.h"

void plain_arithmetic_encode(FILE *input, bitfile_t *output);
void plain_arithmetic_decode(bitfile_t *input, FILE *output);

#endif // METHODS_PLAIN_ARITHMETIC_H
