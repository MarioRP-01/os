#!/bin/bash

if test $# -ne 1 ; then
	echo "Wrong number of parameters" ; exit 1
fi

if ! test -f $1 ; then 
	echo "The parameter is not a file" ; exit 2
fi

FILE=$1
MY_PATH=$PATH
FOUND=0

for DIR in $(echo $MY_PATH | tr ":" "\n") ; do
	FILE_IF_EXIST=$(find -name $DIR)
	if ! test -z $FILE_IF_EXIST ; then
		FOUND=1
		echo $FILE_IF_EXIST
	fi
done
if test $FOUND -eq 0 ; then
	echo "The file given is not in the path"
fi

exit 0;
