#!/bin/bash

if ! [ "$#" -eq "2" ]
then
	echo "$#"
	echo "Incorrect number of args."
	echo "Correct usage: script1.sh (image directory) (output folder)"
	exit 1
fi
if ! [ -d "$1" ]
then
	echo "First argument is not a directory."
	exit 1
fi
if ! [ -d "$2" ]
then
	echo "Second argument is not a directory."
	exit 1
fi
gcc tile.c image_IO_TGA.c -o tile -lm
./tile $(find $1 -maxdepth 1 -name \*.tga ) $2
