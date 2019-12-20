#!/bin/bash
clear

OIFS="$IFS"
IFS=$'\n'

EXE="./createcat"

#OPTION1=-debug
#OPTION2=-fuzzy
#OPTION3=-mobysearch
#OPTION4=-skiphidden

if [[ -d $1 ]]; then
	for file in `find "$1" -type f -name "*.db"`  
	do
		echo
		$EXE "$file" $OPTION1 $OPTION2 $OPTION3 $OPTION4
	done
elif [[ -f $1 ]]; then
	$EXE "$1" $OPTION1 $OPTION2 $OPTION3 $OPTION4
else
	echo "$1 not a file or folder!"
	exit 1
fi
