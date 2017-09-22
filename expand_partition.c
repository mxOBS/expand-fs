/* 
 * MIT License
 * 
 * Copyright (c) 2017 Josua Mayer <josua.mayer97@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "table.h"

int expand_partition(const char *block_device, size_t part_number);

void usage(const char *name) {
	printf("Usage: %s [-h] -b <block-device> -p <partition-number>\n", name);
}

int main(int argc, char *argv[]) {
	char *block_device = 0;
	char *s_part_number = 0;
	int part_number = 0;
	const char *options;
	int c = 0, s = 0;

	// parse arguments
	options = "hb:p:";
	while ((c = getopt (argc, argv, options)) != -1) {
		switch (c) {
			case 'h':
				usage(argv[0]);
				return 0;
			case 'b':
				block_device = optarg;
				break;
			case 'p':
				s_part_number = optarg;
				break;
			case '?':
				printf("Error: encountered unknown option!\n");
				usage(argv[0]);
				return 1;
		}
	}

	// check arguments
	if(block_device == 0) {
		printf("Error: no block device specified!\n");
		usage(argv[0]);
		return 1;
	}
	if(s_part_number == 0) {
		printf("Error: no partition number specified!\n");
		usage(argv[0]);
		return 1;
	}
	part_number = atoi(s_part_number);
	if(part_number == 0 || part_number < 0) {
		printf("Error: no valid partition number specified!\n");
		return 1;
	}

	// in libfdisk, partition numbers start at 0
	part_number--;

	// check access permissions for block device
	s = access(block_device, F_OK|R_OK|W_OK);
	if(s != 0) {
		printf("Error: no permission for writing %s!\n", block_device);
		return 1;
	}

	// expand partition
	// not
	return expand_partition(block_device, part_number);
}
