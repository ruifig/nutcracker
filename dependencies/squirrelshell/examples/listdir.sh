#!/bin/sh

# List directory contents.

if [ -z $1 ]; then
	path=.
else
	path=$1
fi

for file in `ls -a $path`; do
	if [ $file = . ] || [ $file = .. ]; then
		continue
	fi

	pth=$path/$file
	if [ -d $pth ]; then
		echo "dir : $pth"
	elif [ -f $pth ]; then
		echo "file: $pth"
	fi
done