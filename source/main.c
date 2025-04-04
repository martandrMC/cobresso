#include <stdio.h>
#include <stdlib.h>

#include "bitfile.h"

int main(void) {
	bitfile_writer_t writer = bitfile_open_write("test.bin");
	bitfile_write_num(&writer, 42069, 16);
	bitfile_close_write(&writer);

	exit(EXIT_SUCCESS);
}
