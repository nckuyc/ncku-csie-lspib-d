#!/bin/bash

writefile=$1
writestr=$2

if [ $# -ne 2 ]; then
	echo "Error: Missing arguments"
	exit 1
fi

writedir=$(dirname "$writefile")

mkdir -p "$writedir"
if [ $? -ne 0 ]; then 
	echo "Error: Could not create the directory"
	exit 1
fi

echo "$writestr" > "$writefile"
if [ $? -ne 0 ]; then
	echo "Error: Could not write the file"
	exit 1
fi

