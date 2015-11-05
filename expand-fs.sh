#!/bin/bash -e
#
# Copyright (c) 2015 Josua Mayer
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

# arguments
if [ $# != 2 ] && [ $# != 3 ]; then
	echo "Usage: $0 <device> <partition> [<stage>]"
	exit 1
fi
device=$1
partition=$2
stage=$3

# check runtime dependencies
programs="fdisk grep sed cut printf partprobe fsck resize2fs"
for prog in $programs; do
	s=0
	which $prog 1>/dev/null 2>/dev/null || s=$?
	if [ $s != 0 ]; then
		echo "$prog not found!"
		exit 1
	fi
done

# extract only lines about partitions
fdisk_skip_text() {
	while read line; do
		if [[ "$line" = /dev/* ]]; then
			echo "$line"
		fi
	done
}

# expand partition
if [ -z "$stage" ] || [ "$stage" = "1" ]; then
	# get data on partition
	data=`fdisk -l $device | fdisk_skip_text | sed -e "s;*;;g" -e "s; \+; ;g" | grep "$partition"`
	start=`echo $data | cut -d' ' -f2`

	# extract partition number
	partnum=`echo $partition | sed -e "s;$device;;" -e "s;p;;g"`

	# recreate partition
	printf "d\n%i\nn\np\n%i\n%i\n\nw\nq\n" $partnum $partnum $start | sudo fdisk $device || true
fi

# reload partition table
s=0
partprobe $device || s=$?
if [ $s != 0 ]; then
	echo "Failed to reload partition table, filesystem has not been expanded. Please reboot as soon as possible!"
	exit 2
fi

if [ -z "$stage" ] || [ "$stage" = "2" ]; then
	# resize filesystem
	fsck -f $partition || true
	s=0
	resize2fs $partition || s=$?
	if [ $s != 0 ]; then
		echo "Failed to resize filesystem! This is an undesired state and should not have happened!"
		exit 3
	fi
fi

echo "Finished expanding $partition!"
exit 0
