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

#include <dirent.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

#include "find.h"

int find_root_block_device(char *buffer) {
	int s;

	s = find_root_block_device_by_proc_mounts(buffer);
	if(s == 0) {
		// all good
		return 0;
	}

	s = find_root_block_device_by_stat(buffer);
	if(s == 0) {
		// all good
		return 0;
	}

	// failed
	return 1;
}

int find_root_block_device_by_proc_mounts(char *buffer) {
	FILE *f;
	char fmt[100] = {};
	char line[PATH_MAX] = {};
	char dev[PATH_MAX] = {};
	char mnt[PATH_MAX] = {};
	char fs[PATH_MAX] = {};
	int s = 0;

	f = fopen("/proc/mounts", "r");
	if(f == 0) {
		printf("Error: Failed to open %s for reading!\n", "/proc/mounts");
		return 1;
	}

	// create format-string for scanf (with size of buffers)
	snprintf(fmt, sizeof(fmt), "%%%zus %%%zus %%%zus", sizeof(dev), sizeof(mnt), sizeof(fs));
	//printf("Format string: %s\n", fmt);
	
	// match device-node, mountpoint, filesystem, and then the remainder of the line
	while(fgets(line, PATH_MAX-1, f) != 0) {
		// parse line
		s = sscanf(line, fmt, dev, mnt, fs);

		// see if this is the entry for /
		if(strcmp("/", mnt) == 0) {
			//printf("Notice: Found %s mounted as %s.\n", dev, mnt);

			// use this one and abort search
			strcpy(buffer, dev);

			fclose(f);
			return 0;
		}
	}

	fclose(f);
	return 1;
}

int major = 0;
int minor = 0;
int filter_by_major_minor(const struct dirent *entry);

int find_root_block_device_by_stat(char *buffer) {
	struct stat st = {};
	int s, i;
	struct dirent **dir;
	// first stat / to find out major and minor numbers
	s = stat("/", &st);
	if(s != 0) {
		printf("Error: failed to stat /!\n");
		return 1;
	}

	// st.st_dev is the device containing /
	major = major(st.st_dev);
	minor = minor(st.st_dev);
	//printf("Notice: Found device ID (major:minor): %i:%i.\n", major, minor);

	// traverse /dev to find it
	s = scandir("/dev", &dir, &filter_by_major_minor, &alphasort);
	if(s < 0) {
		printf("Error: failed to scan /dev!\n");
		return 1;
	}
	if(s == 0) {
		//printf("Notice: didn't find a matching file in /dev.\n");
		return 1;
	}

	// Heuristic: pick the first result. The result set is already sorted.
	strcpy(buffer, dir[0]->d_name);
	free(dir[0]);

	// clean up result set
	for(i = 1; i < s; i++) {
		free(dir[i]);
	}
	free(dir);

	// success
	return 0;
}

/**
 * filter dirent entries for block devices with matching major and minor
 */
int filter_by_major_minor(const struct dirent *entry) {
	struct stat st = {};
	char path[PATH_MAX] = {};
	int s, _major, _minor;

	// construct absolute path
	strcat(path, "/dev/");
	strcat(path, entry->d_name);

	s = stat(path, &st);
	if(s != 0) {
		//printf("Notice: skipping file %s: failed to stat.\n", path);
		return 0;
	}

	// is block device?
	if(!S_ISBLK(st.st_mode)) {
		//printf("Notice: skipping file %s: Not a block device.\n", path);
		return 0;
	}

	// get major, minor of special device
	_major = major(st.st_rdev);
	_minor = minor(st.st_rdev);
	if(major != _major || minor != _minor) {
		//printf("Notice: skipping block device %s (major:minor): %i:%i.\n", path, _major, _minor);
		return 0;
	}

	//printf("Notice: keeping block device %s.\n", path);

	// keep entry
	return 1;
}
