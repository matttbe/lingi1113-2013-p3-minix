#!/bin/bash

DISK=/mnt/disk
TEST_FILE=$DISK/test.txt
TMP_FILE_PRE=$DISK/tmp
TMP_FILE0=${TMP_FILE_PRE}0

if test -d $DISK ; then
	echo "$DISK already exists, go next"
else
	echo "create dir: $DISK"
	mkdir $DISK
	/sbin/mkfs.mfs /dev/c0d1
	sleep 1 ## it seems we need a delay
	mount /dev/c0d1 $DISK
	sleep 1 ## it seems we need a delay
fi

echo "Create test file: $TEST_FILE"
dd if=/dev/urandom of=$TEST_FILE count=10 ibs=4096 obs=4096 bs=4096

echo "Fragment this file"
cp $TEST_FILE $TMP_FILE0

# seq is not available..
for i in 1 2 3 4 5; do
	cat $TMP_FILE0 >> $TEST_FILE
	cp  $TMP_FILE0 $TMP_FILE_PRE$i
done
rm $TMP_FILE_PRE*
