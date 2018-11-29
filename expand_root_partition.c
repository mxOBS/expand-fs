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

// enable realpath function
#define __USE_MISC

#include <getopt.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "find.h"
#include "guess.h"
#include "table.h"

void usage(const char *name) {
	printf("Usage: %s [-h]\n", name);
}

int expand_root_partition();

int main(int argc, char *argv[]) {
	const char *options;
	int c;

	// parse arguments
	options = "h";
	while ((c = getopt (argc, argv, options)) != -1) {
		switch (c) {
			case 'h':
				usage(argv[0]);
				return 0;
			case '?':
				printf("Error: encountered unknown option!\n");
				usage(argv[0]);
				return 1;
		}
	}

	return expand_root_partition();
}

int expand_root_partition() {
	char path[PATH_MAX] = {};
	char real_path[PATH_MAX] = {};
	char parent_path[PATH_MAX] = {};
	int s, partno;

	// first find root partition block device
	s = find_root_block_device(path);
	if(s != 0) {
		printf("Error: Failed to find root device!\n");
		return 1;
	}

	// follow symlink if any
	if(realpath(path, real_path) == 0) {
		printf("Error: Failed to dereference path %s!\n", path);
		return 1;
	}

	s = guess_parent_block_device(real_path, parent_path, &partno);
	if(s != 0) {
		printf("Error: Failed to guess parent block device of %s!\n", real_path);
		return s;
	}

	// and now to resize
	s = expand_partition(parent_path, partno);
	if(s != 0) {
		printf("Error: Failed to expand root partition!\n");
		return s;
	}

	return s;
};
