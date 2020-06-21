#!/bin/bash

#path to folder
#path to output file
#searches for largest process indec
#launches the server process
#   parameters: # child process, folder path, output file name
var=$(find $1 -maxdepth 1 -name \*.txt )
SAVE=$IFS
IFS=$'\n'
var=($var)
IFS=$SAVE
pNum=-1

for file in ${var[@]}
do
	data=$(grep '^[0-9]*' $file )
	data=($data)
	if [[ "${data[0]}" -gt "$pNum" ]]
	then
		pNum=${data[0]}
	fi	
done
g++ ../Source/Version3/start3_distributor.cpp -o start3_distributor
g++ ../Source/Version3/start3_processor.cpp -o start3_processor
g++ ../Source/Version3/version3.cpp -o version3
../Source/Version3/version3.cpp $((pNum + 1)) $1 $2

