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
#include <linux/limits.h>
#include <stdio.h>

#include "find.h"

void usage(const char *name) {
	printf("Usage: %s [-h] [-q]\n", name);
}

int main(int argc, char *argv[]) {
	const char *options;
	int c, s, quiet;
	char path[PATH_MAX] = {};

	// parse arguments
	options = "hq";
	quiet = 0;
	while ((c = getopt (argc, argv, options)) != -1) {
		switch (c) {
			case 'h':
				usage(argv[0]);
				return 0;
			case 'q':
				quiet = 1;
				break;
			case '?':
				printf("Error: encountered unknown option!\n");
				usage(argv[0]);
				return 1;
		}
	}

	// find root block
	s = find_root_block_device(path);
	if(s != 0) {
		if(quiet == 0)
			printf("Couldn't find root device!\n");
		return s;
	}

	// print result
	if(quiet == 0) {
			printf("Found root device: %s\n", path);
	} else {
			printf("%s\n", path);
	}

	return s;
}
