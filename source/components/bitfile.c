#include "bitfile.h"

bitfile_t bitfile_open_read(const char *path) {
	bitfile_t result = {0};
	if(path == NULL) result.file = stdin;
	else result.file = fopen(path, "rb");
	result.type = BITFILE_READER;
	return result;
}

bitfile_t bitfile_open_write(const char *path) {
	bitfile_t result = {0};
	if(path == NULL) result.file = stdout;
	else result.file = fopen(path, "wb");
	result.type = BITFILE_WRITER;
	return result;
}

void bitfile_close(bitfile_t *file) {
	switch(file->type) {
		case BITFILE_CLOSED:
			return;
		case BITFILE_READER:
			if(file->file != stdin) fclose(file->file);
			break;
		case BITFILE_WRITER:
			while(file->count != 0) bitfile_write_bit(file, 0);
			if(file->file != stdout) fclose(file->file);
			break;
	}

	file->type = BITFILE_CLOSED;
}

bool bitfile_read_bit(bitfile_t *reader) {
	if(reader->type != BITFILE_READER) return false;
	if(reader->count == 0) {
		fread(&reader->buffer, 1, 1, reader->file);
		reader->count = 8;
	}
	bool bit = (reader->buffer & 1) != 0;
	reader->buffer >>= 1, reader->count--;
	return bit;
}

uint32_t bitfile_read_num(bitfile_t *reader, uint8_t bits, bool msb_first) {
	if(reader->type != BITFILE_READER) return 0;
	uint32_t num = 0;
	for(uint8_t i = 0; i < bits; i++) {
		uint8_t shift = (msb_first ? bits - i - 1 : i);
		bool bit = bitfile_read_bit(reader);
		num |= (bit ? 1 << shift : 0);
	}
	return num;
}

void bitfile_write_bit(bitfile_t *writer, bool bit) {
	if(writer->type != BITFILE_WRITER) return;
	writer->buffer = writer->buffer >> 1 | (bit ? 1 << 7 : 0);
	if(++writer->count == 8) {
		fwrite(&writer->buffer, 1, 1, writer->file);
		writer->count = 0;
	}
}

void bitfile_write_num(bitfile_t *writer, uint32_t num, uint8_t bits, bool msb_first) {
	if(writer->type != BITFILE_WRITER) return;
	for(uint8_t i = 0; i < bits; i++) {
		uint8_t shift = (msb_first ? bits - i - 1 : i);
		bool bit = ((num & 1 << shift) != 0);
		bitfile_write_bit(writer, bit);
	}
}
