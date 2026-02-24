#!/bin/sh

filesdir=$1
searchstr=$2

if [ $# -lt 2 ]; then
	echo "Error: Missing arguments"
	exit 1
fi

if [ ! -d "$filesdir" ]; then
	echo "Error: The directory does not exist"
	exit 1
fi

num_files=$(find "$filesdir" -type f | wc -l)

num_matching=$(grep -r "$searchstr" "$filesdir" | wc -l)

echo "The number of files are $num_files and the number of matching lines are $num_matching"

