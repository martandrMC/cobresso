#ifndef COMPONENTS_BITFILE_H
#define COMPONENTS_BITFILE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct bitfile {
	FILE *file;
	enum {
		BITFILE_CLOSED,
		BITFILE_READER,
		BITFILE_WRITER
	} type;
	uint8_t buffer;
	uint8_t count;
} bitfile_t;

bitfile_t bitfile_open_read(const char *path);
bitfile_t bitfile_open_write(const char *path);
void bitfile_close(bitfile_t *file);

bool bitfile_read_bit(bitfile_t *reader);
uint32_t bitfile_read_num(bitfile_t *reader, uint8_t bits, bool msb_first);

void bitfile_write_bit(bitfile_t *writer, bool bit);
void bitfile_write_num(bitfile_t *writer, uint32_t num, uint8_t bits, bool msb_first);

#endif // COMPONENTS_BITFILE_H
