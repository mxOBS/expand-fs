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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int guess_parent_block_device(const char *block_device, char *parent_block_device, int *partno) {
	const char *spartno;
	int l, i;

	// find alpha-numeric characters at the end
	l = strlen(block_device);
	for(i = 0; i < l; i++) {
		char c = block_device[l-i-1];
		if(c >= '0' && c < '9')
			continue;
		else
			break;
	}
	if(i == 0) {
		printf("Error: Failed to extract partition number from %s!\n", block_device);
		return 1;
	}
	// need no -1 because i points at first non-numeric character counting from the end
	spartno = block_device + l - i;

	*partno = atoi(spartno);
	if(*partno == 0) {
		printf("Error: Failed to convert numeric part to integer: %s!\n", spartno);
		return 1;
	}

	// in libfdisk, partition numbers start at 0
	(*partno)--;

	//printf("Notice: Identified root partition number as %i.\n", *partno);

	// now the first part of block_device is the parent block device, unless this is an unusual device name.
	strcpy(parent_block_device, block_device);
	// terminate string at first byte of partition number
	parent_block_device[l-i] = 0;

	// handle special-case mmcblkXpY
	if(strstr(parent_block_device, "/mmcblk") != 0) {
		//printf("Notice: identified mmc block device name, discarding superfluous suffix \"p\".\n");
		parent_block_device[l-i-1] = 0;
	}

	//printf("Notice: Identified parent block device as %s.\n", parent_block_device);
	return 0;
}
