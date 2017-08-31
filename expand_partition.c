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
#include <libfdisk/libfdisk.h>

int expand_partition(const char *block_device, size_t part_number);

void usage(const char *name) {
	printf("Usage: %s [-h] -b <block-device> -p <partition-number>\n", name);
}

int main(int argc, char *argv[]) {
	char *block_device = 0;
	char *s_part_number = 0;
	int part_number = 0;
	const char *options;
	const char *long_options;
	int c = 0, s = 0;

	// parse arguments
	options = "hb:p:";
	long_options = "block-device:,partition-number:";
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
	// actually partition numbers start at 0 internally
	part_number -= 1;

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

int expand_partition(const char *block_device, size_t part_number) {
	int s = 0;
	struct fdisk_context *ctx = 0;
	struct fdisk_label *lbl = 0;
	struct fdisk_table *tbl = 0;
	struct fdisk_iter *itr = 0;
	struct fdisk_partition *part = 0;
 	struct fdisk_partition *space = 0;
	uint64_t part_size, space_size, new_size;

	// create context
	ctx = fdisk_new_context();
	if(ctx == 0) {
		printf("Error: failed to create context!");
	}

	// first open block device
	s = fdisk_assign_device(ctx, block_device, 0);
	if(s != 0) {
		printf("Error: failed to assign %s!\n", block_device);
		goto error;
	}

	// see if it contains a known partition table
	lbl = fdisk_get_label(ctx, 0);
	if(lbl == 0) {
		printf("Error: failed to get label!\n");
		goto error;
	}
	printf("Found Disk Label %s.\n", fdisk_label_get_name(lbl));

	// get partitions
	s = fdisk_get_partitions (ctx, &tbl);
	if(s != 0) {
		printf("Error: failed to get partitions!\n");
		goto error;
	}

	// add free spaces to table
	s = fdisk_get_freespaces(ctx, &tbl);
	if(s != 0) {
		printf("Error: failed to get freespaces!\n");
		goto error;
	}

	// sort partitions and freespaces by physical location
	s = fdisk_table_sort_partitions(tbl, &fdisk_partition_cmp_start);
	if(s != 0) {
		printf("Error: failed to sort table!\n");
		goto error;
	}

#if 0
	// list all partitions
	itr = fdisk_new_iter(FDISK_ITER_FORWARD);
	while(fdisk_table_next_partition(tbl, itr, &part) == 0) {
		if(fdisk_partition_has_partno(part)) {
			printf("Found partition with number %zu\n", fdisk_partition_get_partno(part));
		} else if(fdisk_partition_is_freespace(part)) {
			printf("Found %lu bztes of free space\n", fdisk_partition_get_size(part));
		}
		part = 0;
	}
	fdisk_free_iter(itr); itr = 0;
#endif

	// loop over all partitions to find the one to resize
	itr = fdisk_new_iter(FDISK_ITER_FORWARD);
	while(fdisk_table_next_partition(tbl, itr, &part) == 0) {
		if(fdisk_partition_has_partno(part) && fdisk_partition_get_partno(part) == part_number) {
			// So this is the one, continue
			break;
		}
		part = 0;
	}
	if(part == 0) {
		printf("Error: failed to find partition %zu!\n", part_number);
		goto error;
	}

	// get the next partition if any
	s = fdisk_table_next_partition(tbl, itr, &space);
	if(s == 1 || s < 0 || !fdisk_partition_is_freespace(space)) {
		// No, partition or free space found
		// Maximum size already reached, report as success
		printf("Notice: Couldn't find any free space after partition.\n");
		goto success;
	}
	// free space was found
	fdisk_free_iter(itr); itr = 0;

	// calculate new size
	part_size = fdisk_partition_get_size(part);
	space_size = fdisk_partition_get_size(space);
	new_size = part_size + space_size;
	if(new_size < part_size) {
		printf("Error: overflow while calculating new size!\n");
		goto error;
	}

	// add size of free space to partition
	s = fdisk_partition_set_size(part, new_size);
	if(s != 0) {
		printf("Error: failed to set partition size to %lu!\n", new_size);
		goto error;
	}

	// apply new size to label
	s = fdisk_set_partition(ctx, part_number, part);
	if(s != 0) {
		printf("Error: failed to set partition!\n");
		goto error;
	}

	// Danger-Zone
	// write changes to disk
	s = fdisk_write_disklabel(ctx);
	if(s != 0) {
		printf("Error: failed to write disk label!\n");
		goto error;
	}

	// print result
	printf("Expanded partition %zu on %s from %lu to %lu bytes.\n", part_number, block_device, part_size, new_size);

	// done
success:
	// label is destroyed with context
	// destroy context
	if(ctx != 0)
		fdisk_unref_context(ctx); ctx = 0;
	return 0;

error:
	// label is destroyed with context
	// destroy context
	if(ctx != 0)
		fdisk_unref_context(ctx); ctx = 0;
	return 1;
}
