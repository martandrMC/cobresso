#ifndef BITFILE_H
#define BITFILE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct bitfile_writer {
	FILE *file;
	uint8_t buffer;
	uint8_t count;
} bitfile_writer_t;

#include "bitfile.h"

bitfile_writer_t bitfile_open_write(const char *path);
void bitfile_close_write(bitfile_writer_t *writer);
void bitfile_write_bit(bitfile_writer_t *writer, bool bit);
void bitfile_write_num(bitfile_writer_t *writer, uint32_t num, uint8_t bits);

#endif //BITFILE_H
