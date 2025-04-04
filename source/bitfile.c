#include "bitfile.h"

bitfile_writer_t bitfile_open_write(const char *path) {
	bitfile_writer_t result = {0};
	if(path == NULL) result.file = stdout;
	else result.file = fopen(path, "wb");
	return result;
}

void bitfile_close_write(bitfile_writer_t *writer) {
	if(writer->file == NULL) return;
	while(writer->count != 0) bitfile_write_bit(writer, 0);
	if(writer->file != stdout) fclose(writer->file);
	writer->file = NULL;
}

void bitfile_write_bit(bitfile_writer_t *writer, bool bit) {
	if(writer->file == NULL) return;
	writer->buffer = writer->buffer >> 1 | (bit ? 1 << 7 : 0);
	if(++writer->count == 8) {
		fwrite(&writer->buffer, 1, 1, writer->file);
		writer->count = 0;
	}
}

void bitfile_write_num(bitfile_writer_t *writer, uint32_t num, uint8_t bits) {
	if(writer->file == NULL) return;
	for(uint8_t i = 0; i < bits; i++) {
		bool bit = ((num & 1 << i) != 0);
		bitfile_write_bit(writer, bit);
	}
}
