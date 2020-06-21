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
var=$(find $1 -maxdepth 1 -name \*.tga )
if [ -z "$var" ]; then
	echo "Directory has no .tga image files."
	exit 1
fi
SAVE=$IFS
IFS=$'\n'
var=($var)
IFS=$SAVE
cd Code
gcc dimensions.c image_IO_TGA.c -o dimensions
gcc crop.c image_IO_TGA.c -o crop
gcc split.c image_IO_TGA.c -o split
gcc rotate.c image_IO_TGA.c -o rotate
cd ..
for file in ${var[@]}; do
	height=$(./Code/dimensions -h $file)
	width=$(./Code/dimensions -w $file)
	re='^[0-9]+$'
	if [[ $width =~ $re ]] && [[ $height =~ $re ]]
	then
		hheight=$((height/2))
		hwidth=$((width/2))
		echo $height $hheight $width $hwidth
		./Code/crop $file $2 0 0 $hheight $hwidth
		./Code/crop $file $2 $hheight 0 $hheight $hwidth
		./Code/crop $file $2 0 $hwidth $hheight $hwidth
		./Code/crop $file $2 $hheight $hwidth $hheight $hwidth
		./Code/split $file $2
		./Code/rotate l $file $2
	else
		echo $height
	fi
done
