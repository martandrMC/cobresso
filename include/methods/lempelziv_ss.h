#ifndef METHODS_LEMPELZIV_SS_H
#define METHODS_LEMPELZIV_SS_H

#include <stdio.h>

#include "components/bitfile.h"

void lempelziv_ss_encode(FILE *input, bitfile_t *output);
void lempelziv_ss_decode(bitfile_t *input, FILE *output);

#endif // METHODS_LEMPELZIV_SS_H
